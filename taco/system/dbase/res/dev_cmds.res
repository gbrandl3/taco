#static char RcsId[]      =
#"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/system/dbase/res/dev_cmds.res,v 1.1 2003-04-25 12:54:17 jkrueger1 Exp $";
#
#/*+*******************************************************************
#
# File:		dev_cmds.res
#
# Project:	Device Servers 
#
# Description:	command name strings as resources
#
# Author(s):	Jens Meyer
#
# Original:	July 1993
#
# Copyright (c) 1992 by European Synchrotron Radiation Facility, 
#                      Grenoble, France
#
#********************************************************************-*/


#
# general commands
#

CMDS/0/0/1:	"DevOn"
CMDS/0/0/2:	"DevOff"
CMDS/0/0/3:	"DevReset"
CMDS/0/0/4:	"DevState"
CMDS/0/0/5:	"DevStandby"
CMDS/0/0/6:	"DevRun"
CMDS/0/0/7:	"DevLocal"
CMDS/0/0/8:	"DevStatus"
CMDS/0/0/9:	"DevOpen"
CMDS/0/0/10:	"DevClose"
CMDS/0/0/11:	"DevSetValue"
CMDS/0/0/12:	"DevReadValue"
CMDS/0/0/13:	"DevInsert"
CMDS/0/0/14:	"DevExtract"
CMDS/0/0/15:	"DevReadPos"
CMDS/0/0/16:	"DevWritPos"
CMDS/0/0/17:	"DevSetParam"
CMDS/0/0/18:	"DevReadParam"
CMDS/0/0/19:	"DevUnlock"

