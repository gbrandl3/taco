#include <DevErrors.h>
#include <NdbmServer.h>


/****************************************************************************
*                                                                           *
*	Server code for the devserverlist_1_svc function       	   	    *
*                           -------------------                       	    *
*                                                                           *
*    Method rule : To retrieve device server list for all the devices       *
*		   defined in the NAMES table			  	    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *NdbmServer::devserverlist_1_svc()
{
    datum 		key;
    string	 	server;
    NdbmNameList 	serv_list;
	
#ifdef DEBUG
    cout << "In devserverlist_1_svc function" << endl;
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
    if (!dbgen.connected)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the server name list from the NAMES table
//
    try 
    {
	for (key = dbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
	{
	    NdbmNamesKey nameskey(key);
			
	    if (nameskey.get_dev_indi() != 1)
		continue;
	    server = nameskey.get_ds_name();
	    serv_list.add_if_new(server);		
	}
	if (dbm_error(dbgen.tid[0]) != 0)
	{			
	    dbm_clearerr(dbgen.tid[0]);
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
	cerr << "Memory allocation error in devserverlist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
//
// Sort device server name list
//
    serv_list.sort_name();
//
// Build the structure returned to caller
//
    browse_back.res_val.arr1_len = serv_list.size();
    if (serv_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in devserverlist" << endl;
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
*	Server code for the devpersnamelist_1_svc function    	    	    *
*                           ---------------------                    	    *
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
db_res *NdbmServer::devpersnamelist_1_svc(nam *server)
{
    datum 	 key;
    string 	 serv,
    		 pers,
    		 user_server(*server);
    NdbmNameList pers_list;
	
#ifdef DEBUG
    cout << "In devpersnamelist_1_svc function for server " << *server << endl;
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
    if (!dbgen.connected)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the family name list for the wanted domain in the NAMES table
//
    try
    {
	for (key = dbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
	{
	    NdbmNamesKey nameskey(key);

	    if (nameskey.get_dev_indi() != 1)
		continue;
	    serv = nameskey.get_ds_name();
	    if (serv != user_server)
		continue;
	    pers = nameskey.get_ds_pers_name();
	    pers_list.add_if_new(pers);
	}
	if (dbm_error(dbgen.tid[0]) != 0)
	{
	    dbm_clearerr(dbgen.tid[0]);
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
	cerr << "Memory allocation error in devpersnamelist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);		
    }
//
// Sort device server personal name list
//
    pers_list.sort_name();
//
// Build the sequence returned to caller
//
    browse_back.res_val.arr1_len = pers_list.size();
    if (pers_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in devpersnamelist" << endl;
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
*	Server code for the hostlist_1_svc function       	   	    *
*                           --------------                       	    *
*                                                                           *
*    Method rule : To device host list where device server should run       *
*		   in the NAMES table			  		    *
*                                                                           *
*    Argin : No argin							    *
*                                                                           *
*    Argout : domain_list : The domain name list 			    *
*                                                                           *
*                                                                           *
****************************************************************************/
db_res *NdbmServer::hostlist_1_svc()
{
    datum key;
    string host;
    NdbmNameList host_list;
	
#ifdef DEBUG
    cout << "In hostlist_1_svc function" << endl;
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
    if (!dbgen.connected)
    {
	browse_back.db_err = DbErr_DatabaseNotConnected;
	return(&browse_back);
    }
//
// Get the server name list from the NAMES table
//
    try 
    {
	for (key = dbm_firstkey(dbgen.tid[0]);key.dptr != NULL;key = dbm_nextkey(dbgen.tid[0]))
	{
//
// Skip record if the device indice is different than 1
//
	    NdbmNamesKey nameskey(key);
			
	    if (nameskey.get_dev_indi() != 1)
		continue;
	    NdbmNamesCont cont(dbgen.tid[0],key);
	    host = cont.get_host_name();
//
// Skip record if the device is not exported (for DS with DB library release 4)
//
	    if (host == "not_exp")
		continue;
	    host_list.add_if_new(host);		
	}
	if (dbm_error(dbgen.tid[0]) != 0)
	{			
	    dbm_clearerr(dbgen.tid[0]);
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
	cerr << "Memory allocation error in devserverlist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
//
// Sort device server host list
//
    host_list.sort_name();
//
// Build the structure returned to caller
//
    browse_back.res_val.arr1_len = host_list.size();
    if (host_list.copy_to_C(browse_back.res_val.arr1_val) != 0)
    {
	cerr << "Memory allocation error in hostlist" << endl;
	browse_back.db_err = DbErr_ServerMemoryAllocation;
	return(&browse_back);
    }
//
// Return data
//
    return(&browse_back);
}
