
/********************************************************************

 File:          m4_xdr.c

 Project:       4 mirrors device

 Description:   xdr_routines for m4 types.

 Author(s):     Christine Dominguez

 Original:      May 1993

Copyright (c) 1990 by European Synchrotron Radiation Facility,
		      Grenoble, France

*******************************************************************-*/

#include <dev_xdr.h>
#include <m4_xdr.h>


/* D_VAR_LONGFLOATSET */

bool_t
xdr_DevVarLongFloatSet(xdrs, objp)
	XDR *xdrs;
	DevVarLongFloatSet *objp;
{
	if (!xdr_long(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->axis,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->value,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
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
	if (!xdr_long(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->value,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
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
	if (!xdr_long(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->pos,
		(u_int *)&objp->length, MAXU_INT, sizeof(float), (xdrproc_t)xdr_float)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
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
	if (!xdr_long(xdrs, &objp->length)) {
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
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->s_stat,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->swit,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->clutch,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
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
	if (!xdr_long(xdrs, &objp->length)) {
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
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->unit,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
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
	if (!xdr_long(xdrs, &objp->length)) {
		return (FALSE);
     	}
     	if (!xdr_array(xdrs, (caddr_t *)&objp->err,
		(u_int *)&objp->length, MAXU_INT, sizeof(long), (xdrproc_t)xdr_long)) {
	  	return (FALSE);
  	}

       return (TRUE);
}




