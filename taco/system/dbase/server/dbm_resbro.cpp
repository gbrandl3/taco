#include <API.h>
#include <DevErrors.h>
#include <db_xdr.h>

// C++ include

#include <new>
#include <string>
#include <iostream>
#include <NdbmClass.h>
#include <NdbmServer.h>
#include <errno.h>


/****************************************************************************
*                                                                           *
*	Server code for the resdomainlist_1_svc function     	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To retrieve resource domain list for all the resources    *
*		   defined in the database				    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *                                                          *
*                                                                           *
****************************************************************************/
db_res *NdbmServer::resdomainlist_1_svc()
{
#ifdef DEBUG
	std::cout << "In resdomainlist_1_svc function" << std::endl;
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
	NdbmNameList dom_list;
	for (long i = 0;i < dbgen.TblNum;i++)
		if ((dbgen.TblName[i] == "names") || (dbgen.TblName[i] == "ps_names"))
		    	continue;
		else
			dom_list.add_if_new(dbgen.TblName[i]);
		
//
// Sort domain name list
//
	dom_list.sort_name();

//
// Bild the structure returned to caller
//
	browse_back.res_val.arr1_len = dom_list.size();
	if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in resdomainlist" << std::endl;
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
db_res *NdbmServer::resfamilylist_1_svc(nam* domain)
{
	long 		i;
	
#ifdef DEBUG
	std::cout << "In resfamilylist_1_svc function for domain " << *domain << std::endl;
#endif
	std::string user_domain(*domain);
	
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
	NdbmNameList 	fam_list;
	try
	{
		datum 		key, 
				key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);
			fam_list.add_if_new(reskey.get_res_fam_name());
		}
		free(key.dptr);
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		std::cerr << err.get_err_message() << std::endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in resfamilylist" << std::endl;
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
	browse_back.res_val.arr1_len = fam_list.size();
	if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in resfamilylist" << std::endl;
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
db_res *NdbmServer::resmemberlist_1_svc(db_res *recev)
{
	long 		i;
	
	
#ifdef DEBUG
	std::cout << "In resmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << std::endl;
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
	std::string user_domain(recev->res_val.arr1_val[0]);
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
	std::string user_family(recev->res_val.arr1_val[1]);
	NdbmNameList 	memb_list;
	try
	{
		datum 	key, 
			key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);
			
			if (reskey.get_res_fam_name() != user_family)
				continue;
			memb_list.add_if_new(reskey.get_res_memb_name());
		}
		free(key.dptr);
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		std::cerr << err.get_err_message() << std::endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in resmemberlist" << std::endl;
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
		
	browse_back.res_val.arr1_len = memb_list.size();
	if (memb_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in resmemberlist" << std::endl;
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
db_res *NdbmServer::resresolist_1_svc(db_res *recev)
{
	long 		i;
	
#ifdef DEBUG
	std::cout << "In resresolist_1_svc function for " << user_domain 
			<< "/" << user_family << "/" << user_member << std::endl;
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
	std::string 	user_domain(recev->res_val.arr1_val[0]);
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
	NdbmNameList 	reso_list;
	std::string	user_family(recev->res_val.arr1_val[1]),
			user_member(recev->res_val.arr1_val[2]);
	try
	{
		datum 	key,
			key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);

			if (reskey.get_res_indi() != 1)
				continue;			
			if (reskey.get_res_fam_name() != user_family)
				continue;
			if (user_member != "*")
				if (reskey.get_res_memb_name() != user_member)
					continue;
			reso_list.add_if_new(reskey.get_res_name());
		}
		free(key.dptr);
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		std::cerr << err.get_err_message() << std::endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in resresolist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}

//
// If resources belongs to SEC domain, replace all occurances of ^ by |
//
	if (user_domain == "sec")
	{
		long l = reso_list.size();
		for (long i = 0; i < l; i++)
		{
			std::string::size_type	pos = 0;
			while ((pos = reso_list[i].find_first_of(SEC_SEP,pos)) != std::string::npos)
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
	browse_back.res_val.arr1_len = reso_list.size();
	if (reso_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in resresolist" << std::endl;
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
db_res *NdbmServer::resresoval_1_svc(db_res *recev)
{
	long 		i;
		
#ifdef DEBUG
	std::cout << "In resresoval_1_svc function for " << user_domain \
	<< "/" << user_family << "/" << user_member << "/" << user_reso << std::endl;
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
	std::string 	user_domain(recev->res_val.arr1_val[0]);
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
	NdbmNameList 	reso_val;
	NdbmDoubleNameList memb_res_list;
	std::string	user_family(recev->res_val.arr1_val[1]),
			user_member(recev->res_val.arr1_val[2]),
			user_reso(recev->res_val.arr1_val[3]);
	try
	{
		datum 	key,
			key2;
		for (key = gdbm_firstkey(dbgen.tid[i]);
			key.dptr != NULL;
			key2 = key, key = gdbm_nextkey(dbgen.tid[i], key), free(key2.dptr))
		{
			NdbmResKey reskey(key);

			if (reskey.get_res_indi() != 1)
				continue;			
			if (reskey.get_res_fam_name() != user_family)
				continue;
			if ((user_member != "*") && (reskey.get_res_memb_name() != user_member))
				continue;
			if ((user_reso != "*") && (reskey.get_res_name() != user_reso))
				continue;
			memb_res_list.add(reskey.get_res_memb_name(), reskey.get_res_name());
		}
		free(key.dptr);
		if (gdbm_error(dbgen.tid[i]) != 0)
		{			
			gdbm_clearerr(dbgen.tid[i]);
			browse_back.db_err = DbErr_DatabaseAccess;
			return(&browse_back);
		}
	}
	catch (NdbmError &err)
	{		
		std::cerr << err.get_err_message() << std::endl;
		browse_back.db_err = err.get_err_code();
		return(&browse_back);
	}
	catch (std::bad_alloc)
	{		
		std::cerr << "Memory allocation error in resresolist" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}


	long nb_memb = memb_res_list.first_name_length();

	for (long k = 0; k < nb_memb; k++)
	{
		long nb_res = memb_res_list.sec_name_length(k);
		for (long j = 0; j < nb_res; j++)
		{
			long		seq = 1;
			std::string	res_val;
			try
			{
				std::string	memb,
						resource;
				memb_res_list.get_record(k, j, memb, resource);
//
// Build the first field of the complete resource name
//
				res_val = user_domain + "/" + user_family + "/" + memb + "/" + resource + ": ";
	
//
// Get resource value
//
				NdbmResKey reskey(user_family, memb, resource, seq);
				while(1)
				{
					if (seq != 1)
						reskey.upd_indi(seq); 
	
					NdbmResCont rescont(dbgen.tid[i],reskey.get_key());
			
					if (seq == 1)
						res_val += rescont.get_res_value();
					else
						res_val += "," + rescont.get_res_value();
					seq++;
				}
			}
			catch (NdbmError)
			{
				if (seq == 1)
				{
					if (gdbm_error(dbgen.tid[i]) != 0)
					{
						gdbm_clearerr(dbgen.tid[i]);			
						browse_back.db_err = DbErr_DatabaseAccess;
						return(&browse_back);
					}
					else if ((user_member != "*") && (user_reso != "*"))
					{
						browse_back.db_err = DbErr_ResourceNotDefined;
						return(&browse_back);
					}
				}
				else
				{
					if (gdbm_error(dbgen.tid[i]) != 0)
					{
						gdbm_clearerr(dbgen.tid[i]);			
						browse_back.db_err = DbErr_DatabaseAccess;
						return(&browse_back);
					}
					reso_val.add_if_new(res_val);
				}
			}	
			catch (std::bad_alloc)
			{
				std::cerr << "Memory allocation error in resresoval" << std::endl;
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
		long j = reso_val.size();
		for (long i = 0; i < j; i++)
		{
			std::string::size_type	pos = 0;
			while ((pos = reso_val[i].find_first_of(SEC_SEP,pos)) != std::string::npos)
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
	browse_back.res_val.arr1_len = reso_val.size();
	if (reso_val.copy_to_C(browse_back.res_val.arr1_val) != 0)
	{
		std::cerr << "Memory allocation error in resresoval" << std::endl;
		browse_back.db_err = DbErr_ServerMemoryAllocation;
		return(&browse_back);
	}
			
//
// Return data
//
	return(&browse_back);
}
