#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <shared.h>
#include <boolean.h>
#include <errno.h>

static int shmid = -1;



/****************************************************************************
*                                                                           *
*		Code for get_shared function           				                   *
*              ----------                                          			 *
*                                                                           *
*    Function rule : To get a nes shared memory segment							 *
*									    														    *
*    Argin : - The shared memory size wanted by the caller						 *
*            - The key																		 *
*				 - A flag to inform the function that the shared memory has to  *
*					be created																	 *
*                                                                           *
*    Argout : - The shared memory identifier											 *
*                                                                           *
*    This function returns the shared memory address or -1 if it fails		 *
*                                                                           *
****************************************************************************/

char *get_shared (size,key_num,create,shar)
int size;
int key_num;
int create;
int *shar;
{
   key_t key_value;
   char *ret_value;

   key_value = (key_t) key_num;

   printf("get_shared(): create %d TRUE %d\n",create,TRUE);

   if (create == TRUE) 
   {

/* First, test to see if a shared memory with the same key already exists */

   	shmid = shmget(key_value,size,0600);
    	if (shmid != -1)
    	{
			fprintf(stderr,"Dataport : Shared memory already registered with key %x\n",key_value);
			return((char *)(-1));
    	}

/* Get a shared memory segment */

    	shmid = shmget(key_value,size,IPC_CREAT | 0666);
   }
   else
    	shmid = shmget(key_value,size,0600);

/* Impossible to get the shared memory segment ! */

   if (shmid == -1)
   {
#if defined (EBUG)
    	perror ("get_shared shmget .. ");
#endif
    	return ((char *)(-1));
   }

/* Attach the process to the new shared memory segment */

   ret_value = (char *)shmat(shmid,(char *)0,0);
   if (ret_value == (char *)(-1))
   {
#if defined (EBUG)
    	perror ("get_shared shmat .. ");
#endif
   }
   *shar = shmid;

   return (ret_value);
}



/****************************************************************************
*                                                                           *
*		Code for release_shared function         				                   *
*              --------------                                       			 *
*                                                                           *
*    Function rule : To detach a process from a shared memory segment.		 *
*									    														    *
*    Argin : - A pointer to the shared memory segment							    *
*                                                                           *
*    Argout : No argout																	    *
*                                                                           *
*    This function always return 0														 *
*                                                                           *
****************************************************************************/

int release_shared (address)
char *address;
{
	shmdt (address);
	return(0);
}



/****************************************************************************
*                                                                           *
*		Code for delete_shared function          				                   *
*              -------------                                        			 *
*                                                                           *
*    Function rule : To delete a shared memory segment						    *
*									    														    *
*    Argin : - The shared memory identifier										    *
*                                                                           *
*    Argout : No argout																		 *
*                                                                           *
*    This function returns 0 if successful. Otherwise, it returns -1.		 *
*    be set								    													 *
*                                                                           *
****************************************************************************/

int delete_shared (sid)
int sid;
{
	int ret;

   ret = shmctl (sid,IPC_RMID,NULL);
   if (ret == -1)
   {
#if defined (EBUG)
		perror ("delete_sema semctl .. ");
#endif
   }
   return (ret);
}
