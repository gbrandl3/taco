#include <DevErrors.h>
#include <MySqlServer.h>
#include <algorithm>


/**
 * retrieve the names of the exported devices of a device server. 
 *
 * @param fil_name A string to filter special device names in the whole list of 
 *	     the exported device
 * @param rqstp
 *
 * @return a pointer to a structure of the db_res type.
 */
db_res *MySQLServer::db_getdevexp_1_svc(nam *fil_name,struct svc_req *rqstp)
{
    std::string 	domain,	
    			family("%"),			
    			member("%");
    std::string::size_type	pos,
			last_pos;
    int 		dev_num = 0;
    char 		**ptra;
    struct sockaddr_in 	so;
    u_short 		prot;
#if defined __GLIBC__  &&  __GLIBC__ >= 2
    socklen_t  		so_size;   /* from POSIX draft - already used by GLIBC */
#else
    int 		so_size;
#endif
//
// If the server is not connected to the database, return an error 
//
    if (dbgen.connected == False)
    {
	std::cerr << "I'm not connected to database" << std::endl;
	browse_back.db_err = DbErr_DatabaseNotConnected;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

#ifdef sun
    if (rqstp->rq_xprt->xp_port == udp_port)
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#else
//
// Retrieve the protocol used to send this request to the server/
//
    so_size = sizeof(so);
    if (getsockname(rqstp->rq_xprt->xp_sock,(struct sockaddr *)&so, (socklen_t *)&so_size) == -1)
    {
	browse_back.db_err = DbErr_MaxDeviceForUDP;
	browse_back.res_val.arr1_len = 0;
	return(&browse_back);
    }

    if (so.sin_port == getUDPPort())
	prot = IPPROTO_UDP;
    else
	prot = IPPROTO_TCP;
#endif
//
// Extract from filter string each part of the filter (domain, family and 
// member). If two / characters can be retrieved in the filter string, this
// means that the domain, family and member part of the filter are initialized.
// If only one / can be retrieved, only the domain and family part are
// initialized and if there is no / in the filter string, just the domain
// is initialized. 
//
    std::string tmpf(*fil_name);
    std::string query;
    if (mysql_db == "tango")
    {
        query = "SELECT CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) FROM device WHERE ";
    }
    else
    {
        query = "SELECT CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) FROM NAMES WHERE ";
    }
/*
 * replace * with mysql wildcard %
 */
    int index;

    while ((index = tmpf.find('*')) != std::string::npos)
    {
	    tmpf.replace(index, 1, 1, '%');
    }

    switch(count(tmpf.begin(), tmpf.end(), '/'))
    {
	case 2 : pos = tmpf.find('/');
		 domain = tmpf.substr(0, pos);	
		 pos = tmpf.find('/', 1 + (last_pos = pos));
		 family = tmpf.substr(last_pos + 1, (pos - last_pos));
		 member = tmpf.substr(pos + 1);
		 if (mysql_db == "tango")
                 {
		     query += (" CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) LIKE '" + tmpf + "'");
                 }
                 else
                 {
		     query += (" CONCAT(DOMAIN, '/', FAMILY, '/', MEMBER) LIKE '" + tmpf + "'");
                 }
		 break;
	case 1 : pos = tmpf.find('/');
		 domain = tmpf.substr(0, pos);	
		 family = tmpf.substr(pos + 1);
		 if (mysql_db == "tango")
                 {
		     query += (" CONCAT(DOMAIN, '/', FAMILY) LIKE '" + tmpf + "'");
                 }
                 else
                 {
		     query += (" CONCAT(DOMAIN, '/', FAMILY) LIKE '" + tmpf + "'");
                 }
		 break;
	case 0 : domain = tmpf;		
                 if (mysql_db == "tango")
                 {
		     query += (" DOMAIN LIKE '" + tmpf + "'");
                 }
                 else
                 {
		     query += (" DOMAIN LIKE '" + tmpf + "'");
                 }
		 break;
	default: std::cerr << "To many '/' in device name." << std::endl;
		 browse_back.db_err = 1;
		 browse_back.res_val.arr1_len = 0;
		 return (&browse_back);		 		 
    }
    if (mysql_db == "tango")
    {
        query += (" AND EXPORTED != 0 AND IOR LIKE 'rpc:%'");
    }
    else
    {
        query += (" AND PROGRAM_NUMBER != 0");
    }
#ifdef DEBUG
    std::cout << "filter domain : " << domain << std::endl;
    std::cout << "filter family : " << family << std::endl;
    std::cout << "filter member : " << member << std::endl;
#endif 
//
// Try to retrieve all tuples in the database NAMES table with the PN column
// different than "not_exp" 
//

    browse_back.res_val.arr1_len = 0;
    browse_back.db_err = 0;
    if (mysql_query(mysql_conn, query.c_str()) == 0) 
    {
//
// Store the key if it is needed later 
//
	    MYSQL_RES *result = mysql_store_result(mysql_conn);
	    MYSQL_ROW row;
	dev_num = mysql_num_rows(result);
	if (prot == IPPROTO_UDP && (dev_num > MAXDEV_UDP))
	{
	    mysql_free_result(result);
	    browse_back.db_err = DbErr_MaxDeviceForUDP;
	    return (&browse_back);
	}
//
// Allocate memory for the pointer's array 
//
	int i = 0;
	try
	{
    	    ptra = new nam[dev_num];
	    for (i = 0; i < dev_num && ((row = mysql_fetch_row(result)) != NULL); i++)
	    {
	    	ptra[i] = new char[strlen(row[0]) + 1]; 
	    	strcpy(ptra[i], row[0]);
	    }
	}
	catch(std::bad_alloc)
    	{
	    mysql_free_result(result);
	    for (int j = 0; j < i; j++)
		delete [] ptra[j];
	    delete [] ptra;
	    browse_back.db_err = DbErr_ServerMemoryAllocation;
	    return(&browse_back);
    	}
	mysql_free_result(result);
    } 
    else
    {
//
// If a problem occurs during database function 
//
    	browse_back.db_err = DbErr_DatabaseAccess;
    	return(&browse_back);
    }
//
// Initialize the structure sended back to client and leave the server 
//
    browse_back.res_val.arr1_len = dev_num;
    browse_back.res_val.arr1_val = ptra;
    return(&browse_back);
}
