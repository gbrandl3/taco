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
 * File:          ManagerP.h
 * 
 * Project:       Device Servers with sun-rpc
 *
 * Description:   Private include file for the network manager
 *
 * Author(s):     Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:      January 1991
 *
 * Version:	$Revision: 1.6 $
 *
 * Date:		$Date: 2006-09-18 21:47:02 $
 *
 *
 *		       All Rights Reserved
 */

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

