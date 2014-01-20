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
 * File         : dc_mfrees.c
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

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

#include <dcP.h>

void usage(const char *cmd)
{
	fprintf(stderr, "usage : %s [options]\n", cmd);
	fprintf(stderr, " Give informations about the data buffer memory. It prints the\n");
	fprintf(stderr, " amount of free memory and the size of the largest free area.\n");
	fprintf(stderr, "        options : -h display this message\n");
	exit(-1);
}
/****************************************************************************
*                                                                           *
*		Code for dc_mfrees command                                  *
*                        ---------                                          *
*                                                                           *
*    Command rule : To give information about the data buffer memory.	    *
*		    This command prints the amout of free memory and the    *
*		    size of the largest free area.			    *
*                                                                           *
*    Synopsis : dc_mfrees 			               		    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	int 		shmid_alloc;
	char 		*shmadr_alloc;
	DevLong		error;
	unsigned int 	free,
			lfree;
	float 		free_f,
			lfree_f,
			data_f;
	int 		nb_area;
	char 		*tmp;
	unsigned int 	diff;
	DevLong		dat_size = 0;
	char 		hostna[HOST_NAME_LENGTH],
			dev_name[DEV_NAME_LENGTH];
	int 		alloc_size;
	db_resource 	res1[] = {
				{"data_size",D_LONG_TYPE, &dat_size},
			};
	int 		res1_size = sizeof(res1) / sizeof(db_resource);
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
		fprintf(stderr, "dc_mfrees : Can't import static database\n");
		fprintf(stderr, "dc_mfrees : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Build device name. It is a function of the host name */
	gethostname(hostna,sizeof(hostna));
	if ((tmp = strchr(hostna,'.')) != NULL) 
	{
		diff = (u_int)(tmp - hostna);
		hostna[diff] = 0;
	}
	strcpy(dev_name,"CLASS/DC/");
	strcat(dev_name,hostna);

/* Retrieve data collector memories size */
	if (db_getresource(dev_name,res1,res1_size,&error)) 
	{
		fprintf(stderr, "dc_mfrees : Can't retrieve resources\n");
		fprintf(stderr, "dc_mfrees : Error code : %d\n",error);
		exit(-1);
	}
	if (dat_size == 0) 
	{
		fprintf(stderr, "dc_mfrees : Resource dat_size not defined\n");
		fprintf(stderr, "dc_mfrees : Sorry but I exit !\n");
		exit(-1);
	}

/* Compute allocation area size */
	alloc_size = (int)(dat_size / 256);

/* Attach the alloc area (it's a shared memory area) to this process data area */
	if ((shmid_alloc = shmget((key_t)KEY_ALLOC,(size_t)alloc_size,0666)) == -1) 
	{
		if (errno == ENOENT)
			fprintf(stderr, "Allocation area does not exist in memory !\n");
		else 
		{
			fprintf(stderr, "dc_mfrees : Can't get the allocation table\n");
			fprintf(stderr, "dc_mfrees : Error code : %d\n",errno);
		}
		exit(-1);
	}

	if ((shmadr_alloc = (char *)shmat(shmid_alloc,(char *)0,0)) == (char *)-1) 
	{
		fprintf(stderr, "dc_mfrees : Can't attach to the allocation area shred memory segment\n");
		fprintf(stderr, "dc_mfrees : Error code : %d\n",errno);
		exit(-1);
	}

/* Call the memory function */
	if (dcmem_info(shmadr_alloc,alloc_size,&lfree,&free,&nb_area,&error)) 
	{
		fprintf(stderr, "dc_mfrees : Error %d during mem_info function !\n",error);
		exit(-1);
	}

/* Print the results */
	free_f = (float)free / 1024.0;
	lfree_f = (float)lfree / 1024.0;
	data_f = (float)dat_size / 1024.0;
	printf("Data buffer size :          %9.2f K-bytes\n",data_f);
	printf("Block size (in bytes) :         %d\n",BLOC_SIZE);
	printf("Current total free memory : %9.2f K-bytes\n",free_f);
	printf("Largest free area :         %9.2f K-bytes\n",lfree_f);
	printf("Number of free area(s) :         %d\n",nb_area);

	return 0;
}