CMDS/0/0/20:	"DevSetConversion"  
CMDS/0/0/21:	"DevSelectCh"  
CMDS/0/0/22:	"DevSetProtThresh"  
CMDS/0/0/23:	"DevReadPressure"  
CMDS/0/0/24:	"DevResetInputBuffer"  
CMDS/0/0/25:	"DevChangeCalibration"  
CMDS/0/0/26:	"DevNAve"  
CMDS/0/0/27:	"DevHello"  
CMDS/0/0/28:	"DevRead"
CMDS/0/0/29:	"DevWrite"
#/*
# * commands for pneumatic valves
# */
CMDS/0/0/30:	"DevReadCommand"  
CMDS/0/0/31:	"DevForce"
CMDS/0/0/32:	"DevUnforce"
#/*
# * commands for power supplies
# */
CMDS/0/0/39:	"DevReadSetValue"
CMDS/0/0/40:	"DevSetVoltage"  
CMDS/0/0/41:	"DevReadVoltage"  
CMDS/0/0/42:	"DevError"  
CMDS/0/0/43:	"DevSetCurrent"  
CMDS/0/0/44:	"DevReadCurrent"  
CMDS/0/0/45:	"DevShortStatus"  
CMDS/0/0/46:	"DevIndicator"  
#/* hazemeyer ps */
CMDS/0/0/47:	"DevGetName"
CMDS/0/0/48:	"DevSetTimeout"
CMDS/0/0/49:	"DevCalibration"
#/*
# * commands for vacuum gauge controller
# */
CMDS/0/0/50:	"DevLongStatus"
CMDS/0/0/51:	"DevRemote"
CMDS/0/0/52:	"DevGaugeStatus"
CMDS/0/0/53:	"DevGaugeOn"
CMDS/0/0/54:	"DevGaugeOff"
CMDS/0/0/55:	"DevChangeSetpoint"
CMDS/0/0/56:	"DevSetGasFactor"
CMDS/0/0/57:	"DevSetFilterTime"
CMDS/0/0/58:	"DevSelectCalibration"
CMDS/0/0/59:	"DevEnergiseRelay"
CMDS/0/0/60:	"DevDeenergiseRelay"
CMDS/0/0/61:	"DevGaugeReadOut"
CMDS/0/0/62:	"DevParamReadOut"
CMDS/0/0/63:	"DevMaxPressure"
#/*
# * commands for riber ion pump controller
# */
CMDS/0/0/70:	"DevHTOn"
CMDS/0/0/71:	"DevHTOff"
CMDS/0/0/72:	"DevReadHT"
CMDS/0/0/73:	"DevReadDU"
CMDS/0/0/74:	"DevDUOn"
CMDS/0/0/75:	"DevDUOff"
CMDS/0/0/76:	"DevReadVacuum"
CMDS/0/0/77:	"DevAbortStartup"
CMDS/0/0/78:	"DevShutdown"
CMDS/0/0/79:	"DevStartBake"
CMDS/0/0/80:	"DevStopBake"
CMDS/0/0/81:	"DevCheckHT"
#/*
# * Gadc related commands
# */
CMDS/0/0/100:	"DevSetPreAmpGain"
CMDS/0/0/101:	"DevReadPreAmpGain"
CMDS/0/0/102:	"DevSetAmpGain"
CMDS/0/0/103:	"DevReadAmpGain"
CMDS/0/0/104:	"DevReadADCMode"
#/*
# * WireScanner related commands
# */
CMDS/0/0/110:	"DevReadValueSingle"
CMDS/0/0/111:	"DevReadValueCycle"     
CMDS/0/0/112:	"DevReadBeamWidth"  
#/*
# * Horizontal wirescanner
# */
CMDS/0/0/120:	"DevSetHPreAmpGain"      
CMDS/0/0/121:	"DevReadHPreAmpGain"   
CMDS/0/0/122:	"DevSetHAmpGain"      
CMDS/0/0/123:	"DevReadHAmpGain"    
CMDS/0/0/124:	"DevReadHADCMode"     
CMDS/0/0/125:	"DevReadHCurrent"       
#/*
# * Vertical wirescanner
# */
CMDS/0/0/130:	"DevSetVPreAmpGain"     
CMDS/0/0/131:	"DevReadVPreAmpGain"     
CMDS/0/0/132:	"DevSetVAmpGain"         
CMDS/0/0/133:	"DevReadVAmpGain"       
CMDS/0/0/134:	"DevReadVADCMode"      
CMDS/0/0/135:	"DevReadVCurrent"     
#/*
# * Commands for THC
# */ 
CMDS/0/0/140:	"DevReadThController"
CMDS/0/0/141:	"DevDownLoadThController"
#/*
# * Commands for PLC
# */ 
CMDS/0/0/150:	"DevIOState"	
CMDS/0/0/151:	"DevForceIO"	
CMDS/0/0/152:	"DevReadIntBit"	
CMDS/0/0/153:	"DevReadIntWord"
CMDS/0/0/154:	"DevReadSysBit"
CMDS/0/0/155:	"DevReadSysWord"
CMDS/0/0/156:	"DevWriteIntBit"
CMDS/0/0/157:	"DevWriteIntWord"
CMDS/0/0/158:	"DevWriteSysBit"
CMDS/0/0/159:	"DevWriteSysWord"
CMDS/0/0/160:	"DevForceIntBit"
CMDS/0/0/161:	"DevEnableDownLoad"
CMDS/0/0/162:	"DevDisableDownLoad"
CMDS/0/0/163:	"DevOkDownLoad"
CMDS/0/0/164:	"DevReadIntBitSeq"
#/*
# * Stepper Motor related commands
# */
CMDS/0/0/166:	"DevSetHomeSlewRate"
CMDS/0/0/167:	"DevReadHomeSlewRate"
CMDS/0/0/168:	"DevMoveMultipleRel"
CMDS/0/0/169:	"DevMoveMultiple"
CMDS/0/0/170:	"DevSetContinuous"  
CMDS/0/0/171:	"DevMoveRelative"  
CMDS/0/0/172:	"DevMoveAbsolute"  
CMDS/0/0/173:	"DevUpdate"  
CMDS/0/0/174:	"DevAbortCommand"  
CMDS/0/0/175:	"DevSetAcceleration"  
CMDS/0/0/176:	"DevSetVelocity"  
CMDS/0/0/177:	"DevSetDirection"  
CMDS/0/0/178:	"DevSetFirstStepRate"  
CMDS/0/0/179:	"DevSetAccSlope"  
CMDS/0/0/180:	"DevReadPosition"  
CMDS/0/0/181:	"DevAdditionalState"  
CMDS/0/0/182:	"DevMoveReference"  
CMDS/0/0/183:	"DevReadAcceleration"  
CMDS/0/0/184:	"DevReadVelocity"  
CMDS/0/0/185:	"DevReadFStepRate"  
CMDS/0/0/186:	"DevReadAccSlope"  
CMDS/0/0/187:	"DevStop"  
CMDS/0/0/188:	"DevStopImm"  
CMDS/0/0/189:	"DevMoveLimit"  
CMDS/0/0/190:	"DevReadSwitches"  
CMDS/0/0/191:	"DevLoadPosition"  
CMDS/0/0/192:	"DevLimitOff"  
CMDS/0/0/193:	"DevLimitOn"  
CMDS/0/0/194:	"DevReadState"  
#/*  JCS  *4
CMDS/0/0/195:	"DevSetMaster"
CMDS/0/0/196:	"DevExitMaster"
CMDS/0/0/197:	"DevDefinePosition"
CMDS/0/0/198:	"DevMajorReset"
#/*
# * Commands for Radio Frequency System
# */
CMDS/0/0/200:	"DevSetup"
CMDS/0/0/201:	"DevReadFrequency"
CMDS/0/0/202:	"DevSetFrepuency"
CMDS/0/0/203:	"DevReadLevel"
CMDS/0/0/204:	"DevSetLevel"

