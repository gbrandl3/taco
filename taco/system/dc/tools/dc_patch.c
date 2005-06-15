#include "config.h"
#include <API.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#ifdef sun
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include <sys/shm.h>
#endif

#include <errno.h>

#include <dcP.h>

void usage(const char *cmd)
{
	fprintf(stderr, "usage : %s [options] <shm name> <offset(in hex)>\n", cmd);
	fprintf(stderr, " Display the content of a shared memory area in hexadecimal values.\n");
	fprintf(stderr, "      options : -h display this message\n");
	exit(-1);
}

/****************************************************************************
*                                                                           *
*		Code for dc_patch command                                   *
*                        ------                                             *
*                                                                           *
*    Command rule : To display the content of a shared memory area	    *
*		    The memory content is displayed in hexadecimal	    *
*                                                                           *
*    Synopsis : dc_patch <shm name> <offset> <size>			    *
*                                                                           *
****************************************************************************/
int main(int argc, char **argv)
{
	int tmp;
	unsigned long offset;
	int shmid_datbuf,shmid_ptr,shmid_alloc;
	int shm,nb_loop;
	char *shm_name;
	char *addr_datbuf,*addr_ptr,*addr_alloc;
	unsigned char tmp_buf[16];
	unsigned int tmp0,tmp1,tmp2,tmp3;
	int i,j,l;
	char *addr;
	long error;
	char hostna[HOST_NAME_LENGTH];
	char dev_name[DEV_NAME_LENGTH];
	int		 ptr_size,alloc_size;
	int 		nb_tot;
	char 		*tmp_ch;
	unsigned int 	diff;
	DevLong		dev_num,
			cell_num,
			dat_size;
	db_resource 	res1[] = {
				{"dev_number",D_LONG_TYPE, &dev_num},
				{"cellar_number",D_LONG_TYPE, &cell_num},
				{"data_size",D_LONG_TYPE, &dat_size},
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
        if (optind != argc - 2)
                usage(argv[0]);


	if (argv[optind + 1][0] != '0') 
	{
		fprintf(stderr, "Offset must be specified in hex. format (0x...)\n");
		exit(-1);
	}
	else 
	{
		argv[optind + 1][1] = tolower(argv[optind + 1][1]);
		if (argv[optind + 1][1] != 'x') 
		{
			fprintf(stderr, "Offset must be specified in hex. format (0x...)\n");
			exit(-1);
		}
	}
	offset = strtoul(argv[optind + 1],(char **)NULL,0);

/* Verify argument validity */
	if (offset < 0) 
	{
		fprintf(stderr,"dc_patch : Offset can't be negative\n");
		exit(-1);
	}

/* Change shared memory name to lowercase letters */
	l = strlen(argv[optind]);
	if ((shm_name = (char *)malloc(l + 1)) == NULL) 
	{
		fprintf(stderr,"dc_mem : Can't alloc memory, exiting...\n");
		exit(-1);
	}
	for (i = 0;i < l;i++)
		shm_name[i] = tolower(argv[optind][i]);
	shm_name[l] = 0;
	
/* Correct shared memory name ? */
	if (strcmp(shm_name,"ptr") == 0) 
		shm = PTR;
	else if (strcmp(shm_name,"data") == 0) 
		shm = DATA;
	else if (strcmp(shm_name,"alloc") == 0) 
		shm = ALLOC;
	else 
	{
		fprintf(stderr,"dc_patch : Bad shared memory name\n");
		fprintf(stderr,"           Authorized name : alloc, ptr and data\n");
		exit(-1);
	}	
	
/* Import static database */
	if (db_import(&error)) 
	{
		fprintf(stderr, "dc_del : Can't import static database\n");
		fprintf(stderr, "dc_del : Check your NETHOST environment variable\n");
		exit(-1);
	}

/* Build device name. It is a function of the host name */

	gethostname(hostna,sizeof(hostna));
	if ((tmp_ch = strchr(hostna,'.')) != NULL) 
	{
		diff = (u_int)(tmp_ch - hostna);
		hostna[diff] = 0;
	}
	strcpy(dev_name,"CLASS/DC/");
	strcat(dev_name,hostna);

/* Retrieve data collector memories size */
	if (db_getresource(dev_name,res1,res1_size,&error)) 
	{
		fprintf(stderr, "dc_mem : Can't retrieve resources\n");
		fprintf(stderr, "dc_mem : Error code : %d\n",error);
		exit(-1);
	}

/* Compute pointers area size and allocation table size */

	nb_tot = dev_num + cell_num;
	ptr_size = (int)((nb_tot * sizeof(dc_dev_param)) + (nb_tot * sizeof(int_level)));
	alloc_size = (int)(dat_size / 256);

	switch (shm) 
	{
		case DATA : 
/* Attach the data buffer to this process data memory (it is a shared memory segment) */
			if ((shmid_datbuf = shmget((key_t)KEY_DATBUF,(size_t)dat_size,0666)) == -1) 
			{
				if (errno == ENOENT) 
				{
					fprintf(stderr,"dc_mem : Data buffer does not exist in memory !\n");
				}
				else 
				{
					fprintf(stderr,"dc_mem : Can't get the data buffer shm id\n");
					fprintf(stderr,"dc_mem : Error code : %d\n",errno);
			     	}
				exit(-1);
			}
			if ((addr_datbuf = (char *)shmat(shmid_datbuf,(char *)0,0)) == (char *)-1) 
			{
				fprintf(stderr,"dc_mem : Can't attach to the data buffer\n");
				fprintf(stderr,"dc_mem : Error code : %d\n",errno);
				exit(-1);
			}
			addr = addr_datbuf;
			break;

		case PTR :
/* Attach the pointers buffer to this process data memory */
			if ((shmid_ptr = shmget((key_t)KEY_PTR,(size_t)ptr_size,0666)) == -1) 
			{
				if (errno == ENOENT) 
				{
					fprintf(stderr,"dc_mem : Pointers buffer does not exist in memory !\n");
				}
				else 
				{
					fprintf(stderr,"dc_mem : Can't get the pointers buffer shm id\n");
					fprintf(stderr,"dc_mem : Error code : %d\n",errno);
				}
				exit(-1);
			}
			if ((addr_ptr = (char *)shmat(shmid_ptr,(char *)0,0)) == (char *)-1) 
			{
				fprintf(stderr,"dc_mem : Can't attach to the pointer buffer\n");
				fprintf(stderr,"dc_mem : Error code : %d\n",errno);
				exit(-1);
			}
			addr = addr_ptr;
			break;

		case ALLOC :
/* Attach the allocation table to this process data memory */
			if ((shmid_alloc = shmget((key_t)KEY_ALLOC,(size_t)alloc_size,0666)) == -1) 
			{
				if (errno == ENOENT) 
				{
					fprintf(stderr,"dc_mem : Allocation area does not exist in memory !\n");
				}
				else 
				{
					fprintf(stderr,"dc_mem : Can't get the allocation table shm id\n");
					fprintf(stderr,"dc_mem : Error code : %d\n",errno);
				}
				exit(-1);
			}
			if ((addr_alloc = (char *)shmat(shmid_alloc,(char *)0,0)) == (char *)-1) 
			{
				fprintf(stderr,"dc_mem : Can't attach to the allocation area\n");
				fprintf(stderr,"dc_mem : Error code : %d\n",errno);
				exit(-1);
			}
			addr = addr_alloc;
			break;
	}

/* Print memory content */
	tmp_buf[0] = (unsigned char)addr[offset];
	printf("mem value (in hex) : %x\n",tmp_buf[0]);
	addr[offset] = 0x0;
	tmp_buf[0] = (unsigned char)addr[offset];
	printf("mem value (in hex) : %x\n",tmp_buf[0]);
}
