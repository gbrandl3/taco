#include <API.h>
#include <DevErrors.h>
#include <db_xdr.h>

// C++ include

#include <new>
#include <string>
#include <iostream>
#include <NdbmClass.h>

/* Some local functions declaration */


/* Variables defined elsewhere */

extern NdbmInfo dbgen;
extern db_res browse_back;


extern int errno;




/****************************************************************************
*                                                                           *
*	Server code for the resdomainlist_1_svc function     	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrive resource domain list for all the resources    *
*		   defined in the database				    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *                                                          *
*                                                                           *
****************************************************************************/


db_res *resdomainlist_1_svc()
{
	datum key;
	char domain[40];
	long i;

	NdbmNameList dom_list;
	
#ifdef DEBUG
	cout << "In resdomainlist_1_svc function" << endl;
#endif

//
// Initialize structure sent back to client
//

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	
//
// If the server is not connected to the database, return error
//

	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}

//
// Find each domain
//

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if ((dbgen.TblName[i] == "names") ||
		    (dbgen.TblName[i] == "ps_names"))
		{
		    	continue;
		}
		else
		{
			dom_list.add_if_new(dbgen.TblName[i]);
		}
	}
		
//
// Sort domain name list
//
	
	dom_list.sort_name();

//
// Bild the structure returned to caller
//

	browse_back.res_val.arr1_len = dom_list.length();
	if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		cerr << "Memory allocation error in resdomainlist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
	
//
// Return data
//

	return(&browse_back);	
}



/****************************************************************************
*                                                                           *
*	Server code for the resfamilylist_1_svc function     	   	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve all the family defined (in resources name)   *
*		   for a specific domain				    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/

db_res *resfamilylist_1_svc(nam* domain)
{
	datum key;
	string family;
	long i;
	NdbmNameList fam_list;
	
#ifdef DEBUG
	cout << "In resfamilylist_1_svc function for domain " << *domain << endl;
#endif

	string user_domain(*domain);
	
//
// Initialize structure sent back to client
//

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
		
//
// If the server is not connected to the database, throw exception
//

	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
	
//
// Find the db table for the specificated domain
//

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == user_domain)
			break;
	}
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}

//
// Get all resources family name defined in this table
//
	
	try
	{
		for (key = dbm_firstkey(dbgen.tid[i]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[i]))
		{
			NdbmResKey reskey(key);
			
			reskey.get_res_fam_name(family);
			fam_list.add_if_new(family);
		}
		if (dbm_error(dbgen.tid[i]) != 0)
		{			
			dbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		cerr << err.get_err_message() << endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (bad_alloc)
	{		
		cerr << "Memory allocation error in resfamilylist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Sort family name list
//
	
	fam_list.sort_name();
	
//
// Build the structure returned to caller
//
	
	browse_back.res_val.arr1_len = fam_list.length();
	if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		cerr << "Memory allocation error in resfamilylist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Return data
//

	return(&browse_back);	
}



/****************************************************************************
*                                                                           *
*	Server code for the resmemberlist_1_svc function 	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve all the family defined (in resources name)   *
*		   for a specific couple domain,family			    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*	     - family : The family name					    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                           *
*                                                                           *
****************************************************************************/

db_res *resmemberlist_1_svc(db_res *recev)
{
	datum key;
	string fam;
	string member;
	NdbmNameList memb_list;
	long i;
	
	string user_domain(recev->res_val.arr1_val[0]);
	string user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
	cout << "In resmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << endl;
#endif
	
//
// Initialize structure sent back to client
//

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	
//
// If the server is not connected to the database, return error
//

	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
	
//
// Find the db table for the specificated domain
//

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == user_domain)
			break;
	}
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}
	
