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
 * File         : dc_devinf.c
 *
 * Project      : Data collector
 *
 * Description  :
 *
 *
 * Author(s)    : E. Taurel
 *                $Author: andy_gotz $
 *
 * Original     : February 1993
 *
 * Version      : $Revision: 1.8 $
 *
 * Date         : $Date: 2009-09-23 11:42:34 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>
#include <DevErrors.h>
#define DC_SERVER 1
#include <dc_xdr.h>
#include <dcP.h>

#include <stdlib.h>

/* Some global variables */

/* Variables defined in dc_svc.c */
extern hash_info 	mem;
extern char 		*addr_ptr,
			*addr_data;
extern int 		dat_size,
			alloc_size,
			ptr_size;

extern int 		req_call;


/**
 * To return to the client a structure with different     
 * information about a device which is defined in the dc 
 * These information are :
 *	- The commands, their argout and the time used to execute them used to poll the device 
 *	- The poll interval
 *	- The offset in the pointer and data shared segment memories
 *	- The time interval between the last six poll 
 *
 * @param rece The pointer to the device name
 * 
 */
dc_devinfx_back *dc_devinfo_1(char **rece)
{
	int i,resu,nb_cmd,ind,ind1,k;
	static dc_devinfx_back retinf;
	static cmd_infox cmd1[10];
	unsigned int off;
	char d_name[60];
	DevLong error;
	int_level *int_array;
	unsigned int *ptr,*tmp_ptr;
	int nb_tot;
	dc_dev_param data;
	dc_dev_param *array;
	unsigned int time_now;
	struct timeval time;
	struct timezone tzone;

/* Miscellaneous initialization */
	retinf.err_code = 0;
	retinf.device.cmd_dev.cmd_dev_len = 0;
	retinf.device.cmd_dev.cmd_dev_val = cmd1;

	nb_tot = mem.hash_table_size + mem.cellar_size;

#ifdef DEBUG
	fprintf(stderr, "Executing dc_devinfo request for %s\n",*rece);
#endif /* DEBUG */

/* Init the number of request to 1 for this call */
	req_call  = 1;

/* Try to retrieve a record in the device_info part of the pointers area
   with the same device name */
	strcpy(d_name,*rece);
	resu = search_dev(d_name,&data,&mem,&ind,&error);

/* Search error */
	if (resu == -1) 
	{
		if (error != DcErr_DeviceNotInPtrsMemory) 
		{
			retinf.err_code = error;	
			return(&retinf);
		}
/* Device does not exists in database */
		else 
		{
			retinf.err_code = DcErr_DeviceNotDefined;
			return(&retinf);
		}
	}

/* The device exists */
/* Init the array of cmd_infox structures in the structure sent back to caller */

	nb_cmd = data.nb_cmd;
	for (i = 0;i <nb_cmd;i++) 
	{
		retinf.device.cmd_dev.cmd_dev_val[i].cmdx = data.poll_cmd[i].cmd;
		retinf.device.cmd_dev.cmd_dev_val[i].cmd_argoutx = data.poll_cmd[i].argout;
		retinf.device.cmd_dev.cmd_dev_val[i].cmd_timex = data.poll_cmd[i].cmd_time;
	}
	retinf.device.cmd_dev.cmd_dev_len = nb_cmd;
		
/* Init polling intervall */
	retinf.device.pollx = data.poll_freq;

/* Are some data available ? */
	if (data.ind_read == 0 && data.ind_write == 0)
		retinf.device.data_base = 0;
	else 
	{
/* Compute the offset to the last pointer for this device */
		off = ((HIST * sizeof(unsigned int *)) * ind) + (nb_tot * sizeof(dc_dev_param));
		retinf.device.ptr_offset = off + (sizeof(unsigned int *) * data.ind_read);

/* Compute offset to data buffer */
		array = (dc_dev_param *)addr_ptr;
		int_array = (int_level *)&array[nb_tot];
		retinf.device.data_offset = (unsigned int)(int_array[ind].data_buf[data.ind_read]);
		ptr = (unsigned int *)(addr_data + retinf.device.data_offset);
		retinf.device.data_base = (unsigned int)addr_data;

/* Compute the interval between the last five records */
		ind1 = data.ind_read;
		for (k = 0;k < 5;k++) 
		{
			ind1--;
			if (ind1 < 0)
				ind1 = HIST - 1;
			if (int_array[ind].data_buf[ind1] == -1) 
			{
				while(k < 5) 
				{
					retinf.device.deltax[k] = 0;
					k++;
				}
				break;
			}
			else 
			{
				tmp_ptr = (unsigned int *)(addr_data + (int)int_array[ind].data_buf[ind1]);
				retinf.device.deltax[k] = ptr[0] - tmp_ptr[0];
				ptr = tmp_ptr;
			}
		}

/* Compute time between the last data update and the actual time */
		gettimeofday(&time,&tzone);
		time_now = ((time.tv_sec - TIME_OFF) * 10) + (time.tv_usec / 100000);
		retinf.device.diff_time = time_now - data.time;
	}

/* Leave server */
	return(&retinf);
}
