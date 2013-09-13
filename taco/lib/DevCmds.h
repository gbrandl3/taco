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
 * File:	DevCmd.h
 *
 * Project:	Device Servers with SUN-RPC
 *
 * Description:	Include file contains al available commands,
 *		their names and all neccessary definitions
 *		for command handling.
 *
 * Author(s):	Jens Meyer
 * 		$Author: jkrueger1 $
 *
 * Original:	June 1991
 *
 * Version:	$Revision: 1.7 $
 *
 * Date:	$Date: 2008-04-06 09:06:58 $
 *
 ********************************************************************-*/

#ifndef _DevCmds_h
#define _DevCmds_h 

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevCmdsh[] = 
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevCmds.h,v 1.7 2008-04-06 09:06:58 jkrueger1 Exp $";
#endif /* _IDENT */


#if !defined(FORTRAN)
#include <dev_xdr.h>
typedef DevLong 	DevCommand;
typedef DevLong 	DevEvent;
typedef DevLong 	DevArgType;
typedef long int (*DevCommandFunction)();
#endif /* FORTRAN */

/*
 * general commands
 */
#define DevOn			1		
#define DevOff			2	
#define DevReset		3	
#define DevState		4
#define DevStandby		5
#define DevRun			6
#define DevLocal		7
#define DevStatus		8	
#define DevOpen			9
#define DevClose		10
#define DevSetValue		11	
#define DevReadValue		12	
#define DevInsert		13
#define DevExtract		14
#define DevReadPos		15
#define DevWritePos		16 
#define DevSetParam		17
#define DevReadParam		18
#define DevUnlock		19

#define DevSetConversion	20
#define DevSelectCh		21
#define DevSetProtThresh	22
#define DevReadPressure		23
#define DevResetInputBuffer	24
#define DevChangeCalibration	25
#define DevNAve			26
#define DevHello                27
#define DevRead			28
#define DevWrite		29
/*
 * commands for pneumatic valves
 */
#define DevReadCommand          30		
#define DevForce		31
#define DevUnforce		32
#define DevAbort		33
/*
 * commands for power supplies
 */
#define DevReadSetValue		39
#define DevSetVoltage		40	
#define DevReadVoltage		41
#define DevError		42
#define DevSetCurrent		43
#define DevReadCurrent		44
#define DevShortStatus		45
#define DevIndicator		46
/* hazemeyer ps */
#define DevGetName		47
#define DevSetTimeout		48
#define DevCalibration		49
/*
 * commands for vacuum gauge controller
 */
#define DevLongStatus		50
#define DevRemote		51
#define DevGaugeStatus		52
#define DevGaugeOn		53
#define DevGaugeOff		54
#define DevChangeSetpoint	55
#define DevSetGasFactor		56
#define DevSetFilterTime	57
#define DevSelectCalibration	58
#define DevEnergiseRelay	59
#define DevDeenergiseRelay	60
#define DevGaugeReadOut		61
#define DevParamReadOut		62
#define DevMaxPressure		63
/*
 * commands for riber ion pump controller
 */
#define DevHTOn			70
#define DevHTOff		71
#define DevReadHT		72
#define DevReadDU		73
#define DevDUOn			74
#define DevDUOff		75
#define DevReadVacuum		76
#define DevAbortStartup		77
#define DevShutdown		78
#define DevStartBake		79
#define DevStopBake		80
#define DevCheckHT		81

/*
 * Gadc related commands
 */
#define DevSetPreAmpGain        100
#define DevReadPreAmpGain       101
#define DevSetAmpGain           102
#define DevReadAmpGain          103
#define DevReadADCMode          104
/*
 * WireScanner related commands
 */
#define DevReadValueSingle      110
#define DevReadValueCycle       111
#define DevReadBeamWidth        112
/*
 * Horizontal wirescanner
 */
