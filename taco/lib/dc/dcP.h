 /*
 Author:	$Author: jkrueger1 $

 Version:	$Revision: 1.2 $

 Date:		$Date: 2005-02-22 15:59:57 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

 *  */

#ifndef _dcP_h
#define _dcP_h

#include <private/ApiP.h>

/* Constants definition */


#define	ALLOC_PARAMETER	1		/* Bad parameter in alloc function */
#define	ALLOC_NO_SPACE	2		/* No more space in shm mem. */
#define ALLOC_TIMER	3		/* Impossible to set the alarm */
#define	ALLOC_TIMEOUT	4		/* Time-out occurs */

#define NO_UNREG	0		/* No unregister when server exit */
#define UNREG		1		/* Unregister server when server exit */

#define	KEY_ALLOC	10		/* Key for the allocation table */
#define	KEY_PTR		11		/* Key for the ptr shared memory */
#define	KEY_DATBUF	12		/* Key for the data shared memory */

#define SEM_KEY		20		/* Key for the allocation area sem. */
#define SEMPTR_KEY	21		/* Key for the pointer semaphores */

#define	MAX_CMD		10		/* Max number of command for a dev. */

#ifdef TEST

#define	DATBUF_SIZE	16384	/* 32 Mb for the data shared memory */
#define PTR_SIZE	4096		/* 2 Mb for the pointers shared mem
					   (5000 devices and 100 cmd result) */
#define MAX_DEV		10		/* Max number of devices */
#define ALLOC_SIZE	64

#else

#define	DATBUF_SIZE	33554432	/* 32 Mb for the data shared memory */
#define PTR_SIZE	2097152		/* 2 Mb for the pointers shared mem
					   (5000 devices and 100 cmd result) */
#define MAX_DEV		5000		/* Max number of devices */
#define ALLOC_SIZE	DATBUF_SIZE / (BLOC_SIZE * 8)

#endif /* TEST */

/* This parameter is very important. It must be change with extreme care.
   Its value must be in the range 2 to 4096 and only a power of 2 */
#define BLOC_SIZE	32		/* One bit in the alloc. table 
					   represents BLOC_SIZE bytes in the 
					   data buffer */
#define	SHIFT		5		/* Shift for bloc size of 32 bytes */
#define MASK_BLK_SIZE	0x1F		/* Mask for a block size of 32 */
#define	MAXERR		3		/* max number of error before recon. */

#ifndef _api_h
#define False	0
#define True	1
#endif /* _api_h */

#define	DATA	1			/* Data buffer (dc_mem utility) */
#define ALLOC	2			/* Alloc. area (dc_mem utility) */
#define PTR	3			/* Pointers buffer (dc_mem utility) */

#define BUF_HEADER_SIZE	sizeof(int) * 2	/* Buffer header size */
#define CMD_HEADER_SIZE	sizeof(int) * 3 /* Cmd header size */

#define	HIST	100			/* Keep the last 100 results */
#define	TIME_OFF	713000000	/* Offset to substract from time */

#define NB_STRUCT	64		/* Number of structures (dc_devdef) */
#define NB_STRUCT_SHIFT	6		/* Number of shift (dc_devdef) */
#define NB_STRUCT_MASK	0x3F		/* Mask to test struct. number */

#define	DC_MAX	10			/* Max number of dc hosts !! */

#define SEC_MARGIN	32		/* A safe margin for memory alloc. */

#define	HASH_BASE	131		/* The base number for the hash func. */

#define TEAM_MASK	0xFC000000	/* Mask for team numb. in cmd. number */
#define	DSID_MASK	0x03FC0000	/* Mask for DS ID in command number */
#define CMD_MASK	0x0003FFFF	/* Mask for real cmd. number */
#define TEAM_SHIFT	26		/* Number of shifts for team number */
#define DSID_SHIFT	18		/* Number of shifts for dsid */

#define	MAX_DOM		20		/* Max number of domain for dc_info */
#define DC_MAX_FILES	250		/* Max number of open files for dc 
					   server */

#define	NETHOST_BLOCK	16		/* Block size for multi-nethost array */

/* Structures definitions */


typedef struct _upd_reqnb {
	int start_req;			/* Req num. between each update during
					   starting phase */
	int start_shift;		/* Number of shift during starting 
					   phase */
	int start;			/* Update number in starting phase */
	int start_nb;			/* Max num. of update during starting
					   phase */
	int update;			/* Req num between each resource upd. */
	int shift;			/* Shift number */
	}upd_reqnb,*Upd_reqnb;

typedef struct _cmd_info {
	int cmd;
	int argout;
	int cmd_time;
	}cmd_info;

typedef struct _int_level {
	unsigned int *data_buf[HIST];
	}int_level;

typedef struct _dc_dev_param {
	char device_name[32];
	int dev_name_length;
	int nb_cmd;
	cmd_info poll_cmd[10];
	int poll_freq;
	int ind_read;
	int ind_write;
	int time;
	int next_rec;
	}dc_dev_param;

typedef struct _hash_info{
	int hash_table_size;
	int cellar_size;
	int sem_id;
	dc_dev_param *parray;
	}hash_info;
	
typedef struct _cmd_sz {
	int cmd;
	int extra;
	int nb_loop;
	}cmd_sz;

typedef struct _dchost {
	char dc_host_name[HOST_NAME_LENGTH];
	CLIENT *rpc_handle;
	int nb_connect;
	char dc_req_name[40];
	int nb_call;
	int serv_num;
	int cantcont_error;
	}dchost;

