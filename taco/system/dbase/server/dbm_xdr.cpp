#include <macros.h>
#include <db_xdr.h>

#if defined (_NT) 
#include <nm_rpc.h>
#else
#include <rpc/rpc.h>
#endif

bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_nam(XDR *xdrs, nam *objp)
#else
xdr_nam(xdrs, objp)
	XDR *xdrs;
	nam *objp;
#endif
{
	if (!xdr_wrapstring(xdrs, objp))
	{
		return (FALSE);
	}
	return (TRUE);
}





bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_arr1(XDR *xdrs, arr1 *objp)
#else
xdr_arr1(xdrs, objp)
	XDR *xdrs;
	arr1 *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->arr1_val, (u_int *)&objp->arr1_len, MAXU_INT, sizeof(nam), (xdrproc_t)xdr_nam))
	{
		return (FALSE);
	}
	return (TRUE);
}


/* For db_dev_export version 1 */


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_devinfo(XDR *xdrs, db_devinfo *objp)
#else
xdr_db_devinfo(xdrs, objp)
	XDR *xdrs;
	db_devinfo *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->dev_name, 24))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name, 20))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->dev_type, 24))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->dev_class, 24))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->p_num))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->v_num))
	{
		return (FALSE);
	}
	return (TRUE);
}




bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_tab_dbdev(XDR *xdrs, tab_dbdev *objp)
#else
xdr_tab_dbdev(xdrs, objp)
	XDR *xdrs;
	tab_dbdev *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_dbdev_val,
			(u_int *)&objp->tab_dbdev_len, MAXU_INT,
			sizeof(db_devinfo), (xdrproc_t)xdr_db_devinfo))
	{
		return (FALSE);
	}
	return (TRUE);
}


/* For db_dev_export version 2 */


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_devinfo_2(XDR *xdrs, db_devinfo_2 *objp)
#else
xdr_db_devinfo_2(xdrs, objp)
	XDR *xdrs;
	db_devinfo_2 *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->dev_name, 24))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name, 20))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->dev_type, 24))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->dev_class, 24))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->p_num))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->v_num))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_tab_dbdev_2(XDR *xdrs, tab_dbdev_2 *objp)
#else
xdr_tab_dbdev_2(xdrs, objp)
	XDR *xdrs;
	tab_dbdev_2 *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_dbdev_val,
			(u_int *)&objp->tab_dbdev_len, MAXU_INT,
			sizeof(db_devinfo_2), (xdrproc_t)xdr_db_devinfo_2))
	{
		return (FALSE);
	}
	return (TRUE);
}



/* For db_dev_export version 3 */


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_devinfo_3(XDR *xdrs, db_devinfo_3 *objp)
#else
xdr_db_devinfo_3(xdrs, objp)
	XDR *xdrs;
	db_devinfo_3 *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->dev_name, 24))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name, 20))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->dev_type, 24))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->dev_class, 24))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->p_num))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->v_num))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->proc_name, 40))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_tab_dbdev_3(XDR *xdrs, tab_dbdev_3 *objp)
#else
xdr_tab_dbdev_3(xdrs, objp)
	XDR *xdrs;
	tab_dbdev_3 *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_dbdev_val,
			(u_int *)&objp->tab_dbdev_len, MAXU_INT,
			sizeof(db_devinfo_3), (xdrproc_t)xdr_db_devinfo_3))
	{
		return (FALSE);
	}
	return (TRUE);
}





bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_res(XDR *xdrs, db_res *objp)
#else
xdr_db_res(xdrs, objp)
	XDR *xdrs;
	db_res *objp;
#endif	/* __STDC__ */
{
	if (!xdr_arr1(xdrs, &objp->res_val))
	{
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}




bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_resimp(XDR *xdrs, db_resimp *objp)
#else
xdr_db_resimp(xdrs, objp)
	XDR *xdrs;
	db_resimp *objp;
#endif	/* __STDC__ */
{
	if (!xdr_tab_dbdev(xdrs, &objp->imp_dev))
	{
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_imperr))
	{
		return (FALSE);
	}
	return (TRUE);
}




bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svc_inf(XDR *xdrs, svc_inf *objp)
#else
xdr_svc_inf(xdrs, objp)
	XDR *xdrs;
	svc_inf *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs,&objp->ho_name,20))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->p_num))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->v_num))
	{
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_putres(XDR *xdrs, putres *objp)
#else
xdr_putres(xdrs, objp)
	XDR *xdrs;
	putres *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs,&objp->res_name,80))
	{
		return(FALSE);
	}
	if (!xdr_wrapstring(xdrs,&objp->res_val))
	{
		return(FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_tab_putres(XDR *xdrs, tab_putres *objp)
#else
xdr_tab_putres(xdrs, objp)
	XDR *xdrs;
	tab_putres *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->tab_putres_val,
			(u_int *)&objp->tab_putres_len, MAXU_INT,
			sizeof(tab_putres), (xdrproc_t)xdr_putres))
	{
		return(FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_cmd_que(XDR *xdrs,cmd_que *objp)
#else
xdr_cmd_que(xdrs,objp)
	XDR *xdrs;
	cmd_que *objp;
#endif	/* __STDC__ */
{
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->xcmd_code))
	{
		return (FALSE);
	}
	return (TRUE);
}

/* event query */
bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_event_que(XDR *xdrs,event_que *objp)
#else
xdr_event_que(xdrs,objp)
	XDR *xdrs;
	event_que *objp;
#endif	/* __STDC__ */
{
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	if (!xdr_u_int(xdrs, &objp->xevent_code))
	{
		return (FALSE);
	}
	return (TRUE);
}



/* For db_psdev_register function */


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_psdev_reg_x(XDR *xdrs,psdev_reg_x *objp)
#else
xdr_psdev_reg_x(xdrs,objp)
	XDR *xdrs;
	psdev_reg_x *objp;
#endif	/* __STDC__ */
{
	if (!xdr_u_int(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs,&objp->h_name,20))
	{
		return(FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->psdev_arr.psdev_arr_val,
			(u_int *)&objp->psdev_arr.psdev_arr_len, MAXU_INT,
			sizeof(psdev_elt), (xdrproc_t)xdr_psdev_elt))
	{
		return (FALSE);
	}
	return (TRUE);
}

bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_psdev_elt(XDR *xdrs,psdev_elt *objp)
#else
xdr_psdev_elt(xdrs,objp)
	XDR *xdrs;
	psdev_elt *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->psdev_name,24))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->poll))
	{
		return (FALSE);
	}
	return (TRUE);
}

bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
 xdr_db_psdev_error(XDR *xdrs,db_psdev_error *objp)
#else
xdr_db_psdev_error(xdrs,objp)
	XDR *xdrs;
	db_psdev_error *objp;
#endif	/* __STDC__ */
{
	if (!xdr_long(xdrs, &objp->error_code))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->psdev_err))
	{
		return (FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_devinfo_svc(XDR *xdrs,db_devinfo_svc *objp)
#else
xdr_db_devinfo_svc(xdrs,objp)
	XDR *xdrs;
	db_devinfo_svc *objp;
#endif /* __STDC__ */
{
	if (!xdr_long(xdrs, &objp->device_type))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->device_exported))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->device_class,DEV_CLASS_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->personal_name,DSPERS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->process_name,PROC_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->server_version))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->program_num))
	{
		return(FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_info_dom_svc(XDR *xdrs,db_info_dom_svc *objp)
#else
xdr_db_info_dom(xdrs,objp)
	XDR *xdrs;
	db_info_dom_svc *objp;
#endif /* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->dom_name,DOMAIN_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->dom_elt))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_var_dom(XDR *xdrs,var_dom *objp)
#else
xdr_var_dom(xdrs,objp)
	XDR *xdrs;
	var_dom *objp;
#endif /* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->dom_val,
			(u_int *)&objp->dom_len, MAXU_INT,
			sizeof(db_info_dom_svc), (xdrproc_t)xdr_db_info_dom_svc))
	{
		return (FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_info_svc(XDR *xdrs,db_info_svc *objp)
#else
xdr_db_info_svc(xdrs,objp)
	XDR *xdrs;
	db_info_svc *objp;
#endif /* __STDC__ */
{
	if (!xdr_long(xdrs, &objp->dev_defined))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->dev_exported))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->psdev_defined))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->res_number))
	{
		return (FALSE);
	}
	if (!xdr_var_dom(xdrs, &objp->dev))
	{
		return (FALSE);
	}
	if (!xdr_var_dom(xdrs, &objp->res))
	{
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_dev(XDR *xdrs,svcinfo_dev *objp)
#else
xdr_svcinfo_dev(xdrs,objp)
	XDR *xdrs;
	svcinfo_dev *objp;
#endif /* __STDC__ */
{
	if (!xdr_long(xdrs, &objp->exported_flag))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->name,DEV_NAME_LENGTH))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_server(XDR *xdrs,svcinfo_server *objp)
