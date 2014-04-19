/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File         : dc_mfree.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author       :
 *                $Author: jkrueger1 $
 *
 * Original     :
 *
 * Version      : $Revision: 1.6 $
 *
 * Date         : $Date: 2008-04-06 09:07:51 $
 *
 */

#ifdef HAVE_CCNFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>


void usage(const char *cmd)
{
	fprintf(stderr,"usage : %s [options]\n", cmd);
	fprintf(stderr, " Display information about the data collector data buffer.\n");
	fprintf(stderr, " The data collector system could be distributed on several hosts.\n"); 
	fprintf(stderr, " This command will retrieve on which host this system is running\n"); 
	fprintf(stderr, " (with resources) and will run a local command (with remote shell) on each host.\n");
	fprintf(stderr, "         options : -h display this message\n");
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for dc_mfree command                                   *
*                        --------                                           *
*                                                                           *
*    Command rule : To display information about the data collector data    *
*		    buffer.						    *
*		    The data collector system could be distributed on       *
*		    several hosts. This command will retrieve on which host *
*		    this system is running (with resources) and will run a  *
*		    local command (with remote shell) on each host          *
*                                                                           *
*    Synopsis : dc_mfree		    				    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	DevLong			error;
	char 			*tmp,
				*rsh,
				*net;
	unsigned int 		diff;
	char 			hostna[HOST_NAME_LENGTH],
				dev_name[DEV_NAME_LENGTH],
				psd_name[PROC_NAME_LENGTH],
				cmd[160];
	int 			i;
	DevVarStringArray 	host_dc = {0, NULL};
	DevString 		dc_path,
				s_dc_path,
				dc_login;
	db_resource 		res1[] = {
					{"host",D_VAR_STRINGARR,&host_dc},
			     	},
				res2[] = {
					{"path",D_STRING_TYPE,&dc_path},
					{"s_path",D_STRING_TYPE,&s_dc_path},
					{"login",D_STRING_TYPE,&dc_login},
				};
	int 			res1_size = sizeof(res1) / sizeof(db_resource);
	int 			res2_size = sizeof(res2) / sizeof(db_resource);
        int             c;
        extern int      optind,
                        optopt;

/* Argument test */
       while ((c = getopt(argc, argv, "h")) != -1)
                switch(c)
                {
                        case 'h' :
                        case '?' :
                                usage(argv[0]);
                }
        if (optind != argc)
                usage(argv[0]);


/* Import static database */
	if (db_import(&error)) 
	{
		fprintf(stderr,"dc_mfree : Can't import static database\n");
		fprintf(stderr,"dc_mfree : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Retrieve on which host the data collector is running */
	if (db_getresource("CLASS/DC/1",res1,res1_size,&error)) 
	{
		fprintf(stderr,"dc_mfree : Can't retrieve resources\n");
		fprintf(stderr,"dc_mfree : Error code : %d\n",error);
		exit(-1);
	}
	if (host_dc.length == 0) 
	{
		fprintf(stderr,"dc_mfree : Resource host not defined\n");
		fprintf(stderr,"dc_mfree : Sorry, but I exit !\n");
		exit(-1);
	}

/* Get the NETHOST environment variable (No need to test if NETHOST is
   defined because this has been done during the static database import
   stuff */

	net = getenv("NETHOST");
	rsh = getenv("TACO_RSH");
	if (rsh == NULL)
#if defined(sun) || defined(linux)
		rsh = "rsh";
#else
		rsh = "remsh";
#endif /* sun || linux */

/* Call the dc_dels command on each dc host */
	for (i = 0;i < host_dc.length;i++) 
	{
/* Retrieve from database the primary path for dc tools */
		psd_name[0] = 0;
		strcpy(psd_name,"class/dc/");
		strcat(psd_name,host_dc.sequence[i]);

		dc_path = NULL;
		s_dc_path = NULL;
		dc_login = NULL;

		if(db_getresource(psd_name,res2,res2_size,&error)) 
		{
			fprintf(stderr,"dc_mfree : Can't retrieve primary dc tools path for %s\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_mfree : Error code : %d\n",error);
			exit(-1);
		}

		if (dc_path == NULL || s_dc_path == NULL || dc_login == NULL) 
		{
			fprintf(stderr,"dc_mfree : Path or login for dc_tools on %s not defined\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_mfree : Sorry, but I exit\n");
			exit(-1);
		}

/* Build the string which is the remsh command */
		snprintf(cmd, sizeof(cmd), 
			"%s %s -l %s \"echo \\\"NETHOST=%s;export NETHOST; %s/%s/dc_mfrees \\\" | sh \"", 
			rsh,host_dc.sequence[i], dc_login, net, dc_path, s_dc_path);

#ifdef DEBUG
		printf("Command string : %s\n",cmd);
#endif /* DEBUG */

		printf("Data collector data buffer status on %s \n\n", host_dc.sequence[i]);
		if(system(cmd))
			printf("failed\n");
	}
	return 0;
}
