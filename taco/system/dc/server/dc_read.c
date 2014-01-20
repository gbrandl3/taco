/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2014 by European Synchrotron Radiation Facility,
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
 * File         : dc_read.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author(s):	  E. Taurel
 *                $Author: andy_gotz $
 *
 * Original     : February 1993
 *
 * Version      : $Revision: 1.7 $
 *
 * Date         : $Date: 2009-09-23 11:42:34 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#define _dc_h
#include <API.h>
#undef _dc_h
#include <dc.h>
#include <DevErrors.h>
#define DC_SERVER 1
#include "dcrd_xdr.h"
#include <dcP.h>

#include <stdlib.h>
#if HAVE_TIME_H
#	include <time.h>
#endif

/* Variables defined in dcrd_svc.c */

extern int shift_dt;
extern int req_call;
extern hash_info mem;
extern char sig_devname[];

extern char 	*addr_ptr,
		*addr_data;

/* Added some global variables. These variables are defined as global to be
   able to free memory after the data have been sent back to the client in
   the dcrd_svc file. */

xresv backv;
xdc *ptr_xdc;
xres *ptr_xres;

mpar_back backm;
xdc *m_ptr_xdc;
xres *m_ptr_xres;
mxres *ptr_mxres;

xresh_mast backh;
xresh *ptr_xresh;

outpar back_def;

int dev_retrieve(char *,int ,int ,xres *,int *);
int dev_mretrieve(char *,int ,int ,dc_dev_param *,int *,unsigned int *,xres *,int *);
int dev_mret(char *,int ,int ,dc_dev_param *,int ,xres *,int *);
int dev_check(char *,int ,int ,unsigned int **,int *,int *);

/**
 * To retrieve from the data collector the result of a single device. Obviously,
 * the device must be polled by the update daemon with the right commands.
 * 
 * @param rece A pointer to a structure of the "xdevget" type 
 *
 * @return This function returns a pointer to a structure of the "xres" type.
 *
 */
xres *dc_devget_1(xdevget *rece)
{
	int i,j,k;
	static xres res;
	static xdc buf;
	int err_code;

/* Miscellaneous initializations */

	res.xerr = 0;
	res.xbufp = &buf;
	res.xbufp->xdc_len = 0;
	strcpy(sig_devname,rece->xdev_name);

#ifdef DEBUG
	printf("\nDc_devget call\n");
	printf("Device name : %s\n",rece->xdev_name);
	printf("Cmd : %d\n",rece->xcmd);
	printf("Argout_type : %d\n",rece->xargout_type);
#endif /* DEBUG */

/* Set the number of request for this call */

	req_call = 1;

/* Call the dev_retrieve function for the device */

	if (dev_retrieve(rece->xdev_name,rece->xcmd,rece->xargout_type,&res,&err_code))
	{
		res.xerr = err_code;
		return(&res);
	}
	res.xerr = err_code;

/* Leave server */

	return(&res);

}



/**
 * To retrieve command result for one device
 *
 * @param dev_name The device name
 * @param cmd The command code
 * @param arg_type The command argout type
 * @param pres A pointer to the structure used to send back data to the caller
 * @param perr The address of an error code
 *
 * @return This function returns 0 if no error occurs. Otherwise, this function
 *    set the error code and returns -1
 *
 */
int dev_retrieve(char *dev_name,int cmd,int arg_type,xres *pres,int *perr)
{
	int resu;
	unsigned int *ptr = NULL;
	struct timeval time;
	struct timezone tzone;
	unsigned int time_ten;
	int_level *int_array;
	int off,found,nb_cmd,buf_size;
	int i,ind;
	dc_dev_param data;
	DevLong error;
	int ptrs_beg;

	ptrs_beg = (mem.hash_table_size + mem.cellar_size) * sizeof(dc_dev_param);
	int_array = (int_level *)&addr_ptr[ptrs_beg];

/* Try to retrieve a record in the device_info part of the pointers area with 
   the same device name */

	resu = search_dev(dev_name,&data,&mem,&ind,&error);

	if (resu == -1)
	{

/* If the device is not in the device_info part of the pointers area, return
   the old error code. If the function retrns other error code, send it back
   to client. */

		if (error = DcErr_DeviceNotInPtrsMemory)
		{
			*perr = DcErr_DeviceNotDefined;
			return(-1);
		}
		else
		{
			*perr = error;
			 return(-1);
		}
	}

/* The device exists
   Return error if the device is a free place on database (nb_cmd = 0) */

	nb_cmd = data.nb_cmd;
	if (nb_cmd == 0)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}

