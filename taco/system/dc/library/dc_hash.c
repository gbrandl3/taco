
/* 
 * File : 	dc_hash.c
 *
 * Author(s):	Emmanuel TAUREL
 *		$Author: jkrueger1 $
 *
 * Original:	May 1993
 *
 * Description:	
 *
 * Version:	$Revision: 1.2 $
 *
 * Date:	$Date: 2004-06-02 16:36:16 $
 *
 */

#include "config.h"
#include <API.h>
#include <dcP.h>
#include <DevErrors.h>
#include <errno.h>

#ifdef sun
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#else
#include <sys/sem.h>
#endif

#include <string.h>

/* External variable */

extern int time_out;
extern int errno;


/****************************************************************************
*                                                                           *
*		Code for hash function                                      *
*                        ----                                               *
*                                                                           *
*    Function rule : To compute from the device name, the indice in the     *
*		     data collector main shared memory. The hashing         *
*		     function comes from the "Handbook of algorithms and    *
*		     data structures" of Gonnet and Baeza-Yates.            *
*                                                                           *
*    Argin : - The device name						    *
*	     - The hash table size					    *
*                                                                           *
*    Argout : - No argout						    *
*                                                                           *
*    This function returns the indice in the hash table			    *
*                                                                           *
****************************************************************************/



int hash(char *dev_name,int hash_table_size)
{
	unsigned int i;

	for (i = 0;*dev_name;dev_name++)
		i = HASH_BASE * i + *dev_name;

	return(i % hash_table_size);
}




/****************************************************************************
*                                                                           *
*		Code for insert_dev function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To insert in the main data collector shared memory a   *
*		     new device. To solve the collision problem inherent to *
*		     simple hashing function, we use the coalesced hahsing  *
*		     with cellar algorithm.				    *
*                                                                           *
*    Argin : - The device name						    *
*	     - A pointer to the structure which must be inserted in the     *
*	       shared memory						    *
*	     - The address of a structure with the shared memory info.      *
*	       These informations are :					    *
*		 	- The hash table size				    *
*			- The cellar table size				    *
*			- The semaphore set identifier			    *
*			- The address of the hash table			    *
*                                                                           *
*    Argout : - A pointer to an error code				    *
*                                                                           *
*    This function returns DS_NOTOK if one error occurs and set the error code.   *
*    Otherwise, the function returns DS_OK and clears the error code	    *
*                                                                           *
****************************************************************************/


int insert_dev(char *devname,dc_dev_param *pdata,hash_info *mem_info,long *perr)
{
	int ind,sem;
	int nextfree;
	int first_loop;
	struct itimerval time,old_time;
	struct sembuf single_sops;
	struct sembuf two_sops[2];
	register dc_dev_param *tmp_parray;
	register int hash_size;
	int sig_rec;

	nextfree = mem_info->cellar_size - 1;
	tmp_parray = mem_info->parray;
	hash_size = mem_info->hash_table_size;

/* Call the hash function */

	ind = hash(devname,hash_size);

/* Take the semaphore */

	time_out = sig_rec = False;
	TAKE_SEM(0,mem_info->sem_id,time,old_time,single_sops,perr,sig_rec);

/* Exit with error if time_out occurs */

	TEST_TIME_OUT(time_out,0,time,old_time,single_sops,mem_info->sem_id,perr,sig_rec);

/* If it is an empty record, update its parameters (no collision) */

	if (tmp_parray[ind].device_name[0] == 0) {
		tmp_parray[ind] = *pdata;
		tmp_parray[ind].next_rec = -1;
#ifdef DEBUG
		printf("Device %s inserted at place %d without collision\n",devname,ind);
#endif /* DEBUG */
		REL_SEM(0,mem_info->sem_id,single_sops,perr);
					      }

/* A collision occurs */

	else {

#ifdef DEBUG
		printf("Collision at place %d\n",ind);
#endif /* DEBUG */

/* Is it for the same device ? */

		if (tmp_parray[ind].dev_name_length == pdata->dev_name_length) {
			if (strcmp(tmp_parray[ind].device_name,pdata->device_name) == 0) {
				REL_SEM(0,mem_info->sem_id,single_sops,perr);
				*perr = DcErr_DeviceAlreadyDefinedInPtrs;
				return(DS_NOTOK);
									}
									}

		if (tmp_parray[ind].next_rec == -1) {
			time_out = sig_rec = False;
			REL_TAKE_SEM(0,1,mem_info->sem_id,time,old_time,two_sops,perr,sig_rec);
			TEST_TIME_OUT(time_out,1,time,old_time,single_sops,mem_info->sem_id,perr,sig_rec);
							}

		else {
			
/* Find the end of the chain */

			first_loop = True;
			while(tmp_parray[ind].next_rec != -1) {
				if (first_loop == True) {
					time_out = sig_rec = False;
					REL_TAKE_SEM(0,1,mem_info->sem_id,time,old_time,two_sops,perr,sig_rec);
					TEST_TIME_OUT(time_out,1,time,old_time,single_sops,mem_info->sem_id,perr,sig_rec);
					first_loop = False;
							    }
				ind = tmp_parray[ind].next_rec;
				if (tmp_parray[ind].dev_name_length == pdata->dev_name_length) {
					if (strcmp(tmp_parray[ind].device_name,pdata->device_name) == 0) {
						REL_SEM(1,mem_info->sem_id,single_sops,perr);
						*perr = DcErr_DeviceAlreadyDefinedInPtrs;
						return(DS_NOTOK);
									}
									}
							}
			}

/* Find the first free record */

		while(tmp_parray[hash_size + nextfree].device_name[0] != 0 && nextfree >= 0)
			nextfree--;

		if (nextfree < 0) {
#ifdef DEBUG
			fprintf(stderr,"Cellar table full !!!\n");
#endif /* DEBUG */
			REL_SEM(1,mem_info->sem_id,single_sops,perr);
			*perr = DcErr_CellarTableFull;
			return(DS_NOTOK);
				  }

/* Insert the record */

		tmp_parray[hash_size + nextfree] = *pdata;
		tmp_parray[hash_size + nextfree].next_rec = -1;
		if (ind < hash_size) {
			time_out = sig_rec = False;
			REL_TAKE_SEM(1,0,mem_info->sem_id,time,old_time,two_sops,perr,sig_rec);
			TEST_TIME_OUT(time_out,0,time,old_time,single_sops,mem_info->sem_id,perr,sig_rec);
					}
		tmp_parray[ind].next_rec = nextfree + hash_size;
#ifdef DEBUG
		printf("Device %s inserted at place %d in cellar table\n",devname,nextfree);
#endif /* DEBUG */

/* Release semaphore */

		if (ind < hash_size) 
			sem = 0;
		else
			sem = 1;
		REL_SEM(sem,mem_info->sem_id,single_sops,perr);

		}

/* Leave function */

	*perr = DS_OK;
	return(DS_OK);

}





