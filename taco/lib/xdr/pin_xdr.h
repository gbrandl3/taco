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
 * File:        pin_xdr.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: xdr structure to read without argin argument the whole
 *              state of the Pinhole device.
 *
 * Author(s):  	Gilbert Pepellin
 * 		$Author: jkrueger1 $
 *
 * Original:	June 1993
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2008-04-06 09:07:23 $
 *
 *******************************************************************-*/

#ifndef PIN_XDR_H
#define PIN_XDR_H

/*
 *  Head structure definition;
 */
struct pin_head {
       DevLong main_st; /* The main device state  :
                         DEVON      : all is good for you
                         DEVWARNING : at least on Maxe acces failed or
                                      one basic device is DEVFAULT.
                         DEVFAULT   : fatal error occured. 
                      */

       DevLong dev_diag; /* device diagnostic. When main_st is not DEVON
                         The bits are numbered 0 to 11 from the right and
                          the value 1 corresponds to an error state.

                          bit 0   :  ReadState    Z1 axis failed.
                          bit 1   :  ReadPosition Z1 axis failed. 
                          bit 2   :  ReadSwitches Z1 axis failed.

                          bit 3   :  ReadState    Z2 axis failed.
                          bit 4   :  ReadPosition Z2 axis failed. 
                          bit 5   :  ReadSwitches Z2 axis failed.

                          bit 6   :  ReadState    TX axis failed.
                          bit 7   :  ReadPosition TX axis failed.
                          bit 8   :  ReadSwitches TX axis failed.

                          bit 9   :  ReadState    TT axis failed.
                          bit 10  :  ReadPosition TT axis failed.
                          bit 11  :  ReadSwitches TT axis failed

                      */       

        DevLong tuned;   /* 1 if the whole component is TUNED (ie the three
                            axes are TUNED
                         0 if at least one axis is NOTTUNED
                      */

        DevLong mode;    /* defines the current operation mode 
                           0 if CALIBRATION mode
                           1 if ALIGNMENT    mode
                           2 if OPERATION   mode
                      */

};
typedef struct pin_head 	pin_head ;
bool_t 	_DLLFunc xdr_pin_head  PT_((XDR *xdrs, pin_head *objp));
long   	_DLLFunc xdr_length_pin_head  PT_((pin_head *objp));


/*
  axis structure definition
*/
struct axis_state { 
       DevLong  unit;        /* current unit: STEPS | MM (for Z1,Z2,and TX axes)
                                          STEPS | DEGREE (for TT axis) */  
       DevLong  moving;      /* DEVMOVING | DEVON | DEVFAULT */
       DevFloat mot_pos;    /* motor position in axis current unit */
       DevFloat enc_pos;    /* encoder position in axis current unit */
       DevLong  switch_st;  /* LIMITSOFF | NEGATLIMIT | POSITLIMIT */
       DevLong  tuned;       /* TUNED | NOTTUNED */
};
typedef struct axis_state 	axis_state;
bool_t 	_DLLFunc xdr_axis_state  PT_((XDR *xdrs, axis_state *objp));
long   	_DLLFunc xdr_length_axis_state  PT_((axis_state *objp));


/*
  whole component structure definition
*/
struct DevPinState { 
       pin_head pin_head;
       axis_state z1_state;
       axis_state z2_state;
       axis_state tx_state;
       axis_state tt_state;      
};
typedef struct DevPinState  	DevPinState;
bool_t 	_DLLFunc xdr_DevPinState  PT_((XDR *xdrs, DevPinState *objp));
long   	_DLLFunc xdr_length_DevPinState  PT_((DevPinState *objp));
#define D_PINSTATE_TYPE         67
#define LOAD_PINSTATE_TYPE(A)   xdr_load_type ( D_PINSTATE_TYPE, \
                                                xdr_DevPinState, \
                                                sizeof(DevPinState), \
                                                xdr_length_DevPinState, \
                                                A )
#endif /* _pin_xdr_h */




