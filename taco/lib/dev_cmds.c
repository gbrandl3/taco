static char RcsId[]      =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/dev_cmds.c,v 1.1 2005-03-29 09:43:07 andy_gotz Exp $";

/*+*******************************************************************

 File:		dev_cmds.c

 Project:	Device Servers 

 Description:	List of command names

 Author(s):	Andy Gotz

 Original:	March 1992

 $Log: not supported by cvs2svn $
 Revision 4.6  2004/09/01 11:35:56  pons
 Added few tango command mappings

 Revision 4.5  2003/10/24 13:04:37  goetz
 added DevReadSigValues; corrected DevSetFrequency

 Revision 4.4  2002/11/07 13:40:26  meyer
 Added DevAbort

 Revision 4.3  2002/10/24 09:06:59  meyer
 Changed TangoState and TangoStatus to DevState and DevStatus.

 * Revision 4.2  2002/10/23  18:58:18  18:58:18  meyer (Jens Meyer)
 * Added TangoState and TangoStatus
 * 
 * Revision 4.1  2001/03/21  18:34:54  18:34:54  goetz (Andy Goetz)
 * removed counter and gategen commands
 * 
 Revision 4.0  1994/03/14 18:23:45  meyer
 Library version with Security System.

 * Revision 3.50  94/03/12  13:58:45  13:58:45  meyer (Jens Meyer)
 * Extended command list with min_access.
 * 
 * Revision 3.24  93/07/23  10:40:16  10:40:16  domingue ( Marie-Christine Dominguez)
 * Add new commands for M4 server
 * 
 * Revision 3.23  1993/07/23  09:00:53  epaud
 * Suppress command for VDL
 *
 * Revision 3.22  93/07/13  17:12:58  17:12:58  epaud (Francis Epaud)
 * Remove command for VDL which are now in a private file
 * 
 * Revision 3.21  93/06/14  09:51:33  09:51:33  claustre (Laurent Claustre)
 * New command for LINAC
 * 
 * Revision 3.20  93/06/08  14:20:58  14:20:58  mettala (Timo Mettala)
 * New commands for icv150
 * 
 * Revision 3.19  93/06/08  10:31:02  10:31:02  meyer
 * Added command for powermeter.
 * 
 * Revision 3.18  93/06/07  08:54:33  08:54:33  domingue ( Marie-Christine Dominguez)
 * Hbm, Clutch and M4 commands
 * 
 * Revision 3.17  93/06/04  15:54:58  15:54:58  claustre (Laurent Claustre)
 * Add commands for LINAC MASTER
 * 
 * Revision 3.16  93/05/27  15:39:52  15:39:52  mettala (Timo Mettala)
 * Again new commands for steppermotors !
 * 
 * Revision 3.15  93/05/03  16:38:17  16:38:17  mettala (Timo Mettala)
 * Added new commands for steppermotors
 * 
 * Revision 3.14  93/02/19  17:28:01  17:28:01  epaud (Francis Epaud)
 * Add LECROY 1551 counter and CAEN V462 gate generator device servers
 * 
 * Revision 3.13  92/12/14  18:40:17  18:40:17  mettala (Timo Mettala)
 * Added commands for maxe, attenuators and slits
 * 
 * Revision 3.14  92/12/14  17:55:27  17:55:27  mettala (Timo Mettala)
 * corrected error in slit commands
 * 
 * Revision 3.13  92/12/14  09:38:11  09:38:11  mettala (Timo Mettala)
 * Added commands dfor maxe, slits and attenuator
 * 
 * Revision 3.12  92/11/02  10:19:18  10:19:18  dserver ()
 * Add data acquisition server VDL (VME Digital Lockin).   F.Epaud.
 * 
 * Revision 3.11  92/10/23  14:45:49  14:45:49  meyer ()
 * Added commands for FrontEnd class.
 * 
 * Revision 3.10  92/09/24  16:26:21  16:26:21  meyer ()
 * added commands for LINAC
 * 
 * Revision 3.9  92/09/02  09:01:57  09:01:57  meyer ()
 * deletetd control characters in file.
 * 
 * Revision 3.8  92/09/02  08:06:52  08:06:52  meyer ()
 * added DevReadAlarm for LIEN autmat.
 * 
 * Revision 3.7  92/09/01  10:40:50  10:40:50  mettala ()
 * added three new cmds for encoder steppermotors
 * 
 * Revision 3.6  92/09/01  14:18:26  14:18:26  meyer ()
 * added commands for LIEN automat and Update Daemon.
 * 
 * Revision 3.4  92/08/31  07:14:54  07:14:54  mettala
 * Added new commands for steppermotors
 * 
 * Revision 3.3  92/08/03  16:17:18  16:17:18  chaize (Jean Michel Chaize)
 * add commands for seismic survey server
 * 
 * Revision 3.2  92/06/17  16:38:58  16:38:58  meyer (Jens Meyer)
 * added commands for current transformer.
 * 
 * Revision 3.1  92/06/02  17:08:56  17:08:56  meyer ()
 * Cut to long steppermotor commands to 19 characters.
 * 
 * Revision 3.0  92/03/31  09:49:31  09:49:31  meyer ()
 * New major library release (3.x)
 * 

 Copyright (c) 1992 by European Synchrotron Radiation Facility, 
                      Grenoble, France

********************************************************************-*/

