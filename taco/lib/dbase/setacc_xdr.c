/*
 Author(s):     $Author: andy_gotz $

 Original:      

 Version:       $Revision: 1.4 $

 Date:          $Date: 2005-06-16 20:43:33 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,
                       Grenoble, France

 *  
 */

#include <macros.h>
#include <db_xdr.h>

#if defined (WIN32) 
#include <rpc.h>
#if 0
#include <nm_rpc.h>
#endif

#else
#include <rpc/rpc.h>
#endif

bool_t _WINAPI xdr_nam(XDR *xdrs, nam *objp)
{
	if (!xdr_wrapstring(xdrs, objp))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_arr1(XDR *xdrs, arr1 *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->arr1_val,
			(u_int *)&objp->arr1_len, MAXU_INT,
			sizeof(nam), (xdrproc_t)xdr_nam))
		return (FALSE);
	return (TRUE);
}

/* For db_dev_export version 1 */
bool_t _WINAPI xdr_db_devinfo(XDR *xdrs, db_devinfo *objp)
{
	if (!xdr_string(xdrs, &objp->dev_name, 24))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name, 20))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dev_type, 24))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dev_class, 24))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->p_num))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->v_num))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_tab_dbdev(XDR *xdrs, tab_dbdev *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_dbdev_val,
			(u_int *)&objp->tab_dbdev_len, MAXU_INT,
			sizeof(db_devinfo), (xdrproc_t)xdr_db_devinfo))
		return (FALSE);
	return (TRUE);
}


/* For db_dev_export version 2 */
bool_t _WINAPI xdr_db_devinfo_2(XDR *xdrs, db_devinfo_2 *objp)
{
	if (!xdr_string(xdrs, &objp->dev_name, 24))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name, 20))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dev_type, 24))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dev_class, 24))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->p_num))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->v_num))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->pid))
		return (FALSE);
	return (TRUE);
}



bool_t _WINAPI xdr_tab_dbdev_2(XDR *xdrs, tab_dbdev_2 *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_dbdev_val,
			(u_int *)&objp->tab_dbdev_len, MAXU_INT,
			sizeof(db_devinfo_2), (xdrproc_t)xdr_db_devinfo_2))
		return (FALSE);
	return (TRUE);
}

/* For db_dev_export version 3 */

bool_t _WINAPI xdr_db_devinfo_3(XDR *xdrs, db_devinfo_3 *objp)
{
	if (!xdr_string(xdrs, &objp->dev_name, DEV_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name, HOST_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dev_type, DEV_TYPE_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->dev_class, DEV_CLASS_LENGTH))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->p_num))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->v_num))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->pid))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->proc_name, PROC_NAME_LENGTH))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_tab_dbdev_3(XDR *xdrs, tab_dbdev_3 *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_dbdev_val,
			(u_int *)&objp->tab_dbdev_len, MAXU_INT,
			sizeof(db_devinfo_3), (xdrproc_t)xdr_db_devinfo_3))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_res(XDR *xdrs, db_res *objp)
{
	if (!xdr_arr1(xdrs, &objp->res_val))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_resimp(XDR *xdrs, db_resimp *objp)
{
	if (!xdr_tab_dbdev(xdrs, &objp->imp_dev))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->db_imperr))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_svc_inf(XDR *xdrs, svc_inf *objp)
{
	if (!xdr_string(xdrs,&objp->ho_name, HOST_NAME_LENGTH))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->p_num))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->v_num))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_putres(XDR *xdrs, putres *objp)
{
	if (!xdr_string(xdrs,&objp->res_name, RES_NAME_LENGTH))
		return(FALSE);
	if (!xdr_wrapstring(xdrs,&objp->res_val))
		return(FALSE);
	return (TRUE);
}


bool_t _WINAPI xdr_tab_putres(XDR *xdrs, tab_putres *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_putres_val,
			(u_int *)&objp->tab_putres_len, MAXU_INT,
			sizeof(tab_putres), (xdrproc_t)xdr_putres))
		return(FALSE);
	return (TRUE);
}


bool_t _WINAPI xdr_cmd_que(XDR *xdrs,cmd_que *objp)
{
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->xcmd_code))
		return (FALSE);
	return (TRUE);
}

/*event query */
bool_t _WINAPI xdr_event_que(XDR *xdrs,event_que *objp)
{
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	if (!xdr_u_int(xdrs, &objp->xevent_code))
		return (FALSE);
	return (TRUE);
}

