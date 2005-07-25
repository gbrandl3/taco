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
 * File:        xdr_typelist.h
 *
 * Project:     Device Servers with sun-rpc
 *
 * Description: Include file with all XDR type definitions
 *		and a global list, specifying a data type.
 *
 * Author(s):  	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	July 1992
 *
 * Version:	$Revision: 1.3 $
 *
 * Date:	$Date: 2005-07-25 13:05:45 $
 *
 *******************************************************************-*/

#ifndef XDR_TYPELIST_H
#define XDR_TYPELIST_H

/*
 *  list of device server types and related xdr_routines
 */

#define D_VOID_TYPE             0
#define D_BOOLEAN_TYPE          1
#define D_SHORT_TYPE            2
#define D_LONG_TYPE             3
#define D_FLOAT_TYPE            4
#define D_DOUBLE_TYPE		5
#define D_STRING_TYPE           6

#define D_FLOAT_READPOINT       7
#define D_STATE_FLOAT_READPOINT 8
#define D_VAR_CHARARR           9
#define D_VAR_SHORTARR          10
#define D_VAR_LONGARR           11
#define D_VAR_FLOATARR          12

#define D_BPM_POSFIELD          13
#define D_BPM_ELECFIELD         14
#define D_WS_BEAMFITPARM        15
#define D_VGC_STATUS		16
#define D_VGC_GAUGE		17
#define D_VGC_CONTROLLER	18
#define D_UNION_TYPE            19

#define D_NEG_STATUS		20
#define D_RAD_DOSE_VALUE	21
#define D_LONG_READPOINT       	22
#define D_DOUBLE_READPOINT      23

#define D_VAR_STRINGARR         24
#define D_VAR_FRPARR            25
#define D_VAR_THARR            	26
#define D_INT_FLOAT_TYPE       	27
#define D_HAZ_STATUS       	28
#define D_VRIF_WDOG       	29
#define D_VRIF_STATUS       	30
#define D_VRIF_POWERSTATUS     	31

#define D_GPIB_WRITE     	32
#define D_GPIB_MUL_WRITE     	33
 
#define D_STATE_INDIC		34
#define D_BPSS_STATE		35
#define D_BPSS_READPOINT	36

#define D_GPIB_RES              37
#define D_GPIB_LOC              38
#define D_PSS_STATUS		39
#define D_RF_SIGCONFIG		40

#define D_BPSS_LINE		41
#define D_CT_LIFETIME		42

#define D_DAEMON_STATUS		43
#define D_DAEMON_DATA		44
#define D_VAR_LRPARR            45
#define D_MULMOVE_TYPE          46

#define D_OPAQUE_TYPE		47
#define D_SEISM_EVENT		48
#define D_SEISM_STAT		49

#define D_LIEN_STATE		50
#define D_BLADE_STATE           51
#define D_PSLIT_STATE           52
#define D_ATTE_TYPE             53

#define D_MOTOR_LONG            54
#define D_MOTOR_FLOAT           55

#define D_VAR_PATTERNARR      	56
#define D_ICV_MODE           	57
#define D_VAR_MSTATARR        	58

#define D_VAR_LONGFLOATSET	59
#define D_VAR_LONGREAD		60
#define D_VAR_POSREAD		61
#define D_VAR_AXEREAD		62
#define D_VAR_PARREAD		63
#define D_VAR_ERRREAD		64

#define D_GRPFP_TYPE		65
#define D_VAR_GRPFPARR		66

#define D_PINSTATE_TYPE		67
#define D_VAR_DOUBLEARR         68

#define D_VAR_ULONGARR		69
#define D_USHORT_TYPE		70
#define D_ULONG_TYPE		71
#define D_VAR_USHORTARR		72
#define D_VAR_SFRPARR		73

typedef struct _DevDataListEntry {
 				 long            	type;
				 DevDataFunction 	xdr;
				 long            	size;
				 DevDataLengthFunction 	xdr_length;
				 } DevDataListEntry;

