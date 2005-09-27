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
 * File:	dc.h
 *
 * Description:
 *
 * Author(s):	Emanuel Taurel
 *		$Author: jensmeyer $
 *
 * Version:	$Revision: 1.7 $
 *
 * Date:	$Date: 2005-09-27 08:04:04 $
 *
 *****************************************************************************/

#ifndef DC_H
#define DC_H

/**
 @defgroup dcAPI Data Collector API
 @ingroup API
 This functions allow the access to the data collector server.
 */
/**
 @defgroup dcAPIintern Data Collector internal functions
 @ingroup dcAPI
 This functions are only for internal use
 */

#ifndef WIN32
#include "config.h"
#endif /* WIN32 */
#include <SysNumbers.h>

/* Structures definitions */

typedef struct _dc_cmd {
	int cmd;
	DevType argout_type;
	}dc_cmd,*Dc_cmd;

typedef struct _dc_dev {
	char *device_name;
	int poll_interval;
	unsigned int nb_cmd;
	dc_cmd *dev_cmd;
	}dc_dev,*Dc_dev;

typedef struct _dc_error {
	int error_code;
	int dev_error;
	}dc_error,*Dc_error;

#ifdef OLDDC
typedef struct _dc_cmd_dat {
	int cmd;
	long cmd_error;
	unsigned int length;
	char *sequence;
	}dc_cmd_dat,*Dc_cmd_dat;
#else 
typedef struct _dc_cmd_dat {
	int cmd;
	long cmd_error;
	int cmd_time;
	unsigned int length;
	char *sequence;
	}dc_cmd_dat,*Dc_cmd_dat;
#endif /* OLDDC */

typedef struct _dc_dev_dat {
	char *device_name;
	unsigned int nb_cmd;
	dc_cmd_dat *cmd_data;
	}dc_dev_dat,*Dc_dev_dat;

typedef struct _dc_cmd_mretdat{
	long cmd_code;
	DevArgument argout;
	DevType argout_type;
	long *cmd_error;
	}dc_cmd_mretdat;

typedef struct _datco {
	char device_name[DEV_NAME_LENGTH];
	char dc_host_name[HOST_NAME_LENGTH];
	int indice;
	long net_ind;
	}datco;

#ifdef OLDDC
typedef struct _dc_dev_retdat {
	devserver ds_ptr;
	DevArgument argout;
	DevType argout_type;
	long *cmd_error;
	}dc_dev_retdat;
#else
typedef struct _dc_dev_retdat {
	datco *dc_ptr;
	DevArgument argout;
	DevType argout_type;
	long *cmd_error;
	}dc_dev_retdat;
#endif /* OLDDC */

#ifdef OLDDC 
typedef struct _dc_dev_mretdat {
	devserver ds_ptr;
	unsigned int nb_cmd;
	dc_cmd_mretdat *cmd_mretdat;
	}dc_dev_mretdat;
#else
typedef struct _dc_dev_mretdat {
	datco *dc_ptr;
	unsigned int nb_cmd;
	dc_cmd_mretdat *cmd_mretdat;
	}dc_dev_mretdat;
#endif /* OLDDC */

typedef struct _dc_dev_imp {
	char *device_name;
	long dc_access;
	datco *dc_ptr;
	long *dc_dev_error;
	}dc_dev_imp;

typedef struct _dc_dev_free {
	datco *dc_ptr;
	long *dc_dev_error;
	}dc_dev_free;

typedef struct _dom_info {
	char dom_name[DOMAIN_NAME_LENGTH];
	int dom_nb_dev;
	}dom_info;

typedef struct _servinf {
	unsigned int free_mem;
	unsigned int mem;
	int nb_dev;
	int dom_nb;
	dom_info dom_array[20];
	}servinf;

typedef struct _dc_devinf_cmd {
	int devinf_cmd;
	int devinf_argout;
	int devinf_time;
	}dc_devinf_cmd;

typedef struct _dc_devinf {
	dc_devinf_cmd devcmd[10];
	int devinf_nbcmd;
	int devinf_poll;
	unsigned int devinf_diff_time;
	unsigned int devinf_ptr_off;
	unsigned int devinf_data_off;
	unsigned int devinf_data_base;
	int devinf_delta[5];
	}dc_devinf;

typedef struct _dc_datacmd {
	DevType argout_type;
	DevArgument argout;
	int length;
	char *sequence;
	}dc_datacmd;

typedef struct _dc_hist {
	DevArgument argout;
	long time;
	long cmd_error;
	}dc_hist;