/****************************************************************************
*                                                                           *
*		Code for search_dev function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To search for a device in the dc main shared memory    *
*		     In this memory, every device are stored as one array   *
*		     element. A hshing function with the coalesced hashing  *
*		     with cellar algorithm has been used to do the storing. *
*                                                                           *
*    Argin : - The device name						    *
*	     - The address of a structure with the shared memory info.      *
*	       These informations are :					    *
*		 	- The hash table size				    *
*			- The cellar table size				    *
*			- The semaphore set identifier			    *
*			- The address of the hash table			    *
*                                                                           *
*    Argout : - The address where to store the searched device information  *
*	      - The indice in the array of the device structure		    *
*	      - A pointer to error code					    *
*                                                                           *
*    This function returns DS_NOTOK if one error occurs and set the error code.   *
*    Otherwise, the function returns DS_OK and clears the error code	    *
*                                                                           *
****************************************************************************/


int search_dev(char *devname,dc_dev_param *pdata,hash_info *mem_info,int *pind,long *perr)
{
	int ind,sem;
	int first_loop;
	struct itimerval time,old_time;
	struct sembuf single_sops;
	struct sembuf two_sops[2];
	register dc_dev_param *tmp_parray;
	int sig_rec;

	time_out = False;
	sig_rec = False;
	first_loop = True;
	tmp_parray = mem_info->parray;

/* Call the hash function */

	ind = hash(devname,mem_info->hash_table_size);

/* Search in the array */

	TAKE_SEM(0,mem_info->sem_id,time,old_time,single_sops,perr,sig_rec);
	TEST_TIME_OUT(time_out,0,time,old_time,single_sops,mem_info->sem_id,perr,sig_rec);

	while (ind != -1 && tmp_parray[ind].device_name[0] != 0 &&
               (strcmp(devname,tmp_parray[ind].device_name) != 0)) {
		ind = tmp_parray[ind].next_rec;
		if (first_loop == True) {
			time_out = sig_rec = False;
			REL_TAKE_SEM(0,1,mem_info->sem_id,time,old_time,two_sops,perr,sig_rec);
			TEST_TIME_OUT(time_out,1,time,old_time,single_sops,mem_info->sem_id,perr,sig_rec);
			first_loop = False;
					}
								  }

/* If the device has been found, return the indice in the array */

	if (ind == -1 || tmp_parray[ind].device_name[0] == 0) {
		if (first_loop == False)
			sem = 1;
		else
			sem = 0;
		REL_SEM(sem,mem_info->sem_id,single_sops,perr);
		*perr = DcErr_DeviceNotInPtrsMemory;
		return(DS_NOTOK);
							  }
	else {
		*pdata = tmp_parray[ind];
		*pind = ind;
		if (first_loop == False)
			sem = 1;
		else
			sem = 0;
		REL_SEM(sem,mem_info->sem_id,single_sops,perr);
		*perr = DS_OK;
		return(DS_OK);
		}
}





