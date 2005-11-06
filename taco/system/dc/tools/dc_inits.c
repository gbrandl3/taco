#include "config.h"
#include <API.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <dcP.h>

#ifdef linux
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif 
#endif /* linux */

/* The shared memory segment id and address */
int 	shmid_ptr,
	shmid_datbuf,
	shmid_alloc;
char 	*shmadr_datbuf,
	*shmadr_ptr,
	*shmadr_alloc;

void from_scratch(int ptr_size, int alloc_size, int dat_size, int nb_tot);
void get_shm_buffer(int ptr_size, int alloc_size, int dat_size, int nb_tot);
void create_sem(void);

void usage(const char *cmd)
{
	fprintf(stderr, "usage : %s [options]\n", cmd);
	fprintf(stderr, "  Initialize the data collector system. This initialisation\n");
	fprintf(stderr, "  can be done from scratch or from a previously backuped database\n");
	fprintf(stderr, "          options: -h display this message\n");
	exit(-1);
}
/****************************************************************************
*                                                                           *
*		Code for dc_inits command                                   *
*                        --------                                           *
*                                                                           *
*    Command rule : To initialize the data collector system.	            *
*		    This initialisation can be done from scrach or from     *
* 		    a previously backuped database                          *
*                                                                           *
*    Synopsis : dc_inits						    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	int 		ds;
	long 		error;
	char 		*tmp;
	unsigned int 	diff;
/*
 * changed the default values from zero to the values I have on id11
 */
	DevLong 	dev_num = 100,
			dat_size = 252488,
			cell_num = 50;
	int 		nb_tot;
	char 		hostna[HOST_NAME_LENGTH],
			dev_name[DEV_NAME_LENGTH];
	int 		ptr_size,
			alloc_size;
	db_resource 	res1[] = {
				{"dev_number", D_LONG_TYPE, &dev_num},
				{"cellar_number", D_LONG_TYPE, &cell_num},
				{"data_size", D_LONG_TYPE, &dat_size},
		    	};
	int 		res1_size = sizeof(res1) / sizeof(db_resource);
        int             c;
        extern int      optind,
                        optopt;

/* Argument test */
       while ((c = getopt(argc, argv, "h")) != -1)
                switch(c)
                {
                        case 'h' :
                        case '?' :
                                usage(argv[0]);
                }
        if (optind != argc)
                usage(argv[0]);


/* Import static database */

	if (db_import(&error)) 
	{
		fprintf(stderr, "dc_inits : Can't import static database\n");
		fprintf(stderr, "dc_inits : Check your NETHOST environment variable\n");
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
		fprintf(stderr, "dc_inits : Can't retrieve resources\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",error);
		exit(-1);
	}
	if (dev_num == 0 || dat_size == 0 || cell_num == 0) 
	{
		fprintf(stderr, "dc_inits : Resource dev_number, cellar_number or data_size not defined\n");
		fprintf(stderr, "dc_inits : Sorry but I exit !\n");
		exit(-1);
	}

/* Compute pointers area size and allocation table size */
	nb_tot = dev_num + cell_num;
	ptr_size = (int)((nb_tot * sizeof(dc_dev_param)) + (nb_tot * sizeof(int_level)));
	alloc_size = (int)(dat_size / 256);

/* Create memories and semaphores */
	from_scratch(ptr_size,alloc_size,dat_size,nb_tot);
	return 0;
}



/**
 * To create the database part of the data collector from scratch. 
 * 
 * @param ptr_size 	The pointer shared memory segment size
 * @param alloc_size	The allocation table size
 * @param dat_size 	The data buffer size
 * @param nb_tot	The sum of device number and element in the cellar table
 * 
 */
void from_scratch(int ptr_size, int alloc_size, int dat_size, int nb_tot)
{
#ifdef DEBUG
	fprintf(stderr, "From scratch");
#endif /* DEBUG */

/* Reserve buffers in shared memory */
	get_shm_buffer(ptr_size, alloc_size, dat_size, nb_tot);

/* Create the semaphore */
	create_sem();
	return;
}

/**
 * To reserve and clear the three buffers in three 
 * different shared memory segments. Thses three buffers are: 
 *	- The data buffer
 *	- The pointers buffer
 *	- The allocation area
 *
 * @param ptr_size	The pointers shared memory segment size
 * @param alloc_size	The allocation table size
 * @param dat_size	The data buffer size
 * @param nb_tot	The number of devices plus the number of elements in the cellar area.
 *
 */

