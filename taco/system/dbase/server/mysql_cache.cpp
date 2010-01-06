/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File:
 *		mysql_cache.cpp
 * Description:
 *
 * Authors:
 *		$Author: andy_gotz $
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2010-01-06 17:36:34 $
 *
 */

#include <MySqlServer.h>

using namespace std;

/* Handle cache key for resource cache map */

// The default class constructor

CacheResKey::CacheResKey()
{
}

// Constructor from elements

CacheResKey::CacheResKey(string &s,long i):fmn(s),ind(i)
{
}

CacheResKey::CacheResKey(char *key_str,long i):ind(i)
{
	string tmp(key_str);
	
	fmn = key_str;
}
	
	
// The copy constructor

CacheResKey::CacheResKey(CacheResKey &ca)
{
	this->fmn = ca.fmn;
	this->ind = ca.ind;
}

// Another copy constructor

CacheResKey::CacheResKey(const CacheResKey &ca)
{
	this->fmn = ca.fmn;
	this->ind = ca.ind;
}

// The class destructor
	
CacheResKey::~CacheResKey()
{
}

// The assignment operator
	
CacheResKey & CacheResKey::operator=(CacheResKey &ca)
{
	this->fmn = ca.fmn;
	this->ind = ca.ind;
	
	return(*this);
}

// Another assignement operator

CacheResKey & CacheResKey::operator=(const CacheResKey &ca)
{
	this->fmn = ca.fmn;
	this->ind = ca.ind;
	
	return(*this);
}

// The < operator
	
bool operator< (const CacheResKey &ca,const CacheResKey &cb)
{
	if (ca.fmn < cb.fmn)
		return(True);
	else if (ca.fmn == cb.fmn)
	{
		if (ca.ind < cb.ind)
			return(True);
		else
			return(False);
	}
	else
		return(False);
}
		
/**
 * Initialise a domain cache
 * Returns 0 on success 1 when failed.
 */
long MySQLServer::fill_cache(std::string domain)
{
  
  string query;
  
  query  =  "SELECT DISTINCT device,name FROM property_device ";
  query +=  "WHERE domain='" + domain + "'";
  
  if (mysql_query(mysql_conn, query.c_str()) != 0)
  {
    logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
    logStream->errorStream() << query << log4cpp::eol;
    return 1;
  }
  
  MYSQL_RES *res_list = mysql_store_result(mysql_conn);
  MYSQL_ROW row;
  
  while((row = mysql_fetch_row(res_list)) != NULL) {
  
    string res_name = string(row[0]) + "/" + string(row[1]);
    
    query =  "SELECT value FROM property_device ";
    query += "WHERE device = '" + std::string(row[0]) + "' AND name= '" + std::string(row[1]) + "' ";
    query += "ORDER BY count ASC";

    if (mysql_query(mysql_conn, query.c_str()) != 0)
    {
      mysql_free_result(res_list);
      logStream->errorStream() << mysql_error(mysql_conn) << log4cpp::eol;
      return 1;
    }

    MYSQL_RES *result = mysql_store_result(mysql_conn);
    MYSQL_ROW row2;
    
    CacheResKey key(res_name,1);
    
    while((row2 = mysql_fetch_row(result)) != NULL) {
    
      string res_value(row2[0]);
      pair<CacheResKey,string> ins(key,res_value);
      res_cache.insert(ins);
      key.inc_key_ind();
      
    }
                
    mysql_free_result(result);    
    
  }

  mysql_free_result(res_list);
  
  return 0;

}

/**
 * To retrieve a resource value from the cache
 *
 * @param tab_name The table name where the ressource can be retrieved
 * @param p_res_name A part of the resource name (FAMILY/MEMBER/RES.)
 * @param The adress where to put the resource value (as a string) 
 * @param The buffer's address used to store temporary results
 *
 * @return 0 if no errors occurs or the error code when there is a problem. 
 */
int MySQLServer::db_find_from_cache(std::string tab_name, std::string res_name, char **out, int *count)
{

  string full_res_name = tab_name + "/" + res_name;
  CacheResKey key(full_res_name,1);
  cache::iterator p;
  vector<string> values;  
  
  p = res_cache.find(key);  
  while (p != res_cache.end())
  {
    values.push_back(p->second);
    key.inc_key_ind();    
    p = res_cache.find(key);  
  }
  
  *count = values.size();

  try {
  
    if( *count==0 ) {
  
      // Not found
      *out = new char[10];
      strcpy(*out,"N_DEF");
    
    } else if ( *count==1 ) {
  
      // Single resource
      *out = new char[values[0].length() + 1];
      strcpy(*out, values[0].c_str());
    
    } else {
  
      // Array resource
      char tmp_count[10];
      string ret_value;
      ret_value += INIT_ARRAY;
      sprintf(tmp_count,"%d",*count);
      ret_value += tmp_count;
      for(int i=0;i<*count;i++) {
        ret_value += SEP_ELT;
        ret_value += values[i];
      }
      *out = new char[ret_value.length() + 1];
      strcpy(*out, ret_value.c_str());
        
    }
  
  } catch(const std::bad_alloc &e) {
    
    logStream->errorStream() << "Error in malloc for out" << log4cpp::eol;
    throw e;
    
  }
  
  return DS_OK;
  
}

/**
 * add a resource to the resource cache
 * 
 * @param res_name The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)
 * @param count Resource index (from 1 to n)
 * @param content The content of the resource
 *
 */
void  MySQLServer::db_insert_into_cache(string res_name, string count, string value) {

  long idx;
  sscanf(count.c_str(),"%d",&idx);
  db_insert_into_cache(res_name,idx,value);
  
}

void  MySQLServer::db_insert_into_cache(string res_name, long count, string value) {

  CacheResKey key(res_name,count);
  pair<CacheResKey,string> ins(key,value);
  res_cache.insert(ins);
  
}

/**
 * delete a resource from the resource cache
 * 
 * @param res_name The full resource name (DOMAIN/FAMILY/MEMBER/R_NAME)
 *
 */
void MySQLServer::db_delete_from_cache(string res_name) {

  CacheResKey key(res_name,1);

  while (res_cache.erase(key) == 1) {
    key.inc_key_ind();
  }

}