/* The command is used for polling ? */

	for (i = 0;i < nb_cmd;i++)
	{
		if (data.poll_cmd[i].cmd == cmd) 
				break;
	}
	if (i == nb_cmd)
	{
		*perr = DcErr_CommandNotUsedForPoll;
		return(-1);
	}

/* Correct argout_type used by the client ? */

	if (data.poll_cmd[i].argout != arg_type)
	{
		*perr = DcErr_IncompatibleCmdArgumentTypes;
		return(-1);
	}

/* Some data available ? */

	if (data.ind_read == 0 && data.ind_write == 0)
	{
		*perr = DcErr_DataNotYetAvailable;
		return(-1);
	}

/* If the device cmd have just been updated and the new command result not
   yet available */

	ptr = (unsigned int *)(addr_data + int_array[ind].data_buf[data.ind_read]);
	
#ifdef DEBUG
	printf("INDEX = %d\n", data.ind_read);
	printf("ADDR_DATA= %p, PTR = %p\n", addr_data, ptr);
	printf("IND = %d\n", ind);
#endif
	if (int_array[ind].data_buf[data.ind_read] == -1)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}

	
	if (data.time == 0)
	{
		buf_size = ptr[1];
		found = False;
		off = 2;
		while (off < buf_size)
		{
			if (ptr[off] == cmd)
			{
				found = True;
				break;
			}
			else
				off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;
		}

		if (found == False)
		{
			*perr = DcErr_DataNotYetAvailable;
			return(-1);
		}
	}

/* Get UNIX time */
				
	if (data.poll_freq != 0)
	{
		gettimeofday(&time,&tzone);
		time_ten = ((time.tv_sec - TIME_OFF) * 10) + (time.tv_usec / 100000);

/* Verify that the update daemon is still running */

		if (time_ten > (ptr[0] + (data.poll_freq << shift_dt)))
		{
			printf("time_ten %d ptr[0] %d poll_freq %d shift_dt %d\n",time_ten, ptr[0],data.poll_freq, shift_dt);
			*perr = DcErr_DataNotUpdated;
			return(-1);
		}
	}

/* Get the right address */

	off = 2;
	while (ptr[off] != cmd)
	{
		off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;	
		if (off > ptr[1])
		{
			*perr = DcErr_CmdNotInDataBuffer;
			return(-1);
		}
	}

/* Read the error code and leave function if different than 0 and different than
   the warning codes */

	*perr = ptr[off + 2];
	if ((ptr[off + 2] != 0) && (((ptr[off + 2] >> DS_CAT_SHIFT) & DS_CAT_MASK) != WarningError) )
		return(-1);

/* Init parameters for XDR routines */

	pres->xbufp->xdc_len = (ptr[off + 1] & 0x00FFFFFF) - (ptr[off + 1] >> 24);
	pres->xbufp->xdc_val = (char *)&(ptr[off + 3]);


/* Leave function */

	return(0);

}



/**
 * To retrieve from the data collector the result of the same command for 
 * several devices.
 * 
 * @param rece A pointer to a structure of the "xdevgetv" type
 *
 * @return This function returns a pointer to a structure of the "xres" type.
 *
 */