/* Functions definitions */


#ifndef __cplusplus
/*
 * OIC version
 */
int _DLLFunc dc_open PT_((dc_dev *poll_dev,unsigned int num_device,dc_error *error));
int _DLLFunc dc_close PT_((char **dev_name,unsigned int num_device,dc_error *error));
int _DLLFunc dc_dataput PT_((dc_dev_dat *dev_data,unsigned int num_device,dc_error *error));

int _DLLFunc dc_import PT_((dc_dev_imp *dc_devimp,unsigned int num_device,long *error));
int _DLLFunc dc_free PT_((dc_dev_free *dc_devfree,unsigned int num_device,long *error));
#ifdef OLDDC
int _DLLFunc dc_devget PT_((devserver ds_ptr,long cmd_code,DevArgument argout,DevType argout_type,long *error));
#else
int _DLLFunc dc_devget PT_((datco *dc_ptr,long cmd_code,DevArgument argout,DevType argout_type,long *error));
#endif /* OLDDC */
int _DLLFunc dc_devgetv PT_((dc_dev_retdat *dev_retdat,unsigned int num_device,long cmd_code,long *error));
int _DLLFunc dc_devgetm PT_((dc_dev_mretdat *dev_mretdat,unsigned int num_device,long *error));
int _DLLFunc dc_info PT_((char *serv_name,servinf *dc_inf,long *error));
int _DLLFunc dc_devall PT_((char *serv_name,char ***devnametab,int *dev_n,long *error));
int _DLLFunc dc_dinfo PT_((char *dev_name,dc_devinf *dc_dev_info,long *error));
int _DLLFunc dc_devinfo PT_((char *serv_name,char *dev_name,dc_devinf *dc_dev_info,long *error));
int _DLLFunc dc_dataconvert PT_((dc_datacmd *data_cmd,unsigned int num_cmd,dc_error *error));
int _DLLFunc dc_devget_history PT_((datco *dc_ptr,long cmd_code,dc_hist *hist_buff,DevType argout_type,long nb_rec,long *error));
int rpc_reconnect_rd PT_( (int ind,long i_net,long *perr));
int dc_rpc_check_clnt_1 PT_( (CLIENT *clnt,char **res,long *perr));
int dc_rpcwr_check_clnt_1 PT_( (CLIENT *clnt,char **res,long *perr));

#else
/*
 * C++ version
 */
extern "C"
{
int _DLLFunc dc_open (dc_dev *poll_dev,unsigned int num_device,dc_error *error);
int _DLLFunc dc_close (char **dev_name,unsigned int num_device,dc_error *error);
int _DLLFunc dc_dataput (dc_dev_dat *dev_data,unsigned int num_device,dc_error *error);
int _DLLFunc dc_import (dc_dev_imp *dc_devimp,unsigned int num_device,long *error);
int _DLLFunc dc_free (dc_dev_free *dc_devfree,unsigned int num_device,long *error);
int _DLLFunc dc_devget (datco *dc_ptr,long cmd_code,DevArgument argout,DevType argout_type,long *error);
int _DLLFunc dc_devgetv (dc_dev_retdat *dev_retdat,unsigned int num_device,long cmd_code,long *error);
int _DLLFunc dc_devgetm (dc_dev_mretdat *dev_mretdat,unsigned int num_device,long *error);
int _DLLFunc dc_info (char *serv_name,servinf *dc_inf,long *error);
int _DLLFunc dc_devall (char *serv_name,char ***devnametab,int *dev_n,long *error);
int _DLLFunc dc_dinfo (char *dev_name,dc_devinf *dc_dev_info,long *error);
int _DLLFunc dc_devinfo (char *serv_name,char *dev_name,dc_devinf *dc_dev_info,long *error);
int _DLLFunc dc_dataconvert (dc_datacmd *data_cmd,unsigned int num_cmd,dc_error *error);
int _DLLFunc dc_devget_history (datco *dc_ptr,long cmd_code,dc_hist *hist_buff,DevType argout_type,long nb_rec,long *error);
int rpc_reconnect_rd(int ind,long i_net,long *perr);
int dc_rpc_check_clnt_1(CLIENT *clnt,char **res,long *perr);
int dc_rpcwr_check_clnt_1(CLIENT *clnt,char **res,long *perr);
}
#endif /* __cplusplus */

/* Error and Warning definitions */
#ifndef WarningError
#define WarningError     63
#endif

#define DcWarn_DataNotUpdated	DcWarnBase + 1

/* Constants definition */

#endif /* _dc_h */