#define DevSetHPreAmpGain       120
#define DevReadHPreAmpGain      121 
#define DevSetHAmpGain          122
#define DevReadHAmpGain         123 
#define DevReadHADCMode         124
#define DevReadHCurrent         125
/*
 * Vertical wirescanner
 */
#define DevSetVPreAmpGain       130
#define DevReadVPreAmpGain      131
#define DevSetVAmpGain          132
#define DevReadVAmpGain         133
#define DevReadVADCMode         134
#define DevReadVCurrent         135
/*
 * Commands for THC
 */ 
#define DevReadThController	140
#define DevDownLoadThController	141
/*
 * Commands for PLC
 */ 
#define DevIOState		150
#define DevForceIO		151
#define DevReadIntBit		152
#define DevReadIntWord		153
#define DevReadSysBit		154
#define DevReadSysWord		155
#define DevWriteIntBit		156
#define DevWriteIntWord		157
#define DevWriteSysBit		158
#define DevWriteSysWord		159
#define DevForceIntBit		160
#define DevEnableDownLoad	161
#define DevDisableDownLoad	162
#define DevOkDownLoad		163
#define DevReadIntBitSeq	164
#define DevReadIntWordSeq	165
/*
 * Stepper Motor related commands
 */
#define DevSetHomeSlewRate              166
#define DevReadHomeSlewRate             167
#define DevMoveMultipleRel		168
#define DevMoveMultiple			169
#define DevSetContinuous         	170
#define DevMoveRelative          	171
#define DevMoveAbsolute          	172
#define DevUpdate                	173
#define DevAbortCommand          	174
#define DevSetAcceleration       	175
#define DevSetVelocity           	176
#define DevSetDirection          	177
#define DevSetFirstStepRate      	178
#define DevSetAccSlope  		179
#define DevReadPosition          	180
#define DevAdditionalState       	181
#define DevMoveReference         	182
#define DevReadAcceleration      	183
#define DevReadVelocity          	184
#define DevReadFStepRate     		185
#define DevReadAccSlope 		186
#define DevStop                 	187
#define DevStopImm              	188
#define DevMoveLimit            	189
#define DevReadSwitches         	190
#define DevLoadPosition         	191
#define DevLimitOff             	192
#define DevLimitOn              	193
#define DevReadState            	194
/*  JCS  */
#define DevSetMaster			195
#define DevExitMaster			196
#define DevDefinePosition		197
#define DevMajorReset			198
/*
 * Commands for Radio Frequency System
 */
#define DevSetup	            	200
#define DevReadFrequency	       	201
#define DevSetFrequency	       		202
#define DevReadLevel	       		203
#define DevSetLevel	       		204

#define DevGetConfFile         		205 
#define DevSaveConfi            	206
#define DevGetControlMode        	207
#define DevGetDirFile           	208
#define DevGetErrors            	209
#define DevGetPrivilege         	210
#define DevGetProtocolFile      	211
#define DevGetSigByName      		212
#define DevGetSigByNumber    		213
#define DevGetTraceFile         	214
#define DevGetTstState          	215
#define DevGetUser              	216
#define DevSetAlarm             	217
#define DevSetControlMode       	218
#define DevSetControlSrc        	219
#define DevSetCrwbs             	220
#define DevSetFilt              	221
#define DevSetHvon              	222
#define DevSetPasswd            	223
#define DevSetProtext           	224
#define DevSetPrivilege         	225
#define DevSetSystate           	226
#define DevSetPointByName    		227
#define DevSetPointByNumber  		228
#define DevSetTransWaveFile     	229
#define DevSetTransConfFile     	230
#define DevSetWaveForm          	231

#define DevGetControlSrc        	232
#define DevGetWaveFile     		233
#define DevSetTstState           	234
#define DevSetUser           		235
#define DevSubState          		236
#define DevGetPointByNumber     	237
#define DevGetSigConfig			238
#define DevUpdateSigConfig		199	
#define DevReadSigValues		239
/*
 * Commands for VideoMultiplexor
 */
