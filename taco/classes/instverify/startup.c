/*********************************************************************
 * File:	startup.c
 *
 * Project:	Device Server Distribution
 *
 * Description:	Startup procedure for Inst_verifyClass. The
 *		startup procedure is the first procedure called
 *		from main() when the device server starts up.
 *		All toplevel devices to be created for the device 
 *		server should be done in startup(). The startup 
 *		should make use of the database to determine which 
 *		devices it should create. Initialisation of devices
 *		is normally done from startup().
 *
 * Author(s);	J.Meyer 
 *		$Author: jkrueger1 $
 *
 * Original:	10.5.95	
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2004-07-13 10:18:25 $
 *
 * Copyright (c) 1994 by European Synchrotron Radiation Facility, 
 *                     Grenoble, France
 *********************************************************************/

#include <Admin.h>
#include <API.h>
#include <DevServer.h>
#include <DevErrors.h>
#include <DevServerP.h>
#include <Inst_verifyP.h>
#include <Inst_verify.h>

extern long debug_flag;

/*
 * pointer to list of devices returned by database.
 *
 */
static char 		**dev_list;
static unsigned int	n_devices;
static Inst_verify	*ds_list;

/**
 * create, initialise and export all of the objects associated with this server
 * 
 * @param svr_name 	the server name
 * @param error      	error return code
 *
 * @return DS_NOTOK on failure, DS_OK otherwise
 */
long startup(char *svr_name, long *error)
{
	long		i,	
			status, 
			n_exported;
	short		iret;

/* 	debug_flag = 0xffffffff; */

	if (db_getdevlist(svr_name,&dev_list,&n_devices,error))
	{
		fprintf(stderr, "startup(): db_getdevlist() failed: %s\n",dev_error_str(*error));
		return(DS_NOTOK);
	}
	printf("following devices found in static database:\n\n");
	for (i=0;i<n_devices;i++)
		printf("\t%s\n",dev_list[i]);

/*
 * create, initialise and export all devices served by this server
 */
	ds_list = (Inst_verify *)malloc(n_devices*sizeof(Inst_verify*));
	if(ds_list == NULL)
	{
		fprintf(stderr, "can't allocate memory for object structures --> exit\n");
		return(DS_NOTOK);
	}
   
	for (i=0, n_exported = 0; i < n_devices; i++, n_exported++)
	{
		printf("\t\tObject %s is\n",dev_list[i]);
		fflush(stdout);

		if (ds__create(dev_list[i], inst_verifyClass, &(ds_list[i]),error) != DS_OK)
		{
			fprintf(stderr, "create failed: %s\n", dev_error_str(*error));
			continue;
		}
		else 
			printf("\t\t- Created\n");

/* 
 * initialise the newly created inst_verify
 */
		if(ds__method_finder(ds_list[i],DevMethodInitialise) (ds_list[i],error)!= DS_OK)
		{
			fprintf(stderr, "initialise failed: %s\n", dev_error_str(*error));
			continue;
		}
		else 
			printf("\t\t- Initialised\n");

/*  
 * now export it to the outside world 
 */
		if (ds__method_finder(ds_list[i],DevMethodDevExport) (dev_list[i],ds_list[i],error) != DS_OK)
		{
			fprintf(stderr, "export failed: %s\n", dev_error_str(*error));
			continue;
		}
		else 
			printf("\t\t- Exported\n\n");
	}

	switch(n_exported)
	{
		case 0:	
			fprintf(stderr, "No devices exported - inst_verify server exiting\n");
			*error = 0;
			iret = DS_NOTOK;
			break;
		case 1:	
			printf("Inst_verify server running with 1 device exported\n");
			*error = 0;
			iret = DS_OK;
			break;
		default:
			printf("Inst_verify server running %d devices exported\n",n_exported);
			*error = 0;
			iret = DS_OK;
	}
	return(iret);
}
