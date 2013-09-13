/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2013 by European Synchrotron Radiation Facility,
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
 * File:        m4_xdr.c
 *
 * Project:       
 *
 * Description: xdr_routines for m4 types.4 mirrors device
 *
 * Author(s):   Christine Dominguez
 *		$Author: jkrueger1 $
 *
 * Original:    May 1993
 *
 * Version:	$Revision: 1.5 $
 *
 * Date:	$Date: 2008-04-06 09:07:21 $
 *
 *******************************************************************-*/

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <dev_xdr.h>
#include <m4_xdr.h>


/* D_VAR_LONGFLOATSET */

bool_t
xdr_DevVarLongFloatSet(xdrs, objp)
	XDR *xdrs;
	DevVarLongFloatSet *objp;
{
	if (!xdr_DevLong(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->axis,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->value,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
        }
     	if (!xdr_array(xdrs, (caddr_t *)&objp->valuef,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
  	}
       return (TRUE);
}

/* D_VAR_LONGREAD */


bool_t
xdr_DevVarLongRead(xdrs, objp)
	XDR *xdrs;
	DevVarLongRead *objp;
{
	if (!xdr_DevLong(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->value,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
  	}
       return (TRUE);
}


/* D_VAR_POSREAD */

bool_t
xdr_DevVarPosRead(xdrs, objp)
	XDR *xdrs;
	DevVarPosRead *objp;
{
	if (!xdr_DevLong(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->pos,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
  	}

       return (TRUE);
}


/* D_VAR_AXEREAD */

bool_t
xdr_DevVarAxeRead(xdrs, objp)
	XDR *xdrs;
	DevVarAxeRead *objp;
{
	if (!xdr_DevLong(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->m_pos,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->s_pos,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->m_stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->s_stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->swit,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->clutch,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
  	}

       return (TRUE);
}

/* D_VAR_PARREAD */

bool_t
xdr_DevVarParRead(xdrs, objp)
	XDR *xdrs;
	DevVarParRead *objp;
{
	if (!xdr_DevLong(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->veloc,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->acc,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->fstep,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->hslew,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->backlash,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->stepmode,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->unit,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
  	}

       return (TRUE);
}

/* D_VAR_ERRREAD */


bool_t
xdr_DevVarErrRead(xdrs, objp)
	XDR *xdrs;
	DevVarErrRead *objp;
{
	if (!xdr_DevLong(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_DevLong)) {
	  	return (FALSE);
  	}

       return (TRUE);
}




