/*********************************************************************

 File:          ManagerP.h

 Project:       Device Servers with sun-rpc

 Description:   Private include file for the network manager

 Author(s):     Jens Meyer
 		$Author: jkrueger1 $

 Original:      January 1991

 Version:	$Revision: 1.5 $

 Date:		$Date: 2005-02-22 15:59:57 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
		       Grenoble, France

		       All Rights Reserved
*********************************************************************/

#ifndef _ManagerP_h
#define _ManagerP_h

extern void	unreg_server ();

/*
 *  definitions for the static database server
 */
/*
 * Definitions for RTDB database
 */
static char	*db_server         = "dbsu_server";
static char   	*db_name           = "Set_bdd";

/*
 * Definitions for DBM and MYSQL databases
 */
static char     *dbm_server 	    = "dbm_server";
static char   	*dbm_name           = "taco";
	
/*
 * Definition for ORACLE database
 */
static char 	*ora_server	    = "db_server";
static char 	*ora_name	    = "res";
static char	*ora_tns	    = "/usr/local/oracle";


typedef struct 	{
		short	msg_server;
		short	db_server;
		short	startup;
		short	security;
		short	request_log;
		short	oracle;
		short	mysql;
		short	dbm;
		} config_flags;

typedef struct 	{
		char 	host_name [HOST_NAME_LENGTH];
		long 	prog_number;
		long 	vers_number;
		} server_conf;
		
#endif /* _ManagerP_h */