typedef struct _dc_nethost_info {
	dchost dchost_arr[DC_MAX];
	char nethost[HOST_NAME_LENGTH];
	long first_imp;
	DevVarStringArray host_dc;
	long max_call_rd;
	}dc_nethost_info;

typedef struct _nethost_call {
	char *nethost;
	long nb_dev;
	long dc_ok;
	}nethost_call;

typedef struct _nethost_index {
	long net_index;
	long nb_dev;
	long dc_host_ok[DC_MAX];
	}nethost_index;

typedef struct _domain_info {
	char dom_name[20];
	int dom_nb_dev;
	}domain_info;

typedef struct _serv {
	unsigned int request;
	int numb;
	}serv;

/* Macro definitions */


/* This macros initialize A with the number of C character in the string
   pointed to by B */

#ifndef _db_setup_h
#define NB_CHAR(A,B,C) {\
	int im,lm;\
	lm = strlen(B);\
	for (im=0;im<lm;im++) {\
		if (B[im] == C)\
			A++;\
			}\
}\

#endif /* _db_setup_h */

/* This macros try to set a semaphore from a set of semaphore with a timeout
   of 300 mS 
   Parameter list : A = Semaphore to set
		    B = Semaphore ID
		    C = Time structure for timer operation
		    D = Time structur for timer operation
		    E = Sembuf structure for semaphore operation
		    F = Pointer to error code 
		    G = Flag for signal received during sem. operation */

#define TAKE_SEM(A,B,C,D,E,F,G) {\
	int resu;\
	C.it_value.tv_sec = 0;\
	C.it_value.tv_usec = 500000;\
	timerclear(&C.it_interval);\
	if (setitimer(ITIMER_REAL,&C,&D) == -1) {\
		*F = DcErr_PtrsTimerError;\
		return(-1);\
							}\
	E.sem_num = A;\
	E.sem_op = -1;\
	E.sem_flg = 0;\
	resu = semop(B,&E,1);\
	if ((resu == -1) && (errno == EINTR)) {\
		G = True;\
					}\
}
	
/* This macro test if a time-out occurs during a semaphore operation.
   Mainly a semaphore get operation
   Parameter list : A = Time out flag
		    B = Semaphore to clear in case of error in function which
                        stops the time-out timer
		    C = Time structure for timer operation
		    D = Time structur for timer operation
		    E = Sembuf structure for semaphore operation 
		    F = Semaphore ID
		    G = Pointer to error code 
		    H = Flag for signal received. during sem. operation */


#define TEST_TIME_OUT(A,B,C,D,E,F,G,H) {\
        if ((A == True) && (H == True)) {\
                A = False;\
                *G = DcErr_PtrsTimedOut;\
                return(-1);\
                                }\
	else {\
		timerclear(&C.it_interval);\
		timerclear(&C.it_value);\
		if (setitimer(ITIMER_REAL,&C,&D) == -1) {\
			E.sem_op = 1;\
			E.sem_num = B;\
			E.sem_flg = 0;\
			semop(F,&E,1);\
			*G = DcErr_PtrsTimerError;\
			return(-1);\
								}\
		}\
}

/* This macro release a semaphore
   Parameter list : A = Semaphore to clear
		    B = Semaphore ID
		    C = Sembuf structure for semaphore operation 
		    D = Pointer to error code */

#define REL_SEM(A,B,C,D) {\
	C.sem_op = 1;\
	C.sem_num = A;\
	C.sem_flg = 0;\
	semop(B,&C,1);\
}
	
/* This macros try to set a semaphore and to clear another semaphore from 
   a set of semaphores. A time-out of 300 mS is registered for the set
   operation 
   Parameter list : A = Semaphore to clear
		    B = Semaphore to set
		    C = Semaphore ID
		    D = Time structure for timer operation
		    E = Time structur for timer operation
		    F = Sembuf structures for semaphore operation 
		    G = Pointer to error code */

#define REL_TAKE_SEM(A,B,C,D,E,F,G,H) {\
	int resu;\
	D.it_value.tv_sec = 0;\
	D.it_value.tv_usec = 300000;\
	timerclear(&D.it_interval);\
	if (setitimer(ITIMER_REAL,&D,&E) == -1) {\
		*G = DcErr_PtrsTimerError;\
		return(-1);\
							}\
	F[0].sem_num = A;\
	F[0].sem_op = 1;\
	F[0].sem_flg = 0;\
	semop(C,F,1);\
	F[0].sem_num = B;\
	F[0].sem_op = -1;\
	F[0].sem_flg = 0;\
	resu = semop(C,F,1);\
	if ((resu == -1) && (errno == EINTR))\
		H = True;\
}


/* Functions definition */


#ifndef OSK
unsigned char *dcmem_alloc(unsigned char *base,register char *tab,int size,int alloc_size,int semid,long *perr);
int dcmem_info(register char *tab,int buf_size,unsigned int *lmem_free,unsigned int *mem_free,int *parea,long *perr);
int dcmem_free(unsigned char *base,unsigned char *base_buf,unsigned char *tab,int buf_size,int semid,long *perr);

int hash(char *dev_name,int hash_table_size);
int insert_dev(char *devname,dc_dev_param *pdata,hash_info *mem_info,long *perr);
int search_dev(char *devname,dc_dev_param *pdata,hash_info *mem_info,int *pind,long *perr);
int delete_dev(char *devname,hash_info *mem_info,long *perr);
int update_dev(int ind,dc_dev_param *pdata,hash_info *mem_info,long *perr);
#else
unsigned char *dcmem_alloc();
int dcmem_info();
int dcmem_free();

int hash();
int insert_dev();
int search_dev();
int delete_dev();
int update_dev();
#endif

#endif /* _dcP_h */