CMDS/0/0/205:	"DevGetConfFile" 
CMDS/0/0/206:	"DevSaveConfi" 
CMDS/0/0/207:	"DevGetControlMode" 
CMDS/0/0/208:	"DevGetDirFile" 
CMDS/0/0/209:	"DevGetErrors"  
CMDS/0/0/210:	"DevGetPrivilege" 
CMDS/0/0/211:	"DevGetProtocolFile" 
CMDS/0/0/212:	"DevGetSigByName" 
CMDS/0/0/213:	"DevGetSigByNumber" 
CMDS/0/0/214:	"DevGetTraceFile" 
CMDS/0/0/215:	"DevGetTstState" 
CMDS/0/0/216:	"DevGetUser" 
CMDS/0/0/217:	"DevSetAlarm" 
CMDS/0/0/218:	"DevSetControlMode" 
CMDS/0/0/219:	"DevSetControlSrc" 
CMDS/0/0/220:	"DevSetCrwbs" 
CMDS/0/0/221:	"DevSetFilt" 
CMDS/0/0/222:	"DevSetHvon" 
CMDS/0/0/223:	"DevSetPasswd" 
CMDS/0/0/224:	"DevSetProtext" 
CMDS/0/0/225:	"DevSetPrivilege" 
CMDS/0/0/226:	"DevSetSystate" 
CMDS/0/0/227:	"DevSetPointByName" 
CMDS/0/0/228:	"DevSetPointByNumber" 
CMDS/0/0/229:	"DevSetTransWaveFile" 
CMDS/0/0/230:	"DevSetTransConfFile" 
CMDS/0/0/231:	"DevSetWaveForm" 

