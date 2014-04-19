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
 * File:		dev_cmds.c
 *
 * Project:	Device Servers 
 *
 * Description:	List of command names
 *
 * Author(s):	Andy Gotz
 *
 * Original:	March 1992
 *
 * Version:	$Revision: 1.4 $
 *
 * Date:	$Date: 2012-11-14 07:56:36 $
 *
 ********************************************************************-*/

#include <ExpDsNumbers.h>
#include <DevCcdCmds.h>
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
{ DevWritePos,           "DevWritePos" },  
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

/*** DevCcdCmds ***/
{DevCcdStart, "DevCcdStart"},
{DevCcdStop, "DevCcdStop"},
{DevCcdRead, "DevCcdRead"},
{DevCcdSetExposure, "DevCcdSetExposure"},
{DevCcdGetExposure, "DevCcdGetExposure"},
{DevCcdSetRoI, "DevCcdSetRoI"},
{DevCcdGetRoI, "DevCcdGetRoI"},
{DevCcdSetBin, "DevCcdSetBin"},
{DevCcdGetBin, "DevCcdGetBin"},
{DevCcdSetTrigger, "DevCcdSetTrigger"},
{DevCcdGetTrigger, "DevCcdGetTrigger"},
{DevCcdGetLstErrMsg, "DevCcdGetLstErrMsg"},
{DevCcdXSize, "DevCcdXSize"},
{DevCcdYSize, "DevCcdYSize"},
{DevCcdSetADC, "DevCcdSetADC"},
{DevCcdGetADC, "DevCcdGetADC"},
{DevCcdSetSpeed, "DevCcdSetSpeed"},
{DevCcdGetSpeed, "DevCcdGetSpeed"},
{DevCcdSetShutter, "DevCcdSetShutter"},
{DevCcdGetShutter, "DevCcdGetShutter"},
{DevCcdSetFrames, "DevCcdSetFrames"},
{DevCcdGetFrames, "DevCcdGetFrames"},
{DevCcdCommand, "DevCcdCommand"},
{DevCcdDepth, "DevCcdDepth"},
{DevCcdSetMode, "DevCcdSetMode"},
{DevCcdGetMode, "DevCcdGetMode"},
{DevCcdSetChannel, "DevCcdSetChannel"},
{DevCcdGetChannel, "DevCcdGetChannel"},
{DevCcdSetRingBuf, "DevCcdSetRingBuf"},
{DevCcdGetRingBuf, "DevCcdGetRingBuf"},
{DevCcdLive, "DevCcdLive"},
{DevCcdWriteFile, "DevCcdWriteFile"},
{DevCcdReset, "DevCcdReset"},
{DevCcdGetIdent, "DevCcdGetIdent"},
{DevCcdGetType, "DevCcdGetType"},
{DevCcdSetKinWinSize, "DevCcdSetKinWinSize"},
{DevCcdGetKinWinSize, "DevCcdGetKinWinSize"},
{DevCcdSetKinetics, "DevCcdSetKinetics"},
{DevCcdGetKinetics, "DevCcdGetKinetics"},
{DevCcdCorrect, "DevCcdCorrect"},
{DevCcdSetFilePar, "DevCcdSetFilePar"},
{DevCcdGetFilePar, "DevCcdGetFilePar"},
{DevCcdHeader, "DevCcdHeader"},
{DevCcdSetFormat, "DevCcdSetFormat"},
{DevCcdGetFormat, "DevCcdGetFormat"},
{DevCcdSetViewFactor, "DevCcdSetViewFactor"},
{DevCcdGetViewFactor, "DevCcdGetViewFactor"},
{DevCcdSetHwPar, "DevCcdSetHwPar"},
{DevCcdGetHwPar, "DevCcdGetHwPar"},
{DevCcdGetCurrent, "DevCcdGetCurrent"},
{DevCcdGetBuffer, "DevCcdGetBuffer"},
{DevCcdGetBufferInfo, "DevCcdGetBufferInfo"},
{DevCcdReadAll, "DevCcdReadAll"},
{DevCcdWriteAll, "DevCcdWriteAll"},
{DevCcdDezinger, "DevCcdDezinger"},
{DevCcdSetThreshold, "DevCcdSetThreshold"},
{DevCcdGetThreshold, "DevCcdGetThreshold"},
{DevCcdSetMaxExposure, "DevCcdSetMaxExposure"},
{DevCcdGetMaxExposure, "DevCcdGetMaxExposure"},
{DevCcdSetGain, "DevCcdSetGain"},
{DevCcdGetGain, "DevCcdGetGain"},
{DevCcdReadJpeg, "DevCcdReadJpeg"},
{DevCcdRefreshTime, "DevCcdRefreshTime"},
{DevCcdOutputSize, "DevCcdOutputSize"},
{DevCcdGetTGradient, "DevCcdGetTGradient"},
{DevCcdGetChanges, "DevCcdGetChanges"},
{DevCcdCalibrate, "DevCcdCalibrate"},
{DevCcdSetThumbnail1, "DevCcdSetThumbnail1"},
{DevCcdSetThumbnail2, "DevCcdSetThumbnail2"},
{DevCcdWriteThumbnail1, "DevCcdWriteThumbnail1"},
{DevCcdWriteThumbnail2, "DevCcdWriteThumbnail2"},
};

/*
 *  number of commands in name list
 */

long max_cmds = (sizeof(DevCmdNameList)/sizeof(DevCmdNameListEntry));
