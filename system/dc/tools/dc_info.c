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
 * File         : dc_info.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author       :
 *                $Author: andy_gotz $
 *
 * Original     :
 *
 * Version      : $Revision: 1.7 $
 *
 * Date         : $Date: 2009-09-23 11:42:34 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#define _dc_h
#include <API.h>
#undef _dc_h
#include <dc.h>

#include <stdio.h>
#include <stdlib.h>

#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_IPC_H
#	include <sys/ipc.h>
#endif
#if HAVE_SYS_SHM_H
#	include <sys/shm.h>
#endif
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif

#include <errno.h>

#include <dcP.h>


db_resource res2[2];

void usage(const char *cmd)
{
	fprintf(stderr,"usage : %s [options]\n", cmd);
	fprintf(stderr, "  Display some main information on the data collector system.\n"); 
	fprintf(stderr, "  These information are :\n");
	fprintf(stderr, "   - The hosts where data collector are running\n");
	fprintf(stderr, "   - The number of read and write servers on every hosts\n");
	fprintf(stderr, "   - The number of devices register in the data collector\n");
	fprintf(stderr, "   - The number of devices register in the data collector for every domain\n");
	fprintf(stderr, "   - The data buffer size and the free memory in this data buffer\n");
	fprintf(stderr, "         options: -h display this message\n");
	exit(-1);
}
/****************************************************************************
*                                                                           *
*		Code for dc_info command                                    *
*                        -------                                            *
*                                                                           *
*    Command rule : To display some main information on the data collector  *
*		    system. These information are :			    *
*		  	- The hosts where data collector are running	    *
*			- The number of read and write servers on every     *
*			  hosts						    *
*			- The number of devices register in the data	    *
*			  collector					    *
*			- The number of devices register in the data        *
*			  collector for every domain			    *
*			- The data buffer size and the free memory in this  *
*			  data buffer					    *
*                                                                           *
*    Synopsis : dc_info                    				    *
*                                                                           *
****************************************************************************/
int main(argc,argv)
int argc;
char *argv[];
{
	char resname_rd[RES_NAME_LENGTH];
	char resname_wr[RES_NAME_LENGTH];
	int i,j,k,resu,found;
	int nb_dom_sum,nb_dom;
	int cptr = 0;
	DevLong rd_num,wr_num;
	float 			free_f,
				data_f;
	DevLong			error;
	servinf 		dcinf[10];
	dom_info 		dom_array[MAX_DOM];
	DevVarStringArray 	host_dc = {0, NULL};
	db_resource 		res1[] = {
					{"host",D_VAR_STRINGARR,&host_dc},
				};
	int 			res1_size = sizeof(res1) / sizeof(db_resource);
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



/* Miscellaneous init. */
	for (i= 0;i < MAX_DOM;i++) 
	{
		dom_array[i].dom_name[0] = 0;
		dom_array[i].dom_nb_dev = 0;
	}

/* Import static database */
	if (db_import(&error)) 
	{
		fprintf(stderr,"dc_info : Can't import static database\n");
		fprintf(stderr,"dc_info : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Retrieve data collector host name */
	if (db_getresource("CLASS/DC/1",res1,res1_size,&error)) 
	{
		fprintf(stderr,"dc_info : Can't retrieve resources\n");
		fprintf(stderr,"dc_info : Error code : %d\n",error);
		exit(-1);
	}
	if (host_dc.length == 0) 
	{
		fprintf(stderr,"dc_info : Resource host not defined\n");
		fprintf(stderr,"dc_info : Sorry, but I exit ! \n");
		exit(-1);
	}

/* Print dc host number */
	printf("The data collector is distributed on %d host(s)\n",host_dc.length);
	for (i = 0;i < host_dc.length;i++) 
	{
/* Retrieve server numbers for one host and print them */
		strcpy(resname_rd,host_dc.sequence[i]);
		strcat(resname_rd,"_rd");	
		strcpy(resname_wr,host_dc.sequence[i]);
		strcat(resname_wr,"_wr");	
		res2[0].resource_name = resname_rd;
		res2[0].resource_adr = &rd_num;
		res2[0].resource_type = D_LONG_TYPE;
		res2[1].resource_name = resname_wr;
		res2[1].resource_adr = &wr_num;
		res2[1].resource_type = D_LONG_TYPE;

		if (db_getresource("CLASS/DC/server_nb",res2,2,&error)) 
		{
			fprintf(stderr,"dc_info : Can't retrieve resources\n");
			fprintf(stderr,"dc_info : Error code : %d\n",error);
			exit(-1);
		}

		printf("      dc_host : %s\n",host_dc.sequence[i]);
		printf("           reading server(s) : %d\n",rd_num);
		printf("           writing server(s) : %d\n",wr_num);
	}
		
/* Ask the dc_info request on every host where a dc is running */
	for (i = 0;i < host_dc.length;i++) 
	{
		resu = dc_info(host_dc.sequence[i],&dcinf[i],&error);
		if (resu == -1) 
		{
			fprintf(stderr,"dc_info : Bad answer from %s\n",host_dc.sequence[i]);
			fprintf(stderr,"dc_info : Sorry, but I exit\n");
			exit(-1);
		}
	}

/* Print memory information */
	printf("\nData collector data buffer status\n");
	for (i = 0;i < host_dc.length;i++) 
	{
		printf("       dc_host : %s\n",host_dc.sequence[i]);
		free_f = (float)(dcinf[i].free_mem) / 1024.0;
		data_f = (float)(dcinf[i].mem) / 1024.0;
		printf("            Data buffer size :          %9.2f K-bytes\n",data_f);
		printf("            Current total free memory : %9.2f K-bytes\n",free_f);
	}

/* Compute how many devices in each ESRF domain */
	nb_dom_sum = 0;
	for (i = 0;i < host_dc.length;i++) 
	{
		nb_dom = dcinf[i].dom_nb;
		for (j = 0;j < nb_dom;j++) 
		{
			found = False;
			for (k = 0;k < nb_dom_sum;k++) 
			{
				if (strcmp(dcinf[i].dom_array[j].dom_name, dom_array[k].dom_name) == 0) 
				{
					dom_array[k].dom_nb_dev += dcinf[i].dom_array[j].dom_nb_dev;
					found = True;
					break;
				}
			}
			if (found == False) 
			{
				strcpy(dom_array[k].dom_name,dcinf[i].dom_array[j].dom_name);
				dom_array[k].dom_nb_dev = dcinf[i].dom_array[j].dom_nb_dev;
				nb_dom_sum++;
			}
		}
		cptr = cptr + dcinf[i].nb_dev;
	}
		
/* Display devices results */
	printf("\n%d devices are registered in the data collector\n",cptr);
	for (i = 0;i < nb_dom_sum;i++) 
	{
		k = strlen(dom_array[i].dom_name);
		for (j = 0;j < k;j++)
			dom_array[i].dom_name[j] = toupper(dom_array[i].dom_name[j]);
		printf("   %d for the %s domain\n",dom_array[i].dom_nb_dev,dom_array[i].dom_name);
	}
	return 0;
}