xresv *dc_devgetv_1(xdevgetv *rece)
{
	int i,j,k;
	int err_code;
	int dev_num;
	int cmd;

/* Miscellaneous initializations */

	dev_num = rece->xdevgetv_len;
	backv.xgen_err = 0;
	cmd = rece->xdevgetv_val[0].xcmd;
	strcpy(sig_devname,rece->xdevgetv_val[0].xdev_name);

#ifdef DEBUG
	printf("\nDc_devgetv call for %d devices\n",dev_num);
	for (i = 0;i < dev_num;i++)
	{
		printf("Device name : %s\n",rece->xdevgetv_val[i].xdev_name);
		printf("Cmd : %d\n",rece->xdevgetv_val[i].xcmd);
		printf("Argout_type : %d\n",rece->xdevgetv_val[i].xargout_type);
	}
#endif /* DEBUG */

/* Allocate memory for the array of "xdc" and "xres" structures */

	if ((ptr_xdc = (xdc *)calloc(dev_num,sizeof(xdc))) == NULL)
	{
		backv.xgen_err = DcErr_ServerMemoryAllocation;
		backv.xresa.xresv_len = 0;
		return(&backv);
	}
	if ((ptr_xres = (xres *)calloc(dev_num,sizeof(xres))) == NULL)
	{
		free(ptr_xdc);
		backv.xgen_err = DcErr_ServerMemoryAllocation;
		backv.xresa.xresv_len = 0;
		return(&backv);
	}

/* Init structures used to send back data to the caller */

	backv.xresa.xresv_len = dev_num;
	backv.xresa.xresv_val = ptr_xres;
	for (i = 0;i < dev_num;i++)
	{
		backv.xresa.xresv_val[i].xerr = 0;
		ptr_xdc[i].xdc_len = 0;
		backv.xresa.xresv_val[i].xbufp = &(ptr_xdc[i]);
	}

/* Call the dev_retrieve function for every device */

	for (i = 0;i < dev_num;i++)
	{
		dev_retrieve(rece->xdevgetv_val[i].xdev_name,cmd,rece->xdevgetv_val[i].xargout_type,
			    &(backv.xresa.xresv_val[i]),&err_code);
		backv.xresa.xresv_val[i].xerr = err_code;
	}

/* Init. the number of request for this call */

	req_call = dev_num;

/* Leave server */

	return(&backv);

}




/**
 * To retrieve from the data collector the result of the several commands for 
 * several devices.
 * 
 * @param rece A pointer to a structure of the "mpar" type
 * 
 * @return This function returns a pointer to a structure of the "mpar_back" type.
 *
 */
mpar_back *dc_devgetm_1(mpar *rece)
{
	int i,j,k;
	int err_code;
	int ind;
	int resu;
	int tmp_nbcmd,tmp;
	int dev_num,nb_cmd;
	dc_dev_param ret;
	unsigned int time_ten;

/* Miscellaneous initializations */

	dev_num = rece->mpar_len;
	backm.xgen_err = 0;
	backm.xxres.xxres_len = 0;
	err_code = 0;
	strcpy(sig_devname,rece->mpar_val[0].xdev_name);

#ifdef DEBUG
	printf("\nDc_devgetm call for %d devices\n",dev_num);
	for (i = 0;i < dev_num;i++)
	{
		printf("Device name : %s\n",rece->mpar_val[i].xdev_name);
		nb_cmd = rece->mpar_val[i].mcmd.mcmd_len;
		printf("%d commands for this device\n",nb_cmd);
		for (j = 0;j < nb_cmd;j++)
		{
			printf("Cmd : %d\n",rece->mpar_val[i].mcmd.mcmd_val[j].xcmd);
			printf("Argout_type : %d\n",rece->mpar_val[i].mcmd.mcmd_val[j].xargout_type);
		}
	}
#endif /* DEBUG */

/* Compute the whole number of commands (for all the devices) */ 

	nb_cmd = 0;
	for (i = 0;i < dev_num;i++)
		nb_cmd = nb_cmd + rece->mpar_val[i].mcmd.mcmd_len;

/* Allocate memory for the array of "xdc", "xres" and "mxres" structures */

	if ((m_ptr_xdc = (xdc *)calloc(nb_cmd,sizeof(xdc))) == NULL)
	{
		backv.xgen_err = DcErr_ServerMemoryAllocation;
		backv.xresa.xresv_len = 0;
		return(&backm);
	}
	if ((m_ptr_xres = (xres *)calloc(nb_cmd,sizeof(xres))) == NULL)
	{
		free(m_ptr_xdc);
		backv.xgen_err = DcErr_ServerMemoryAllocation;
		backv.xresa.xresv_len = 0;
		return(&backm);
	}
	if ((ptr_mxres = (mxres *)calloc(dev_num,sizeof(xres))) == NULL)
	{
		free(m_ptr_xdc);
		free(m_ptr_xres);
		backv.xgen_err = DcErr_ServerMemoryAllocation;
		backv.xresa.xresv_len = 0;
		return(&backm);
	}

/* Init structures used to send back data to the caller */

	tmp_nbcmd = 0;
	tmp = 0;
	backm.xxres.xxres_len = dev_num;
	backm.xxres.xxres_val = ptr_mxres;
	for (i = 0;i < dev_num;i++)
	{
		nb_cmd = rece->mpar_val[i].mcmd.mcmd_len;
		backm.xxres.xxres_val[i].mxres_len = nb_cmd;
		backm.xxres.xxres_val[i].mxres_val = &(m_ptr_xres[tmp_nbcmd]);
		for (j = 0;j < nb_cmd;j++)
		{
			backm.xxres.xxres_val[i].mxres_val[j].xerr = 0;
			tmp = j + tmp_nbcmd;
			m_ptr_xdc[tmp].xdc_len = 0;
			backm.xxres.xxres_val[i].mxres_val[j].xbufp = &(m_ptr_xdc[tmp]);
		}
		tmp_nbcmd = tmp_nbcmd + nb_cmd;
	}

/* Call the dev_mretrieve function for every device */

	req_call = 0;
	for (i = 0;i < dev_num;i++)
	{
		nb_cmd = rece->mpar_val[i].mcmd.mcmd_len;
		req_call = req_call + nb_cmd;
		dev_mretrieve(rece->mpar_val[i].xdev_name,
			      rece->mpar_val[i].mcmd.mcmd_val[0].xcmd,
			      rece->mpar_val[i].mcmd.mcmd_val[0].xargout_type,
			      &ret,&ind,&time_ten,
			      &(backm.xxres.xxres_val[i].mxres_val[0]),
			      &err_code);
		backm.xxres.xxres_val[i].mxres_val[0].xerr = err_code;
		if (err_code > 1000)
			ind = -1;
		nb_cmd--;

/* For some error codes, it is not necessary to try for the other commands
   specified for this device */

		if (err_code == DcErr_DatabaseError || err_code == DcErr_DeviceNotDefined || err_code == DcErr_DataNotUpdated)
		{
			k = 1;
			for(j = 0;j < nb_cmd;j++)
			{
				backm.xxres.xxres_val[i].mxres_val[k].xerr = err_code;
				k++;
			}
			continue;
		}

/* Get command result for the remaining commands */

		k = 1;
		for (j = 0;j < nb_cmd;j++)
		{
			err_code = 0;
			dev_mret(rece->mpar_val[i].xdev_name,
				 rece->mpar_val[i].mcmd.mcmd_val[k].xcmd,
				 rece->mpar_val[i].mcmd.mcmd_val[k].xargout_type,
				 &ret,ind,
				 &(backm.xxres.xxres_val[i].mxres_val[k]),
				 &(backm.xxres.xxres_val[i].mxres_val[k].xerr));
			k++;
		}
	}


/* Leave server */

	return(&backm);

}



