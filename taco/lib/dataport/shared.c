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
 * File:	shared.c
 *
 * Description:
 *
 * Author(s):	
 *		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.7 $
 *
 * Date:	$Date: 2008-10-23 05:25:05 $
 *
 ****************************************************************************/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include <string.h>
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
#include <errno.h>
#include "shared.h"
#include "boolean.h"

static int shmid = -1;



/**@ingroup dataportAPI
 * Get a new shared memory segment
 *
 * @param size 	 The shared memory size wanted by the caller
 * @param key_num The key
 * @param create  A flag to inform the function that the shared memory has to be created
 * @param shar 	 The shared memory identifier
 *
 * @return the shared memory address 
 * @retval -1 if it fails
 */
char *get_shared (int size,int key_num,int create,int *shar)
{
	key_t key_value;
	char *ret_value;

	key_value = (key_t) key_num;

#ifdef EBUG
	printf("get_shared(): create %d TRUE %d\n",create,TRUE);
#endif

	if (create == TRUE) 
	{
/* 
 * First, test to see if a shared memory with the same key already exists 
 */
		shmid = shmget(key_value,size,0600);
		if (shmid != -1)
		{
			fprintf(stderr,"Dataport : Shared memory already registered with key %x\n",key_value);
			return((char *)(-1));
		}

/* 
 * Get a shared memory segment 
 */
		shmid = shmget(key_value,size,IPC_CREAT | 0666);
	}
	else
		shmid = shmget(key_value,size,0600);

/* 
 * Impossible to get the shared memory segment ! 
 */
	if (shmid == -1)
	{
#if defined (EBUG)
		perror ("get_shared shmget .. ");
#endif
    		return ((char *)(-1));
	}

/* 
 * Attach the process to the new shared memory segment 
 */
	ret_value = (char *)shmat(shmid,(char *)0,0);
#if defined (EBUG)
	if (ret_value == (char *)(-1))
		perror ("get_shared shmat .. ");
#endif
	*shar = shmid;
	return (ret_value);
}



/**@ingroup dataportAPI
 * Detach a process from a shared memory segment.
 *
 * @param address A pointer to the shared memory segment
 * 
 * @retval DS_OK
 */
int release_shared (char *address)
{
	shmdt (address);
	return (0);
}



/**@ingroup dataportAPI
 * Delete a shared memory segment
 *
 * @param sid The shared memory identifier
 *
 * @retval DS_OK if successful. 
 * @retval DS_NOTOK otherwise
 */
int delete_shared (int sid)
{
	int ret = shmctl (sid,IPC_RMID,NULL);
#if defined (EBUG)
	if (ret == -1)
		perror ("delete_sema semctl .. ");
#endif
	return (ret);
}
