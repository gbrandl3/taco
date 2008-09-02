/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File         : dc_put.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author(s)      E. Taurel:
 *                $Author: jkrueger1 $
 *
 * Original     : February 1993
 *
 * Version      : $Revision: 1.7 $
 *
 * Date         : $Date: 2008-09-02 13:03:15 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <DevErrors.h>
#define DC_SERVER 1
#include <dc.h>
#include <dcP.h>
#include <dc_xdr.h>
#include <stdlib.h>

#include <stdlib.h>
#if HAVE_TIME_H
#	include <time.h>
#endif

/* Variables defined in dc_svc.c */

extern hash_info mem;
extern int alloc_size;
extern char *addr_ptr,*addr_alloc,*addr_data;
extern int semid;
extern int req_call;

/*
 * uncomment this if you want to check the data collector data
 * buffer. ONLY do this if you are really desperate and the dc 
 * is crashing and you cannot find out why. Check to see if the
 * dc memory is filling up with the dc_mfrees tool - andy 22/1/2008
 */
/*
#define DC_SANITY
*/
extern int *dc_sanity_bufsize;

int dev_add_data(dev_dat *dev_info, int *perr);
int add_data(dev_dat *dev_info, dc_dev_param *pdata, int ind, int *perr);

/**
 * To store in the data collector some command result.
 * The command result are stored in a shared memory area often called "data buffer"
 * 
 * @param rece  A pointer to a structure of the "arr1" type
 *            The definition of the arr1 type is :
 *            struct {
 *              u_int arr1_len;     The number of strings
 *              char **arr1_val;    A pointer to the array of strings
 *                  } 
 *
 * @return This function returns a pointer to a structure of the "db_res" type.
 *    The definition of this structure is :
 *    struct {
 *      arr1 rev_val;   A structure of the arr1 type (see above) with the
 *                     resources values information transferred as strings
 *      int db_err;    The database error code
 *                     0 if no error
 *          }
 */
dc_xdr_error *dc_dataput_1(dev_datarr *rece)
{
	int dev_number;
	int i,j,k;
	static dc_xdr_error ret_err;
	int err_code;
	int nb_cmd;
	int cmd_size;
	int cmd_err;
#ifdef DEBUG
	printf("enter dc_dataput\n");
#endif /* DEBUG */

/* Miscellaneous initializations */
	dev_number = rece->dev_datarr_len;
	ret_err.error_code = 0;
	ret_err.dev_error = 0;

#ifdef DEBUG
	printf("\nDc_dataput call for %d devices\n",dev_number);
	for (i = 0; i < dev_number;i++) 
	{
		printf("Device name : %s\n",rece->dev_datarr_val[i].xdev_name);
		nb_cmd = rece->dev_datarr_val[i].xcmd_dat.xcmd_dat_len;
		printf("%d commands for this device\n",nb_cmd);
		for (j = 0;j < nb_cmd;j++) 
		{
			printf("Cmd : %d\n",rece->dev_datarr_val[i].xcmd_dat.xcmd_dat_val[j].xcmd);
			cmd_err = rece->dev_datarr_val[i].xcmd_dat.xcmd_dat_val[j].xcmd_error;
			printf("Error code : %d\n",cmd_err);
			if (cmd_err == 0) 
			{
				cmd_size = rece->dev_datarr_val[i].xcmd_dat.xcmd_dat_val[j].xsequence.xsequence_len;
				printf("Cmd size : %d\n",cmd_size);
			}
		}
	}
#endif /* DEBUG */

/* Init. the req_call parameter for this call */
	req_call = dev_number;

/* Call the dev_add_data function for every device */
	for (i = 0;i < dev_number;i++) 
	{
		if (dev_add_data(&(rece->dev_datarr_val[i]),&err_code)) 
		{
			ret_err.error_code = err_code;
			ret_err.dev_error = i + 1;
			return(&ret_err);
		}
	}


/* Leave server */
#ifdef DEBUG
	printf("leave dc_dataput\n");
#endif /* DEBUG */
	return(&ret_err);

}



/**
 * To verify that the device from wich we want to store
 * command results in the data buffer is defined for the 
 * data collector. If OK, store command results in the data buffer
 * 
 * @param dev_info A pointer to the structure which contains the device information (dev_dat type)
 * @param perr	   A pointer to the error code
 *
 * @return This function returns DS_OK if no error occurs. Otherwise, this function   
 *    	set the error code and returns DS_NOTOK
 *
 */
