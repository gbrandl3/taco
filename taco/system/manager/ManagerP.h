/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * Version:	$Revision: 1.9 $
 *
 * Date:		$Date: 2008-12-02 09:32:36 $
 *
 *
 *		       All Rights Reserved
 */

#ifndef _ManagerP_h
#define _ManagerP_h

extern void	unreg_server (int);

/*
 *  definitions for the static database server
 */
/*
 * Definitions for RTDB database
 */
static const char	*db_server         = "dbsu_server";
static const char   	*db_name           = "Set_bdd";

/*
 * Definitions for DBM and MYSQL databases
 */
static const char     *dbm_server 	    = "dbm_server";
static const char   	*dbm_name           = "taco";
	
/*
 * Definition for ORACLE database
 */
static const char 	*ora_server	    = "db_server";
static const char 	*ora_name	    = "res";
static const char	*ora_tns	    = "/usr/local/oracle";


typedef enum{E_ORACLE = 0, E_MYSQL, E_GDBM, E_SQLITE} dbm_type;


typedef struct 	{
		short	msg_server;
		short	db_server;
		short	startup;
		short	security;
		short	request_log;
		dbm_type dbm;
} manager_config_flags;

typedef struct 	{
		char 	host_name [HOST_NAME_LENGTH];
		long 	prog_number;
		long 	vers_number;
		} server_conf;
		
#endif /* _ManagerP_h */

