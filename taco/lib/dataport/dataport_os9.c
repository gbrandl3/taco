
/***************************************************************************
 *
 * $Source: /home/jkrueger1/sources/taco/backup/taco/lib/dataport/dataport_os9.c,v $
 *
 * $Author: jkrueger1 $
 * $Date: 2003-05-21 16:19:00 $
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
 * @param name name of Dataport
 * @param size size of Dataport body
 *
 * @return  NULL on failure, pointer to Dataport otherwise.
 */
extern Dataport *CreateDataport(name,size)   char   *name;
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



/************************************************************************
 Function   :  extern long CloseDataport()

 Description:  close Dataport, releasing system resources

 Arg(s) In  :  thisdataport   Dataport to close
	       name	      name of the dataport (used only in UNIX version)

 Return(s)  :  minus one on failure, zero otherwise.
*************************************************************************/
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



/************************************************************************
 Function   :  extern Dataport *OpenDataport()

 Description:  link to a Dataport

 Arg(s) In  :  name      name of Dataport
	       size	 not used
 Return(s)  :  NULL on failure, pointer to Dataport otherwise.

*************************************************************************/
extern Dataport *OpenDataport(name, size)   char   *name;
				      long   size;
{
	Dataport *thisdataport;

	thisdataport=(Dataport *)modlink(name,MT_ANY);
	if(thisdataport==(Dataport *)(-1))
		return(NULL);
	return(thisdataport);
}



/************************************************************************
 Function   :  extern long AccessDataport()

 Description:  request exclusive access to a Dataport

 Arg(s) In  :  dp         Dataport

 Return(s)  :  -1 on failure, 0 otherwise.
*************************************************************************/
extern long AccessDataport(dp)   Dataport   *dp;
{
	if(_ev_wait_t(dp->sem,0,32767,OS9_DEADLOCK_WAIT,0)==(-1))
		return(-1);
	return(0);
}



/************************************************************************
 Function   :  extern long ReleaseDataport()

 Description:  relinquish exclusive access to Dataport

 Arg(s) In  :  dp      Dataport

 Return(s)  :  -1 on failure, 0 otherwise.
*************************************************************************/
extern long ReleaseDataport(dp)   Dataport   *dp;
{
	int	ret = 0;

	ret=_ev_signal(dp->sem,0x8000);
	return (errno!=0) ? (-1) : (0);
}



/************************************************************************
 Function   :  long InitialiseSemaphore()

 Description:  Create an event used as a semaphore

 Arg(s) In  :  name      name of Dataport

 Arg(s) Out :  sem       The semaphore ID

 Return(s)  :  NULL if successful, -1 otherwise.
*************************************************************************/
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



/************************************************************************
 Function   :  long DeinitialiseSemaphore()

 Description:  Delete the event

 Arg(s) In  :  name      name of Dataport
	       sem	 semaphore ID

 Return(s)  :  NULL on failure, pointer to Dataport otherwise.
*************************************************************************/
static long DeinitialiseSemaphore(name,sem)   char         *name;
                                              Semaphore   *sem;
{
	do
	{
		_ev_unlink(*sem);
	} while(_ev_delete(name)!=(-1));
	return(0);
}

