#include <stdio.h>
#include <dataport.h>
#include <boolean.h>
#include <shared.h>
#include <sema.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>


/****************************************************************************
*
*		Code for build_key function 
*               --------------------------- 
* 
*    Function rule : To build a unique key from the dataport name
*
*    Argin : - The dataport name
* 
*    Argout : No argout	
* 
*    This function returns the key or -1 if it fails
*
****************************************************************************/

int build_key (name)
char *name;
{
	int key;

/* Test dataport name length */

  	if (strlen(name)>25)
	{
		fprintf(stderr,"build_key(): dataport name too long (>25)\n");
		return (-1); /* name too long */
	}

/* Build key */

	for (key = 0;*name;name++)
	{
		key = HASH_BASE * key + *name;
	}

  	return (key % HASH_MOD);
}

/****************************************************************************
*
*		Code for CreateDataport function
*               --------------------------------
*
*    Function rule : To create a complete dataport (shared memory and its 
*							associated semaphore)
*
*    Argin : - The dataport name
*            - The shared memory size wanted by the user
*
*    Argout : No argout
*
*    This function returns the datport pointer or NULL if it fails
*
****************************************************************************/

Dataport *CreateDataport(name,size)
char *name;
long size;
{
   int key;
   Dataport *dp;
   int shmid;
	int semid;
	int full_size;

/* Build a key from the dataport name */

   key = build_key(name);
   if (key == -1)
		return(NULL);

/* Get shared memory and semaphore */

	full_size = size + sizeof(Dataport);
   dp = (Dataport *)get_shared(full_size,key,TRUE,&shmid);
   if (dp==(Dataport *)(-1))
   	return (NULL);
   dp->shar = shmid;

/* Get semaphore to protect the shared memory */

   semid = define_sema(1,key,TRUE);
	if (semid == -1)
		return(NULL);
	dp->sema = semid;

/* Init. dataport own parameters and leave function */

   dp->creator_pid = getpid();
   dp->key = key;
   return (dp);
}

/****************************************************************************
*
*		Code for OpenDataport function
*               ------------------------------
*
*    Function rule : To link a process to an existing dataport
*
*    Argin : - The dataport name
*            - The dataport shared memory size
*
*    Argout : No argout
*
*    This function returns a pointer to the dataport shm or NULL if it
*	  fails
*
****************************************************************************/

Dataport *OpenDataport(name,size)
char *name;
long size;
{
   int key;
   Dataport *dp;
   int shmid;
	int semid;

/* Build the key from dataport name */

   key = build_key(name);
   if (key == -1)
   	return (NULL); /* Name not in lookup table */

/* Link the process to the dataport shared memory */

   dp = (Dataport *)get_shared (size, key, FALSE, &shmid);
   if (dp==(Dataport *)(-1))
    	return (NULL); 
   dp->shar = shmid;

/* Link the process to the dataport semaphore */

   semid = define_sema (1, key, FALSE);
	if (semid == -1)
   {
		release_shared(dp);
		return(NULL);
   }
   dp->sema = semid;
   return (dp);
}

/****************************************************************************
*
*		Code for AccessDataport function
*               --------------------------------
*
*    Function rule : To take a dataport control
*
*    Argin : - Address of dataport shared memory
*
*    Argout : No argout
*
*    This function returns 0 when the process gets the dataport control or
*	  -1 when it fails
*
****************************************************************************/

long AccessDataport(dp)
Dataport *dp;
{
   return(get_sema (dp->sema));
}

/****************************************************************************
*
*		Code for AccessDataportNoWait function
*               --------------------------------------
*
*    Function rule : To take a dataport control
*
*    Argin : - Address of dataport shared memory
*
*    Argout : No argout
*
*    This function returns 0 when the process gets the dataport control or
*	  immediately -1 when it fails
*
****************************************************************************/

long AccessDataportNoWait(dp)
Dataport *dp;
{
   return(get_sema_nowait (dp->sema));
}

/****************************************************************************
*
*		Code for ReleaseDataport function
*               ---------------------------------
*
*    Function rule : To release a dataport control
*
*    Argin : - Address of dataport shared memory
*
*    Argout : No argout
*
*    This function returns 0 if the dataport is released. Otherwise, the
*	  function returns -1.
*
****************************************************************************/

long ReleaseDataport(dp)
Dataport *dp;
{
   return(release_sema (dp->sema));
}

/****************************************************************************
*
*		Code for CloseDataport function
*               -------------------------------
*
*    Function rule : To return the free memory in a block and to return
*		     				largest free area.
*
*    Argin : - Address of the allocation table
*            - The buffer size
*
*    Argout : - The largest free area size
*	      	  - The amount of free memory
*
*    This function returns -1 if one error occurs and the error code will
*    be set
*
****************************************************************************/

long CloseDataport(dp,name)
Dataport *dp;
char *name;
{
   int key,shar;

/* The caller is the dataport creator ? */

/* JMC suppress that test on september 8 94 *******
*   if (dp->creator_pid != getpid())
*   {
*    	fprintf(stderr,"This process did not create the dataport\n");
*    	return (-1); 
*   }
*/
/* Build the key from dataport name */

   key = build_key(name);
   if (key==-1)
   {
    	fprintf(stderr,"The key for this name could not be found\n");
    	return (-1); /* Name not in lookup table */
   }

/* Is it the correct key ? */

   if (dp->key != key)
   {
    	fprintf(stderr,"This dataport does not correspond to the name\n");
    	return (-1); /* Name and dp to not correspond */
   }

/* Delete dataport semaphore */

   delete_sema (dp->sema);

/* Release and delte the shared memory */

   shar = dp->shar;
   release_shared((char *)dp);
   delete_shared (shar);

   return (0);
}
