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
 * Version      : $Revision: 1.5 $
 *
 * Date         : $Date: 2007-03-22 13:44:46 $
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
#if HAVE_TIME_H
#	include <time.h>
#endif

/* Variables defined in dc_svc.c */

extern hash_info mem;

extern char *addr_ptr,*addr_alloc,*addr_data;
extern int semid;
extern int req_call;


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
			if (cmd_err == 0) {
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
	printf("end dc_dataput\n");
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
	long error;

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
	int buf_size;
	cmd_sz cmd_info[MAX_CMD];
	int_level *int_array;
	struct timeval time;
	struct timezone tzone;
	unsigned int time_ten;
	unsigned int *ptr;
	cmd_dat *tmp_ptr;
	unsigned char *tmp_ptr1;
	dc_dev_param *array;
	long error;
	unsigned int *old_ptr;
	int old_ind;

/* Init pointer to itermediate level pointers buffer */
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
		    dev_info->xcmd_dat.xcmd_dat_val[i].xcmd_error==0 ) {
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
		buf_size = buf_size + cmd_info[i].cmd + CMD_HEADER_SIZE;
	if ((buf_size & MASK_BLK_SIZE) != 0)	
		buf_size = (buf_size & 0xFFFFFFE0) + BLOC_SIZE;

/* Get UNIX time */
	gettimeofday(&time,&tzone);
	time_ten = ((time.tv_sec - TIME_OFF) * 10) + (time.tv_usec / 100000);

/* If intermediate level pointer is NULL, allocate memory (in the data buffer)
   for these new command results and init pointer */
	old_ptr = int_array[ind].data_buf[pdata->ind_write];
	if (old_ptr == NULL) 
	{
		ptr = (unsigned int *)dcmem_alloc((unsigned char *)addr_data,
					(char *)addr_alloc,
				  	buf_size,ALLOC_SIZE,semid,&error);
		if (ptr == NULL) 
		{
			*perr = DcErr_DatBufAllocError;
			return(DS_NOTOK);
		}
		int_array[ind].data_buf[pdata->ind_write] = ptr;

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
	}
	else 
	{
/* Get data collector buffer size and compare it to actual buffer size */
		ptr = int_array[ind].data_buf[pdata->ind_write];
		if (ptr[1] != buf_size) 
		{
/* Free the old buffer */
			if (dcmem_free((unsigned char *)addr_alloc,
			       (unsigned char *)addr_data,(unsigned char *)ptr,
				ptr[1],semid,&error)) 
			{
				*perr = DcErr_CantFreeDataBuffer;
				return(DS_NOTOK);
			}

/* Allocate a new buffer with the right size */
			ptr = (unsigned int *)dcmem_alloc((unsigned char *)addr_data,
					(char *)addr_alloc,
				  	buf_size,ALLOC_SIZE,semid,&error);
			if (ptr == NULL) 
			{
				*perr = DcErr_DatBufAllocError;
				return(DS_NOTOK);
			}
			int_array[ind].data_buf[pdata->ind_write] = ptr;
		}

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
	}

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
		dcmem_free((unsigned char *)addr_alloc,(unsigned char*)addr_data,
			   (unsigned char *)ptr,buf_size,semid,&error);
		int_array[ind].data_buf[old_ind] = old_ptr;
		*perr = error;
		return(DS_NOTOK);
	}

/* Leave function */
	return(DS_OK);

}