CMDS/0/0/232:	"DevGetControlSrc"
CMDS/0/0/233:	"DevGetWaveFile"
CMDS/0/0/234:	"DevSetTstState"
CMDS/0/0/235:	"DevSetUser"
CMDS/0/0/236:	"DevSubState"
CMDS/0/0/237:	"DevGetPointByNumber"
CMDS/0/0/238:	"DevGetSigConfig"
CMDS/0/0/199:	"DevUpdateSigConfig"
CMDS/0/0/239:	"DevReadSigValues"
#/*
# * Commands for VideoMultiplexor
# */
CMDS/0/0/240:	"DevReadCh"
CMDS/0/0/241:	"DevSelectMon"
CMDS/0/0/242:	"DevReadMon"
CMDS/0/0/243:	"DevSetClamp"
CMDS/0/0/244:	"DevReadClamp"
CMDS/0/0/245:	"DevSetSynchEltec"
CMDS/0/0/246:	"DevReadSynchEltec"
CMDS/0/0/247:	"DevSetSynchComp"
CMDS/0/0/248:	"DevReadSynchComp"
CMDS/0/0/249:	"DevSetMode"
CMDS/0/0/250:	"DevReadMode"
#/*
# * Commands for ADAS icv150
# */
CMDS/0/0/260:	"DevStartConversion"
CMDS/0/0/261:	"DevStopConversion"
CMDS/0/0/262:	"DevSetChanGain"
CMDS/0/0/263:	"DevReadValues"
CMDS/0/0/264:	"DevTrigReadValues"
CMDS/0/0/265:	"DevReadChannel"
CMDS/0/0/266:	"DevTrigReadChannel" 
CMDS/0/0/267:	"DevSetNum"
CMDS/0/0/268:	"DevStore"
CMDS/0/0/269:	"DevRestore"
#/*
# * Commands for teslameter
# */
CMDS/0/0/270:	"DevSetSense"
CMDS/0/0/271:	"DevReadSense"
CMDS/0/0/272:	"DevSetUnit"
CMDS/0/0/273:	"DevReadUnit"
CMDS/0/0/274:	"DevReadDisplay"
CMDS/0/0/275:	"DevSetRemote"
CMDS/0/0/276:	"DevReadRemote"
CMDS/0/0/277:	"DevSetPreset"
CMDS/0/0/278:	"DevReadPreset"
CMDS/0/0/279:	"DevSetMuxChannel"
CMDS/0/0/280:	"DevReadMuxChannel"
CMDS/0/0/281:	"DevSetNChannels"
CMDS/0/0/282:	"DevReadNChannels"
CMDS/0/0/283:	"DevSetSearch"
CMDS/0/0/284:	"DevReadSearch"
CMDS/0/0/285:	"DevSetSearchTime"
CMDS/0/0/286:	"DevReadSearchTime"
CMDS/0/0/287:	"DevResetTimebase"
CMDS/0/0/288:	"DevReadHardStatus"
#/*
# * Commands for Beam Position Monitors
# */
CMDS/0/0/300:	"DevBpmSetMode" 
CMDS/0/0/301:	"DevBpmReadMode"
CMDS/0/0/302:	"DevBpmSetPar"
CMDS/0/0/303:	"DevBpmReadPar"
CMDS/0/0/304:	"DevBpmSetStat"
CMDS/0/0/305:	"DevBpmReadStat"
CMDS/0/0/306:	"DevBpmReadLast"
CMDS/0/0/307:	"DevBpmReadNew"
CMDS/0/0/308:	"DevBpmReadElec"
CMDS/0/0/309:	"DevBpmReadHard"
CMDS/0/0/310:	"DevBpmReadCali"
CMDS/0/0/311:	"DevBpmWrite"
CMDS/0/0/312:	"DevStGrp"
CMDS/0/0/313:	"DevRdGrp"
CMDS/0/0/314:	"DevEnaVp"
CMDS/0/0/315:	"DevDisVp"
CMDS/0/0/316:	"DevStiVp"
CMDS/0/0/317:	"DevStRfG"
CMDS/0/0/318:	"DevSlRfO"
CMDS/0/0/319:	"DevStTiW"
CMDS/0/0/320:	"DevStHwM"
CMDS/0/0/321:	"DevSlTrS"
CMDS/0/0/322:	"DevSlEle"
CMDS/0/0/323:	"DevSnFtR"
CMDS/0/0/324:	"DevSnSoT"
CMDS/0/0/325:	"DevRdHwS"
CMDS/0/0/326:	"DevRdMem"
CMDS/0/0/327:	"DevRdCnt"
CMDS/0/0/328:	"DevRsAdc"
CMDS/0/0/329:	"DevStDir"
CMDS/0/0/330:	"DevSlNPa"
#/*
# * Commands for Vpdu
# */
CMDS/0/0/340:	"DevRdDlR"
CMDS/0/0/341:	"DevWrDlR"
CMDS/0/0/342:	"DevRdCoR"
CMDS/0/0/343:	"DevWrCoR"
CMDS/0/0/344:	"DevSetCoR"
CMDS/0/0/345:	"DevResCoR"
CMDS/0/0/346:	"DevSofTr"
CMDS/0/0/347:	"DevSetWd"
#/*
# * Commands for Timer
# */
CMDS/0/0/350:	"DevSetDelay"
#/*
# * Commands for G64 ADC
# */
CMDS/0/0/360:	"DevMemRd"
CMDS/0/0/361:	"DevAdcEn"
#/*
# * Commands for Gescio1
# */
CMDS/0/0/370:	"DevRd16b"
CMDS/0/0/371:	"DevWr16b"
CMDS/0/0/372:	"DevWrMs16b"
CMDS/0/0/373:	"DevReRd16b"
CMDS/0/0/374:	"DevSet16b"
CMDS/0/0/375:	"DevRset16b"
CMDS/0/0/376:	"DevMod16b"
#/*
# * Commands for ImageUx/ImageOS9
# */
CMDS/0/0/390:	"DevDisplayBeam"
#/*
# * Commands for Booster Power Supplies System
# */
CMDS/0/0/377:	"DevWarmUp"
CMDS/0/0/378:	"DevMacro"
CMDS/0/0/379:	"DevLock"
CMDS/0/0/380:	"DevFree"
CMDS/0/0/381:	"DevStatusMacro"
CMDS/0/0/382:	"DevStopMacro"
CMDS/0/0/383:	"DevSetPhase"
CMDS/0/0/384:	"DevReadPhase"
CMDS/0/0/385:	"DevSetPeriod"
CMDS/0/0/386:	"DevReadPeriod"
CMDS/0/0/387:	"DevSetAc"
CMDS/0/0/388:	"DevReadAc"
CMDS/0/0/389:	"DevSetDc"
CMDS/0/0/391:	"DevReadDc"
CMDS/0/0/392:	"DevSetIacInv"
CMDS/0/0/393:	"DevReadIacInv"
CMDS/0/0/394:	"DevCouple"
CMDS/0/0/395:	"DevTrack"
CMDS/0/0/396:	"DevUnTrack"
CMDS/0/0/397:	"DevSearch"
CMDS/0/0/398:	"DevReadTemper"
CMDS/0/0/399:	"DevReadPhLagUi"
CMDS/0/0/400:	"DevSetDc"
#/*
# * Commands for Gespac Digital to Analogic 8 Channels Converter
# */
CMDS/0/0/420:	"DevSetRange"
CMDS/0/0/421:	"DevGetRange"
CMDS/0/0/422:	"DevSetChannel"
CMDS/0/0/423:	"DevGetChannel"
CMDS/0/0/424:	"DevSetIncrement"
CMDS/0/0/425:	"DevGetIncrement"
CMDS/0/0/426:	"DevIncrement"
CMDS/0/0/427:	"DevDecrement"
#/*
# * Commands for Diagnostics Current Transformer
# */
CMDS/0/0/440:	"DevGetTest"
CMDS/0/0/441:	"DevSetTest"
CMDS/0/0/442:	"DevGetCalib"
CMDS/0/0/443:	"DevSetCalib"
CMDS/0/0/444:	"DevReadVar"
CMDS/0/0/445:	"DevReadEnd"
CMDS/0/0/446:	"DevGetSens"
CMDS/0/0/447:	"DevSetSens"
CMDS/0/0/448:	"DevSetSensMan"
CMDS/0/0/449:	"DevReadSens"
CMDS/0/0/450:	"DevGetOffsetB"
CMDS/0/0/451:	"DevWriteAcqu"
CMDS/0/0/452:	"DevReadAcqu"
CMDS/0/0/453:	"DevGetAverage"
CMDS/0/0/454:	"DevGetAcquInfos"
CMDS/0/0/455:	"DevGetMpvErrors"
CMDS/0/0/456:	"DevReadLife"
CMDS/0/0/457:	"DevDtLifeTime"
CMDS/0/0/458:	"DevDiLifeTime"
CMDS/0/0/459:	"DevGetDeltaTime"
CMDS/0/0/460:	"DevSetDeltaTime"
CMDS/0/0/461:	"DevGetDeltaInt"
CMDS/0/0/462:	"DevSetDeltaInt"
CMDS/0/0/463:	"DevGetMeasureTime"
CMDS/0/0/464:	"DevSetMeasureTime"
#/*
# * DiagNostics SY and SR Tunning Monitor
# */
CMDS/0/0/480:	"DevGetBeam"
CMDS/0/0/481:	"DevSetBeam"
CMDS/0/0/482:	"DevGetStrength"
CMDS/0/0/483:	"DevSetStrength"
CMDS/0/0/484:	"DevGetMethod"
CMDS/0/0/485:	"DevSetMethod"
CMDS/0/0/486:	"DevGetPlane"
CMDS/0/0/487:	"DevSetPlane"
CMDS/0/0/488:	"DevGetDelays"
CMDS/0/0/489:	"DevSetDelays"
CMDS/0/0/490:	"DevSetConfig"
CMDS/0/0/491:	"DevSetTimers"
#/*
# * Rug Beam Monitoring System
# */
CMDS/0/0/510:	"DevGetMode"
CMDS/0/0/511:	"DevGetLength"
CMDS/0/0/512:	"DevGetCurrent"
CMDS/0/0/513:	"DevSetAutoDump"
#/*
# * Commands for the Vrif class
# */
CMDS/0/0/520:	"DevResetVme"
CMDS/0/0/521:	"DevSetAsIndic"
CMDS/0/0/522:	"DevEnWdog"
CMDS/0/0/523:	"DevDisWdog"
CMDS/0/0/524:	"DevReadVmeStat"
CMDS/0/0/525:	"DevReadPowerStat"
CMDS/0/0/526:	"DevReadCrateNum"
#/*
# *  Commands for HLS system
# */
CMDS/0/0/530:	"DevFullData"
#/*
# * Commands for Piezo
# */
CMDS/0/0/540:	"DevMovePosition"
#/*
# * Commands for seismic survey
# */
CMDS/0/0/550:	"DevGetEventList"
CMDS/0/0/551:	"DevGetEventList"
CMDS/0/0/552:	"DevReadLastEvent"
CMDS/0/0/553:	"DevReadSpectra"
#/*
# * Commands for LIEN automat
# */
CMDS/0/0/560:	"DevReadThreshold"
CMDS/0/0/561:	"DevSetThreshold"
CMDS/0/0/562:	"DevReadInput"
CMDS/0/0/563:	"DevReadOutput"
CMDS/0/0/564:	"DevReadCutTh"
CMDS/0/0/565:	"DevReadCmds"
CMDS/0/0/566:	"DevSetCmds"
CMDS/0/0/567:	"DevEraseCmds"
CMDS/0/0/568:	"DevReadCmdErr"
CMDS/0/0/569:	"DevReadNew"
CMDS/0/0/570:	"DevWriteNew"
CMDS/0/0/571:	"DevReadAll"
CMDS/0/0/572:	"DevReadAlarm"
#/*
# * Commands for the Update Daemon
# */
CMDS/0/0/580:	"DevGetDeviceIdent"
CMDS/0/0/581:	"DevInitialiseDevice"
CMDS/0/0/582:	"DevStartPolling"
CMDS/0/0/583:	"DevStopPolling"
CMDS/0/0/584:	"DevChangeInterval"
CMDS/0/0/585:	"DevDefineMode"
CMDS/0/0/586:	"DevPollStatus"
CMDS/0/0/587:	"DevAccessStatus"
CMDS/0/0/588:	"DevGetDeviceName"
#/*
# *  Commands for LINAC
# */
CMDS/0/0/600:	"DevWriteAna"
CMDS/0/0/601:	"DevWriteDig"
CMDS/0/0/602:	"DevDelFile"
CMDS/0/0/603:	"DevDirFiles"
CMDS/0/0/604:	"DevSelectFile"
CMDS/0/0/605:	"DevReadConfig"
CMDS/0/0/606:	"DevViewFile"
#/*
# *  Commands for FrontEnd Class
# */
CMDS/0/0/620:	"DevAxeType"
CMDS/0/0/621:	"DevCloseShutter"
CMDS/0/0/622:	"DevCloseAbsorber"
CMDS/0/0/623:	"DevStopBeam"
#/*
# *  Commands for Powermeter
# */
CMDS/0/0/640:	"DevReadPower"