/**
 * To retrieve command result for one device
 * 
 * @param dev_name The device name
 * @param cmd The command code
 * @param arg_type The command argout type
 * @param pret A pointer to the record retrieved from device_info part of 
 *	       the pointers area
 * @param pind The address where to store the indice in the device-info
 *	       array of the device record
 * @param ptime	The address where to store the UNIX time (in tenth of a second)
 * @param pres 	A pointer to the structure used to send back data to the caller
 * @param perr	The address of an error code
 *
 * @return This function returns 0 if no error occurs. Otherwise, this function
 *    set the error code and returns -1
 *
 */
int dev_mretrieve(char *dev_name,int cmd,int arg_type,dc_dev_param *pret,int *pind,unsigned int *ptime,xres *pres,int *perr)
{
	int resu;
	unsigned int *ptr;
	struct timeval time;
	struct timezone tzone;
	unsigned int time_ten;
	int_level *int_array;
	int off,found,nb_cmd,buf_size;
	int i,ind;
	int ptrs_beg;
	DevLong error;

	ptrs_beg = (mem.hash_table_size + mem.cellar_size) * sizeof(dc_dev_param);
	int_array = (int_level *)&addr_ptr[ptrs_beg];

/* Try to retrieve a record in the device_info part of the pointers area with 
   the same device name */

	resu = search_dev(dev_name,pret,&mem,&ind,&error);

	if (resu == -1)
	{

/* If the device is not in the device_info part of the pointers area, returns 
   the old error code. If the function returns other error code, send it back
   to the client. */

		if (error == DcErr_DeviceNotInPtrsMemory)
		{
			*perr = DcErr_DeviceNotDefined;
			 return(-1);
		}
		else
		{
			*perr = error;
			return(-1);
		}
	}
	*pind = ind;

/* The device exists
   Return error if the device is a free place on database (nb_cmd = 0) */

	nb_cmd = pret->nb_cmd;
	if (nb_cmd == 0)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}

