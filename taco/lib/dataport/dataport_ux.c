#include <stdio.h>
#include <dataport.h>
#include <boolean.h>
#include <shared.h>
#include <sema.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>

/**@ingroup dataportAPI
 * Build a unique key from the dataport name
 *
 * @param name The dataport name
 * 
 * @return the key or -1 if it fails
 */

int build_key (char *name)
{
	int key;

/* 
 * Test dataport name length 
 */
  	if (strlen(name)>25)
	{
		fprintf(stderr,"build_key(): dataport name too long (>25)\n");
		return (-1); /* name too long */
	}

/* 
 * Build key 
 */
	for (key = 0;*name;name++)
		key = HASH_BASE * key + *name;

  	return (key % HASH_MOD);
}

/**@ingroup dataportAPI
 * Create a complete dataport (shared memory and its associated semaphore)
 *
 * @param name 	The dataport name
 * @param size 	The shared memory size wanted by the user
 *
 * @return the datport pointer or NULL if it fails
 */
Dataport *CreateDataport(char *name, long size)
{
	Dataport 	*dp;
	int 		key,
			shmid,
			semid,
			full_size;

/* 
 * Build a key from the dataport name 
 */
	key = build_key(name);
	if (key == -1)
		return(NULL);

/* 
 * Get shared memory and semaphore 
 */
	full_size = size + sizeof(Dataport);
	dp = (Dataport *)get_shared(full_size,key,TRUE,&shmid);
	if (dp==(Dataport *)(-1))
		return (NULL);
	dp->shar = shmid;

/* 
 * Get semaphore to protect the shared memory 
 */
	semid = define_sema(1,key,TRUE);
	if (semid == -1)
		return(NULL);
	dp->sema = semid;

/* 
 * Init. dataport own parameters and leave function 
 */
	dp->creator_pid = getpid();
	dp->key = key;
	return (dp);
}

/**@ingroup dataportAPI
 * Link a process to an existing dataport
 *
 * @param name The dataport name
 * @param size The dataport shared memory size
 *
 * @return a pointer to the dataport shm or NULL if it fails
 */
Dataport *OpenDataport(char *name,long size)
{
	Dataport 	*dp;
	int 		key,
			shmid,
			semid;

/* 
 * Build the key from dataport name 
 */
	key = build_key(name);
	if (key == -1)
		return (NULL); /* Name not in lookup table */

/* 
 * Link the process to the dataport shared memory 
 */
	dp = (Dataport *)get_shared (size, key, FALSE, &shmid);
	if (dp==(Dataport *)(-1))
		return (NULL); 
	dp->shar = shmid;

/* 
 * Link the process to the dataport semaphore 
 */
	semid = define_sema (1, key, FALSE);
	if (semid == -1)
	{
		release_shared(dp);
		return(NULL);
	}
	dp->sema = semid;
	return (dp);
}

/**@ingroup dataportAPI
 * Take a dataport control
 *
 * @param dp Address of dataport shared memory
 *
 * @returns DS_OK when the process gets the dataport control or DS_NOTOK when it fails
 */
long AccessDataport(Dataport *dp)
{
	return(get_sema (dp->sema));
}

/**@ingroup dataportAPI
 * Take a dataport control
 *
 * @param dp Address of dataport shared memory
 *
 *    This function returns 0 when the process gets the dataport control or
 *	  immediately -1 when it fails
 */
long AccessDataportNoWait(Dataport *dp)
{
	return(get_sema_nowait (dp->sema));
}

/**@ingroup dataportAPI
 * Release a dataport control
 *
 * @param dp Address of dataport shared memory
 *
 * @return DS_OK if the dataport is released. Otherwise, the function returns DS_NOTOK.
 */
long ReleaseDataport(Dataport *dp)
{
	return(release_sema (dp->sema));
}

/**@ingroup dataportAPI
 * Return the free memory in a block and to return largest free area.
 *
 * @param dp Address of dataport shared memory
 * @param name The dataport name
 * - Address of the allocation table
 * - The buffer size
 * - The largest free area size
 * - The amount of free memory
 *
 * @return DS_NOTOK if one error occurs and the error code will be set
 */
long CloseDataport(Dataport *dp,char *name)
{
	int 	key,
		shar;

/* 
 * The caller is the dataport creator ? 
 */

/* JMC suppress that test on september 8 94 *******
*	if (dp->creator_pid != getpid())
*	{
*		fprintf(stderr,"This process did not create the dataport\n");
*		return (DS_NOTOK); 
*	}
*/
/* 
 * Build the key from dataport name 
 */
	key = build_key(name);
	if (key==-1)
	{
		fprintf(stderr,"The key for this name could not be found\n");
		return (DS_NOTOK); /* Name not in lookup table */
	}

/* 
 * Is it the correct key ? 
 */
	if (dp->key != key)
	{
		fprintf(stderr,"This dataport does not correspond to the name\n");
		return (DS_NOTOK); /* Name and dp to not correspond */
	}

/* 
 * Delete dataport semaphore 
 */
	delete_sema (dp->sema);

/* 
 * Release and delte the shared memory 
 */
	shar = dp->shar;
	release_shared((char *)dp);
	delete_shared (shar);

	return (DS_OK);
}