int dev_add_data(dev_dat *dev_info, int *perr)
{
	char d_name[60];
	int resu;
	int ind;
	dc_dev_param data;
	DevLong error;

/* Try to retrieve a record in the device_info part of the pointers area with
   the same device name */
	strcpy(d_name,dev_info->xdev_name);
	resu = search_dev(d_name,&data,&mem,&ind,&error);

	if (resu == -1) 
	{
/* Search error */
		if (error != DcErr_DeviceNotInPtrsMemory ) 
		{
#ifdef DEBUG
			printf("endf dev_add_data\n");
#endif /* DEBUG */
			*perr = error;
			return(DS_NOTOK);
		}

/* Device does not exists in database */
		else 
		{
			*perr = DcErr_DeviceNotDefined;
			return(DS_NOTOK);
		}
	}

/* The device exists so, store new data in the data buffer */
	if (add_data(dev_info,&data,ind,perr))
		return(DS_NOTOK);
			
/* Leave function */
	return(0);
}



/**
 * To store in the "data buffer" the command results for only one device
 *
 * @param dev_info A pointer to the structure which contains the device 
 *	     information (dev_dat type)
 * @param pdata	   The address of the device information stucture
 * @param ind	   The index of the device info in the pointers area array
 * @param perr	   A pointer to the error code
 * 
 * @return This function returns DS_OK no error occurs. Otherwise, this function
 *    set the error code and returns DS_NOTOK
 *
 */