/* The command is used for polling ? */

	for (i = 0;i < nb_cmd;i++)
	{
		if (pret->poll_cmd[i].cmd == cmd) 
				break;
	}
	if (i == nb_cmd)
	{
		*perr = DcErr_CommandNotUsedForPoll;
		return(-1);
	}

/* Correct argout_type used by the client ? */

	if (pret->poll_cmd[i].argout != arg_type)
	{
		*perr = DcErr_IncompatibleCmdArgumentTypes;
		return(-1);
	}

/* Some data available ? */

	if (pret->ind_read == 0 && pret->ind_write == 0)
	{
		*perr = DcErr_DataNotYetAvailable;
		return(-1);
	}

/* If the device cmd have just been updated and the new command result not
   yet available. The ptr pointer is checked in the case of the search_dev 
   function succeed but in between the device has been removed from the dc
   by another process.  */

	ptr = (unsigned int *)(addr_data + int_array[ind].data_buf[pret->ind_read]);
	
	if (ptr == NULL)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}

	
	if (pret->time == 0)
	{
		buf_size = ptr[1];
		found = False;
		off = 2;
		while (off < buf_size)
		{
			if (ptr[off] == cmd)
			{
				found = True;
				break;
			}
			else
				off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;
		}

		if (found == False)
		{
			*perr = DcErr_DataNotYetAvailable;
			return(-1);
		}
	}

/* Get UNIX time */
				
	if (pret->poll_freq != 0)
	{
		gettimeofday(&time,&tzone);
		*ptime = ((time.tv_sec - TIME_OFF) * 10) + (time.tv_usec / 100000);

/* Verify that the update daemon is still running */

		if (*ptime > (ptr[0] + (pret->poll_freq << shift_dt)))
		{
			*perr = DcErr_DataNotUpdated;
			return(-1);
		}
	}

/* Get the right address */

	off = 2;
	while (ptr[off] != cmd)
	{
		off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;	
		if (off > ptr[1])
		{
			*perr = DcErr_CmdNotInDataBuffer;
			return(-1);
		}
	}

/* Read the error code and leave function if different than 0 */

	*perr = ptr[off + 2];
	if ((ptr[off + 2] != 0) && (((ptr[off + 2] >> DS_CAT_SHIFT) & DS_CAT_MASK) != WarningError))
		return(-1);

/* Init parameters for XDR routines */

	pres->xbufp->xdc_len = (ptr[off + 1] & 0x00FFFFFF) - (ptr[off + 1] >> 24);
	pres->xbufp->xdc_val = (char *)&(ptr[off + 3]);

/* Leave function */

	return(0);

}



/**
 * To retrieve command result for one device
 *
 * @param dev_name The device name
 * @param cmd The command code
 * @param arg_type The command argout type
 * @param pret A pointer to the record retrieved from the device_info part
 *	       of the pointers area
 * @param ind The indice of the record in the device-info array
 *	       If the indice is -1, this means that the first retrieve
 *	       function in the dev_mretrieve function fails
 * @param pres A pointer to the structure used to send back data to the caller
 * @param perr The address of an error code
 *
 * @return This function returns 0 if no error occurs. Otherwise, this function
 *    set the error code and returns -1
 *
 */
int dev_mret(char *dev_name,int cmd,int arg_type,dc_dev_param *pret,int ind,xres *pres,int *perr)
{
	unsigned int *ptr;
	struct timeval time;
	struct timezone tzone;
	unsigned int time_ten;
	int_level *int_array;
	int off,found,nb_cmd,buf_size;
	int i;
	int ptrs_beg;
	int resu;
	DevLong error;

/* If ind is -1, redo the search function */

	if (ind == -1)
	{
		resu = search_dev(dev_name,pret,&mem,&ind,&error);
		if (resu == -1)
		{
			*perr = error;
			return(-1);
		}
	}

/* Some init */

	ptrs_beg = (mem.hash_table_size + mem.cellar_size) * sizeof(dc_dev_param);
	int_array = (int_level *)&addr_ptr[ptrs_beg];

/* The command is used for polling ? */

	nb_cmd = pret->nb_cmd;
	for (i = 0;i < nb_cmd;i++)
	{
		if (pret->poll_cmd[i].cmd == cmd) 
				break;
	}
	if (i == nb_cmd)
	{
		*perr = DcErr_CommandNotUsedForPoll;
		return(-1);
	}

/* Correct argout_type used by the client ? */

	if (pret->poll_cmd[i].argout != arg_type)
	{
		*perr = DcErr_IncompatibleCmdArgumentTypes;
		return(-1);
	}

/* Some data available ? */

	if (pret->ind_read == 0 && pret->ind_write == 0)
	{
		*perr = DcErr_DataNotYetAvailable;
		return(-1);
	}

/* If the device cmd have just been updated and the new command result not
   yet available */

	ptr = (unsigned int *)(addr_data + int_array[ind].data_buf[pret->ind_read]);
	
	if (ptr == NULL)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}
	
	if (pret->time == 0)
	{
		buf_size = ptr[1];
		found = False;
		off = 2;
		while (off < buf_size)
		{
			if (ptr[off] == cmd)
			{
				found = True;
				break;
			}
			else
				off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;
		}

		if (found == False)
		{
			*perr = DcErr_DataNotYetAvailable;
			return(-1);
		}
	}

