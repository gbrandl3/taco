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
 * File:	dataport_os9.c
 *
 * Description:
 *
 * Author(s):
 * 		$Author: jkrueger1 $
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2005-07-25 12:54:15 $
 *
 ***************************************************************************/

#include <stdio.h>
#include <signal.h>

#include <dataport.h>
#include <dataportP.h>
#include <sema.h>
#include <events.h>

#include <errno.h>
#ifdef _UCC
#include <errno_os9.h>
#endif /* _UCC */

static long InitialiseSemaphore();
static long DeinitialiseSemaphore();

/**
 * create a Dataport
 * 
 * @param name the name of Dataport
 * @param size the size of Dataport body
 *
 * @return  NULL on failure, pointer to Dataport otherwise.
 */
extern Dataport *CreateDataport(name, size)   char   *name;
                                             long   size;
{
	Dataport	*thisdataport;
	unsigned	fullsize;
	short		attr,
			perm;
	long		ret;

/* 
 * verify inputs 
 */
	if(strlen(name)>10)
	{
		fprintf(stderr, "can't crate dataport, name too long (max 10 char)\n");
		return(NULL);
	}
	if(size<=0)
		return(NULL);

/* 
 * open a data module for the port 
 */
	fullsize=size+sizeof(Dataport);
	attr=mkattrevs(MA_REENT,1);
	perm=   MP_OWNER_READ|MP_OWNER_WRITE| MP_GROUP_READ|MP_GROUP_WRITE| MP_WORLD_READ|MP_WORLD_WRITE;

	_mkdata_module(name,fullsize,attr,perm);
	thisdataport=(Dataport *)modlink(name,MT_ANY);

	if(thisdataport==(Dataport *)(-1))
	{
#ifdef EBUG
		fprintf(stderr,"modlink(%s) failed\n",name,fullsize);
#endif
		return(NULL);
	}

/* 
 * initialise values in dataport 
 *
 * initialise pid entry
 */
	thisdataport->pid=getpid();

/* 
 * initialise semaphore entry 
 */
	if(InitialiseSemaphore(name,&(thisdataport->sem))!=0)
	{
#ifdef EBUG
		fprintf(stderr,"InitialisedSemaphore() failed\n");
#endif
/* 
 * unlink datamodule 
 */
		munlink(thisdataport);
		return(NULL);
	}

	strcpy(thisdataport->semname,name);

/* 
 * return pointer to initialised dataport 
 */
	return(thisdataport);
}

/**
 * close Dataport, releasing system resources
 * 
 * @param thisdataport	Dataport to close
 * @param name		name of the dataport (used only in UNIX version)
 * 
 * @return  minus one on failure, zero otherwise.
 */
extern long CloseDataport(thisdataport, name)   Dataport *thisdataport;
						char     *name;
{
	long   	i,
		ret;

/* 
 * check process id 
 */
/* 
	if(getpid()!=thisdataport->pid)
		return(-1);
*/

/* 
 * delete the semaphore 
 */
	if(DeinitialiseSemaphore(thisdataport->semname,&(thisdataport->sem))==(-1))
	{
		fprintf(stderr,"DeinitialiseSemaphore() failed\n");
		return(-1);
	}

/* 
 * unlink the data module until it goes away 
 */
	do 
	{
		ret = munload(name,0);
	}while(ret != -1);
	return(0);
}

/**
 * link to a Dataport
 *
 * @param name name of Dataport
 * @param size not used
 * 
 * @return NULL on failure, pointer to Dataport otherwise.
 */
extern Dataport *OpenDataport(name, size)   char   *name;
				      long   size;
{
	Dataport *thisdataport;

	thisdataport=(Dataport *)modlink(name,MT_ANY);
	if(thisdataport==(Dataport *)(-1))
		return(NULL);
	return(thisdataport);
}

/**
 * request exclusive access to a Dataport
 * @param dp Dataport
 *
 * @return -1 on failure, 0 otherwise.
 */
extern long AccessDataport(dp)   Dataport   *dp;
{
	if(_ev_wait_t(dp->sem,0,32767,OS9_DEADLOCK_WAIT,0)==(-1))
		return(-1);
	return(0);
}


/**
 * relinquish exclusive access to Dataport
 * @param dp      Dataport
 * 
 * @return  -1 on failure, 0 otherwise.
 */
extern long ReleaseDataport(dp)   Dataport   *dp;
{
	int	ret = 0;

	ret=_ev_signal(dp->sem,0x8000);
	return (errno!=0) ? (-1) : (0);
}

/**
 * Create an event used as a semaphore
 * @param name      name of Dataport
 * @param sem       The ID of the created semaphore 
 * 
 * @return NULL if successful, -1 otherwise.
 */
static long InitialiseSemaphore(name,sem)   char         *name;
                                            Semaphore   *sem;
{
	int semid;

	semid=_ev_creat(0,-1,1,name);
	if (semid == -1)
		return(-1);
	*sem = semid;
	return(0);
}

/**
 * Delete the event
 *
 * @param name      name of Dataport
 * @param sem	 semaphore ID
 * 
 * @return  NULL on failure, pointer to Dataport otherwise.
 */
static long DeinitialiseSemaphore(name,sem)   char         *name;
                                              Semaphore   *sem;
{
	do
	{
		_ev_unlink(*sem);
	} while(_ev_delete(name)!=(-1));
	return(0);
}