void get_shm_buffer(int ptr_size, int alloc_size, int dat_size, int nb_tot)
{
	int 		i,
			*tmp_ptr,
			tmp_size;
	dc_dev_param 	*array;

/* Reserve the data buffer in a shared memory segment */
	if ((shmid_datbuf = shmget((key_t)KEY_DATBUF,(size_t)dat_size,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_inits : Can't get the data buffer\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",errno);
		exit(-1);
	}
#ifdef DEBUG
	fprintf(stderr, "Data buffer shm id : %d\n",shmid_datbuf);
#endif /* DEBUG */
	if ((shmadr_datbuf = (char *)shmat(shmid_datbuf,(char *)0,0)) == (char *)-1) 
	{
		fprintf(stderr, "dc_inits : Can't attach to the data buffer shred memory segment\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",errno);
		exit(-1);
	}

/* Reserve the pointers buffer in shared memory */
	if ((shmid_ptr = shmget((key_t)KEY_PTR,(size_t)ptr_size,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_inits : Can't get the pointers buffer\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",errno);
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		exit(-1);
	}
#ifdef DEBUG
	fprintf(stderr, "Pointer buffer shm id : %d\n",shmid_ptr);
#endif /* DEBUG */
	
	if ((shmadr_ptr = (char *)shmat(shmid_ptr,(char *)0,0)) == (char *)-1) 
	{
		fprintf(stderr, "dc_inits : Can't attach to the pointers buffer shred memory segment\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",errno);
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmctl(shmid_ptr,IPC_RMID,0);
		exit(-1);
	}

/* Reserve the allocation table also in shared memory */
	if ((shmid_alloc = shmget((key_t)KEY_ALLOC,(size_t)alloc_size,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_inits : Can't get the allocation table\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",errno);
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		exit(-1);
	}
#ifdef DEBUG
	printf("Allocation table shm id : %d\n",shmid_alloc);
#endif /* DEBUG */

	if ((shmadr_alloc = (char *)shmat(shmid_alloc,(char *)0,0)) == (char *)-1) 
	{
		fprintf(stderr, "dc_inits : Can't attach to the allocation area shred memory segment\n");
		fprintf(stderr, "dc_inits : Error code : %d\n",errno);
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		shmctl(shmid_alloc,IPC_RMID,0);
		exit(-1);
	}

/* Clear the allocation area */

	tmp_size = alloc_size >> 2;
	tmp_ptr = (int *)shmadr_alloc;
	for (i = 0;i < tmp_size;i++) 
	{
		*tmp_ptr = 0;
		tmp_ptr++;
	}

/* Clear the pointers table and set all the next records value to 1 */

	for (i = 0;i < ptr_size;i++)
		shmadr_ptr[i] = 0;
	array = (dc_dev_param *)shmadr_ptr;
	for (i = 0;i < nb_tot;i++)
		array[i].next_rec = 1;
	return;
}



/**
 * To create and initialise the semaphore used to protect the allocation area.	
 */
void create_sem(void)
{
	int semid;
#ifdef linux
	union semun semval;
#else
	int semval;
#endif /* linux */
	int semid1;

/* Get the semaphore to protect the allocation area */

	if ((semid = semget((key_t)SEM_KEY,1,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_inits : Can't get the semaphore\n");
		perror("dc_inits ");
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		shmdt(shmadr_alloc);
		shmctl(shmid_alloc,IPC_RMID,0);
		exit(-1);
	}

/* Set the initial value of the semaphore to one. One means free and 0
   means occupied ! */

#ifdef linux
	semval.val = 1;
#else
	semval = 1;
#endif /* linux */

#ifdef sun
	if (semctl(semid,0,SETVAL,&semval) == -1) 
#else
	if (semctl(semid,0,SETVAL,semval) == -1) 
#endif /* sun */
	{
		fprintf(stderr, "dc_inits : Can't initialise the semaphore\n");
		perror("dc_inits ");
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		shmdt(shmadr_alloc);
		shmctl(shmid_alloc,IPC_RMID,0);
		exit(-1);
	}

/* Get the semaphores set to protect the pointers memory */
	if ((semid1 = semget((key_t)SEMPTR_KEY,2,IPC_CREAT | 0666)) == -1) 
	{
		fprintf(stderr, "dc_inits : Can't get the semaphore\n");
		perror("dc_inits ");
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		shmdt(shmadr_alloc);
		shmctl(shmid_alloc,IPC_RMID,0);
		exit(-1);
	}

/* Set the initial value of the semaphore to one. One means free and 0
   means occupied ! */
#ifdef sun
	if (semctl(semid1,0,SETVAL,&semval) == -1) 
#else
	if (semctl(semid1,0,SETVAL,semval) == -1) 
#endif /* sun */
	{
		fprintf(stderr, "dc_inits : Can't initialise the semaphore\n");
		perror("dc_inits ");
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		shmdt(shmadr_alloc);
		shmctl(shmid_alloc,IPC_RMID,0);
		exit(-1);
	}

#ifdef sun
	if (semctl(semid1,1,SETVAL,&semval) == -1) 
#else
	if (semctl(semid1,1,SETVAL,semval) == -1) 
#endif /* sun */
	{
		fprintf(stderr, "dc_inits : Can't initialise the semaphore\n");
		perror("dc_inits ");
		shmdt(shmadr_datbuf);
		shmctl(shmid_datbuf,IPC_RMID,0);
		shmdt(shmadr_ptr);
		shmctl(shmid_ptr,IPC_RMID,0);
		shmdt(shmadr_alloc);
		shmctl(shmid_alloc,IPC_RMID,0);
		exit(-1);
	}
	return;
}