#include <DevCmds.h>

DevCmdNameListEntry DevCmdNameList[] = {
{ DevOn , 		 "DevOn" }, 
{ DevOff , 		 "DevOff" },
{ DevReset,            	 "DevReset" }, 
{ DevState,            	 "DevState" },
{ DevStandby,          	 "DevStandby" },
{ DevRun,             	 "DevRun" },
{ DevLocal,        	 "DevLocal" },
{ DevStatus,           	 "DevStatus" },    
{ DevOpen,               "DevOpen" },  
{ DevClose,              "DevClose" }, 
{ DevSetValue,           "DevSetValue" }, 
{ DevReadValue,          "DevReadValue" },  
{ DevInsert,             "DevInsert" },  
{ DevExtract,            "DevExtract" }, 
{ DevReadPos,            "DevReadPos" },  
{ DevWritePos,           "DevWritPos" },  
{ DevSetParam,		 "DevSetParam" },
{ DevReadParam,		 "DevReadParam" },
{ DevUnlock,		 "DevUnlock" },

{ DevSetConversion,      "DevSetConversion" },  
{ DevSelectCh,           "DevSelectCh" },  
{ DevSetProtThresh,      "DevSetProtThresh" },  
{ DevReadPressure,       "DevReadPressure" },  
{ DevResetInputBuffer,   "DevResetInputBuffer" },  
{ DevChangeCalibration,  "DevChangeCalibration" },  
{ DevNAve,               "DevNAve" },  
{ DevHello,              "DevHello" },  
{ DevRead,		 "DevRead" },
{ DevWrite,		 "DevWrite" },
/*
 * commands for pneumatic valves
 */
{ DevReadCommand,        "DevReadCommand" },  
{ DevForce,		 "DevForce" },
{ DevUnforce,		 "DevUnforce" },
{ DevAbort,		 "DevAbort" },
/*
 * commands for power supplies
 */
{ DevReadSetValue,	 "DevReadSetValue" },
{ DevSetVoltage,         "DevSetVoltage" },  
{ DevReadVoltage,        "DevReadVoltage" },  
{ DevError,              "DevError" },  
{ DevSetCurrent,         "DevSetCurrent" },  
{ DevReadCurrent,        "DevReadCurrent" },  
{ DevShortStatus,        "DevShortStatus" },  
{ DevIndicator,          "DevIndicator" },  
/* hazemeyer ps */
{ DevGetName,		 "DevGetName" },
{ DevSetTimeout,	 "DevSetTimeout" },
{ DevCalibration,	 "DevCalibration" },
/*
 * commands for vacuum gauge controller
 */
{ DevLongStatus, 	"DevLongStatus" },
{ DevRemote, 		"DevRemote" },
{ DevGaugeStatus, 	"DevGaugeStatus" },
{ DevGaugeOn, 		"DevGaugeOn" },
{ DevGaugeOff, 		"DevGaugeOff" },
{ DevChangeSetpoint, 	"DevChangeSetpoint" },
{ DevSetGasFactor, 	"DevSetGasFactor" },
{ DevSetFilterTime, 	"DevSetFilterTime" },
{ DevSelectCalibration, "DevSelectCalibration" },
{ DevEnergiseRelay, 	"DevEnergiseRelay" },
{ DevDeenergiseRelay, 	"DevDeenergiseRelay" },
{ DevGaugeReadOut, 	"DevGaugeReadOut" },
{ DevParamReadOut, 	"DevParamReadOut" },
{ DevMaxPressure,	"DevMaxPressure" },
/*
 * commands for riber ion pump controller
 */
{ DevHTOn, 		"DevHTOn" },
{ DevHTOff, 		"DevHTOff" },
{ DevReadHT, 		"DevReadHT" },
{ DevReadDU, 		"DevReadDU" },
{ DevDUOn, 		"DevDUOn" },
{ DevDUOff, 		"DevDUOff" },
{ DevReadVacuum, 	"DevReadVacuum" },
{ DevAbortStartup, 	"DevAbortStartup" },
{ DevShutdown, 		"DevShutdown" },
{ DevStartBake,		"DevStartBake" },
{ DevStopBake,		"DevStopBake" },
{ DevCheckHT,		"DevCheckHT" },
/*
 * Gadc related commands
 */
{ DevSetPreAmpGain, 	"DevSetPreAmpGain" },
{ DevReadPreAmpGain, 	"DevReadPreAmpGain" },
{ DevSetAmpGain, 	"DevSetAmpGain" },
{ DevReadAmpGain, 	"DevReadAmpGain" },
{ DevReadADCMode, 	"DevReadADCMode" },
/*
 * WireScanner related commands
 */
{ DevReadValueSingle,	"DevReadValueSingle" },
{ DevReadValueCycle,	"DevReadValueCycle" },     
{ DevReadBeamWidth,	"DevReadBeamWidth" },     
/*
 * Horizontal wirescanner
 */
{ DevSetHPreAmpGain,	"DevSetHPreAmpGain" },      
{ DevReadHPreAmpGain,	"DevReadHPreAmpGain" },   
{ DevSetHAmpGain,	"DevSetHAmpGain" },      
{ DevReadHAmpGain,	"DevReadHAmpGain" },    
{ DevReadHADCMode,	"DevReadHADCMode" },     
{ DevReadHCurrent,	"DevReadHCurrent" },       
/*
 * Vertical wirescanner
 */
{ DevSetVPreAmpGain,	"DevSetVPreAmpGain" },     
{ DevReadVPreAmpGain,	"DevReadVPreAmpGain" },     
{ DevSetVAmpGain,	"DevSetVAmpGain" },         
{ DevReadVAmpGain,	"DevReadVAmpGain" },       
{ DevReadVADCMode,	"DevReadVADCMode" },      
{ DevReadVCurrent,	"DevReadVCurrent" },     
/*
 * Commands for THC
 */ 
{ DevReadThController,		"DevReadThController" },
{ DevDownLoadThController,	"DevDownLoadThController" },
/*
 * Commands for PLC
 */ 
{ DevIOState,		"DevIOState" },	
{ DevForceIO,		"DevForceIO" },	
{ DevReadIntBit,	"DevReadIntBit" },	
{ DevReadIntWord,	"DevReadIntWord" },
{ DevReadSysBit,	"DevReadSysBit" },
{ DevReadSysWord,	"DevReadSysWord" },
{ DevWriteIntBit,	"DevWriteIntBit" },
{ DevWriteIntWord,	"DevWriteIntWord" },
{ DevWriteSysBit,	"DevWriteSysBit" },
{ DevWriteSysWord,	"DevWriteSysWord" },
{ DevForceIntBit,	"DevForceIntBit" },
{ DevEnableDownLoad,	"DevEnableDownLoad" },
{ DevDisableDownLoad,	"DevDisableDownLoad" },
{ DevOkDownLoad,	"DevOkDownLoad" },
{ DevReadIntBitSeq,	"DevReadIntBitSeq" },
/*
 * Stepper Motor related commands
 */
{ DevSetHomeSlewRate,	   "DevSetHomeSlewRate" },
{ DevReadHomeSlewRate,	   "DevReadHomeSlewRate" },
{ DevMoveMultipleRel,	   "DevMoveMultipleRel" },
{ DevMoveMultiple,	   "DevMoveMultiple" },
{ DevSetContinuous,    	   "DevSetContinuous" },  
{ DevMoveRelative,   	   "DevMoveRelative" },  
{ DevMoveAbsolute,         "DevMoveAbsolute" },  
{ DevUpdate,               "DevUpdate" },  
{ DevAbortCommand,         "DevAbortCommand" },  
{ DevSetAcceleration,      "DevSetAcceleration" },  
{ DevSetVelocity, 	   "DevSetVelocity" },  
{ DevSetDirection, 	   "DevSetDirection" },  
{ DevSetFirstStepRate,     "DevSetFirstStepRate" },  
{ DevSetAccSlope, 	   "DevSetAccSlope" },  
{ DevReadPosition, 	   "DevReadPosition" },  
{ DevAdditionalState, 	   "DevAdditionalState" },  
{ DevMoveReference, 	   "DevMoveReference" },  
{ DevReadAcceleration, 	   "DevReadAcceleration" },  
{ DevReadVelocity,  	   "DevReadVelocity" },  
{ DevReadFStepRate,	   "DevReadFStepRate" },  
{ DevReadAccSlope,	   "DevReadAccSlope" },  
{ DevStop, 		   "DevStop" },  
{ DevStopImm, 		   "DevStopImm" },  
{ DevMoveLimit, 	   "DevMoveLimit" },  
{ DevReadSwitches, 	   "DevReadSwitches" },  
{ DevLoadPosition, 	   "DevLoadPosition" },  
{ DevLimitOff, 		   "DevLimitOff" },  
{ DevLimitOn, 		   "DevLimitOn" },  
{ DevReadState, 	   "DevReadState" },  
/*  JCS  */
{ DevSetMaster,		   "DevSetMaster" },
{ DevExitMaster,	   "DevExitMaster" },
{ DevDefinePosition,	   "DevDefinePosition" },
{ DevMajorReset,	   "DevMajorReset" },
/*
 * Commands for Radio Frequency System
 */
{ DevSetup,		"DevSetup" },
{ DevReadFrequency,	"DevReadFrequency" },
{ DevSetFrequency,	"DevSetFrequency" },
{ DevReadLevel,		"DevReadLevel" },
{ DevSetLevel,		"DevSetLevel" },

{ DevGetConfFile,       "DevGetConfFile" }, 
{ DevSaveConfi,         "DevSaveConfi" }, 
{ DevGetControlMode,    "DevGetControlMode" }, 
{ DevGetDirFile,        "DevGetDirFile" }, 
{ DevGetErrors,         "DevGetErrors" },  
{ DevGetPrivilege,      "DevGetPrivilege" }, 
{ DevGetProtocolFile,   "DevGetProtocolFile" }, 
{ DevGetSigByName,   	"DevGetSigByName" }, 
{ DevGetSigByNumber, 	"DevGetSigByNumber" }, 
{ DevGetTraceFile,      "DevGetTraceFile" }, 
{ DevGetTstState,       "DevGetTstState" }, 
{ DevGetUser,           "DevGetUser" }, 
{ DevSetAlarm,          "DevSetAlarm" }, 
{ DevSetControlMode,    "DevSetControlMode" }, 
{ DevSetControlSrc,     "DevSetControlSrc" }, 
{ DevSetCrwbs,          "DevSetCrwbs" }, 
{ DevSetFilt,           "DevSetFilt" }, 
{ DevSetHvon,           "DevSetHvon" }, 
{ DevSetPasswd,         "DevSetPasswd" }, 
{ DevSetProtext,        "DevSetProtext" }, 
{ DevSetPrivilege,      "DevSetPrivilege" }, 
{ DevSetSystate,        "DevSetSystate" }, 
{ DevSetPointByName, 	"DevSetPointByName" }, 
{ DevSetPointByNumber, 	"DevSetPointByNumber" }, 
{ DevSetTransWaveFile,  "DevSetTransWaveFile" }, 
{ DevSetTransConfFile,  "DevSetTransConfFile" }, 
{ DevSetWaveForm,       "DevSetWaveForm" }, 

{ DevGetControlSrc,	"DevGetControlSrc" },
{ DevGetWaveFile,	"DevGetWaveFile" },
{ DevSetTstState,	"DevSetTstState" },
{ DevSetUser,		"DevSetUser" },
{ DevSubState,		"DevSubState" },
{ DevGetPointByNumber,	"DevGetPointByNumber" },
{ DevGetSigConfig,	"DevGetSigConfig" },
{ DevReadSigValues,	"DevReadSigValues" },
/*
 * Commands for VideoMultiplexor
 */
{ DevReadCh, 		"DevReadCh" },
{ DevSelectMon, 	"DevSelectMon" },
{ DevReadMon, 		"DevReadMon" },
{ DevSetClamp, 		"DevSetClamp" },
{ DevReadClamp, 	"DevReadClamp" },
{ DevSetSynchEltec, 	"DevSetSynchEltec" },
{ DevReadSynchEltec, 	"DevReadSynchEltec" },
{ DevSetSynchComp, 	"DevSetSynchComp" },
{ DevReadSynchComp, 	"DevReadSynchComp" },
{ DevSetMode, 		"DevSetMode" },
{ DevReadMode, 		"DevReadMode" },
/*
 * Commands for ADAS icv150
 */
{ DevStartConversion, 	"DevStartConversion" },
{ DevStopConversion, 	"DevStopConversion" },
{ DevSetChanGain, 	"DevSetChanGain" },
{ DevReadValues, 	"DevReadValues" },
{ DevTrigReadValues, 	"DevTrigReadValues" },
{ DevReadChannel, 	"DevReadChannel" },
{ DevTrigReadChannel, 	"DevTrigReadChannel" }, 
{ DevSetNum, 		"DevSetNum" },
{ DevStore, 		"DevStore" },
{ DevRestore,		"DevRestore" },
/*
 * Commands for teslameter
 */
{ DevSetMode,		"DevSetMode" },
{ DevReadMode,		"DevReadMode" },
{ DevSetSense,		"DevSetSense" },
{ DevReadSense,		"DevReadSense" },
{ DevSetUnit,		"DevSetUnit" },
{ DevReadUnit,		"DevReadUnit" },
{ DevReadDisplay,	"DevReadDisplay" },
{ DevSetRemote,		"DevSetRemote" },
{ DevReadRemote,	"DevReadRemote" },
{ DevSetPreset,		"DevSetPreset" },
{ DevReadPreset,	"DevReadPreset" },
{ DevSetMuxChannel,	"DevSetMuxChannel" },
{ DevReadMuxChannel,	"DevReadMuxChannel" },
{ DevSetNChannels,	"DevSetNChannels" },
{ DevReadNChannels,	"DevReadNChannels" },
{ DevSetSearch,		"DevSetSearch" },
{ DevReadSearch,	"DevReadSearch" },
{ DevSetSearchTime,	"DevSetSearchTime" },
{ DevReadSearchTime,	"DevReadSearchTime" },
{ DevResetTimebase,	"DevResetTimebase" },
{ DevReadHardStatus,	"DevReadHardStatus" },
/*
 * Commands for Beam Position Monitors
 */
{ DevBpmSetMode, 	"DevBpmSetMode" }, 
{ DevBpmReadMode, 	"DevBpmReadMode" },
{ DevBpmSetPar,		"DevBpmSetPar" },
{ DevBpmReadPar,	"DevBpmReadPar" },
{ DevBpmSetStat,	"DevBpmSetStat" },
{ DevBpmReadStat,	"DevBpmReadStat" },
{ DevBpmReadLast,	"DevBpmReadLast" },
{ DevBpmReadNew,	"DevBpmReadNew" },
{ DevBpmReadElec,	"DevBpmReadElec" },
{ DevBpmReadHard,	"DevBpmReadHard" },
{ DevBpmReadCali,	"DevBpmReadCali" },
{ DevBpmWrite,		"DevBpmWrite" },
{ DevStGrp,		"DevStGrp" },
{ DevRdGrp,		"DevRdGrp" },
{ DevEnaVp,		"DevEnaVp" },
{ DevDisVp,		"DevDisVp" },
{ DevStiVp,		"DevStiVp" },
{ DevStRfG,		"DevStRfG" },
{ DevSlRfO,		"DevSlRfO" },
{ DevStTiW,		"DevStTiW" },
{ DevStHwM,		"DevStHwM" },
{ DevSlTrS,		"DevSlTrS" },
{ DevSlEle,		"DevSlEle" },
{ DevSnFtR,		"DevSnFtR" },
{ DevSnSoT,		"DevSnSoT" },
{ DevRdHwS,		"DevRdHwS" },
{ DevRdMem,		"DevRdMem" },
{ DevRdCnt,		"DevRdCnt" },
{ DevRsAdc,		"DevRsAdc" },
{ DevStDir,		"DevStDir" },
{ DevSlNPa,		"DevSlNPa" },
/*
 * Commands for Vpdu
 */
{ DevRdDlR,		"DevRdDlR" },
{ DevWrDlR,		"DevWrDlR" },
{ DevRdCoR,		"DevRdCoR" },
{ DevWrCoR,		"DevWrCoR" },
{ DevSetCoR,		"DevSetCoR" },
{ DevResCoR,		"DevResCoR" },
{ DevSofTr,		"DevSofTr" },
{ DevSetWd,		"DevSetWd" },
/*
 * Commands for Timer
 */
{ DevSetDelay,		"DevSetDelay" },
/*
 * Commands for G64 ADC
 */
{ DevMemRd,		"DevMemRd" },
{ DevAdcEn,		"DevAdcEn" },
/*
 * Commands for Gescio1
 */
{ DevRd16b,		"DevRd16b" },
{ DevWr16b,		"DevWr16b" },
{ DevWrMs16b,		"DevWrMs16b" },
{ DevReRd16b,		"DevReRd16b" },
{ DevSet16b,		"DevSet16b" },
{ DevRset16b,		"DevRset16b" },
{ DevMod16b,		"DevMod16b" },
/*
 * Commands for ImageUx/ImageOS9
 */
{ DevDisplayBeam,	"DevDisplayBeam" },
/*
 * Commands for Booster Power Supplies System
 */
{ DevWarmUp,      	"DevWarmUp" },
{ DevMacro,       	"DevMacro" },
{ DevLock,        	"DevLock" },
{ DevFree,        	"DevFree" },
{ DevStatusMacro, 	"DevStatusMacro" },
{ DevStopMacro,   	"DevStopMacro" },
{ DevSetPhase,    	"DevSetPhase" },
{ DevReadPhase,   	"DevReadPhase" },
{ DevSetPeriod,   	"DevSetPeriod" },
{ DevReadPeriod,  	"DevReadPeriod" },
{ DevSetAc,       	"DevSetAc" },
{ DevReadAc,      	"DevReadAc" },
{ DevSetDc,       	"DevSetDc" },
{ DevReadDc,      	"DevReadDc" },
{ DevSetIacInv,   	"DevSetIacInv" },
{ DevReadIacInv,  	"DevReadIacInv" },
{ DevCouple,      	"DevCouple" },
{ DevTrack,       	"DevTrack" },
{ DevUnTrack,     	"DevUnTrack" },
{ DevSearch,      	"DevSearch" },
{ DevReadTemper,  	"DevReadTemper" },
{ DevReadPhLagUi, 	"DevReadPhLagUi" },
{ DevSetDc, 		"DevSetDc" },
/*
 * Commands for Gespac Digital to Analogic 8 Channels Converter
 */
{ DevSetRange,		"DevSetRange" },
{ DevGetRange,		"DevGetRange" },
{ DevSetChannel,	"DevSetChannel" },
{ DevGetChannel,	"DevGetChannel" },
{ DevSetIncrement,	"DevSetIncrement" },
{ DevGetIncrement,	"DevGetIncrement" },
{ DevIncrement,		"DevIncrement" },
{ DevDecrement,		"DevDecrement" },
/*
 * Commands for Diagnostics Current Transformer
 */
{ DevGetTest,		"DevGetTest" },
{ DevSetTest,		"DevSetTest" },
{ DevGetCalib,		"DevGetCalib" },
{ DevSetCalib,		"DevSetCalib" },
{ DevReadVar,		"DevReadVar" },
{ DevReadEnd,		"DevReadEnd" },
{ DevGetSens,		"DevGetSens" },
{ DevSetSens,		"DevSetSens" },
{ DevSetSensMan,	"DevSetSensMan" },
{ DevReadSens,		"DevReadSens" },
{ DevGetOffsetB,	"DevGetOffsetB" },
{ DevWriteAcqu,		"DevWriteAcqu" },
{ DevReadAcqu,		"DevReadAcqu" },
{ DevGetAverage,	"DevGetAverage" },
{ DevGetAcquInfos,	"DevGetAcquInfos" },
{ DevGetMpvErrors,	"DevGetMpvErrors" },
{ DevReadLife,		"DevReadLife" },
{ DevDtLifeTime,	"DevDtLifeTime" },
{ DevDiLifeTime,	"DevDiLifeTime" },
{ DevGetDeltaTime,	"DevGetDeltaTime" },
{ DevSetDeltaTime,	"DevSetDeltaTime" },
{ DevGetDeltaInt,	"DevGetDeltaInt" },
{ DevSetDeltaInt,	"DevSetDeltaInt" },
{ DevGetMeasureTime,	"DevGetMeasureTime" },
{ DevSetMeasureTime,	"DevSetMeasureTime" },
/*
 * DiagNostics SY and SR Tunning Monitor
 */
{ DevGetBeam,		"DevGetBeam" },
{ DevSetBeam,		"DevSetBeam" },
{ DevGetStrength,	"DevGetStrength" },
{ DevSetStrength,	"DevSetStrength" },
{ DevGetMethod,		"DevGetMethod" },
{ DevSetMethod,		"DevSetMethod" },
{ DevGetPlane,		"DevGetPlane" },
{ DevSetPlane,		"DevSetPlane" },
{ DevGetDelays,		"DevGetDelays" },
{ DevSetDelays,		"DevSetDelays" },
{ DevSetConfig,		"DevSetConfig" },
{ DevSetTimers,		"DevSetTimers" },
/*
 * Rug Beam Monitoring System
 */
{ DevGetMode,		"DevGetMode" },
{ DevGetLength,		"DevGetLength" },
{ DevGetCurrent,	"DevGetCurrent" },
{ DevSetAutoDump,	"DevSetAutoDump" },
/*
 * Commands for the Vrif class
 */
{ DevResetVme,		"DevResetVme" },
{ DevSetAsIndic,	"DevSetAsIndic" },
{ DevEnWdog,		"DevEnWdog" },
{ DevDisWdog,		"DevDisWdog" },
{ DevReadVmeStat,	"DevReadVmeStat" },
{ DevReadPowerStat,	"DevReadPowerStat" },
{ DevReadCrateNum,	"DevReadCrateNum" },
/*
 *  Commands for HLS system
 */
{ DevFullData,		"DevFullData" },
/*
 * Commands for Piezo
 */
{ DevMovePosition,	"DevMovePosition" },
/*
 * Commands for seismic survey
 */
{ DevGetEventList,       "DevGetEventList"},
{ DevReadEvent,          "DevGetEventList"},
{ DevReadLastEvent,      "DevReadLastEvent"},
{ DevReadSpectra,        "DevReadSpectra"},
/*
 * Commands for LIEN automat
 */
{ DevReadThreshold,	"DevReadThreshold"},
{ DevSetThreshold,	"DevSetThreshold"},
{ DevReadInput,		"DevReadInput"},
{ DevReadOutput,	"DevReadOutput"},
{ DevReadCutTh,		"DevReadCutTh"},
{ DevReadCmds,		"DevReadCmds"},
{ DevSetCmds,		"DevSetCmds"},
{ DevEraseCmds,		"DevEraseCmds"},
{ DevReadCmdErr,	"DevReadCmdErr"},
{ DevReadNew,		"DevReadNew"},
{ DevWriteNew,		"DevWriteNew"},
{ DevReadAll,		"DevReadAll"},
{ DevReadAlarm,		"DevReadAlarm"},
/*
 * Commands for the Update Daemon
 */
{ DevGetDeviceIdent,	"DevGetDeviceIdent"},
{ DevInitialiseDevice,	"DevInitialiseDevice"},
{ DevStartPolling,	"DevStartPolling"},
{ DevStopPolling,	"DevStopPolling"},
{ DevChangeInterval,	"DevChangeInterval"},
{ DevDefineMode,	"DevDefineMode"},
{ DevPollStatus,	"DevPollStatus"},
{ DevAccessStatus,	"DevAccessStatus"},
{ DevGetDeviceName,	"DevGetDeviceName"},
/*
 *  Commands for LINAC
 */
{ DevWriteAna,		"DevWriteAna"},
{ DevWriteDig,		"DevWriteDig"},
{ DevDelFile,		"DevDelFile"},
{ DevDirFiles,		"DevDirFiles"},
{ DevSelectFile,	"DevSelectFile"},
{ DevReadConfig,	"DevReadConfig"},
{ DevViewFile,		"DevViewFile"},
/*
 *  Commands for FrontEnd Class
 */
{ DevAxeType,		"DevAxeType"},
{ DevCloseShutter,	"DevCloseShutter"},
{ DevCloseAbsorber,	"DevCloseAbsorber"},
{ DevStopBeam,		"DevStopBeam"},
/*
 *  Commands for Powermeter
 */
{ DevReadPower,		"DevReadPower"},



/*
 *      Commands for DAS group
 *      Device Servers for beamline control
 */

/*
 * Commands for GPIB
 */
{ DevIntClear,		"DevIntClear" },
{ DevClear,		"DevClear" },
{ DevParPoll,		"DevParPoll" },
{ DevTrigger,		"DevTrigger" },
{ DevLocalLock,		"DevLocalLock" },
{ DevSerPoll,		"DevSerPoll" },
{ DevGetDevs,		"DevGetDevs" },
{ DevSetDevs,		"DevSetDevs" },
{ DevPassCtrl,		"DevPassCtrl" },
{ DevParPollConf,	"DevParPollConf" },
{ DevParPollDis,	"DevParPollDis" },
{ DevParPollUnc,	"DevParPollUnc" },
{ DevListen,		"DevListen" },
{ DevTalk,		"DevTalk" },
{ DevUnListen,		"DevUnListen" },
{ DevUnTalk,		"DevUnTalk" },
{ DevGetLocal,		"DevGetLocal" },
{ DevSetLocal,		"DevSetLocal" },
{ DevMulClear,		"DevMulClear" },
{ DevMulListen,		"DevMulListen" },
{ DevMulLocal,		"DevMulLocal" },
{ DevMulPassCtrl,	"DevMulPassCtrl" },
{ DevMulParPollDis,	"DevMulParPollDis" },
{ DevMulParPollUnc,	"DevMulParPollUnc" },
{ DevMulRemote,		"DevMulRemote" },
{ DevMulWrite,		"DevMulWrite" },
/*
 * Commands for incremental encoders
 */
{ DevReadCount,		"DevReadCount" },
{ DevWriteCount,	"DevWriteCount" },
{ DevGetDig,		"DevGetDig" },
{ DevGetInd,		"DevGetInd" },
/* 
 * New commands for steppermotors 
 */
{ DevTrigClear,         "DevTrigClear"},
{ DevWriteMux,          "DevWriteMux"},
{ DevStartMemWrite,     "DevStartMemWrite"},
{ DevStopMemWrite,      "DevStopMemWrite"},
{ DevMoveAbsoluteDel,   "DevMoveAbsoluteDel"},
{ DevMoveRelativeDel,   "DevMoveRelativeDel"},
{ DevTrigMotion,        "DevTrigMotion"},
{ DevSetStepMode,       "DevSetStepMode"},
{ DevReadStepMode,      "DevReadStepMode"},
{ DevWriteWait,         "DevWriteWait"},
{ DevReadMulPos,        "DevReadMulPos"},
{ DevReadMulEncPos,     "DevReadMulEncPos"},
{ DevReadFactor,        "DevReadFactor"},
{ DevSetUnits,          "DevSetUnits"},
{ DevReadEncPos,        "DevReadEncPos"},
{ DevLoadEncPos,        "DevLoadEncPos"},
{ DevCloseBrake,        "DevCloseBrake"},
{ DevOpenBrake,         "DevOpenBrake"},
{ DevReadUnits,         "DevReadUnits"},
{ DevReadBrakeState,    "DevReadBrakeState"},
{ DevSetBacklash,       "DevSetBacklash"},
{ DevReadBacklash,      "DevReadBacklash"},
{ DevMulState,          "DevMulState"},
{ DevReadMulAcc,        "DevReadMulAcc"},
{ DevReadMulFSR,        "DevReadMulFSR"},
{ DevReadMulVel,        "DevReadMulVel"},
{ DevReadMulHSR,        "DevReadMulHSR"},
{ DevReadMulBrState,    "DevReadMulBrState"},
{ DevReadMulSwitches,   "DevReadMulSwitches"},
{ DevReadMulUnits,      "DevReadMulUnits"},
{ DevReadMulBacklash,   "DevReadMulBacklash"},
{ DevMoveEncAbsolute,   "DevMoveEncAbsolute"},
{ DevStartServo,        "DevStartServo"},
{ DevStopServo,         "DevStopServo"},
{ DevEnablePower,       "DevEnablePower"},
{ DevDisablePower,      "DevDisablePower"},
 
/* Commands for slits */
{ DevReadBladeState,    "DevReadBladeState"},
{ DevReadPslitState,    "DevReadPslitState"},
{ DevSetRelGap,         "DevSetRelGap"},
{ DevSetAbsGap,         "DevSetAbsGap"},
{ DevSetRelOff,         "DevSetRelOff"},
{ DevSetAbsOff,         "DevSetAbsOff"},
{ DevSwitchOn,          "DevSwitchOn"},
{ DevSwitchOff,         "DevSwitchOff"},
{ DevSetZero,           "DevSetZero"},
{ DevMakeReference,     "DevMakeReference"},
{ DevSetTuned,          "DevSetTuned"},

/* Command(s) for attenuators */
{ DevGotoFilter,        "DevGotoFilter"},    

/***** Command(s) for LECROY 1151 counter	-FEP-  *****
 *
 * removed for time being ... andy 20/3/2001
 *
{DevCntIdentify,		"DevCntIdentify"},
{DevCntStatus,			"DevCntStatus"},
{DevCntReset, 			"DevCntReset"},
{DevCntAbort, 			"DevCntAbort"},
{DevCntClearChannel,	"DevCntClearChannel"},
{DevCntPresetChannel,	"DevCntPresetChannel"},
{DevCntReadChannel, 	"DevCntReadChannel"},

/***** Command(s) for CAEN V462 gate generator	-FEP-  *****
{DevGategenIdentify,"DevGategenIdentify"},
{DevGategenStatus,"DevGategenStatus"},
{DevGategenReset, "DevGategenReset"},
{DevGategenStopChannel, "DevGategenStopChannel"},
{DevGategenStartChannel,"DevGategenStartChannel"},
{DevGategenPresetChannel,"DevGategenPresetChannel"},
 */

/*** Hbm ***/
{DevReadErr,"DevReadErr"},

/*** M4 ****/
{DevReadMot,"DevReadMot"},
{DevReadSen,"DevReadSen"},
{DevReadAxe,"DevReadAxe"},
{DevReadClutch,"DevReadClutch"},
{DevApplyBrake,"DevApplyBrake"},
{DevReleaseBrake,"DevReleaseBrake"},
{DevClutchTrig,"DevClutchTrig"},
{DevReport,"DevReport"},
{DevLoadMot,"DevLoadMot"},
{DevLoadEnc,"DevLoadEnc"},
/*
 * {DevEnableSw,"DevEnableSw"},
 * {DevDisableSw,"DevDisableSw"},
 * {DevMoveGroup,"DevMoveGroup"},
 */

/*** Clutch ***/
{DevClutchOn,"DevClutchOn"},
{DevClutchOff,"DevClutchOff"},
{DevClutchState,"DevClutchState"},

/*** New commands for IcvAdc *** TM ***/
{DevTrigReadAll,"DevTrigReadAll"},
{DevReadChanGain,"DevReadChanGain"},
{DevSetLimits,"DevSetLimits"},
{DevReadLimits,"DevReadLimits"},
{DevReadAlarmAll,"DevReadAlarmAll"},
{DevSetMinMax,"DevSetMinMax"},
{DevReadMinMax,"DevReadMinMax"},  

/*** Tango mapping commands ***/
{DevState, "State"},
{DevStatus,"Status"},
{DevReset, "Reset"},
{DevOff, "Off"},
{DevStandby, "Standby"},
{DevOn, "On"},
{DevOpen, "Open"},
{DevClose, "Close"},
};

/*
 *  number of commands in name list
 */

long max_cmds = (sizeof(DevCmdNameList)/sizeof(DevCmdNameListEntry));