int add_data(dev_dat *dev_info, dc_dev_param *pdata, int ind, int *perr)
{
	unsigned int nb_cmd;
	char bad_cmd = False;
	int cmd;
	int i,j,k,l;
	int buf_size, old_buf_size;
	cmd_sz cmd_info[MAX_CMD];
	int_level *int_array;
	struct timeval time;
	struct timezone tzone;
	unsigned int time_ten;
	unsigned int *ptr;
	cmd_dat *tmp_ptr;
	unsigned char *tmp_ptr1;
	dc_dev_param *array;
	DevLong error;
	unsigned int *old_ptr;
	int old_ind;
	unsigned int ptr_base;

/* Init pointer to intermediate level pointers buffer */
	array = (dc_dev_param *)addr_ptr;
	int_array = (int_level *)&array[mem.hash_table_size + mem.cellar_size];

/* Verify that the command number and command code are those defined in the
   data collector database */

	nb_cmd = dev_info->xcmd_dat.xcmd_dat_len;
	if (nb_cmd != pdata->nb_cmd) 
	{
		*perr = DcErr_BadCmdNumber;
		return(DS_NOTOK);
	}

	for (i = 0;i < nb_cmd;i++) 
	{
		cmd = dev_info->xcmd_dat.xcmd_dat_val[i].xcmd;
		for (j = 0; j < pdata->nb_cmd;j++) 
		{
			if (pdata->poll_cmd[j].cmd == cmd) 
			{
				bad_cmd = False;
				break;
			}
			else 
				bad_cmd = True;
		}

		if (bad_cmd == True) 
		{
			*perr = DcErr_CmdNotDefinedForDevice;
			return(DS_NOTOK);
		}
		else
			continue;
	}
	
/* Check that the data buffer is not NULL */

	for (i = 0;i < nb_cmd;i++) 
	{
		if( dev_info->xcmd_dat.xcmd_dat_val[i].xsequence.xsequence_val==NULL &&
		    dev_info->xcmd_dat.xcmd_dat_val[i].xcmd_error==0 ) 
		{
			dev_info->xcmd_dat.xcmd_dat_val[i].xcmd_error = DcErr_ErrDuringExtractFunction;
		}			
	}


/* Change command result size to be a multiple of four (Command results are
   stored by packet of 4 bytes) */
	for (i = 0;i < nb_cmd;i++) 
	{
		if ((dev_info->xcmd_dat.xcmd_dat_val[i].xcmd_error != 0) &&
		    (((dev_info->xcmd_dat.xcmd_dat_val[i].xcmd_error >> DS_CAT_SHIFT ) & DS_CAT_MASK) != WarningError) ) 
		{
			cmd_info[i].cmd = 0;
			cmd_info[i].extra = 4;
			cmd_info[i].nb_loop = 0;
		}
		else 
		{
			cmd_info[i].cmd = dev_info->xcmd_dat.xcmd_dat_val[i].xsequence.xsequence_len;
			switch(cmd_info[i].cmd & 0x3) 
			{
				case 0 : 
					cmd_info[i].extra = 0;
					break;

				case 1 : 
					cmd_info[i].cmd = cmd_info[i].cmd + 3;
					cmd_info[i].extra = 3;
					break;

				case 2 : 
					cmd_info[i].cmd = cmd_info[i].cmd + 2;
					cmd_info[i].extra = 2;
					break;

				case 3 : 
					cmd_info[i].cmd = cmd_info[i].cmd + 1;
					cmd_info[i].extra = 1;
					break;
			}
			cmd_info[i].nb_loop = (cmd_info[i].cmd >> 2) - 1;
		}
	}

/* Compute buffer size */
	buf_size = BUF_HEADER_SIZE;
	for (i = 0;i < nb_cmd;i++)
	{
		buf_size = buf_size + cmd_info[i].cmd + CMD_HEADER_SIZE;
/*
		printf("add_data(): buf_size %d xdr sequence length %d cmd %d nb_loop %d\n",buf_size, 
			dev_info->xcmd_dat.xcmd_dat_val[i].xsequence.xsequence_len,cmd_info[i].cmd,cmd_info[i].nb_loop);
*/
	}
	old_buf_size = buf_size;
	if ((buf_size & MASK_BLK_SIZE) != 0)	
		buf_size = (buf_size & 0xFFFFFFE0) + BLOC_SIZE;
#ifdef DC_SANITY
	/* sanity check - keep a copy of the buf_size to compare with later on */
	dc_sanity_bufsize[ind*HIST+pdata->ind_write] = buf_size;
#endif /* DC_SANITY */

/* Get UNIX time */
	gettimeofday(&time,&tzone);
	time_ten = ((time.tv_sec - TIME_OFF) * 10) + (time.tv_usec / 100000);

/* If intermediate level pointer is NULL, allocate memory (in the data buffer)
   for these new command results and init pointer */
#ifdef DEBUG
	printf("INDEX = %d\n", pdata->ind_write);
	printf("IND = %d\n", ind);
#endif
	if (int_array[ind].data_buf[pdata->ind_write] == -1) 
	{
		old_ptr = NULL;
#ifdef DEBUG
		printf("add_data(%s) buf_size %d calling dcmem_alloc()\n",dev_info->xdev_name,buf_size);
#endif
		ptr = (unsigned int *)dcmem_alloc((unsigned char *)addr_data, (char *)addr_alloc, buf_size, alloc_size, semid, &error);
		ptr_base = (unsigned char*)ptr-(unsigned char*)addr_data;
#ifdef DEBUG
		printf("add_data(%s): %p buf_size %d  ptr to base 0x%x returned from dcmem_alloc()\n",dev_info->xdev_name, ptr, buf_size,ptr_base); 
#endif
		if (ptr == NULL) 
		{
			*perr = DcErr_DatBufAllocError;
			return(DS_NOTOK);
		}
	}
	else 
	{
/* Get data collector buffer size and compare it to actual buffer size */
/* add the offset to the addr_ptr to get the data pointer */
		old_ptr = ptr = (unsigned int *)(addr_data + int_array[ind].data_buf[pdata->ind_write]);
		if (ptr[1] != buf_size) 
		{
/* Free the old buffer */
			if (dcmem_free((unsigned char *)addr_alloc, (unsigned char *)addr_data,(unsigned char *)ptr, ptr[1],semid,&error)) 
			{
				*perr = DcErr_CantFreeDataBuffer;
				return(DS_NOTOK);
			}

/* Allocate a new buffer with the right size */
			ptr = (unsigned int *)dcmem_alloc((unsigned char *)addr_data, (char *)addr_alloc, buf_size,alloc_size,semid,&error);
			ptr_base = (unsigned char*)ptr-(unsigned char*)addr_data;
#ifdef DEBUG
			printf("add_data(%s): %p buf_size %d  ptr to base 0x%x returned from dcmem_alloc()\n",dev_info->xdev_name, ptr, buf_size,ptr_base); 
#endif
			if (ptr == NULL) 
			{
				*perr = DcErr_DatBufAllocError;
				return(DS_NOTOK);
			}
		}
		else
		{
			ptr_base =  int_array[ind].data_buf[pdata->ind_write];
		}
	}
/* write instead of pointer the offset to addr_data */
	int_array[ind].data_buf[pdata->ind_write] = ptr_base;

/* Create the buffer in the data collector data buffer */
	ptr[0] = time_ten;
	ptr[1] = buf_size;
	i = 2;
	for (l = 0;l < nb_cmd;l++) 
	{
		tmp_ptr = &(dev_info->xcmd_dat.xcmd_dat_val[l]);
		ptr[i] = tmp_ptr->xcmd;
		if (cmd_info[l].cmd == 0)
			ptr[i + 1] = 0;
		else
			ptr[i + 1] = (cmd_info[l].cmd) + (cmd_info[l].extra << 24);
		ptr[i + 2] = tmp_ptr->xcmd_error;
		i = i + 3;
		k = 0;
		tmp_ptr1 = (unsigned char *)tmp_ptr->xsequence.xsequence_val;
		for (j = 0;j < cmd_info[l].nb_loop;j++) 
		{
#ifndef linux
			ptr[i + j] = (tmp_ptr1[k] << 24) + (tmp_ptr1[k + 1] << 16) + (tmp_ptr1[k + 2] << 8) + (tmp_ptr1[k + 3]);
#else
			ptr[i + j] = (tmp_ptr1[k]) + (tmp_ptr1[k + 1] << 8) + (tmp_ptr1[k + 2] << 16) + (tmp_ptr1[k + 3] << 24);
#endif
			k = k + 4;
		}
		i = i + j;
		switch(cmd_info[l].extra) 
		{
			case 0 : 
#ifndef linux
				ptr[i] = (tmp_ptr1[k] << 24) + (tmp_ptr1[k + 1] << 16) + (tmp_ptr1[k + 2] << 8) + (tmp_ptr1[k + 3]);
#else	
				ptr[i] = (tmp_ptr1[k]) + (tmp_ptr1[k + 1] << 8) + (tmp_ptr1[k + 2] << 16) + (tmp_ptr1[k + 3] << 24);
#endif
				break;

			case 1 :
#ifndef linux
				ptr[i] = (tmp_ptr1[k] << 24) + (tmp_ptr1[k + 1] << 16) + (tmp_ptr1[k + 2] << 8);
#else
				ptr[i] = (tmp_ptr1[k]) + (tmp_ptr1[k + 1] << 8) + (tmp_ptr1[k + 2] << 16);
#endif
				break;

			case 2 :
#ifndef linux
				ptr[i] = (tmp_ptr1[k] << 24) + (tmp_ptr1[k + 1] << 16);
#else	
				ptr[i] = (tmp_ptr1[k]) + (tmp_ptr1[k + 1] << 8);
#endif
				break;

			case 3 :
#ifndef linux
				ptr[i] = (tmp_ptr1[k] << 24);
#else
				ptr[i] = (tmp_ptr1[k]);
#endif
				break;

			case 4 :
				break;
		}
		if (cmd_info[l].cmd != 0)
			i++;
	}
#ifdef DEBUG
	printf("ADDR_DATA = %p PTR = %p\n", addr_data, ptr);
	for (i = 0; i < 5; ++i)
		printf("PTR[%d] = %d\n", i, ptr[i]);
#endif
	

/* Prepare the stucture used to update the device_info part of the data 
   collector (change read and write indexes and time)
   Don't forget that the read and write indexes must be managed as a circular
   buffer indexes */
	old_ind = pdata->ind_write;
	if (pdata->ind_read == 0 && pdata->ind_write == 0) 
	{
		pdata->ind_write++;
		if (pdata->ind_write == HIST)
			pdata->ind_write = 0;
	}
	else 
	{
		pdata->ind_write++;
		if (pdata->ind_write == HIST)
			pdata->ind_write = 0;
		pdata->ind_read++;
		if (pdata->ind_read == HIST)
			pdata->ind_read = 0;
	}

/* For each command, update the command time in device_info part */
	for (i = 0;i < nb_cmd;i++) 
	{
		cmd = dev_info->xcmd_dat.xcmd_dat_val[i].xcmd;
		for (j = 0;j < pdata->nb_cmd;j++) 
		{
			if (cmd == pdata->poll_cmd[j].cmd)
				break;
		}
		pdata->poll_cmd[j].cmd_time = dev_info->xcmd_dat.xcmd_dat_val[i].xcmd_time;
	}
	pdata->time = time_ten;

/* Update device record. If it fails, free the previously allocated memory
   and restore the old pointer in the pointers area. */
	if (update_dev(ind,pdata,&mem,&error)) 
	{
		dcmem_free((unsigned char *)addr_alloc,(unsigned char*)addr_data, (unsigned char *)ptr,buf_size,semid,&error);
// write instead of pointer the offset to addr_data
		if (old_ptr)
			int_array[ind].data_buf[old_ind] = (void *)old_ptr - (void *)addr_data;
		else
			int_array[ind].data_buf[old_ind] = -1;
		*perr = error;
		return(DS_NOTOK);
	}

#ifdef DC_SANITY
/* check the sanity of the dc memory ... */
	dc_sanity_check(&mem, addr_ptr);
#endif /* DC_SANITY */
/* Leave function */
	return(DS_OK);

}

