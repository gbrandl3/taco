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
 * File:	sema.c
 *
 * Description:
 *
 * Author(s):
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 12:54:15 $
 *
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "sema.h"
#include "boolean.h"

#ifdef _solaris
union semun {
	int		val;
	struct semid_ds *buf;
	ushort		*array;
};
#endif /* _solaris */

static struct sembuf sema[1];
extern int errno;

/**@ingroup dataportAPI
 * Create a new semaphore to protect the dataport shared memory	
 *
 * @param number  The number of semaphore in the set
 * @param key_num The key
 * @param create  A flag to inform the function that the sem. must be created  
 * 
 * @return the semaphore ID or -1 if it fails
 */
int define_sema (int number,int key_num,int create)
{
  	key_t key_value;
	int i,ret;
#ifdef sun
	union semun sem;
#endif
	ushort semarray[1];
	static int semid = -1;

	key_value = (key_t)key_num;

	if (create == TRUE)
	{
/* 
 * First test to see if a semaphore with the same key already exists 
 */
		semid = semget(key_value,number,0666);
		if (semid != -1)
		{
			fprintf(stderr,"Dataport : A semaphore with key %d already exists !\n",key_value);
			return(-1);
		}

/* 
 * Create the semaphore 
 */
		semid = semget(key_value,number,IPC_CREAT | 0666);

/* 
 * Set initial value of semaphore to 1 
 */
		semctl(semid,0,SETVAL,1);
  	}
  	else
		semid = semget(key_value,number,0666);

/* 
 * Impossible to get the semaphore ID 
 */
  	if (semid == -1)
  	{
#if defined (EBUG)
    		perror ("define_sema semget .. ");
#endif
    		return (-1);
  	}

/* 
 * Set semaphore to free (1) 
 */
  	if (create == TRUE)
  	{
    		semarray[0]=1;
#ifdef sun
		sem.array = semarray;
#endif
    		for (i=0;i<number;i++)
		{
#ifdef sun
     			if (semctl(semid,0,SETALL,sem)==-1)
#else
	     		if ((semctl(semid,0,SETALL,(ushort *)semarray))==-1)
#endif
     			{
#if defined (EBUG)
     				perror ("define_sema semctl .. ");
#endif
     				return (-1);
	     		}
	   	}
  	}
  	return (semid);
}

/**@ingroup dataportAPI
 * Remove a semaphore
 *
 * - The semaphore ID
 *
 * @return DS_OK if the semaphore is correctly deleted. 
 *    Otherwise, the function returns DS_NOTOK
 */
int delete_sema(int sph)
{
	int ret = semctl (sph,0,IPC_RMID,NULL);

	if (ret == -1)
	{
#if defined (EBUG)
		perror ("delete_sema semctl .. ");
#endif
	}
	return (ret);
}

/**@ingroup dataportAPI
 * Set the semaphore to occupied. If the semaphore is already set, the process 
 * waits with a time-out.
 *
 * @param sph The semapore ID
 *
 * @returns DS_OK if it is possible to take the sem. Otherwise, it returns DS_NOTOK.
 */
int get_sema (int sph)
{
	int ret;
	struct sembuf sops[1];

	sops[0].sem_num = 0;    /* Number of semaphore */
	sops[0].sem_op = -1;    /* P (get) */
	sops[0].sem_flg = 0;    /* wait if necessary */

/* 
 * Set an alarm 
 */
	alarm (DEADLOCK_WAIT);

/* 
 * try to take the semaphore 
 */
	ret = semop(sph,sops,1);
	if (ret == -1)
	{
		if (errno == EINTR)
			fprintf(stderr,"Dataport : Fails to take the semaphore, time-out!\n");
		else
			fprintf(stderr,"Dataport : Fails to take the semaphore\n");
		return(-1);
	}

/* 
 * Clear the alarm 
 */
	alarm (0);
	return(0);
}

/**@ingroup dataportAPI
 * Set the semaphore to occupied. If the semaphore is already set, the process 
 * returns immediately with DS_NOTOK
 *
 * @param sph The semapore ID
 * 
 * @return DS_OK if it is possible to take the sem. Otherwise, it returns DS_NOTOK.
 */
int get_sema_nowait (int sph)
{
	int ret;
	struct sembuf sops[1];

	sops[0].sem_num = 0;    /* Number of semaphore */
	sops[0].sem_op = -1;    /* P (get) */
	sops[0].sem_flg = IPC_NOWAIT;    /* DO NOT WAIT */

/* 
 * try to take the semaphore 
 */
   	printf("get_sema_nowait(): semctl(GETVAL) %d before\n", semctl(sph,0,GETVAL,NULL));

	ret = semop(sph,sops,1);

	printf("get_sema_nowait(): semctl(GETVAL) %d after\n",semctl(sph,0,GETVAL,NULL));

	return(ret);
}

/**@ingroup dataportAPI
 * release a semaphore
 *
 * @param sph The semaphore ID
 * 
 * @return DS_OK if the semaphore is correctly released. otherwise, it returns -1.
 */
int release_sema (int sph)
{
	int ret = 0;
	struct sembuf sops[1];

	sops[0].sem_num = 0;  /* Number of semaphore */
	sops[0].sem_op = 1;     /* V (release) */
	sops[0].sem_flg = 0;    /* wait if necessary */

	ret = semop(sph,sops,1);
	if (ret == -1)
	{
#if defined (EBUG)
		perror ("release_sema semop .. ");
#endif
		return(-1);
	}
	return (0);
}