#define DevReadCh			240
#define DevSelectMon			241
#define DevReadMon			242
#define DevSetClamp			243
#define DevReadClamp			244
#define DevSetSynchEltec		245
#define DevReadSynchEltec		246
#define DevSetSynchComp			247
#define DevReadSynchComp		248
#define DevSetMode			249
#define DevReadMode			250
/*
 * Commands for ADAS icv150
 */
#define DevStartConversion      	260
#define DevStopConversion       	261
#define DevSetChanGain          	262
#define DevReadValues           	263
#define DevTrigReadValues      		264
#define DevReadChannel          	265
#define DevTrigReadChannel     		266
#define DevSetNum               	267
#define DevStore                	268
#define DevRestore                	269
/*
 * Commands for teslameter
 */
#define DevSetSense			270
#define DevReadSense			271
#define DevSetUnit			272
#define DevReadUnit			273
#define DevReadDisplay			274
#define DevSetRemote			275
#define DevReadRemote			276
#define DevSetPreset			277
#define DevReadPreset			278
#define DevSetMuxChannel		279
#define DevReadMuxChannel		280
#define DevSetNChannels			281
#define DevReadNChannels		282
#define DevSetSearch			283
#define DevReadSearch			284
#define DevSetSearchTime		285
#define DevReadSearchTime		286
#define DevResetTimebase		287
#define DevReadHardStatus		288
/*
 * Commands for Beam Position Monitors
 */
#define DevBpmSetMode  	300      /*set BPM mode (normal, first turn or maint. */
#define DevBpmReadMode  301      /* read BPM mode */
#define DevBpmSetPar    302      /* set user parameter table values */
#define DevBpmReadPar   303      /* read user parameter table values */
#define DevBpmSetStat   304      /* set one BPM status (active or not) */
#define DevBpmReadStat  305      /* read all BPM status values */
#define DevBpmReadLast  306      /* read last set of position values */
#define DevBpmReadNew   307      /* read new set of position values */
#define DevBpmReadElec  308      /* read last set of electrode values */
#define DevBpmReadHard  309      /* read BPM hardware status lines (GESCIO1) */
#define DevBpmReadCali  310      /* read BPM electrode calibration constants */
#define DevBpmWrite     311      /*write a set of position values (test only) */
#define DevStGrp   312    /* set BPM group activity status */
#define DevRdGrp   313    /* read BPM group activity status */
#define DevEnaVp   314    /* enable VPDU board */
#define DevDisVp   315    /* disable VPDU board */
#define DevStiVp   316    /* set channel delay values for VPDU board */
#define DevStRfG   317    /* set RFP gain by GESCIO1 board */
#define DevSlRfO   318    /* select RFP output by GESCIO1 board */
#define DevStTiW   319    /* set time window by GESCIO1 board */
#define DevStHwM   320    /* set hardware mode by GESCIO1 board */
#define DevSlTrS   321    /* select trigger source by GESCIO1 board */
#define DevSlEle   322    /* select electrode by GESCIO1 board */
#define DevSnFtR   323    /* send FTPP reset pulse by GESCIO1 board */
#define DevSnSoT   324    /* send soft trigger pulse by GESCIO1 board */
#define DevRdHwS   325    /* read hardware status lines by GESCIO1 board */
#define DevRdMem   326    /* read GADCM memory block */
#define DevRdCnt   327    /* read counter of GADCM memory block */
#define DevRsAdc   328    /* reset GADCM board */
#define DevStDir   329    /* set direct/auxiliary RF-mux. by GESCIO1 board */
#define DevSlNPa   330    /* select Nth passage of beam by GESCIO1 board */
/*
 * Commands for Vpdu
 */