/* For db_psdev_register function */
bool_t _WINAPI xdr_psdev_reg_x(XDR *xdrs,psdev_reg_x *objp)
{
	if (!xdr_u_int(xdrs, &objp->pid))
		return (FALSE);
	if (!xdr_string(xdrs,&objp->h_name, HOST_NAME_LENGTH))
		return(FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->psdev_arr.psdev_arr_val,
			(u_int *)&objp->psdev_arr.psdev_arr_len, MAXU_INT,
			sizeof(psdev_elt), (xdrproc_t)xdr_psdev_elt))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_psdev_elt(XDR *xdrs,psdev_elt *objp)
{
	if (!xdr_string(xdrs, &objp->psdev_name, DS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->poll))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_psdev_error(XDR *xdrs,db_psdev_error *objp)
{
	if (!xdr_long(xdrs, &objp->error_code))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->psdev_err))
		return (FALSE);
	return (TRUE);
}


bool_t _WINAPI xdr_db_devinfo_svc(XDR *xdrs,db_devinfo_svc *objp)
{
	if (!xdr_long(xdrs, &objp->device_type))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->device_exported))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->device_class,DEV_CLASS_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->personal_name,DSPERS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->process_name,PROC_NAME_LENGTH))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->server_version))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->pid))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->program_num))
		return(FALSE);
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_info_dom_svc(XDR *xdrs,db_info_dom_svc *objp)
{
	if (!xdr_string(xdrs, &objp->dom_name,DOMAIN_NAME_LENGTH))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->dom_elt))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_var_dom(XDR *xdrs,var_dom *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->dom_val,
			(u_int *)&objp->dom_len, MAXU_INT,
			sizeof(db_info_dom_svc), (xdrproc_t)xdr_db_info_dom_svc))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_info_svc(XDR *xdrs,db_info_svc *objp)
{
	if (!xdr_long(xdrs, &objp->dev_defined))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->dev_exported))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->psdev_defined))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->res_number))
		return (FALSE);
	if (!xdr_var_dom(xdrs, &objp->dev))
		return (FALSE);
	if (!xdr_var_dom(xdrs, &objp->res))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_svcinfo_dev(XDR *xdrs,svcinfo_dev *objp)
{
	if (!xdr_long(xdrs, &objp->exported_flag))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->name,DEV_NAME_LENGTH))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_svcinfo_server(XDR *xdrs,svcinfo_server *objp)
{
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->dev_val,
			(u_int *)&objp->dev_len, MAXU_INT,
			sizeof(svcinfo_dev), (xdrproc_t)xdr_svcinfo_dev))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_svcinfo_svc(XDR *xdrs,svcinfo_svc *objp)
{
	if (!xdr_string(xdrs, &objp->process_name,PROC_NAME_LENGTH))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->pid))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->program_num))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
		return (FALSE);
	if (!xdr_array(xdrs, (caddr_t *)&objp->embedded_val,
			(u_int *)&objp->embedded_len, MAXU_INT,
			sizeof(svcinfo_server), (xdrproc_t)xdr_svcinfo_server))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_poller_svc(XDR *xdrs,db_poller_svc *objp)
{
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->personal_name,DSPERS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->process_name,PROC_NAME_LENGTH))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->pid))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_delupd_error(XDR *xdrs,db_delupd_error *objp)
{
	if (!xdr_long(xdrs, &objp->type))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->number))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->error))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_arr1_array(XDR *xdrs, db_arr1_array *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->arr,
			(u_int *)&objp->arr_length, MAXU_INT,
			sizeof(arr1), (xdrproc_t)xdr_arr1))
		return (FALSE);
	if (!xdr_int(xdrs, &objp->arr_type))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_svc_net(XDR *xdrs, db_svc_net *objp)
{
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->personal_name,DSPERS_NAME_LENGTH))
		return (FALSE);
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->pid))
		return (FALSE);
	if (!xdr_u_long(xdrs, &objp->program_num))
		return (FALSE);
	return (TRUE);
}

bool_t _WINAPI xdr_db_svcarray_net(XDR *xdrs, db_svcarray_net *objp)
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence,
			(u_int *)&objp->length, MAXU_INT,
			sizeof(db_svc_net), (xdrproc_t)xdr_db_svc_net))
		return (FALSE);
	if (!xdr_long(xdrs, &objp->db_err))
		return (FALSE);
	return (TRUE);
}