#else
xdr_svcinfo_server(xdrs,objp)
	XDR *xdrs;
	svcinfo_server *objp;
#endif /* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->dev_val,
			(u_int *)&objp->dev_len, MAXU_INT,
			sizeof(svcinfo_dev), (xdrproc_t)xdr_svcinfo_dev))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_svcinfo_svc(XDR *xdrs,svcinfo_svc *objp)
#else
xdr_svcinfo_svc(xdrs,objp)
	XDR *xdrs;
	svcinfo_svc *objp;
#endif /* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->process_name,PROC_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->program_num))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_array(xdrs, (caddr_t *)&objp->embedded_val,
			(u_int *)&objp->embedded_len, MAXU_INT,
			sizeof(svcinfo_server), (xdrproc_t)xdr_svcinfo_server))
	{
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_poller_svc(XDR *xdrs,db_poller_svc *objp)
#else
xdr_db_poller_svc(xdrs,objp)
	XDR *xdrs;
	db_poller_svc *objp;
#endif /* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->personal_name,DSPERS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->process_name,PROC_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}


bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_delupd_error(XDR *xdrs,db_delupd_error *objp)
#else
xdr_db_delupd_error(xdrs,objp)
	XDR *xdrs;
	db_delupd_error *objp;
#endif /* __STDC__ */
{
	if (!xdr_long(xdrs, &objp->type))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->number))
	{
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->error))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_arr1_array(XDR *xdrs, db_arr1_array *objp)
#else
xdr_db_arr1_array(xdrs, objp)
	XDR *xdrs;
	db_arr1_array *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->arr,
			(u_int *)&objp->arr_length, MAXU_INT,
			sizeof(arr1), (xdrproc_t)xdr_arr1))
	{
		return (FALSE);
	}
	
	if (!xdr_int(xdrs, &objp->arr_type))
	{
		return (FALSE);
	}
	return (TRUE);
}



bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_svc_net(XDR *xdrs, db_svc_net *objp)
#else
xdr_db_svc_net(xdrs, objp)
	XDR *xdrs;
	db_svc_net *objp;
#endif	/* __STDC__ */
{
	if (!xdr_string(xdrs, &objp->server_name,DS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->personal_name,DSPERS_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_string(xdrs, &objp->host_name,HOST_NAME_LENGTH))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->pid))
	{
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->program_num))
	{
		return (FALSE);
	}
	return (TRUE);
}





bool_t _WINAPI
#if defined __STDC__ | defined __STDCPP__
xdr_db_svcarray_net(XDR *xdrs, db_svcarray_net *objp)
#else
xdr_db_svcarray_net(xdrs, objp)
	XDR *xdrs;
	db_svcarray_net *objp;
#endif	/* __STDC__ */
{
	if (!xdr_array(xdrs, (caddr_t *)&objp->sequence,
			(u_int *)&objp->length, MAXU_INT,
			sizeof(db_svc_net), (xdrproc_t)xdr_db_svc_net))
	{
		return (FALSE);
	}
	
	if (!xdr_long(xdrs, &objp->db_err))
	{
		return (FALSE);
	}
	return (TRUE);
}
