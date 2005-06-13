#include "config.h"
#include <API.h>
#include <stdio.h>

#ifdef sun
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include <sys/shm.h>
#endif

#include <sys/sem.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <dcP.h>

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
	DevString	passwd = NULL;
	unsigned int diff;
	DevLong 	dev_num,
			dat_size,
			cell_num;
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

/* Argument test */
	if (argc != 2) 
	{
		fprintf(stderr, "dc_dels usage : dc_dels <system password>\n");
		exit(-1);
	}

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
	l1 = strlen(argv[1]);
	if (passwd == NULL) 
		l2 = 0;
	else
		l2 = strlen(passwd);

	if ((l1 != l2) || strcmp(argv[1],passwd)) 
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
