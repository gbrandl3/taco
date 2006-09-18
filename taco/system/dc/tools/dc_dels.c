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
 * File         : dc_dels.c
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
 * Date         : $Date: 2006-09-18 21:50:15 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <stdio.h>

#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_IPC_H
#	include <sys/ipc.h>
#endif
#if HAVE_SYS_SHM_H
#	include <sys/shm.h>
#endif
#if HAVE_SYS_SEM_H
#	include <sys/sem.h>
#endif
#include <string.h>
#include <errno.h>
#if HAVE_UNISTD_H
#	include <unistd.h>
#endif

#include <dcP.h>

void usage(const char *cmd)
{
	fprintf(stderr, "usage : %s [options] <system password>\n", cmd);
	fprintf(stderr, " Remove the data collector system from memory.\n");
	fprintf(stderr, "       options : -h display this message\n");
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for dc_dels command                                    *
*                        -------                                            *
*                                                                           *
*    Command rule : To remove the data collector system from memory  	    *
*                                                                           *
*    Synopsis : dc_dels <password> 					    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	int shmid_datbuf,shmid_ptr,shmid_alloc;
	int semid,semid1;
	int nb_tot;
	int 		l1,
			l2;
	long 		error;
	char 		*tmp;
	DevString	passwd = "setup";
	unsigned int diff;
	DevLong 	dev_num=100,
			dat_size=252488,
			cell_num=50;
	char 		hostna[HOST_NAME_LENGTH],
			dev_name[DEV_NAME_LENGTH];
	int 		ptr_size,
			alloc_size;
	db_resource 	res1[] = {
				{"dev_number",D_LONG_TYPE, &dev_num},
				{"cellar_number",D_LONG_TYPE, &cell_num},
				{"data_size",D_LONG_TYPE, &dat_size},
				{"password",D_STRING_TYPE, &passwd},
		     	};
	int res1_size = sizeof(res1) / sizeof(db_resource);
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
        if (optind != argc - 1)
                usage(argv[0]);

/* Import static database */
	if (db_import(&error)) 
	{
		fprintf(stderr, "dc_dels : Can't import static database\n");
		fprintf(stderr, "dc_dels : Check your NETHOST environment variable\n");
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
		printf("dc_dels : Can't retrieve resources\n");
		printf("dc_dels : Error code : %d\n",error);
		exit(-1);
	}

/* Compute pointers area size and allocation table size */
	nb_tot = dev_num + cell_num;
	ptr_size = (int)((nb_tot * sizeof(dc_dev_param)) + (nb_tot * sizeof(int_level)));
	alloc_size = (int)(dat_size / 256);

/* Check password validity */
	l1 = strlen(argv[optind]);
	if (passwd == NULL) 
		l2 = 0;
	else
		l2 = strlen(passwd);

	if ((l1 != l2) || strcmp(argv[optind],passwd)) 
	{
		fprintf(stderr, "dc_dels : Bad password\n");
		fprintf(stderr, "dc_dels : Sorry but I exit\n");
		exit(-1);
	}

/* Remove the data buffer from memory (it is a shared memory segment) */
	if ((shmid_datbuf = shmget((key_t)KEY_DATBUF,(size_t)dat_size,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't get the data buffer shm id\n");
		fprintf(stderr, "dc_dels : Error code : %d\n",errno);
	}
	else if (shmctl(shmid_datbuf,IPC_RMID,0) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't remove the data buffer from memory\n");
		fprintf(stderr, "dc_dels : Error code : %d\n",errno);
	}

/* Remove the pointers buffer from memory */
	if ((shmid_ptr = shmget((key_t)KEY_PTR,(size_t)ptr_size,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't get the pointers buffer shm id\n");
		fprintf(stderr, "dc_dels : Error code : %d\n",errno);
	}
	
	else if (shmctl(shmid_ptr,IPC_RMID,0) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't remove the ptr buffer from memory\n");
		fprintf(stderr, "dc_dels : Error code : %d\n",errno);
	}

/* Remove the allocation table from memory */
	if ((shmid_alloc = shmget((key_t)KEY_ALLOC,(size_t)alloc_size,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't get the allocation table shm id\n");
		fprintf(stderr, "dc_dels : Error code : %d\n",errno);
	}
	else if (shmctl(shmid_alloc,IPC_RMID,0) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't remove the allocation table from memory\n");
		fprintf(stderr, "dc_dels : Error code : %d\n",errno);
	}

/* Remove the allocation area semaphore */
	if ((semid = semget(SEM_KEY,1,0666)) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't get the semaphore id\n");
		perror("dc_dels ");
	}
	else if (semctl(semid,0,IPC_RMID,0) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't remove semaphore from memory\n");
		perror("dc_dels ");
	}

/* Remove the pointer memory semaphores */
	if ((semid1 = semget(SEMPTR_KEY,2,0666)) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't get the pointer semaphores id\n");
		perror("dc_dels ");
	}
	else if (semctl(semid1,0,IPC_RMID,0) == -1) 
	{
		fprintf(stderr, "dc_dels : Can't remove pointer semaphores from memory\n");
		perror("dc_dels ");
	}
	return;
}