#define DevRdDlR 	340      /* read 24 bit delay register */
#define DevWrDlR 	341      /* write 24 bit delay register */
#define DevRdCoR 	342      /* read 8 bit control register */
#define DevWrCoR 	343      /* write 8 bit control register */
#define DevSetCoR 	344      /* set single bits in 8 bit control register */
#define DevResCoR 	345      /*reset single bits in 8 bit control register*/
#define DevSofTr 	346      /* trigger board once by internal trigger */
#define DevSetWd 	347      /* set 8 bit pulse width register */
/*
 * Commands for Timer
 */
#define DevSetDelay 	350      /* set delay value */
/*
 * Commands for G64 ADC
 */
#define DevMemRd 	360      /* read bytes in board memory */
#define DevAdcEn 	361      /* enable/disable A/D conversions */
/*
 * Commands for Gescio1
 */
#define DevRd16b 	370  /* read 16 bit input register */
#define DevWr16b 	371  /* write 16 bit output register */
#define DevWrMs16b 	372  /* write 16 bit output register with mask */
#define DevReRd16b 	373  /* re-read 16 bit output register */
#define DevSet16b 	374  /* set specified bits in 16 bit output register */
#define DevRset16b 	375  /*reset specified bits in 16 bit output register*/
#define DevMod16b 	376  /*modify specified bits in 16 bit output register*/
/*
 * Commands for ImageUx/ImageOS9
 */
#define DevDisplayBeam 	390
/*
 * Commands for booster power supply system
 */
#define DevWarmUp	377
#define DevMacro	378
#define DevLock		379
#define DevFree		380
#define DevStatusMacro  381
#define DevStopMacro	383
#define DevSetPhase	384
#define DevReadPhase	385
#define DevSetPeriod	386
#define DevReadPeriod	387
#define DevSetAc	388
#define DevReadAc	389
#define DevReadDc	391
#define DevSetIacInv	392
#define DevReadIacInv	393
#define DevCouple	394
#define DevTrack	395
#define DevUnTrack	396
#define DevSearch	397
#define DevReadTemper	398
#define DevReadPhLagUi	399
#define DevSetDc	400
/*
 * Commands for Gespac Digital to Analogic 8 Channels Converter
 */
#define DevSetRange	420
#define DevGetRange	421
#define DevSetChannel	422
#define DevGetChannel	423
#define DevSetIncrement	424
#define DevGetIncrement 425
#define DevIncrement	426
#define DevDecrement	427
/*
 * Commands for Diagnostics Current Transformer
 */
#define DevGetTest 	440
#define DevSetTest	441
#define DevGetCalib 	442
#define DevSetCalib	443
#define DevReadVar  	444
#define DevReadEnd	445
#define DevGetSens 	446
#define DevSetSens 	447
#define DevSetSensMan	448
#define DevReadSens 	449
#define DevGetOffsetB	450
#define DevWriteAcqu 	451
#define DevReadAcqu	452
#define DevGetAverage	453
#define DevGetAcquInfos 454
#define DevGetMpvErrors 455
#define DevReadLife 	456
#define DevDtLifeTime 	457
#define DevDiLifeTime 	458
#define DevGetDeltaTime 459
#define DevSetDeltaTime 460
#define DevGetDeltaInt 	461
#define DevSetDeltaInt 	462
#define DevGetMeasureTime 	463
#define DevSetMeasureTime 	464
/*
 * DiagNostics SY and SR Tunning Monitor
 */
#define DevGetBeam 	480
#define DevSetBeam	481
#define DevGetStrength 	482
#define DevSetStrength	483
#define DevGetMethod 	484
#define DevSetMethod	485
#define DevGetPlane 	486
#define DevSetPlane	487
#define DevGetDelays 	488
#define DevSetDelays	489
#define DevSetConfig 	490
#define DevSetTimers	491
/*
 * Rug Beam Monitoring System
 */
#define DevGetMode	510
#define DevGetLength	511
#define DevGetCurrent	512
#define DevSetAutoDump	513
/*
 * Commands for the Vrif class
 */