/* Get the right address */

	off = 2;
	while (ptr[off] != cmd)
	{
		off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;	
		if (off > ptr[1])
		{
			*perr = DcErr_CmdNotInDataBuffer;
			return(-1);
		}
	}

/* Read the error code and leave function if different than 0 */

	*perr = ptr[off + 2];
	if ((ptr[off + 2] != 0) && (((ptr[off + 2] >> DS_CAT_SHIFT) & DS_CAT_MASK) != WarningError))
		return(-1);

/* Init parameters for XDR routines */

	pres->xbufp->xdc_len = (ptr[off + 1] & 0x00FFFFFF) - (ptr[off + 1] >> 24);
	pres->xbufp->xdc_val = (char *)&(ptr[off + 3]);


/* Leave function */

	return(0);

}



/**
 * To check that a device is define for the data collector running on this crate.
 * This is used by the multi-machine data collector to retrieve on which system 
 * the device is defined (for the data collector point of view)
 * 
 * @param rece A list of device name
 *
 * @return This function returns a pointer to a structure of the "xres" type.
 *
 */

outpar *dc_devdef_1(imppar *rece)
{
	int i,j,tp;
	int dev_num;
	int d_num;
	int resu;
	int ind;
	dc_dev_param data;
	DevLong error;

/* Miscellaneous initialization */

	dev_num = rece->imppar_len;
	back_def.xgen_err = 0;
	d_num = 0;
	j = 0;
	req_call = 0;
	strcpy(sig_devname,rece->imppar_val[0]);

#ifdef DEBUG
	printf("Dc_devdef call for %d devices\n",dev_num);
	for (i = 0;i < dev_num;i++) 
		printf("Device name : %s\n",rece->imppar_val[i]);
#endif /* DEBUG */

/* Allocate an array for 64 devices in error. This array will be reallocted
   if more than 64 devices are not defined in this data collector */

	if ((back_def.taberr.taberr_val = (xdev_err *)calloc(NB_STRUCT,sizeof(xdev_err))) == NULL)
	{
		back_def.xgen_err = DcErr_ServerMemoryAllocation;
		back_def.taberr.taberr_len = 0;
		return(&back_def);
	}

/* Try to retrieve each device in the device_info part of the pointers area */

	for (i = 0;i < dev_num;i++)
	{
		resu = search_dev(rece->imppar_val[i],&data,&mem,&ind,&error);

/* If the function fails, two case are possible. If the function returns 
   a research algorithm arror, send it back to the client and stop the
   loop. If the function returns an error because the device is not
   registered in the single dc, return this info to the client but don't abort
   the loop */

		if (resu == -1)
		{
			if (error == DcErr_DeviceNotInPtrsMemory)
			{
				back_def.taberr.taberr_val[j].deverr = DcErr_DeviceNotDefined;
				back_def.taberr.taberr_val[j].devnumb = i;
				j++;
			}
			else
			{
				back_def.xgen_err = DcErr_DatabaseError;
				back_def.taberr.taberr_len = 0;
				return(&back_def);
			}
		}

/* The device exists
   Return error if the device is a free place on database (nb_cmd = 0) */

		else
		{
			if (data.nb_cmd == 0)
			{
				back_def.taberr.taberr_val[j].deverr = DcErr_DeviceNotDefined;
		  		back_def.taberr.taberr_val[j].devnumb = i;
				j++;
			}
		}

/* Realloc the device error array if necessary */

		if ((j != 0) && ((j & NB_STRUCT_MASK) == NB_STRUCT_MASK))
		{
			tp = j >> NB_STRUCT_SHIFT;
			if ((back_def.taberr.taberr_val = (xdev_err *)realloc(back_def.taberr.taberr_val,sizeof(xdev_err) * ((tp + 2) << NB_STRUCT_SHIFT))) == NULL)
			{
				back_def.xgen_err = DcErr_ServerMemoryAllocation;
				back_def.taberr.taberr_len = 0;
				return(&back_def);
			}
		}
	}

/* Set the last sent back parameters */

	back_def.taberr.taberr_len = j;

/* Set the number of requests for this call */

	req_call = dev_num;

/* Leave server */

	return(&back_def);
}



