#include <DevErrors.h>
#include <NdbmServer.h>


/****************************************************************************
*                                                                           *
*	Server code for the devdomainlist_1_svc function       	   	    *
*                           -------------------                       	    *
*                                                                           *
*    Method rule : To device domain list for all the device name defined    *
*		   in the NAMES and PS_NAMES tables			    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *NdbmServer::devdomainlist_1_svc(void)
{
    NdbmNameList 	dom_list;
	
#ifdef DEBUG
    cout << "In devdomainlist_1_svc function" << endl;
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
    if (dbgen.connected)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the domain name list from the NAMES table
//
    try 
    {
	for (datum key = dbm_firstkey(dbgen.tid[0]); key.dptr != NULL; key = dbm_nextkey(dbgen.tid[0]))
	{
	    NdbmNamesCont cont(dbgen.tid[0], key);
	    dom_list.add_if_new(cont.get_dev_domain_name());		
	}
	if (dbm_error(dbgen.tid[0]) != 0)
	{			
	    dbm_clearerr(dbgen.tid[0]);
	    browse_back.db_err = DbErr_DatabaseAccess;
	    return(&browse_back);			
	}
//
// Add new domain from the PS_NAMES table
//
	for (datum key = dbm_firstkey(dbgen.tid[dbgen.ps_names_index]); 
		     key.dptr != NULL; 
		     key = dbm_nextkey(dbgen.tid[dbgen.ps_names_index]))
	{
	    NdbmPSNamesKey pskey(key);
	    dom_list.add_if_new(pskey.get_psdev_domain_name());
	}
	if (dbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
	{
	    dbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
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
	cerr << "Memory allocation error in devdomainlist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
//
// Sort domain name list
//
    dom_list.sort_name();
//
// Build the structure returned to caller
//
    browse_back.res_val.arr1_len = dom_list.size();
    if (dom_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in devdomainlist" << endl;
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
*	Server code for the devfamilylist_1_svc function    	    	    *
*                           -------------------                    	    *
*                                                                           *
*    Method rule : To device family list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain		    *
*                                                                           *
*    Argin : - domain : The domain name					    *
*                                                                           *
*    Argout : - family_list : The family name list 			    *
*                                                                           * 
*                                                                           *
****************************************************************************/
db_res *NdbmServer::devfamilylist_1_svc(nam * domain)
{
    string		user_domain(*domain);
    NdbmNameList 	fam_list;
	
#ifdef DEBUG
    cout << "In devfamilylist_1_svc function for domain " << *domain << endl;
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
// Get the family name list for the wanted domain in the NAMES table
//
    try
    {
	for (datum key = dbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
	{
	    NdbmNamesCont cont(dbgen.tid[0],key);
		
	    if (cont.get_dev_domain_name() != user_domain)
		continue;
	    fam_list.add_if_new(cont.get_dev_fam_name());
	}
	if (dbm_error(dbgen.tid[0]) != 0)
	{
	    dbm_clearerr(dbgen.tid[0]);
	    browse_back.db_err = DbErr_DatabaseAccess;
	    return(&browse_back);
	}
//
// Add family name list for the wanted domain in from the PS_NAMES table
//
	for (datum key = dbm_firstkey(dbgen.tid[dbgen.ps_names_index]); 
	     key.dptr != NULL; 
	     key = dbm_nextkey(dbgen.tid[dbgen.ps_names_index]))
	{
	    NdbmPSNamesKey pskey(key);
		
	    if (pskey.get_psdev_domain_name() != user_domain)
		continue;
	    fam_list.add_if_new(pskey.get_psdev_fam_name());
	}
	if (dbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
	{
	    dbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
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
	cerr << "Memory allocation error in devfamilylist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);		
    }
//
// Sort family name list
//
    fam_list.sort_name();
//
// Build the sequence returned to caller
//
    browse_back.res_val.arr1_len = fam_list.size();
    if (fam_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in devfamilylist" << endl;
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
*	Server code for the devmemberlist_1_svc fucntion 	    	    *
*                           -------------------                     	    *
*                                                                           *
*    Method rule : To device member list for all the device defined in the  *
*		   NAMES and PS_NAMES tables for a given domain and family  *
*                                                                           *
*    Argin : - domain : The domain name					    *
*	     - family : The family name					    *
*                                                                           *
*    Argout : - member_list : The member name list			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *NdbmServer::devmemberlist_1_svc(db_res *recev)
{
    NdbmNameList memb_list;
//
// Build strings from input names
//
    string user_domain(recev->res_val.arr1_val[0]);
    string user_family(recev->res_val.arr1_val[1]);
	
#ifdef DEBUG
    cout << "In devmemberlist_1_svc function for domain " << user_domain << " and family " << user_family << endl;
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
// Get the member name list for the wanted domain and family from NAMES table
//
    try
    {
	for (datum key = dbm_firstkey(dbgen.tid[0]); key.dptr != NULL; key = dbm_nextkey(dbgen.tid[0]))
	{
	    NdbmNamesCont cont(dbgen.tid[0],key);
	    if (cont.get_dev_domain_name() != user_domain || cont.get_dev_fam_name() != user_family)
		continue;
	    memb_list.add_if_new(cont.get_dev_memb_name());
	}
	if (dbm_error(dbgen.tid[0]) != 0)
	{
	    dbm_clearerr(dbgen.tid[0]);
	    browse_back.db_err = DbErr_DatabaseAccess;
	    return(&browse_back);
	}
//
// Add member name for the wanted domain and family from PS_NAMES table
//
	for (datum key = dbm_firstkey(dbgen.tid[dbgen.ps_names_index]); 
	     key.dptr != NULL; 
	     key = dbm_nextkey(dbgen.tid[dbgen.ps_names_index]))
	{
	    NdbmPSNamesKey pskey(key);
		
	    if (pskey.get_psdev_domain_name() != user_domain || pskey.get_psdev_fam_name() != user_family)
		continue;
	    memb_list.add_if_new(pskey.get_psdev_memb_name());
	}
	if (dbm_error(dbgen.tid[dbgen.ps_names_index]) != 0)
	{
	    dbm_clearerr(dbgen.tid[dbgen.ps_names_index]);
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
	cerr << "Memory allocation error in devmemberlist" << endl;
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
	cerr << "Memory allocation error in devmemberlist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
//
// Return data
//
    return(&browse_back);	
}