#define DevResetVme		520
#define DevSetAsIndic		521
#define DevEnWdog		522
#define DevDisWdog		523
#define DevReadVmeStat		524
#define DevReadPowerStat	525
#define DevReadCrateNum		526
/*
 *  Commands for HLS system 
 */
#define DevFullData		530
/* 
 * Commands for Piezo 
 */
#define DevMovePosition  	540
/* 
 * Commands for Seismic survey
 */
#define DevGetEventList		550
#define DevReadEvent		551
#define DevReadLastEvent	552
#define DevReadSpectra		553
/* 
 * Commands for LIEN automat 
 */
#define DevReadThreshold  	560
#define DevSetThreshold  	561
#define DevReadInput	  	562
#define DevReadOutput	  	563
#define DevReadCutTh	  	564
#define DevReadCmds	  	565
#define DevSetCmds	  	566
#define DevEraseCmds	  	567
#define DevReadCmdErr	  	568
#define DevReadNew	  	569
#define DevWriteNew	  	570
#define DevReadAll	  	571
#define DevReadAlarm	  	572
/* 
 * Commands for the Update Daemon 
 */
#define DevGetDeviceIdent  	580
#define DevInitialiseDevice  	581
#define DevStartPolling	  	582
#define DevStopPolling	  	583
#define DevChangeInterval  	584
#define DevDefineMode	  	585
#define DevPollStatus	  	586
#define DevAccessStatus	  	587
#define DevGetDeviceName  	588
#define DevRemoveDevice         589
/*
 *  Commands for LINAC
 */
#define DevWriteAna  		600
#define DevWriteDig  		601
#define DevDelFile              602
#define DevDirFiles             603
#define DevSelectFile           604
#define DevReadConfig           605
#define DevViewFile		606
/*
 *  Commands for FrontEnd Class
 */
#define DevAxeType 		620
#define DevCloseShutter		621
#define DevCloseAbsorber 	622
#define DevStopBeam 		623
/*
 *  Commands for Powermeter
 */
#define DevReadPower		640


/*
 *	Commands for DAS group
 *	Device Servers for beamline control
 */

/* 
 * Commands for GPIB 
 */
#define DevIntClear     	1003
#define DevClear        	1004
#define DevParPoll      	1005
#define DevTrigger      	1007
#define DevLocalLock    	1009
#define DevSerPoll      	1010
#define DevGetDevs      	1011
#define DevSetDevs      	1012
#define DevPassCtrl     	1013
#define DevParPollConf  	1014
#define DevParPollDis   	1015
#define DevParPollUnc   	1016
#define DevListen       	1017
#define DevTalk         	1018
#define DevUnListen     	1019
#define DevUnTalk       	1020
#define DevGetLocal     	1021
#define DevSetLocal     	1022
#define DevMulClear     	1023
#define DevMulListen    	1024
#define DevMulLocal     	1025
#define DevMulPassCtrl  	1026
#define DevMulParPollDis 	1027
#define DevMulParPollUnc 	1028
#define DevMulRemote     	1029
#define DevMulWrite      	1030
/* 
 * Commands for incremental encoders 
 */
#define DevReadCount     	1050
#define DevWriteCount    	1051
#define DevGetDig        	1052
#define DevGetInd        	1053

