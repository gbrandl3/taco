
/*+*******************************************************************

 File:          slit_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the SLIT data types.

 Author(s):  	Timo Mettala

 Original:	September 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1992 by European Synchrotron Radiation Facility,
		       Grenoble, France

*******************************************************************-*/

#ifndef _slit_xdr_h
#define _slit_xdr_h

/*
 *  definitions for slits
 */
 struct pslit_head {
       long pmain_st; /* The main device state ( from the state handler):
                         DEVON : all is good for you
                         DEVWARNING : at least on maxe acces failed or
                                      one basic device is DEVFAULT.
                         DEVFAULT : fatal error occured. */
       long dev_diag; /* device diagnostic. When pmain_st is not DEVON
                         The bits are numbered 0 to 23 from the right and
                          the value 1 corresponds to an error state.

                          bit 0 :  ReadState up blade failed.
                          bit 1 :  ReadPosition up blade failed. 
                          bit 2 :  ReadEncPos up blade failed.
                          bit 3 :  ReadSwitches up blade failed.
                          bit 4 :  ReadBrakeState up blade failed
                          bit 5 :  ReadTemperature up blade failed.
                          bit 6 :  ReadState down blade failed.
                          bit 7 :  ReadPosition down blade failed.
                          bit 8 :  ReadEncPos down blade failed.
                          bit 9 :  ReadSwitches down blade failed.
                          bit 10 : ReadBrakeState down blade failed
                          bit 11 : ReadTemperature down blade failed.
                          bit 12 : ReadState front blade failed.
                          bit 13 : ReadPosition front blade failed.
                          bit 14 : ReadEncPos front blade failed.
                          bit 15 : ReadSwitches front blade failed.
                          bit 16 : ReadBrakeState front blade failed
                          bit 17 : ReadTemperature front blade failed.
                          bit 18 : ReadState back blade failed.
                          bit 19 : ReadPosition back blade failed.
                          bit 20 : ReadEncPos back blade failed.
                          bit 21 : ReadSwitches back blade failed.
                          bit 22 : ReadBrakeState back blade failed
                          bit 23 : ReadTemperature back blade failed.

                      */       


       long unit;    /* the current unit :
                        1: STEPS: 2: MM; 3: MICRONS; 4: ROTATIONS;
                        5: DEGREES; 6: ARC_SEC
                     */ 
};
typedef struct pslit_head pslit_head;
bool_t 	_DLLFunc xdr_pslit_head PT_((XDR *xdrs, pslit_head *objp));


struct blade_state {
       long values_st; /* availability of each value. 1 if not available
                         00001 mot_pos
                         00010 enc_pos
                         00100 temp
                         01000 brake
                         10000 switch
                       */                       
       long mot_moving; /* 1 if the motor is moving 0 if not */
       float mot_pos;   /* in current unit */
       float enc_pos;   /* in current unit */
       float temp;      /* in degrees */
       long  brake;     /* DEVOPEN | DEVCLOSE | DEVFAULT */
       long  switch_st; /* LIMITSOFF | NEGATLIMIT | POSITLIMIT */
       long tuned;      /* TUNED | NOTTUNED */
};
typedef struct blade_state blade_state;
bool_t 	_DLLFunc xdr_blade_state PT_((XDR *xdrs, blade_state *objp));



struct DevBladeState {
       pslit_head pslit_head;
       blade_state up_state;
       blade_state do_state;
       blade_state fr_state;      
       blade_state ba_state;
};
typedef struct DevBladeState  	DevBladeState;
bool_t 	_DLLFunc xdr_DevBladeState PT_((XDR *xdrs, DevBladeState *objp));
#define D_BLADE_STATE           51
#define LOAD_BLADE_STATE(A)     xdr_load_type ( D_BLADE_STATE, \
                                                xdr_DevBladeState, \
                                                sizeof(DevBladeState), \
                                                NULL, \
                                                A )


struct pslit_state{
     long values_st; /* availability of each value. 1 if not available 
                       00000001 : gap
                       00000010 : offset
                       00000100 : temp_1 (up or front)
                       00001000 : temp_2 (down or back )
                       00010000 : brake_1 (up or front)
                       00100000 : brake_2 (down or back)
                       01000000 : switch_1 (up or front)
                       10000000 : switch_2 (down or back)
                       */ 
     float gap;      /* in current unit */
     float offset;   /* in current unit */
     float temp_1;   /* up or front blade in degrees */
     float temp_2;   /* down or back blade in degrees */
     long brake_1;   /* up or front blade: DEVOPEN | DEVCLOSE | DEVFAULT */
     long brake_2;   /* down or back DEVOPEN | DEVCLOSE | DEVFAULT */
     long switch_1;  /* up or front  LIMITSOFF | NEGATLIMIT | POSITLIMIT */
     long switch_2;  /* down or back LIMITSOFF | NEGATLIMIT | POSITLIMIT */
};

typedef struct pslit_state pslit_state; 
bool_t 	_DLLFunc xdr_pslit_state PT_((XDR *xdrs, pslit_state *objp));

struct DevPslitState {
 	pslit_head pslit_head;
        pslit_state vertical;
        pslit_state horizontal;
};
typedef struct  DevPslitState  	DevPslitState;
bool_t 	_DLLFunc xdr_DevPslitState PT_((XDR *xdrs, DevPslitState *objp));
#define D_PSLIT_STATE           52
#define LOAD_PSLIT_STATE(A)     xdr_load_type ( D_PSLIT_STATE, \
                                                xdr_DevPslitState, \
                                                sizeof(DevPslitState), \
                                                NULL, \
                                                A )

#endif /* _slit_xdr_h */

