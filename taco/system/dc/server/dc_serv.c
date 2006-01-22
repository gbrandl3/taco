#include "config.h"
#include <API.h>
#include <DevErrors.h>
#define DC_SERVER 1
#include <dc_xdr.h>
#include <dcP.h>

#include <stdlib.h>

/* Variables defined in dc_svc.c */

extern hash_info mem;
extern char *addr_ptr,*addr_alloc,*addr_data;

extern int semid;
extern int req_call;

/****************************************************************************
*                                                                           *
*		Server code for dc_open function                            *
*                               -------                                     *
*                                                                           *
*    Function rule : To initialize in the data collector database part all  *
*		     the information needed for further dc_dataput and      *
*		     of course, dc_devget.. functions.			    *
*		     files) a resource value                                *
*                                                                           *
*    Argin : A pointer to a structure of the "arr1" type                    *
*            The definition of the arr1 type is :                           *
*            struct {                                                       *
*              u_int arr1_len;     The number of strings                    *
*              char **arr1_val;    A pointer to the array of strings        *
*                  }                                                        *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;   A structure of the arr1 type (see above) with the    *
*                     resources values information transferred as strings   *
*      int db_err;    The database error code                               *
*                     0 if no error                                         *
*          }                                                                *
*                                                                           *
*****************************************************************************/


dc_xdr_error *dc_open_1(rece)
dc_open_in *rece;
{
	int num_dev;
	int i,j,nb_cmd;
	dc_dev_x *ptr;
	dc_cmd_x *tmp;
	int err_code;
	static dc_xdr_error ret_err;

/* Miscellaneous initialization */

	num_dev = rece->dc_open_in_len;
	err_code = 0;
	ret_err.error_code = 0;
	ret_err.dev_error = 0;

#ifdef DEBUG
	printf("\nDc_open call for %d devices\n",rece->dc_open_in_len);
	for (i = 0;i < num_dev;i++) {
		ptr = &(rece->dc_open_in_val[i]);
		printf("\nDevice name : %s\n",ptr->dev_name);
		printf("Polling interval : %d\n",ptr->poll_int);
		nb_cmd = ptr->dc_cmd_ax.dc_cmd_ax_len;
		printf("Number of commands : %d\n",nb_cmd);
		tmp = &(ptr->dc_cmd_ax.dc_cmd_ax_val[0]);
		if (nb_cmd > MAX_CMD)
			nb_cmd = MAX_CMD;
		for (j = 0;j <nb_cmd;j++) {
			printf("Command code : %d\n",tmp->cmd);
			printf("Argout type : %d\n",tmp->cmd_argout);
			tmp++;
					  }
				    }
#endif /* DEBUG */

/* Init. the request number */

	req_call = num_dev;

/* Call the dev_open function for every device */

	for (i = 0;i < num_dev;i++) {
		if (dev_open(&(rece->dc_open_in_val[i]),&err_code)) {
			ret_err.error_code = err_code;
			ret_err.dev_error = i + 1;
			return(&ret_err);
				}
					}

/* Leave server */

#ifdef DEBUG
	printf("end dc_open\n");
#endif /* DEBUG */
	return(&ret_err);

}





/****************************************************************************
*                                                                           *
*		Code for dev_open function                                  *
*                        --------                                           *
*                                                                           *
*    Function rule : To initialize or update in pointers area/device info   *
*		     part of the data collector the device information	    *
*		     This function is called for every device		    *
*                                                                           *
*    Argin : - A pointer to the structure which contains the device         *
*	     information (dc_dev_x type)				    *
*	     - A pointer to the error code				    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no error occurs. Otherwise, this function   *
*    set the error code and returns -1					    *
*                                                                           *
*****************************************************************************/