/****************************************************************************
*                                                                           *
*		Code for delete_dev function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To delete from the dc main shared memory, device       *
*		     information.					    *
*                                                                           *
*    Argin : - The device name						    *
*	     - The address of a structure with the shared memory info.      *
*	       These informations are :					    *
*		 	- The hash table size				    *
*			- The cellar table size				    *
*			- The semaphore set identifier			    *
*			- The address of the hash table			    *
*                                                                           *
*    Argout : - A pointer to an error code				    *
*                                                                           *
*    This function returns DS_NOTOK if one error occurs and set the error code.   *
*    Otherwise, the function  returns DS_OK	and clears the error code.   	    *
*                                                                           *
****************************************************************************/

int delete_dev(char *devname,hash_info *mem_info,long *perr)
{
	int ind,prev_ind,sem,first_loop;
	int first_ind,next_ind;
	struct itimerval time,old_time;
	struct sembuf single_sops;
	struct sembuf two_sops[2];
	register dc_dev_param *tmp_parray;
	register hash_size;
	dc_dev_param tmp_data;
	int semid;
	int sig_rec;

	first_loop = True;
	tmp_parray = mem_info->parray;
	hash_size = mem_info->hash_table_size;
	semid = mem_info->sem_id;

/* First search the record in the array */

	ind = hash(devname,hash_size);
	time_out = sig_rec = False;
	TAKE_SEM(0,semid,time,old_time,single_sops,perr,sig_rec);
	TEST_TIME_OUT(time_out,0,time,old_time,single_sops,semid,perr,sig_rec);

	while (ind != -1 && tmp_parray[ind].device_name[0] != 0 &&
               (strcmp(devname,tmp_parray[ind].device_name) != 0)) {
		prev_ind = ind;
		ind = tmp_parray[ind].next_rec;
		if (first_loop == True) {
			time_out = sig_rec = False;
			REL_TAKE_SEM(0,1,semid,time,old_time,two_sops,perr,sig_rec);
			TEST_TIME_OUT(time_out,1,time,old_time,single_sops,semid,perr,sig_rec);
			first_loop = False;
					}
								  }

/* If the device is not in the ptrs shared memory, leave function with
   error flag set */

	if (ind == -1 || tmp_parray[ind].device_name[0] == 0) {
		if (first_loop == False)
			sem = 1;
		else
			sem = 0;
		REL_SEM(sem,semid,single_sops,perr);
		*perr = DcErr_DeviceNotInPtrsMemory;
		return(DS_NOTOK);
							  }

/* If this record is the last one in a chain */

	if (tmp_parray[ind].next_rec == -1) {

/* If the record is the only one in the list (inserted with no collision) */

		if (ind < hash_size) {
			tmp_parray[ind].next_rec = 1;
			tmp_parray[ind].device_name[0] = 0;
			tmp_parray[ind].dev_name_length = 0;
			REL_SEM(0,semid,single_sops,perr);
				}

		else {

/* The record is the last one in a chain and it is stored in the cellar area */

			if (prev_ind < hash_size) {
				time_out = sig_rec = False;
				REL_TAKE_SEM(1,0,semid,time,old_time,two_sops,perr,sig_rec);
				TEST_TIME_OUT(time_out,0,time,old_time,single_sops,semid,perr,sig_rec);
				tmp_parray[prev_ind].next_rec = -1;
				time_out = sig_rec = False;
				REL_TAKE_SEM(0,1,semid,time,old_time,two_sops,perr,sig_rec);
				TEST_TIME_OUT(time_out,1,time,old_time,single_sops,semid,perr,sig_rec);
						   }
			else 
				tmp_parray[prev_ind].next_rec = -1;

			tmp_parray[ind].next_rec = 1;
			tmp_parray[ind].device_name[0] = 0;
			tmp_parray[ind].dev_name_length = 0;
			REL_SEM(1,semid,single_sops,perr);
		     }
					}

/* The record is not the last one in a chain */

	else {

/* If this record is the first one in the list, copy the second record in
   this place and mark the second record as free */

		if (ind < hash_size) {
			next_ind = tmp_parray[ind].next_rec;
			time_out = sig_rec = False;
			REL_TAKE_SEM(0,1,semid,time,old_time,two_sops,perr,sig_rec);
			TEST_TIME_OUT(time_out,1,time,old_time,single_sops,semid,perr,sig_rec);
			tmp_data = tmp_parray[next_ind];
			tmp_parray[next_ind].device_name[0] = 0;
			tmp_parray[next_ind].next_rec = 1;
			tmp_parray[next_ind].dev_name_length = 0;
			time_out = sig_rec = False;
			REL_TAKE_SEM(1,0,semid,time,old_time,two_sops,perr,sig_rec);
			TEST_TIME_OUT(time_out,0,time,old_time,single_sops,semid,perr,sig_rec);
			tmp_parray[ind] = tmp_data;
			REL_SEM(0,semid,single_sops,perr);
					    }

		else {

/* The record is inserted in a chain so, delete it and update the next record
   parameter in the previous chain record. */

			if (prev_ind > hash_size) {

/* In this case, all the records to modify are in the cellar area so, no need
   to take the hash table semaphore */

				next_ind = tmp_parray[ind].next_rec;
				tmp_parray[ind].next_rec = 1;
				tmp_parray[ind].device_name[0] = 0;
				tmp_parray[ind].dev_name_length = 0;
				tmp_parray[prev_ind].next_rec = next_ind;
				REL_SEM(1,semid,single_sops,perr);
						}

			else {

				next_ind = tmp_parray[ind].next_rec;
				time_out = sig_rec = False;
				REL_TAKE_SEM(1,0,semid,time,old_time,two_sops,perr,sig_rec);
				TEST_TIME_OUT(time_out,0,time,old_time,single_sops,semid,perr,sig_rec);
				tmp_parray[prev_ind].next_rec = next_ind;
				time_out = sig_rec = False;
				REL_TAKE_SEM(0,1,semid,time,old_time,two_sops,perr,sig_rec);
				TEST_TIME_OUT(time_out,1,time,old_time,single_sops,semid,perr,sig_rec);
				tmp_parray[ind].next_rec = 1;
				tmp_parray[ind].device_name[0] = 0;
				tmp_parray[ind].dev_name_length = 0;
				REL_SEM(1,semid,single_sops,perr);
				}
		
			}
		}

	*perr = DS_OK;
	return(DS_OK);
	
}




