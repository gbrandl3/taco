/*
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 ESRF, www.esrf.fr
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
 * File:        db_reconnect.cpp
 *
 * Description: Code for db_resget test program
 *              Command rule : To get a resource from the static database.
 *              Synopsis : db_resget <resource name>
 *
 * Author(s):
 *              $Author: jkrueger1 $
 *
 * Version:     $Revision: 1.3 $
 *
 * Date:        $Date: 2009-01-16 15:18:43 $
 */

#include <API.h>
#include <DevServer.h>
#include <private/ApiP.h>
#include <DevErrors.h>
#include <macros.h>
#include <db_setup.h>
#include <db_xdr.h>

extern dbserver_info         db_info;
extern configuration_flags   *config_flags;
extern nethost_info          *multi_nethost;

static char devname[] = "test/sr/1";

static db_resource     res_tab[] =
{
	{"my_res",    D_VAR_STRINGARR,  NULL},
};

static long nb_res =sizeof(res_tab)/sizeof(db_resource);

int main(int argc,char **argv)
{
	DevLong error=0;
	long status;
	char *seq[3];
	CLIENT *new_client,
	*old_client;

	DevVarStringArray value;
	value.length = 1;
	seq[0] = "val 1";
	seq[1] = "val 2";
	seq[2] = "val 3"; 
	value.sequence = seq;
 
	res_tab[0].resource_adr = &value;
 
	status = db_putresource(devname,res_tab,nb_res,&error);
 
	if( status!=DS_OK ) 
	{
		char *str_err = dev_error_str(error);
		printf("db_putresource failed : %s\n",str_err);
		free(str_err);
		exit(0);
	}

/* Force database reconnection */
 
	old_client = db_info.conf->clnt;
	config_flags->database_server = False;
	config_flags->configuration = False;
	if(db_import(&error))
	{
		char *str_err = dev_error_str(error);
		printf("db_import failed : %s\n",str_err);
		free(str_err);
		exit(0);
	}
	new_client = db_info.conf->clnt;
	if (old_client != new_client)
		clnt_destroy(old_client);
 
/* --------------------------- */
 
	status = db_putresource(devname,res_tab,nb_res,&error);
 
	if( status!=DS_OK ) 
	{
		char *str_err = dev_error_str(error);
		printf("db_putresource failed : %s\n",str_err);
		free(str_err);
		exit(0);
	}
}