//
// Get all resources family name defined in this table
//
	
	try
	{
		for (key = dbm_firstkey(dbgen.tid[i]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[i]))
		{
			NdbmResKey reskey(key);
			
			reskey.get_res_fam_name(fam);
			if (fam != user_family)
				continue;
			reskey.get_res_memb_name(member);
			memb_list.add_if_new(member);
		}
		if (dbm_error(dbgen.tid[i]) != 0)
		{			
			dbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		cerr << err.get_err_message() << endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (bad_alloc)
	{		
		cerr << "Memory allocation error in resmemberlist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
		
//
// Sort member name list
//
	
	memb_list.sort_name();
	
//
// Build the structure returned to caller
//
		
	browse_back.res_val.arr1_len = memb_list.length();
	if (memb_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		cerr << "Memory allocation error in resmemberlist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//

	return(&browse_back);	
}



/****************************************************************************
*                                                                           *
*	Server code for the resresolist_1_svc function 	    	    	    *
*                           -----------------                     	    *
*                                                                           *
*    Method rule : To retrieve a list of resources for a given device	    *
*                                                                           *
*    Argin : - domain : The device domain name				    *
*	     - family : The device family name				    *
*	     - member : The device member name				    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                    	    *
*                                                                           *
****************************************************************************/

db_res *resresolist_1_svc(db_res *recev)
{
	datum key;
	string fam;
	string memb;
	string resource;
	NdbmNameList reso_list;
	string::size_type pos;
	long i,l;
	
	string user_domain(recev->res_val.arr1_val[0]);
	string user_family(recev->res_val.arr1_val[1]);
	string user_member(recev->res_val.arr1_val[2]);
	
#ifdef DEBUG
	cout << "In resresolist_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << endl;
#endif
	
//
// Initialize structure sent back to client
//

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
	
//
// If the server is not connected to the database, return error
//

	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
	
//
// Find the db table for the specificated domain
//

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == user_domain)
			break;
	}
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}
	
//
// Get all resources for the specified domain,family,member
// The test to know if the resource is a new one is done by the index value
// which is 1 for all new resource
//

	try
	{
		for (key = dbm_firstkey(dbgen.tid[i]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[i]))
		{
			long ind;
			NdbmResKey reskey(key);

			reskey.get_res_indi(ind);
			if (ind != 1)
				continue;			
			reskey.get_res_fam_name(fam);
			if (fam != user_family)
				continue;
			if (user_member != "*")
			{
				reskey.get_res_memb_name(memb);
				if (memb != user_member)
					continue;
			}
			reskey.get_res_name(resource);
			reso_list.add_if_new(resource);
		}
		if (dbm_error(dbgen.tid[i]) != 0)
		{			
			dbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		cerr << err.get_err_message() << endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (bad_alloc)
	{		
		cerr << "Memory allocation error in resresolist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

//
// If resources belongs to SEC domain, replace all occurances of ^ by |
//

	if (user_domain == "sec")
	{
		l = reso_list.length();
		for (i = 0;i < l;i++)
		{
			pos = 0;
			while ((pos = reso_list[i].find_first_of(SEC_SEP,pos)) != string::npos)
			{
				(reso_list[i])[pos] = '|';
				pos++;
			}
		}
	}
			
//
// Sort resource name list
//
	
	reso_list.sort_name();
	
//
// Build the structure returned to caller
//
		
	browse_back.res_val.arr1_len = reso_list.length();
	if (reso_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		cerr << "Memory allocation error in resresolist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//

	return(&browse_back);	
}


/****************************************************************************
*                                                                           *
*	Server code for the resresoval_1_svc function     	    	    *
*                           ----------------                     	    *
*                                                                           *
*    Method rule : To retrieve a resource value (as strings)		    *
*                                                                           *
*    Argin : - domain : The device domain name				    *
*	     - family : The device family name				    *
*	     - member : The device member name				    *
*	     - resource : The resource name				    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                    	    *
*                                                                           *
****************************************************************************/

db_res *resresoval_1_svc(db_res *recev)
{
	string res_val;
	NdbmNameList reso_val;
	datum key;
	string fam;
	string memb;
	string resource;
	string tmp_res;
	long i,j,k,nb_memb,nb_res,seq;
	string::size_type pos;
	
	NdbmDoubleNameList memb_res_list;
	
	string user_domain(recev->res_val.arr1_val[0]);
	string user_family(recev->res_val.arr1_val[1]);
	string user_member(recev->res_val.arr1_val[2]);
	string user_reso(recev->res_val.arr1_val[3]);
		
#ifdef DEBUG
	cout << "In resresoval_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << "/" << user_reso << endl;
#endif
	
//
// Initialize structure sent back to client
//

	browse_back.db_err = 0;
	browse_back.res_val.arr1_len = 0;
	browse_back.res_val.arr1_val = NULL;
		
//
// If the server is not connected to the database, throw exception
//

	if (dbgen.connected == False)
	{
		browse_back.db_err = DbErr_DatabaseNotConnected;
		return(&browse_back);
	}
	
//
// Find the db table for the specificated domain
//

	for (i = 0;i < dbgen.TblNum;i++)
	{
		if (dbgen.TblName[i] == user_domain)
			break;
	}
	if (i == dbgen.TblNum)
	{
		browse_back.db_err = DbErr_DomainDefinition;
		return(&browse_back);
	}

//
// Get a list of all members and resource name
//

	try
	{
		for (key = dbm_firstkey(dbgen.tid[i]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[i]))
		{
			long ind;
			NdbmResKey reskey(key);

			reskey.get_res_indi(ind);
			if (ind != 1)
				continue;			
			reskey.get_res_fam_name(fam);
			if (fam != user_family)
				continue;
			reskey.get_res_memb_name(memb);
			if (user_member != "*")
			{
				if (memb != user_member)
					continue;
			}
			reskey.get_res_name(resource);
			if (user_reso != "*")
			{
				if (resource != user_reso)
					continue;
			}			
			memb_res_list.add(memb,resource);
		}
		if (dbm_error(dbgen.tid[i]) != 0)
		{			
			dbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		cerr << err.get_err_message() << endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (bad_alloc)
	{		
		cerr << "Memory allocation error in resresolist" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}


	nb_memb = memb_res_list.first_name_length();

	for (k = 0;k < nb_memb;k++)
	{
		nb_res = memb_res_list.sec_name_length(k);
		
		for (j = 0;j < nb_res;j++)
		{
			try
			{
		
				memb_res_list.get_record(k,j,memb,resource);
//
// Build the first field of the complete resource name
//

				res_val = user_domain + "/" + user_family + "/" + memb + "/" + resource + ": ";
	
//
// Get resource value
//

				seq = 1;

				NdbmResKey reskey(user_family,memb,resource,seq);
		
				while(1)
				{
					if (seq != 1)
						reskey.upd_indi(seq); 
	
					NdbmResCont rescont(dbgen.tid[i],reskey.get_key());
			
					rescont.get_res_value(tmp_res);
					if (seq == 1)
						res_val = res_val + tmp_res;
					else
						res_val = res_val + "," + tmp_res;
					seq++;
				}
			}
			catch (NdbmError)
			{
				if (seq == 1)
				{
					if (dbm_error(dbgen.tid[i]) != 0)
					{
						dbm_clearerr(dbgen.tid[i]);			
						browse_back.db_err = DbErr_DatabaseAccess;
						return(&browse_back);
					}
					else
					{
						if (user_member != "*")
						{
							if (user_reso != "*")
							{
								browse_back.db_err = DbErr_ResourceNotDefined;
								return(&browse_back);
							}
						}
					}
				}
				else
				{
					if (dbm_error(dbgen.tid[i]) != 0)
					{
						dbm_clearerr(dbgen.tid[i]);			
						browse_back.db_err = DbErr_DatabaseAccess;
						return(&browse_back);
					}
					reso_val.add_if_new(res_val);
				}
			}	
			catch (bad_alloc)
			{
				cerr << "Memory allocation error in resresoval" << endl;
				browse_back.db_err = DbErr_ServerMemoryAllocation;
				return(&browse_back);
			}	

		}
	}
			
//
// If resources belongs to SEC domain, replace all occurances of ^ by |
//

	if (user_domain == "sec")
	{
		j = reso_val.length();
		for (i = 0;i < j;i++)
		{
			pos = 0;
			while ((pos = reso_val[i].find_first_of(SEC_SEP,pos)) != string::npos)
			{
				(reso_val[i])[pos] = '|';
				pos++;
			}
		}
	}	
//
// Sort the resource array
//

	reso_val.sort_name();		
	
//
// Build the structure returned to caller
//
		
	browse_back.res_val.arr1_len = reso_val.length();
	if (reso_val.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		cerr << "Memory allocation error in resresoval" << endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//

	return(&browse_back);
}
