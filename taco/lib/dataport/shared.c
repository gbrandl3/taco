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



/**@ingroup dataportAPI
 * Get a nes shared memory segment
 *
 * @param size 	 The shared memory size wanted by the caller
 * @param key_num The key
 * @param create  A flag to inform the function that the shared memory has to be created
 * @param shar 	 The shared memory identifier
 *
 * @return the shared memory address or -1 if it fails
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
 * @return DS_OK
 */
int release_shared (char *address)
{
	shmdt (address);
	return(0);
}



/**@ingroup dataportAPI
 * Delete a shared memory segment
 *
 * @param sid The shared memory identifier
 *
 * @return DS_OK if successful. Otherwise, it returns DS_NOTOK. 
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