/**
 * To retrieve from the data collector the history of a command result.
 *
 * @param rece A pointer to a structure of the "xdevgeth" type
 * 
 * @return This function returns a pointer to a structure of the "xresh_mast"type.     								    *
 *                                                                           *
 */
xresh_mast *dc_devgeth_1(xdevgeth *rece)
{
	int i;
	int err_code;
	int nb_rec;
	int cmd,time;
	int ind,off,next;
	int ret;
	unsigned int *base_adr;
	unsigned int *ptr;

/* Miscellaneous initializations */

	nb_rec = rece->xnb_rec;
	err_code = 0;
	backh.xgen_err = 0;
	cmd = rece->xcmd;
	strcpy(sig_devname,rece->xdev_name);

#ifdef DEBUG
	printf("\nDc_devget_history call for device %s\n",rece->xdev_name);
	printf("Cmd : %d\n",rece->xcmd);
	printf("Argout_type : %d\n",rece->xargout_type);
	printf("Record number : %d\n",rece->xnb_rec);
#endif /* DEBUG */

/* Allocate memory for the array of "xdc" and "xresh" structures */

	if ((ptr_xdc = (xdc *)calloc(nb_rec,sizeof(xdc))) == NULL)
	{
		backh.xgen_err = DcErr_ServerMemoryAllocation;
		backh.xresb.xresh_len = 0;
		return(&backh);
	}
	if ((ptr_xresh = (xresh *)calloc(nb_rec,sizeof(xresh))) == NULL)
	{
		free(ptr_xdc);
		backh.xgen_err = DcErr_ServerMemoryAllocation;
		backh.xresb.xresh_len = 0;
		return(&backh);
	}

/* Init structures used to send back data to the caller */

	backh.xresb.xresh_len = nb_rec;
	backh.xresb.xresh_val = ptr_xresh;
	for (i = 0;i < nb_rec;i++)
	{
		backh.xresb.xresh_val[i].xerr = 0;
		ptr_xdc[i].xdc_len = 0;
		backh.xresb.xresh_val[i].xbufp = &(ptr_xdc[i]);
	}

/* Call the dev_check function for the device */

	if (dev_check(rece->xdev_name,cmd,rece->xargout_type, &base_adr,&ind,&err_code) == -1)
	{
		if (err_code != DcWarn_DataNotUpdated)
		{
			free(ptr_xdc);
			free(ptr_xresh);
			backh.xgen_err = err_code;
			backh.xresb.xresh_len = 0;
			return(&backh);
		}
	}
		
/* Get all the command result */

	for (i = 0;i < nb_rec;i++)
	{
		ptr = (unsigned int *)(addr_data + (unsigned int)base_adr[ind]);

		if ((int)base_adr[ind] == -1)
			ptr_xresh[i].xerr = DcErr_DataNotYetAvailable;
		else
		{
			time = ptr[0];
			off = 2;
			next = False;
			while(ptr[off] != cmd)
			{
				off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;
				if (off > ptr[1])
				{
					ptr_xresh[i].xerr = DcErr_BadCommandForThisRec;
					next = True;
					break;
				}
			}
			if (next != True)
			{
				if (ptr[off + 2] == 0)
				{
					if (err_code == DcWarn_DataNotUpdated)
						ptr_xresh[i].xerr = DcWarn_DataNotUpdated;
					else
						ptr_xresh[i].xerr = 0;
				}
				else
					ptr_xresh[i].xerr = ptr[off + 2];

				ptr_xresh[i].xtime = time;
				if ((ptr[off + 2] == 0) || (((ptr[off + 2] >> DS_CAT_SHIFT) & DS_CAT_MASK) == WarningError))
				{
					ptr_xresh[i].xbufp->xdc_len = (ptr[off + 1] & 0x00FFFFFF) - (ptr[off + 1] >> 24);
					ptr_xresh[i].xbufp->xdc_val = (char *)&(ptr[off + 3]);
				}
			}
		}
		ind--;
		if (ind < 0)
			ind = HIST - 1;
	}
					
/* Init. the number of request for this call */

	req_call = nb_rec;

/* Leave server */

	return(&backh);

}