#/*
# *      Commands for DAS group
# *      Device Servers for beamline control
# */
#
#/*
# * Commands for GPIB
# */
CMDS/0/0/1003:	"DevIntClear"
CMDS/0/0/1004:	"DevClear"
CMDS/0/0/1005:	"DevParPoll"
CMDS/0/0/1007:	"DevTrigger"
CMDS/0/0/1009:	"DevLocalLock"
CMDS/0/0/1010:	"DevSerPoll"
CMDS/0/0/1011:	"DevGetDevs"
CMDS/0/0/1012:	"DevSetDevs"
CMDS/0/0/1013:	"DevPassCtrl"
CMDS/0/0/1014:	"DevParPollConf"
CMDS/0/0/1015:	"DevParPollDis"
CMDS/0/0/1016:	"DevParPollUnc"
CMDS/0/0/1017:	"DevListen"
CMDS/0/0/1018:	"DevTalk"
CMDS/0/0/1019:	"DevUnListen"
CMDS/0/0/1020:	"DevUnTalk"
CMDS/0/0/1021:	"DevGetLocal"
CMDS/0/0/1022:	"DevSetLocal"
CMDS/0/0/1023:	"DevMulClear"
CMDS/0/0/1024:	"DevMulListen"
CMDS/0/0/1025:	"DevMulLocal"
CMDS/0/0/1026:	"DevMulPassCtrl"
CMDS/0/0/1027:	"DevMulParPollDis"
CMDS/0/0/1028:	"DevMulParPollUnc"
CMDS/0/0/1029:	"DevMulRemote"
CMDS/0/0/1030:	"DevMulWrite"
#/*
# * Commands for incremental encoders
# */
CMDS/0/0/1050:	"DevReadCount"
CMDS/0/0/1051:	"DevWriteCount"
CMDS/0/0/1052:	"DevGetDig"
CMDS/0/0/1053:	"DevGetInd"
#/* 
# * New commands for steppermotors 
# */
CMDS/0/0/1060:	"DevTrigClear"
CMDS/0/0/1061:	"DevWriteMux"
CMDS/0/0/1062:	"DevStartMemWrite"
CMDS/0/0/1063:	"DevStopMemWrite"
CMDS/0/0/1064:	"DevMoveAbsoluteDel"
CMDS/0/0/1065:	"DevMoveRelativeDel"
CMDS/0/0/1066:	"DevTrigMotion"
CMDS/0/0/1067:	"DevSetStepMode"
CMDS/0/0/1068:	"DevReadStepMode"
CMDS/0/0/1069:	"DevWriteWait"
CMDS/0/0/1070:	"DevReadMulPos"
CMDS/0/0/1071:	"DevReadMulEncPos"
CMDS/0/0/1072:	"DevReadFactor"
CMDS/0/0/1073:	"DevSetUnits"
CMDS/0/0/1074:	"DevReadEncPos"
CMDS/0/0/1075:	"DevLoadEncPos"
CMDS/0/0/1076:	"DevCloseBrake"
CMDS/0/0/1077:	"DevOpenBrake"
CMDS/0/0/1078:	"DevReadUnits"
CMDS/0/0/1079:	"DevReadBrakeState"
CMDS/0/0/1080:	"DevSetBacklash"
CMDS/0/0/1081:	"DevReadBacklash"
CMDS/0/0/1082:	"DevMulState"
CMDS/0/0/1083:	"DevReadMulAcc"
CMDS/0/0/1084:	"DevReadMulFSR"
CMDS/0/0/1085:	"DevReadMulVel"
CMDS/0/0/1086:	"DevReadMulHSR"
CMDS/0/0/1087:	"DevReadMulBrState"
CMDS/0/0/1088:	"DevReadMulSwitches"
CMDS/0/0/1089:	"DevReadMulUnits"
CMDS/0/0/1090:	"DevReadMulBacklash"
CMDS/0/0/1091:	"DevMoveEncAbsolute"
CMDS/0/0/1092:	"DevStartServo"
CMDS/0/0/1093:	"DevStopServo"
CMDS/0/0/1094:	"DevEnablePower"
CMDS/0/0/1095:	"DevDisablePower"
# 
#/***** Command(s) for LECROY 1151 counter	-FEP-  *****/
CMDS/0/0/1110:	"DevCntIdentify"
CMDS/0/0/1111:	"DevCntStatus"
CMDS/0/0/1112:	"DevCntReset"
CMDS/0/0/1113:	"DevCntAbort"
CMDS/0/0/1114:	"DevCntClearChannel"
CMDS/0/0/1115:	"DevCntPresetChannel"
CMDS/0/0/1116:	"DevCntReadChannel"
#
#/***** Command(s) for CAEN V462 gate generator	-FEP-  *****/
CMDS/0/0/1120:	"DevGategenIdentify"
CMDS/0/0/1121:	"DevGategenStatus"
CMDS/0/0/1122:	"DevGategenReset"
CMDS/0/0/1123:	"DevGategenStopChannel"
CMDS/0/0/1124:	"DevGategenStartChannel"
CMDS/0/0/1125:	"DevGategenPresetChannel"
#
#/* Commands for slits */
CMDS/0/0/1200:	"DevReadBladeState"
CMDS/0/0/1201:	"DevReadPslitState"
CMDS/0/0/1202:	"DevSetRelGap"
CMDS/0/0/1203:	"DevSetAbsGap"
CMDS/0/0/1204:	"DevSetRelOff"
CMDS/0/0/1205:	"DevSetAbsOff"
CMDS/0/0/1206:	"DevSwitchOn"
CMDS/0/0/1207:	"DevSwitchOff"
CMDS/0/0/1208:	"DevSetZero"
CMDS/0/0/1209:	"DevMakeReference"
CMDS/0/0/1210:	"DevSetTuned"
#
#/* Command(s) for attenuators */
CMDS/0/0/1230:	"DevGotoFilter"    
#
#/*** Hbm ***/
CMDS/0/0/1241:	"DevReadErr"
#
#/*** M4 ****/
CMDS/0/0/1250:	"DevReadMot"
CMDS/0/0/1251:	"DevReadSen"
CMDS/0/0/1252:	"DevReadAxe"
CMDS/0/0/1253:	"DevReadClutch"
CMDS/0/0/1254:	"DevApplyBrake"
CMDS/0/0/1255:	"DevReleaseBrake"
CMDS/0/0/1256:	"DevClutchTrig"
CMDS/0/0/1257:	"DevReport"
CMDS/0/0/1258:	"DevLoadMot"
CMDS/0/0/1259:	"DevLoadEnc"
#
#/*** Clutch ***/
CMDS/0/0/1280:	"DevClutchOn"
CMDS/0/0/1281:	"DevClutchOff"
CMDS/0/0/1282:	"DevClutchState"
#
#/*** New commands for IcvAdc *** TM ***/
CMDS/0/0/1300:	"DevTrigReadAll"
CMDS/0/0/1301:	"DevReadChanGain"
CMDS/0/0/1302:	"DevSetLimits"
CMDS/0/0/1303:	"DevReadLimits"
CMDS/0/0/1304:	"DevReadAlarmAll"
CMDS/0/0/1305:	"DevSetMinMax"
CMDS/0/0/1306:	"DevReadMinMax" 

#/*** New commands for BeamShutter ** JMV ***/

CMDS/0/0/1307:  "DevSetFastMode"
CMDS/0/0/1308:  "DevSetStandardMode"
CMDS/0/0/1309:  "DevSetFastDelay"
#
# commands for opening/closing frontend shutter/absorber
#
CMDS/0/0/6002:  "DevShutter"
CMDS/0/0/6003:  "DevAbsorber"