/****************************************************************************
*                                                                           *
*		Code for update_dev function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To update device information in the data collector     *
*		     pointers shared memory				    *
*                                                                           *
*    Argin : - The indice in the array of the device to be updated	    *
*	     - A pointer to the structure with the new data to be updated   *
*	     - The address of a structure with the shared memory info.      *
*	       These informations are :					    *
*		 	- The hash table size				    *
*			- The cellar table size				    *
*			- The semaphore set identifier			    *
*			- The address of the hash table			    *
*                                                                           *
*    Argout : - A pointer to an error code				    *
*                                                                           *
*    This function returns DS_NOTOK if one error occurs and set the error code.   *
*    Otherwise, the function returns DS_OK and clears the error code	    *
*                                                                           *
****************************************************************************/


int update_dev(int ind,dc_dev_param *pdata,hash_info *mem_info,long *perr)
{
	int semid,tmp_ind;
	int hash_size;
	int sem;
	struct itimerval time,old_time;
	struct sembuf single_sops;
	struct sembuf two_sops[2];
	register dc_dev_param *tmp_parray;
	int sig_rec;

	tmp_parray = mem_info->parray;
	hash_size = mem_info->hash_table_size;
	semid = mem_info->sem_id;

/* According to the ind function parameter, take the right semaphore */

	if (ind > hash_size)
		sem = 1;
	else
		sem = 0;
	time_out = sig_rec = False;
	TAKE_SEM(sem,semid,time,old_time,single_sops,perr,sig_rec);
	TEST_TIME_OUT(time_out,sem,time,old_time,single_sops,semid,perr,sig_rec);

/* Correct device name ? */

	if (tmp_parray[ind].dev_name_length != pdata->dev_name_length) {
		REL_SEM(sem,semid,single_sops,perr);
		*perr = DcErr_BadPtrsMemoryIndice;
		return(DS_NOTOK);
								       }
	if (strcmp(tmp_parray[ind].device_name,pdata->device_name) != 0) {
		REL_SEM(sem,semid,single_sops,perr);
		*perr = DcErr_BadPtrsMemoryIndice;
		return(DS_NOTOK);
									}

/* Update data in the array */

	tmp_ind = tmp_parray[ind].next_rec;
	tmp_parray[ind] = *pdata;
	tmp_parray[ind].next_rec = tmp_ind;

/* Leave function */

	REL_SEM(sem,semid,single_sops,perr);
	*perr = DS_OK;
	return(DS_OK);

}
