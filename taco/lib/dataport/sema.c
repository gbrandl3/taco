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

/************************************************************************
*                                                                       *
*		Code for define_sema function  		                *
*                        -----------                  			*
*                                                                       *
*    Function rule : To create a new semaphore to protect the dataport  *
*						   shared memory	*
*								        *
*    Argin : - The number of semaphore in the set			*
*            - The key							*
*	     - A flag to inform the function that the sem. must be      *
*	       created  i						*
*                                                                       *
*    Argout : No argout							*
*                                                                       *
*    This function returns the semaphore ID or -1 if it fails	 	*	
*                                                                       *
*************************************************************************/

int define_sema (number,key_num,create)
int number;
int key_num;
int create;
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

/* First test to see if a semaphore with the same key already exists */

		semid = semget(key_value,number,0666);
		if (semid != -1)
    	{
			fprintf(stderr,"Dataport : A semaphore with key %d already exists !\n",key_value);
			return(-1);
    	}

/* Create the semaphore */

    	semid = semget(key_value,number,IPC_CREAT | 0666);

/* Set initial value of semaphore to 1 */

	semctl(semid,0,SETVAL,1);


  	}

  	else
    	semid = semget(key_value,number,0666);

/* Impossible to get the semaphore ID */

  	if (semid == -1)
  	{
#if defined (EBUG)
    	perror ("define_sema semget .. ");
#endif
    	return (-1);
  	}

/* Set semaphore to free (1) */

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

/*************************************************************************
*                                                                        *
*		Code for delete_sema function  		                 *
*                        -----------                   			 *
*                                                                        *
*    Function rule : To remove a semaphore				 *
*								         *
*    Argin : - The semaphore ID					 i	 *
*                                                                        *
*    Argout : No argout							 *
*                                                                        *
*    This function returns 0 if the semaphore is correctly deleted.      *
*    Otherwise, the function returns -1				 i	 *
*                                                                        *
**************************************************************************/

int delete_sema (sph)
int sph;
{
	int ret;

	ret = semctl (sph,0,IPC_RMID,NULL);
	if (ret == -1)
	{
#if defined (EBUG)
		perror ("delete_sema semctl .. ");
#endif
	}
   return (ret);
}

/*************************************************************************
*                                                                        *
*		Code for get_sema function             	                 *
*              --------                                 		 *
*                                                                        *
*    Function rule : To set the semaphore to occupied. If the semaphore  *
* 		     is already set, the process waits with a time-out.	 *
*								    	 *
*    Argin : - The semapore ID						 *
*                                                                        *
*    Argout : No argout							 *
*                                                                        *
*    This function returns 0 if it is possible to take the sem.          *
*    Otherwise, it returns -1.						 *
*                                                                        *
**************************************************************************/

int get_sema (sph)
int sph;
{
	int ret;
	struct sembuf sops[1];

   sops[0].sem_num = 0;    /* Number of semaphore */
   sops[0].sem_op = -1;    /* P (get) */
   sops[0].sem_flg = 0;    /* wait if necessary */

/* Set an alarm */

   alarm (DEADLOCK_WAIT);

/* try to take the semaphore */

   ret = semop(sph,sops,1);

   if (ret == -1)
   {
		if (errno == EINTR)
			fprintf(stderr,"Dataport : Fails to take the semaphore, time-out!\n");
		else
			fprintf(stderr,"Dataport : Fails to take the semaphore\n");
		return(-1);
   }

/* Clear the alarm */

   alarm (0);

   return(0);
}

/*************************************************************************
*                                                                        *
*		Code for get_sema_nowait function             	                 *
*              --------                                 		 *
*                                                                        *
*    Function rule : To set the semaphore to occupied. If the semaphore  *
* 		     is already set, the process returns immediately with -1
*								    	 *
*    Argin : - The semapore ID						 *
*                                                                        *
*    Argout : No argout							 *
*                                                                        *
*    This function returns 0 if it is possible to take the sem.          *
*    Otherwise, it returns -1.						 *
*                                                                        *
**************************************************************************/

int get_sema_nowait (sph)
int sph;
{
	int ret;
	struct sembuf sops[1];

   sops[0].sem_num = 0;    /* Number of semaphore */
   sops[0].sem_op = -1;    /* P (get) */
   sops[0].sem_flg = IPC_NOWAIT;    /* DO NOT WAIT */

/* try to take the semaphore */

   printf("get_sema_nowait(): semctl(GETVAL) %d before\n",semctl(sph,0,GETVAL,NULL));

   ret = semop(sph,sops,1);

   printf("get_sema_nowait(): semctl(GETVAL) %d after\n",semctl(sph,0,GETVAL,NULL));

   return(ret);
}

/***************************************************************************
*                                                                          *
*		Code for release_sema function    	                   *
*                        ------------                  			   *
*                                                                          *
*    Function rule : To release a semaphore				   *
*									   *
*    Argin : - The semaphore ID						   *
*                                                                          *
*    Argout : No argout							   *
*                                                                          *
*    This function returns 0 if the semaphore is correctly released.       *
*    otherwise, it returns -1.						   *
*                                                                          *
****************************************************************************/

int release_sema (sph)
int sph;
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