/* New commands for steppermotors */
#define DevTrigClear       1060
#define DevWriteMux        1061
#define DevStartMemWrite   1062
#define DevStopMemWrite    1063
#define DevMoveAbsoluteDel 1064
#define DevMoveRelativeDel 1065
#define DevTrigMotion      1066
#define DevSetStepMode     1067
#define DevReadStepMode    1068
#define DevWriteWait       1069
#define DevReadMulPos      1070
#define DevReadMulEncPos   1071
#define DevReadFactor      1072
#define DevSetUnits        1073
#define DevReadEncPos      1074
#define DevLoadEncPos      1075
#define DevCloseBrake      1076
#define DevOpenBrake       1077
#define DevReadUnits       1078
#define DevReadBrakeState  1079
#define DevSetBacklash     1080
#define DevReadBacklash    1081
#define DevMulState        1082
#define DevReadMulAcc      1083
#define DevReadMulFSR      1084
#define DevReadMulVel      1085
#define DevReadMulHSR      1086
#define DevReadMulBrState  1087
#define DevReadMulSwitches 1088
#define DevReadMulUnits    1089
#define DevReadMulBacklash 1090
#define DevMoveEncAbsolute 1091
#define DevStartServo      1092
#define DevStopServo       1093
#define DevEnablePower     1094
#define DevDisablePower    1095

/* 1080 to 1099 reserved for StepperMotors	*/

/* definitions 1110 to 1125 (LeCroy/CAEN) removed - AB */
#define DevCntStatus      1111

/* Numbers from here to 1199 reserved for Data acquisition  */


/* Commands for slits */
#define DevReadBladeState 1200
#define DevReadPslitState 1201
#define DevSetRelGap      1202
#define DevSetAbsGap      1203
#define DevSetRelOff      1204
#define DevSetAbsOff      1205
#define DevSwitchOn       1206
#define DevSwitchOff      1207
#define DevSetZero        1208
#define DevMakeReference  1209
#define DevSetTuned       1210

/* 1210 -1229 reserver for slits */

/* Command(s) for attenuators */
#define DevGotoFilter     1230

/* 1230 -1240 reserver for attenuators */

/* Commands for Hbm */
#define DevReadErr	  1241

/* Commands for M4 */
#define DevReadMot	  1250
#define DevReadSen	  1251
#define DevReadAxe	  1252
#define DevReadClutch	  1253
#define DevApplyBrake	  1254
#define DevReleaseBrake	  1255
#define DevClutchTrig	  1256
#define DevReport	  1257
#define DevLoadMot	  1258
#define DevLoadEnc	  1259

/* Commands for clutch */
#define DevClutchOn	  1280
#define DevClutchOff	  1281
#define DevClutchState	  1282
 
/*** New commands for icv150 ** TM ***/

#define DevTrigReadAll  1300
#define DevReadChanGain 1301
#define DevSetLimits    1302
#define DevReadLimits   1303
#define DevReadAlarmAll 1304
#define DevSetMinMax    1305
#define DevReadMinMax   1306
 
/*** New commands for BeamShutter ** JMV ***/

#define DevSetFastMode		1307
#define DevSetStandardMode	1308
#define DevSetFastDelay		1309


/*** More generic commands  ***/

#define DevGetDsConfig    1500
#define DevGetDebugFlags  1501
#define DevSetDebugFlags  1502
#define DevOpenDynOutput  1503
#define DevCloseDynOutput 1504
#define GetDevList        1505
#define DevGetInfo        1506

#define DevHookInstall    1510
#define DevHookRelease    1511
#define DevGetDrvDebug    1512
#define DevSetDrvDebug    1513


/*
 *  definitions for class command list
 */
#if !defined(FORTRAN)
typedef struct _DevCommandListEntry 
	{
		DevCommand 		cmd;
                DevCommandFunction 	fn;
		DevArgType		argin_type;
		DevArgType		argout_type;
		long			min_access;
		DevString		cmd_name;
	} DevCommandListEntry;

typedef struct _DevCommandListEntry *DevCommandList;

/* events */
typedef struct _DevEventListEntry 
	{
        	DevEvent	event;
                DevArgType      argout_type;
		DevString	event_name;
        } DevEventListEntry;

typedef struct _DevEventListEntry *DevEventList;    

/* for global command table */
typedef struct _DevCmdNameListEntry 
	{
		int value;
		DevString name;
	} DevCmdNameListEntry;

#endif /* FORTRAN */
#endif /* _DevCmds_h */

