/*+*******************************************************************

 File:          seism_xdr.h

 Project:       Device Servers with sun-rpc

 Description:   Include file for XDR definitions
		of the Seismic Survey System data types.

 Author(s):  	Jean Michel Chaize

 Original:	August 1992


 $Revision: 1.1 $
 $Date: 2003-04-25 11:21:47 $

 $Author: jkrueger1 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility,PI
		       Grenoble, France

*******************************************************************-*/

#ifndef _seism_xdr_h
#define _seism_xdr_h

struct DevSeismEvent {
	int   nsta;
        short weight[16];
	short coincid_sum;
	short pre_eventtime;
	short post_eventtime;
        char  tape_id;
        char  file_num[3];
	float t_beg;
	float t_ana;
	int   win_type;
	float win_width;
        int   inst_cor_flag;
	float rms_fmin;
	float rms_fmax;
	int   smooth_flag;
	float smo_width;
	float freq_inc;
	int   nspecout;
	float out_freqinc;
		     };
typedef struct DevSeismEvent 	DevSeismEvent;
bool_t 	_DLLFunc xdr_DevSeismEvent PT_((XDR *xdrs, DevSeismEvent *objp));
#define D_SEISM_EVENT           48
#define LOAD_SEISM_EVENT(A)     xdr_load_type ( D_SEISM_EVENT, \
                                                xdr_DevSeismEvent, \
                                                sizeof(DevSeismEvent), \
                                                NULL, \
                                                A )

struct DevSeismStat {
                char comp;
		DevString sismo_type;
		float n_lat;
		float e_lon;
		float elev;
		float freq_prop;
		float damping;
		float gain;
		float sampling;
		float transduct;
		float conv;
		DevString pos;
		float high_pass;
		float tr_mode;
		float tr_hpass;
		float tr_lpass;
		float tr_sta;
		float tr_lta;
		int tr_stalta;
		int trigger;
		int v_test;
		float vmin_a;
		float vmax_a;
		float vmin_w;
		float vmax_w;
		float dmin;
		float dmax;
		float rms_time;
		float rms_spec;
		    };
typedef struct DevSeismStat 	DevSeismStat;
bool_t 	_DLLFunc xdr_DevSeismStat PT_((XDR *xdrs, DevSeismStat *objp));
#define D_SEISM_STAT            49
#define LOAD_SEISM_STAT(A)      xdr_load_type ( D_SEISM_STAT, \
                                                xdr_DevSeismStat, \
                                                sizeof(DevSeismStat), \
                                                NULL, \
                                                A )

#endif /* _seism_xdr_h */