static DevDataListEntry DevData_List [] =
{
   {D_VOID_TYPE,    xdr_void,       0,                 xdr_length_DevVoid},
   {D_BOOLEAN_TYPE, xdr_char,       sizeof(DevBoolean),xdr_length_DevBoolean},
   {D_SHORT_TYPE,   xdr_short,      sizeof(DevShort),  xdr_length_DevShort},
   {D_LONG_TYPE,    xdr_long,       sizeof(DevLong),   xdr_length_DevLong}, 
   {D_FLOAT_TYPE,   xdr_float,      sizeof(DevFloat),  xdr_length_DevFloat},
   {D_DOUBLE_TYPE,  xdr_double,     sizeof(DevDouble), xdr_length_DevDouble},
   {D_STRING_TYPE,  xdr_wrapstring, sizeof(DevString), xdr_length_DevString},
   {D_FLOAT_READPOINT,      xdr_DevFloatReadPoint, sizeof(DevFloatReadPoint), xdr_length_DevFloatReadPoint},
   {D_STATE_FLOAT_READPOINT,xdr_DevStateFloatReadPoint, sizeof(DevStateFloatReadPoint), xdr_length_DevStateFloatReadPoint},
   {D_VAR_CHARARR,      xdr_DevVarCharArray,   sizeof(DevVarCharArray), xdr_length_DevVarCharArray},
   {D_VAR_SHORTARR,     xdr_DevVarShortArray,  sizeof(DevVarShortArray), xdr_length_DevVarShortArray},
   {D_VAR_LONGARR,      xdr_DevVarLongArray,   sizeof(DevVarLongArray), xdr_length_DevVarLongArray},
   {D_VAR_FLOATARR,     xdr_DevVarFloatArray,  sizeof(DevVarFloatArray), xdr_length_DevVarFloatArray},
   {D_BPM_POSFIELD,     xdr_DevBpmPosField,    sizeof(DevBpmPosField), xdr_length_DevBpmPosField},
   {D_BPM_ELECFIELD,    xdr_DevBpmElecField,   sizeof(DevBpmElecField), xdr_length_DevBpmElecField},
   {D_WS_BEAMFITPARM,   xdr_DevWsBeamFitParm,  sizeof(DevWsBeamFitParm),  0},
   {D_VGC_STATUS,       xdr_DevVgcStat,        sizeof(DevVgcStat), xdr_length_DevVgcStat},
   {D_VGC_GAUGE,        xdr_DevVgcGauge,       sizeof(DevVgcGauge), xdr_length_DevVgcGauge},
   {D_VGC_CONTROLLER,   xdr_DevVgcController,  sizeof(DevVgcController), xdr_length_DevVgcController},
   {D_UNION_TYPE,       xdr_DevUnion,          sizeof(DevUnion), 0},
   {D_NEG_STATUS,       xdr_DevNeg14StatusRec, sizeof(DevNeg14StatusRec), 0},
   {D_RAD_DOSE_VALUE,   xdr_DevRadiationDoseValue, sizeof(DevRadiationDoseValue), 0},
   {D_LONG_READPOINT,   xdr_DevLongReadPoint,  sizeof(DevLongReadPoint),  xdr_length_DevLongReadPoint},
   {D_DOUBLE_READPOINT, xdr_DevDoubleReadPoint, sizeof(DevDoubleReadPoint), xdr_length_DevDoubleReadPoint},
   {D_VAR_STRINGARR,    xdr_DevVarStringArray, sizeof(DevVarStringArray), xdr_length_DevVarStringArray},
   {D_VAR_FRPARR,       xdr_DevVarFloatReadPointArray, sizeof(DevVarFloatReadPointArray), xdr_length_DevVarFloatReadPointArray},
   {D_VAR_THARR,        xdr_DevVarTHArray,     sizeof(DevVarTHArray), xdr_length_DevVarTHArray},
   {D_INT_FLOAT_TYPE,   xdr_DevIntFloat,       sizeof(DevIntFloat), xdr_length_DevIntFloat},
   {D_HAZ_STATUS,       xdr_DevHazStatus,      sizeof(DevHazStatus),      0},
   {D_VRIF_WDOG,        xdr_DevVrifWdog,       sizeof(DevVrifWdog),       0},
   {D_VRIF_STATUS,      xdr_DevVrifVmeStat,    sizeof(DevVrifVmeStat),    0},
   {D_VRIF_POWERSTATUS, xdr_DevVrifPowerStat,  sizeof(DevVrifPowerStat),  0},
   {D_GPIB_WRITE,       xdr_DevGpibWrite,      sizeof(DevGpibWrite),      0},
   {D_GPIB_MUL_WRITE,   xdr_DevGpibMulWrite,   sizeof(DevGpibMulWrite),   0},
   {D_STATE_INDIC,      xdr_DevStateIndic,      sizeof(DevStateIndic), xdr_length_DevStateIndic},
   {D_BPSS_STATE,       xdr_DevBpssState,       sizeof(DevBpssState), xdr_length_DevBpssState},
   {D_BPSS_READPOINT,   xdr_DevBpssReadPoint,   sizeof(DevBpssReadPoint), xdr_length_DevBpssReadPoint},
   {D_GPIB_RES,         xdr_DevGpibRes,         sizeof(DevGpibRes),       0},
   {D_GPIB_LOC,         xdr_DevGpibLoc,         sizeof(DevGpibLoc),       0},
   {D_PSS_STATUS,       xdr_DevDaresburyStatus, sizeof(DevDaresburyStatus), xdr_length_DevDaresburyStatus},
   {D_RF_SIGCONFIG,     xdr_DevRfSigConfig ,    sizeof(DevRfSigConfig ),  0},
   {D_BPSS_LINE,        xdr_DevBpssLine,        sizeof(DevBpssLine),      0},
   {D_CT_LIFETIME,      xdr_DevVarCtIntLifeTimeArray, sizeof(DevVarCtIntLifeTimeArray), xdr_length_DevVarCtIntLifeTimeArray},
   {D_DAEMON_STATUS,    xdr_DevDaemonStatus,    sizeof(DevDaemonStatus),  0},
   {D_DAEMON_DATA,      xdr_DevDaemonData,      sizeof(DevDaemonData),    0},
   {D_VAR_LRPARR,       xdr_DevVarLongReadPointArray, sizeof(DevVarLongReadPointArray), xdr_length_DevVarLongReadPointArray},
   {D_MULMOVE_TYPE,     xdr_DevMulMove,         sizeof(DevMulMove),       0},
   {D_OPAQUE_TYPE,      xdr_DevOpaque,          sizeof(DevOpaque), xdr_length_DevOpaque},
   {D_SEISM_EVENT,      xdr_DevSeismEvent,      sizeof(DevSeismEvent),    0},
   {D_SEISM_STAT,       xdr_DevSeismStat,       sizeof(DevSeismStat),     0},
   {D_LIEN_STATE,       xdr_DevLienState,       sizeof(DevLienState), xdr_length_DevLienState},
   {D_BLADE_STATE,      xdr_DevBladeState,      sizeof(DevBladeState),    0},
   {D_PSLIT_STATE,      xdr_DevPslitState,      sizeof(DevPslitState),    0},
   {D_ATTE_TYPE,        xdr_DevAtte,            sizeof(DevAtte),      	  0},
   {D_MOTOR_LONG,       xdr_DevMotorLong,       sizeof(DevMotorLong),     0},
   {D_MOTOR_FLOAT,      xdr_DevMotorFloat,      sizeof(DevMotorFloat),    0},
   {D_VAR_PATTERNARR,   xdr_DevVarPatternArray, sizeof(DevVarPatternArray), 0},
   {D_ICV_MODE,         xdr_DevIcv101Mode,      sizeof(DevIcv101Mode),    0},
   {D_VAR_MSTATARR,     xdr_VarSysStateArray,   sizeof(VarSysStateArray), xdr_length_VarSysStateArray},
   {D_VAR_LONGFLOATSET,	xdr_DevVarLongFloatSet,	sizeof(DevVarLongFloatSet),0},
   {D_VAR_LONGREAD,	xdr_DevVarLongRead,	sizeof(DevVarLongRead),0},
   {D_VAR_POSREAD,	xdr_DevVarPosRead,	sizeof(DevVarPosRead),0},
   {D_VAR_AXEREAD,	xdr_DevVarAxeRead,	sizeof(DevVarAxeRead),0},
   {D_VAR_PARREAD,	xdr_DevVarParRead,	sizeof(DevVarParRead),0},
   {D_VAR_ERRREAD,	xdr_DevVarErrRead,	sizeof(DevVarErrRead),0},
   {D_GRPFP_TYPE,	xdr_DevGrpFramePair,	sizeof(DevGrpFramePair), xdr_length_DevGrpFramePair},
   {D_VAR_GRPFPARR,	xdr_DevGrpFramePairArray, sizeof(DevGrpFramePairArray), xdr_length_DevGrpFramePairArray},
   {D_PINSTATE_TYPE,	xdr_DevPinState,	sizeof(DevPinState), xdr_length_DevPinState},
   {D_VAR_DOUBLEARR,    xdr_DevVarDoubleArray,  sizeof(DevVarDoubleArray), xdr_length_DevVarDoubleArray},
   {D_VAR_ULONGARR,	xdr_DevVarULongArray,	sizeof(DevVarULongArray), xdr_length_DevVarULongArray},
   {D_USHORT_TYPE,	xdr_u_short, sizeof(DevUShort), xdr_length_DevUShort}
   {D_ULONG_TYPE	xdr_u_long, sizeof(DevULong), xdr_length_DevULong},
   {D_VAR_USHORTARR,	xdr_DevVarULongArray, 	sizeof(DevVarUShortArray), xdr_length_DevVarULongArray}	
   {D_VAR_SFRPARR	xdr_DevVarStateFloatReadPointArray, sizeof(DevVarStateFloatReadPointArray), xdr_length_DevVarStateFloatReadPointArray},
};


/*
 * number of defined data types
 */

#define NUMBER_OF_TYPES	((sizeof(DevData_List)/sizeof(DevDataListEntry)) +1)	


#endif /* _xdr_typelist_h */