/* dc sanity check - check to see if any of the memory has been overwritten in the dc */

int dc_sanity_check(hash_info *mem_info, char *addr_ptr)
{
	register dc_dev_param *tmp_parray;
        dc_dev_param *array;
        int_level *int_array;
	unsigned int off, buf_size, buf, buf_cmp;
	unsigned int *ptr;
	int i, j;

	tmp_parray = mem_info->parray;
	array = (dc_dev_param *)addr_ptr;
	int_array = (int_level *)&array[mem_info->hash_table_size + mem_info->cellar_size];
	for (i=0; i< (mem_info->hash_table_size+mem_info->cellar_size); i++) 
	{
		if (tmp_parray[i].device_name[0] != 0) 
		{
			j = 0;
			if ((*(int*)(int_array+i)) != 0) 
			{
				while (int_array[i].data_buf[j] != 0 && j < HIST) 
				{
					/* calculate the buf_size from the buf lengths of all 
					   the command results for this device. this should check
					   that all the command results are still sanely stored
					   in the shared memory and not been inadvertantly overwritten
					 */
					off = 2;
					ptr = (unsigned int*)(addr_data + int_array[i].data_buf[j]);
					buf_size = ptr[1];
					buf = buf_cmp = 2;
               				while (buf_cmp < buf_size)
                			{
						buf = buf + (ptr[off + 1] & 0x00FFFFFF) + CMD_HEADER_SIZE;
						buf_cmp = buf;
        					if ((buf_cmp & MASK_BLK_SIZE) != 0) 
						{
                					buf_cmp = (buf_cmp & 0xFFFFFFE0) + BLOC_SIZE;
						}
                                		off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;

                			}
					if (buf_cmp != dc_sanity_bufsize[i*HIST+j]) 
					{
						printf("dc_sanity_check(): %d compare bufsize %d with sanity bufsize %d\n",j,buf_cmp, dc_sanity_bufsize[i*HIST+j]);
						printf("dc sanity check : error detected - dc memory is corrupt for device %s !\n",tmp_parray[i].device_name);
                                        	off = 2;
                                        	ptr = (unsigned int *)(addr_data + int_array[i].data_buf[j]);
                                        	buf = buf_cmp = 2;
                                        	while (buf_cmp < buf_size)
                                        	{
                                                	buf = buf + (ptr[off + 1] & 0x00FFFFFF) + CMD_HEADER_SIZE;
                                                	buf_cmp = buf;
                                                	if ((buf_cmp & MASK_BLK_SIZE) != 0) 
							{
                                                        	buf_cmp = (buf_cmp & 0xFFFFFFE0) + BLOC_SIZE;
                                                	}
                                                	printf("dc_sanity_check(): buf_size %d buf_cmp %d off%d ptr[off+1] %d\n",buf_size,buf_cmp,off,ptr[off+1]);
                                                	off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;
                                        	}
 
 						printf("PANIC ! dc sanity compromised (hint use dc_mfrees to check the dc memory usage)\n");
						//exit(-1);
					}
					j++;
				}
			}
		}
	}

}