/**
 * To retrieve command result for one device
 * 
 * @param dev_name The device name
 * @param cmd The command code
 * @param arg_type The command argout type
 * @param pbase_adr The address where to store the base address of the device
 *	       pointer area
 * @param pind The address where to store the read index in the device
 *	       pointer area
 * @param perr The address of an error code
 *
 * @return This function returns 0 if no error occurs. Otherwise, this function 
 *    set the error code and returns -1
 *
 */
int dev_check(char *dev_name,int cmd,int arg_type,unsigned int **pbase_adr,int *pind,int *perr)
{
	int resu;
	unsigned int *ptr;
	struct timeval time;
	struct timezone tzone;
	unsigned int time_ten;
	int_level *int_array;
	int off,found,nb_cmd,buf_size;
	int i,ind;
	dc_dev_param data;
	DevLong error;
	int ptrs_beg;

	ptrs_beg = (mem.hash_table_size + mem.cellar_size) * sizeof(dc_dev_param);
	int_array = (int_level *)&addr_ptr[ptrs_beg];

/* Try to retrieve a record in the device_info part of the pointers area with 
   the same device name */

	resu = search_dev(dev_name,&data,&mem,&ind,&error);

	if (resu == -1)
	{

/* If the device is not in the device_info part of the pointers area, return
   the old error code. If the function retrns other error code, send it back
   to client. */

		if (error = DcErr_DeviceNotInPtrsMemory)
		{
			*perr = DcErr_DeviceNotDefined;
			return(-1);
		}
		else
		{
			*perr = error;
			 return(-1);
		}
	}

/* Init. caller parameters */

	*pbase_adr = (unsigned int *)int_array[ind].data_buf;
	*pind = data.ind_read;

/* The device exists
   Return error if the device is a free place on database (nb_cmd = 0) */

	nb_cmd = data.nb_cmd;
	if (nb_cmd == 0)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}

/* The command is used for polling ? */

	for (i = 0;i < nb_cmd;i++)
	{
		if (data.poll_cmd[i].cmd == cmd) 
				break;
	}
	if (i == nb_cmd)
	{
		*perr = DcErr_CommandNotUsedForPoll;
		return(-1);
	}

/* Correct argout_type used by the client ? */

	if (data.poll_cmd[i].argout != arg_type)
	{
		*perr = DcErr_IncompatibleCmdArgumentTypes;
		return(-1);
	}

/* Some data available ? */

	if (data.ind_read == 0 && data.ind_write == 0)
	{
		*perr = DcErr_DataNotYetAvailable;
		return(-1);
	}

/* If the device cmd have just been updated and the new command result not
   yet available */

	ptr = (unsigned int *)(addr_data + int_array[ind].data_buf[data.ind_read]);
	
	if (ptr == NULL)
	{
		*perr = DcErr_DeviceNotDefined;
		return(-1);
	}
	
	if (data.time == 0)
	{
		buf_size = ptr[1];
		found = False;
		off = 2;
		while (off < buf_size)
		{
			if (ptr[off] == cmd)
			{
				found = True;
				break;
			}
			else
				off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;
		}

		if (found == False)
		{
			*perr = DcErr_DataNotYetAvailable;
			return(-1);
		}
	}

/* Get the right address */

	off = 2;
	while (ptr[off] != cmd)
	{
		off = off + ((ptr[off + 1] & 0x00FFFFFF) >> 2) + 3;	
		if (off > ptr[1])
		{
			*perr = DcErr_CmdNotInDataBuffer;
			return(-1);
		}
	}

/* Get UNIX time */
				
	if (data.poll_freq != 0)
	{
		gettimeofday(&time,&tzone);
		time_ten = ((time.tv_sec - TIME_OFF) * 10) + (time.tv_usec / 100000);

/* Verify that the update daemon is still running */

		if (time_ten > (ptr[0] + (data.poll_freq << shift_dt)))
		{
			*perr = DcWarn_DataNotUpdated;
			return(-1);
		}
	}

/* Leave function */

	return(0);

}