int dev_open(dev_info,perr)
dc_dev_x *dev_info;
int *perr;
{
	int resu;
	char d_name[60];
	dc_dev_param data;
	int ind;
	long error;

/* Check that command number is correct */

	if (dev_info->dc_cmd_ax.dc_cmd_ax_len == 0) {
		*perr = DcErr_NoCmdForDevice;
		return(-1);
						    }

	if (dev_info->dc_cmd_ax.dc_cmd_ax_len > MAX_CMD) {
		*perr = DcErr_TooManyCmdForDevice;
		return(-1);
						    }

/* Copy device name in the d_name array */

	d_name[0] = 0;
	strcpy(d_name,dev_info->dev_name);

/* Try to retrieve a record in the pointers area/device info with the same 
   device name */

	resu = search_dev(d_name,&data,&mem,&ind,&error);

	if (resu == -1) {
 
/* If the device is not in the pointers area/device info, it is a new device.
   If the function returns other error code, sent it back to client */

		if (error == DcErr_DeviceNotInPtrsMemory) {
			if (add_device(dev_info,d_name,perr)) 
				return(-1);
								}
		else {
			 *perr = error;
			 return(-1);
		     }
			}

/* The search_dev function does not return error so, the device is already
   defined in the data collector. Just update the device information */

	else {
		if (upd_device(dev_info,d_name,ind,&data,perr))
				return(-1);
	     }

	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for add_device function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To add a new device in the pointers/device_info area   *
*                                                                           *
*    Argin : - A pointer to the structure which contains the device         *
*	     information (dc_dev_x type)				    *
*	     - A pointer to the device name			    	    *
*            - A pointer to the error code				    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no error occurs. Otherwise, this function   *
*    set the error code and returns -1					    *
*                                                                           *
*****************************************************************************/

int add_device(dev_inf,d_name,perr)
dc_dev_x *dev_inf;
char *d_name;
int *perr;
{
	int i,j,resu;
	dc_dev_param data;
	int dev_number;
	long error;

/* A special case with no meaning : adding a device but with 0 cmd !! */

	if (dev_inf->dc_cmd_ax.dc_cmd_ax_len == 0) {
		*perr = 0;
		return(0);
						   }

/* Init device information stucture */

	strcpy(data.device_name,d_name);
	data.dev_name_length = strlen(d_name);
	data.nb_cmd = dev_inf->dc_cmd_ax.dc_cmd_ax_len;
	for (i = 0; i < data.nb_cmd;i++) {
		data.poll_cmd[i].cmd = dev_inf->dc_cmd_ax.dc_cmd_ax_val[i].cmd;
		data.poll_cmd[i].argout = dev_inf->dc_cmd_ax.dc_cmd_ax_val[i].cmd_argout;
		data.poll_cmd[i].cmd_time = 0;
					}
	for (j = i; j < MAX_CMD;j++) {
		data.poll_cmd[j].cmd = 0;
		data.poll_cmd[j].argout = 0;
		data.poll_cmd[j].cmd_time = 0;
					}
	data.poll_freq = dev_inf->poll_int;
	data.ind_read = data.ind_write = 0;
	data.time = 0;

/* Insert this new device into the pointers/device_info area */

	resu = insert_dev(d_name,&data,&mem,&error);
	if (resu == -1) {
		*perr = error;
		return(-1);
			}
	else
		return(0);

}



/****************************************************************************
*                                                                           *
*		Code for upd_device function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To update device information which are stored in       *
*		     the DCINF database table				    *
*                                                                           *
*    Argin : - A pointer to the structure which contains the device         *
*	     information (dc_dev_x type)				    *
*	     - A pointer to the device name			    	    *
*	     - The indice in the device_info area of the device structure   *
*	     - The address of the structure returned by the previous call   *
*	       the serching function					    *
*            - A pointer to the error code				    *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no error occurs. Otherwise, this function   *
*    set the error code and returns -1					    *
*                                                                           *
*****************************************************************************/

int upd_device(dev_inf,d_name,ind,pdata,perr)
dc_dev_x *dev_inf;
char *d_name;
int ind;
dc_dev_param *pdata;
int *perr;
{
	dc_dev_param reta;
	int i,j;
	long error;

/* Build the new device information structure */

	strcpy(reta.device_name,d_name);
	reta.dev_name_length = strlen(d_name);
	reta.nb_cmd = dev_inf->dc_cmd_ax.dc_cmd_ax_len;
	for (i = 0; i < reta.nb_cmd;i++) {
		reta.poll_cmd[i].cmd = dev_inf->dc_cmd_ax.dc_cmd_ax_val[i].cmd;
		reta.poll_cmd[i].argout = dev_inf->dc_cmd_ax.dc_cmd_ax_val[i].cmd_argout;
		reta.poll_cmd[i].cmd_time = 0;
					}
	for (j = i; j < MAX_CMD;j++) {
		reta.poll_cmd[j].cmd = 0;
		reta.poll_cmd[j].argout = 0;
		reta.poll_cmd[j].cmd_time = 0;
					}
	reta.poll_freq = dev_inf->poll_int;
	reta.ind_read = pdata->ind_read;
	reta.ind_write = pdata->ind_write;
	reta.time = 0;

/* Update the device_info pointers area */

	if (update_dev(ind,&reta,&mem,&error)) {
		*perr = error;
		return(-1);
					       }

	return(0);

}



/****************************************************************************
*                                                                           *
*		Server code for dc_close function                           *
*                               --------                                    *
*                                                                           *
*    Function rule : To unregister a list of devices from the data collector*
*		     After this call, it will not be possible to store      *
*		     device command result in the data collector	    *
*                                                                           *
*    Argin : The name of the device server                                  *
*            The definition of the nam type is :                            *
*            typedef char *nam;                                             *
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns a pointer to a structure of the "db_res" type.   *
*    The definition of this structure is :                                  *
*    struct {                                                               *
*      arr1 rev_val;   A structure of the arr1 type (see above) with the    *
*                     devices names                                         *
*      int db_err;    The database error code                               *
*                     0 if no error                                         *
*            }                                                              *
*                                                                           *
****************************************************************************/

dc_xdr_error *dc_close_1(dev_name)
register name_arr *dev_name;
{
	int dev_number;
	int i;
	static dc_xdr_error ret_err;
	int err_code;
	int ptrs_beg;

	dev_number = dev_name->name_arr_len;
	ret_err.error_code = 0;
	ret_err.dev_error = 0;

#ifdef DEBUG
	for (i = 0; i < dev_number;i++)
		printf("Device name : %s\n",dev_name->name_arr_val[i]);
#endif /* DEBUG */

/* Init. the request number for this call and compute the offset from
   the pointers memory shared momory to the beginning of the real pointers 
   area */

	req_call = dev_number;
	ptrs_beg = (mem.hash_table_size + mem.cellar_size) * sizeof(dc_dev_param);

/* Call the dev_remove function for every device */

	for (i = 0;i < dev_number;i++) {
		if (dev_remove(dev_name->name_arr_val[i],ptrs_beg,&err_code)) {
			ret_err.error_code = err_code;
			ret_err.dev_error = i + 1;
			return(&ret_err);
				}
					}
/* Leave server */

#ifdef DEBUG
	printf("end dc_close\n");
#endif /* DEBUG */
	return(&ret_err);

}



/****************************************************************************
*                                                                           *
*		Code for dev_remove function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To unregister a device from the data collector         *
*                                                                           *
*    Argin : - A pointer to the device name				    *
*	     - A pointer to the error code				    *
*	     - The offset from the pointers shared memory segment beginning *
*	       to the beginning of the pointers themself		    * 
*                                                                           *
*    Argout : No argout                                                     *
*                                                                           *
*    This function returns 0 if no error occurs. Otherwise, this function   *
*    set the error code and returns -1					    *
*                                                                           *
*****************************************************************************/

int dev_remove(dev_name,ptrs_beg,perr)
char *dev_name;
int ptrs_beg;
int *perr;
{
	int resu;
	long error;
	unsigned int *ptr;
	int ind,size;
	int_level *int_array;
	dc_dev_param data;
	int i;

#ifdef DEBUG
	printf("begin dev_remove\n");
#endif /* DEBUG */
/* Init pointer to intermediate level pointers */

	int_array = (int_level *)&addr_ptr[ptrs_beg];

/* Try to retrieve a device in the device_info area with the same device
   name */

	resu = search_dev(dev_name,&data,&mem,&ind,&error);
	if (resu == -1) {
#ifdef DEBUG
		printf("endf (search_dev) dev_remove\n");
#endif /* DEBUG */
		if (error == DcErr_DeviceNotInPtrsMemory) {
			*perr = DcErr_DeviceNotDefined;
			return(-1);
							}
		else {
			*perr = error;
			return(-1);
		     }
			}

/* The device exists, so do the work
   Free the data buffers used for this device */

	for (i = 0;i < HIST;i++) {
		ptr = int_array[ind].data_buf[i];
		if (ptr == NULL)
			break;
		size = ptr[1];
		if (dcmem_free((unsigned char*)addr_alloc,(unsigned char *)addr_data,
			        (unsigned char *)ptr,size,semid,&error)) {
#ifdef DEBUG
			printf("endf (dcmem_free) dev_remove\n");
#endif /* DEBUG */
			*perr = DcErr_CantFreeDataBuffer;
			return(-1);
								}
		int_array[ind].data_buf[i] = NULL;
				}

/* Delete the device from the device_info area */

	if (delete_dev(dev_name,&mem,&error)) {
#ifdef DEBUG
		printf("endf (delete_dev) dev_remove\n");
#endif /* DEBUG */
		*perr = error;;
		return(-1);
						}

/* Leave function */

#ifdef DEBUG
	printf("end dev_remove\n");
#endif /* DEBUG */
	return(0);

}
