/*+*******************************************************************

 File:          vgc_xdr.c

 Project:       Device Servers with sun-rpc

 Description:   xdr_routines for vacuum gauge controller types.

 Author(s):     Jens Meyer
		$Author: jkrueger1 $

 Original:      January 1991

 Version:	$Revision: 1.2 $

 Date:		$Date: 2004-02-06 13:11:22 $

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <vgc_xdr.h>


bool_t
xdr_DevVgcStat(xdrs, objp)
	XDR *xdrs;
	DevVgcStat *objp;
{
	if (!xdr_opaque(xdrs, (caddr_t)objp->status, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->error, 8)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcStat(objp)
	DevVgcStat *objp;
{
	long	length = 0;

	length = length + 8;
	length = length + 8;

	return (length);
}



bool_t
xdr_DevVgcGauge(xdrs, objp)
	XDR *xdrs;
	DevVgcGauge *objp;
{
	if (!xdr_opaque(xdrs, (caddr_t)objp->status, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->error, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->relay_status, 16)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->type)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->number)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->gauge_status, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->gauge_error, 8)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->pressure)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcGauge (objp)
	DevVgcGauge *objp;
{
	long	length = 0;

	length = length + 8;
	length = length + 8;
	length = length + 16;
        length = length + xdr_length_DevChar (&objp->type);
        length = length + xdr_length_DevChar (&objp->number);
	length = length + 8;
	length = length + 8;
        length = length + xdr_length_DevDouble (&objp->pressure);

	return (length);
}





bool_t
xdr_DevVgcPiraniGauge(xdrs, objp)
	XDR *xdrs;
	DevVgcPiraniGauge *objp;
{
	if (!xdr_char(xdrs, &objp->type)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->number)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->calibration)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->gas_factor)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->status, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->error, 8)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->pressure)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcPiraniGauge (objp)
	DevVgcPiraniGauge *objp;
{
	long	length = 0;

        length = length + xdr_length_DevChar (&objp->type);
        length = length + xdr_length_DevChar (&objp->number);
        length = length + xdr_length_DevChar (&objp->calibration);
        length = length + xdr_length_DevDouble (&objp->gas_factor);
	length = length + 8;
	length = length + 8;
        length = length + xdr_length_DevDouble (&objp->pressure);

	return (length);
}





bool_t
xdr_DevVgcPenningGauge(xdrs, objp)
	XDR *xdrs;
	DevVgcPenningGauge *objp;
{
	if (!xdr_char(xdrs, &objp->type)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->number)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->filter)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->start_time)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->calibration)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->min_current)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->status, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->error, 8)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->pressure)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcPenningGauge (objp)
	DevVgcPenningGauge *objp;
{
	long	length = 0;

        length = length + xdr_length_DevChar (&objp->type);
        length = length + xdr_length_DevChar (&objp->number);
        length = length + xdr_length_DevChar (&objp->filter);
        length = length + xdr_length_DevLong ((long *)&objp->start_time);
        length = length + xdr_length_DevChar (&objp->calibration);
        length = length + xdr_length_DevDouble (&objp->min_current);
	length = length + 8;
	length = length + 8;
        length = length + xdr_length_DevDouble (&objp->pressure);

	return (length);
}





bool_t
xdr_DevVgcRelay(xdrs, objp)
	XDR *xdrs;
	DevVgcRelay *objp;
{
	if (!xdr_char(xdrs, &objp->letter)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->status)) {
		return (FALSE);
	}
	if (!xdr_double(xdrs, &objp->setpoint)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->ass_gauge)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcRelay (objp)
	DevVgcRelay *objp;
{
	long	length = 0;

        length = length + xdr_length_DevChar (&objp->letter);
        length = length + xdr_length_DevChar (&objp->status);
        length = length + xdr_length_DevDouble (&objp->setpoint);
        length = length + xdr_length_DevChar (&objp->ass_gauge);

	return (length);
}




bool_t
xdr_DevVgcSystem(xdrs, objp)
	XDR *xdrs;
	DevVgcSystem *objp;
{
	if (!xdr_char(xdrs, &objp->interlock)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->relay_conf)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->cold_cathode)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->ROM_version, 4)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcSystem (objp)
	DevVgcSystem *objp;
{
	long	length = 0;

        length = length + xdr_length_DevChar (&objp->interlock);
        length = length + xdr_length_DevChar (&objp->relay_conf);
        length = length + xdr_length_DevChar (&objp->cold_cathode);
	length = length + 4;

	return (length);
}




bool_t
xdr_DevVgcController(xdrs, objp)
	XDR *xdrs;
	DevVgcController *objp;
{
	if (!xdr_vector(xdrs, (char *)objp->pirani, 2, sizeof(DevVgcPiraniGauge), (xdrproc_t)xdr_DevVgcPiraniGauge)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->penning, 4, sizeof(DevVgcPenningGauge), (xdrproc_t)xdr_DevVgcPenningGauge)) {
		return (FALSE);
	}
	if (!xdr_vector(xdrs, (char *)objp->relay, 12, sizeof(DevVgcRelay), (xdrproc_t)xdr_DevVgcRelay)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->nb_pir)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->nb_pen)) {
		return (FALSE);
	}
	if (!xdr_int(xdrs, &objp->nb_relay)) {
		return (FALSE);
	}
	if (!xdr_DevVgcSystem(xdrs, &objp->syst)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->status, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->error, 8)) {
		return (FALSE);
	}
	if (!xdr_opaque(xdrs, (caddr_t)objp->relay_status, 16)) {
		return (FALSE);
	}
	return (TRUE);
}

long
xdr_length_DevVgcController (objp)
	DevVgcController *objp;
{
	long    type_length;
	long	length = 0;

	/* length of two DevVgcPiraniGauge structures */
	if ( (type_length = xdr_length_DevVgcPiraniGauge (&objp->pirani[0])) 
		== -1 )
		return (-1);
	length = length + 2 * type_length;

	/* length of four DevVgcPenningGauge structures */
	if ( (type_length = xdr_length_DevVgcPenningGauge (&objp->penning[0])) 
		== -1 )
		return (-1);
	length = length + 4 * type_length;

	/* length of twelve DevVgcRelay structures */
	if ( (type_length = xdr_length_DevVgcRelay (&objp->relay[0])) 
		== -1 )
		return (-1);
	length = length + 12 * type_length;

        length = length + xdr_length_DevLong ((long *)&objp->nb_pir);
        length = length + xdr_length_DevLong ((long *)&objp->nb_pen);
	length = length + xdr_length_DevLong ((long *)&objp->nb_relay);
	if ( (type_length = xdr_length_DevVgcSystem (&objp->syst)) == -1 )
		return (-1);
	length = length + type_length;
	length = length + 8;
	length = length + 8;
	length = length + 16;

	return (length);
}
