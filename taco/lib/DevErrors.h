
/*+*******************************************************************

 File:		DevErrors.h

 Project:	Device Server Errors

 Description:	Public include file for device server error messages

 Author(s):	Andy Goetz
		Jens Meyer
		$Author§

 Original:	March 1990

 Version:	$Revision: 1.1 $

 Date:		$Date: 2003-03-14 12:22:07 $

 Copyright (c) 1990 by European Synchrotron Radiation Facility, 
                       Grenoble, France

********************************************************************-*/

#ifndef _DevErrors_h
#define _DevErrors_h

/*
 *  standart header string to use "what" or "ident".
 */
#ifdef _IDENT
static char DevErrorsh[] =
"@(#)$Header: /home/jkrueger1/sources/taco/backup/taco/lib/DevErrors.h,v 1.1 2003-03-14 12:22:07 jkrueger1 Exp $";
#endif /* _IDENT */

/*
 * The structure field for the error number was changed for the
 * LYNX port from errno to dev_errno. Due to compiler problems.
 */

#if !defined(FORTRAN)
typedef struct { short err_type;
                 long dev_errno;
               } DevErr;
#endif /* FORTRAN */
/*
 * device server error messages
 */

/*
 *  Number 0-9 RPC error message
 */

#define DevErr_CannotCreateClientHandle		1
#define DevErr_RPCTimedOut			2
#define DevErr_RPCFailed			3
#define DevErr_MsgImportFailed			4
#define DevErr_BadXdrDataTypeDefinition		5
#define DevErr_XdrTypeListOverflow		6
#define DevErr_XdrDataTypeNotLoaded		7
#define DevErr_DeviceNotImportedYet		8

/*
 *  Number 10-29 API-LIB client part
 */

#define DevErr_DbImportFailed			10
#define DevErr_ExceededMaximumNoOfServers	11
#define DevErr_BadServerConnection		12
#define DevErr_DevArgTypeNotRecognised		13
#define DevErr_InsufficientMemory		14
#define DevErr_NethostNotDefined		15
#define DevErr_NoNetworkManagerAvailable	16
#define DevErr_NetworkManagerNotResponding	17
#define DevErr_SignalOutOfRange 		18
#define DevErr_CannotSetSignalHandler 		19
#define DevErr_DevArgTypeNotCorrectImplemented	20
#define DevErr_UnknownInputParameter		21
#define DevErr_CannotConnectToHost		22
#define DevErr_CannotDecodeArguments    	23

/*
 *  Number 30-49 API-LIB server part
 */

#define DevErr_DISPLAYNotSet			30
#define DevErr_DeviceOfThisNameNotServed	31
#define DevErr_ExceededMaximumNoOfDevices	32
#define DevErr_IncompatibleCmdArgumentTypes	33
#define DevErr_CommandNotImplemented		34	
#define DevErr_ServerAlreadyExists		35
#define DevErr_ServerRegisteredOnDifferentHost	36
#define DevErr_XDRLengthCalculationFailed	37
#define DevErr_NameStringToLong         	38
#define DevErr_DeviceNoLongerExported   	39
#define DevErr_ExceededMaximumNoOfClientConn   	40
#define DevErr_AsynchronousServerNotImported   	41
#define DevErr_AsynchronousCallsNotSupported    42
#define DevErr_NoCallbackSpecified 		43
#define DevErr_ExceededMaxNoOfPendingCalls	44

/*
 *  Number 50-199 device server error messages
 */

/*
 * general error definitions
 */
#define DevErr_DeviceSyntaxError		50
#define DevErr_DeviceNoHardware			51
#define DevErr_DeviceHardwareError		52
#define DevErr_DeviceTypeNotRecognised		53	
#define DevErr_DeviceIllegalParameter		54
#define DevErr_ValueOutOfBounds			55
#define DevErr_DeviceTimedOut			56
#define DevErr_SemaphoreTimedOut		57
#define DevErr_AttemptToViolateStateMachine	58
#define DevErr_UnrecognisedState		59
#define DevErr_CommandIgnored			60
#define DevErr_CommandFailed			61
#define DevErr_FieldBusError			62
#define DevErr_CantCreateCommPort		63
#define DevErr_CannotForkProcess		64
#define DevErr_BadChecksum			65
#define DevErr_DeviceInFaultState               66
#define DevErr_Reset                            67
#define DevErr_ConnectionError                  68
#define DevErr_BusError                         69

#define DevErr_DeviceNoInputAvailable		71
#define DevErr_DeviceNotInRemote		72
#define DevErr_DeviceOpen			73
#define DevErr_DeviceWrite			74
#define DevErr_DeviceRead			75

#define DevErr_DevicePolarityWrong		81
#define DevErr_DeviceErrorToCheck		82
#define DevErr_NoInputVoltage			83
#define DevErr_WrongDataType			84
#define DevErr_RWAccessToDeviceFailed		85
#define DevErr_OpenAccessToDeviceFailed		86

/*
 * errors for JCS 			
 */
#define DevErr_JackUpperLimit			 98
#define DevErr_JackLowerLimit			 99
#define DevErr_JackLocked			100
#define DevErr_JackMotorCard			101
#define DevErr_JackMotorPower			102
#define DevErr_JackCPU				103
#define DevErr_JackBufferFull			104
#define DevErr_GirderUpperRight			105
#define DevErr_GirderLowerRight			106
#define DevErr_GirderUpperLeft			107
#define DevErr_GirderLowerLeft			108

/*
 * errors for Ribber Ion Pump Controller
 */
#define DevErr_NoHighVoltage			110
#define DevErr_ShortCircuit			111
#define DevErr_HTPlugDisconnected		112
#define DevErr_ThermalCutout			113
#define DevErr_DUNotConnected			114
#define DevErr_HTNotConnected			115
#define DevErr_BadPumpVolume			116
#define DevErr_BadVacuum			117
#define DevErr_HTInUseByAnotherChannel		118
#define DevErr_HTOn				119
#define DevErr_DUOn				120
#define DevErr_VoltageFalling			121
#define DevErr_FailedToAchievePowerThreshold	122
/*  
 *  errors for power supplies
 */
#define DevErr_DeviceNotReady			140
#define DevErr_AccessToDMFailed			141
#define DevErr_UnknownResponse			142
#define DevErr_NotMaster			143
/*
 *  errors for GADC
 */
#define DevErr_DeviceBusy                       150
#define DevErr_DataStaleOrOverflow              151
#define DevErr_NoData                           152
#define DevErr_ErroneousDataDiscarded           153
#define DevErr_FBusFailed                       155
#define DevErr_FBusOverFlow                     156
#define DevErr_LinkToSignalFailed               157
#define DevErr_InternalServerErrorDetected      158
#define DevErr_DeviceNotActive		    	159
/*
 *  errors for pneum. Valves server
 */
#define DevErr_CommandNotAvailableForManValve   170
#define DevErr_ValveInFaultState                171
#define DevErr_PenningGaugeInterlockActiv       172
#define DevErr_PenningGaugeNotOperating         173
#define DevErr_PlcPowerSupplyFault              174
#define DevErr_PlcFault                         175
#define DevErr_ManValveStillClosed              176
#define DevErr_UswitchIndicationWrong           177
#define DevErr_ErrorModeNotImplemented          178
#define DevErr_CommandIgnoredByValve            179
#define DevErr_BitNotContainedInWord            180
#define DevErr_ManValveIsMoving                 181
#define DevErr_ValveInBlockedState              182
#define DevErr_AirPressureFailure		183
#define DevErr_ValveInMovingState               184
#define DevErr_PlcProgramStopped               	185
/*
 *  errors for X-dev_menu
 */
#define DevErr_OutParameterTypeUnknown		190
#define DevErr_InParameterTypeUnknown		191

/*
 *  errors for VGC
 */
#define DevErr_PressureTooHigh          	500
#define DevErr_AssociatedPiraniOff      	501
#define DevErr_Configuration            	502
#define DevErr_GaugeExternallyInhibited 	503
#define DevErr_GaugeSpecificError       	504
#define DevErr_VgcInLocalMode           	505
#define DevErr_VgcInFaultState          	506
#define DevErr_SerialLineInitFailed     	507
#define DevErr_BatteryLow               	508
#define DevErr_InternalSettingsLost     	509
#define DevErr_CommandNotReferenced     	510
#define DevErr_OpenCircuit              	511
#define DevErr_LowPressure              	512
#define DevErr_ParameterOutOfRange      	513
#define DevErr_CommandNotAccepted       	514
#define DevErr_HVSupplyLow              	515
#define DevErr_PiraniInterlock          	516
#define DevErr_MaxPressureExeeded       	517
/*
 *  errors for NEG pumps
 */
#define DevErr_CannotOpenDacAccess             	530
#define DevErr_VacuumInterlock                  531
#define DevErr_NegPumpInterlock                 532
#define DevErr_DeviceStillStarting	        533
/*
 * errors for LINAC
 */
#define DevErr_Interlock			550


/*
 *  Number 200-220 network manager error messages
 */

#define DevErr_DSHOMENotSet			200

/*
 *  Number 250-299 security system error messages
 */

#define DevErr_UserIdIsNotCorrect       	250
#define DevErr_NetworkAccessDenied      	251
#define DevErr_GroupIdIsNotCorrect      	252
#define DevErr_AccessDenied             	253
#define DevErr_UndefAccessRightInRes    	254
#define DevErr_UndefAccessRight         	255
#define DevErr_NoValidPasswdEntry       	256
#define DevErr_NoValidGroupEntry        	257
#define DevErr_NoValidHostEntry         	258

#define DevErr_DeviceIsLockedInAdminMode        259
#define DevErr_DeviceIsLockedInSiMode           260
#define DevErr_SecurityKeyNotValid              261
#define DevErr_CmdAccessDenied                  262
#define DevErr_SIAccessWasCanceled              263
#define DevErr_AdminAccessWasCanceled		264
#define DevErr_CannotStoreSecKey		265
#define DevErr_CantChangeProtWithOpenSIAccess   266
#define DevErr_NoSingleUserModeOnVersion3       267

/*
 *  Number 300-320 message server error messages
 */

#define DevErr_CannotOpenErrorFile              300
#define DevErr_CannotOpenPipe			301
#define DevErr_CannotWriteToPipe		302

/*
 *  Number 400-500 database server error messages
 */

#define DbErr_NoDatabase			400
#define DbErr_CannotCreateClientHandle		401
#define DbErr_RPCreception                      402
#define DbErr_DatabaseAccess                    403
#define DbErr_BooleanResource                   404
#define DbErr_MemoryFree                        405
#define DbErr_ClientMemoryAllocation           	406
#define DbErr_ServerMemoryAllocation            407
#define DbErr_DomainDefinition                  408
#define DbErr_ResourceNotDefined                409
#define DbErr_DeviceServerNotDefined            410
#define DbErr_MaxNumberOfDevice         	411
#define DbErr_DeviceNotDefined                  412
#define DbErr_HostName                          413
#define DbErr_DeviceNotExported         	414
#define DbErr_FamilyDefinition			415
#define DbErr_TimeCriterion			416
#define DbErr_BooleanDefinition 		417
#define DbErr_BadValueCriterion			418
#define DbErr_BadBooleanCriterion		419
#define DbErr_BadStringCriterion		420
#define DbErr_MemberDefinition			421
#define DbErr_DataDefinition			422
#define DbErr_BadStructureDefinition		423
#define DbErr_BadParameters			424
#define DbErr_OS9_FileAccess			425
#define DbErr_NethostNotDefined			426
#define DbErr_NoManagerAvailable		427
#define DbErr_ManagerNotResponding		428
#define DbErr_ManagerReturnError		429
#define DbErr_DbServerNotExported		430
#define DbErr_MaxDeviceForUDP			431
#define DbErr_OtherProcessOnDb			432
#define DbErr_TooManyInfoForUDP			433
#define DbErr_BadResourceType			434
#define DbErr_StringTooLong			435
#define DbErr_DatabaseNotConnected		436
#define DbErr_NameAlreadyUsed			437
#define DbErr_CannotEncodeArguments		438
#define DbErr_CannotDecodeResult		439
#define	DbErr_CantGetContent			440
#define	DbErr_IndTooLarge			441
#define	DbErr_BadContSyntax			442
#define	DbErr_BadDevSyntax			443
#define	DbErr_CantBuildKey			444
#define	DbErr_BadKeySyntax			445
#define DbErr_BadResSyntax			446
#define DbErr_CantOpenResFile			447
#define DbErr_DoubleTupleInNames		448
#define DbErr_DeviceAlreadyRegistered		449
#define DbErr_DoubleTupleInRes			449
#define DbErr_NoPassword			450
#define DbErr_NoPollerFound			451
#define DbErr_BadServerSyntax			452
#define DbErr_CantInitMapCache			453


/*
 *  Number 1000-1100 data collector server error messages
 */


#define DcErr_CannotCreateClientHandle		1001
#define DcErr_RPCreception                      1002
#define DcErr_BadParameters			1003
#define DcErr_ClientMemoryAllocation           	1004
#define DcErr_CantBuildStaDbConnection		1005
#define DcErr_CantGetDcHostInfo			1006
#define	DcErr_CantGetDcResources		1007
#define DcErr_CantGetDcServerNetInfo		1008
#define	DcErr_TooManyCmdForDevice		1009
#define	DcErr_NoCmdForDevice			1010
#define	DcErr_DatabaseError			1011
#define DcErr_DeviceNotDefined			1012
#define DcErr_BadCmdNumber			1013
#define DcErr_CmdNotDefinedForDevice		1014
#define DcErr_DatBufAllocError			1015
#define DcErr_CantFreeDataBuffer		1016
#define DcErr_DataNotUpdated			1017
#define DcErr_IncompatibleCmdArgumentTypes	1018
#define DcErr_DataNotYetAvailable		1019
#define DcErr_CommandNotUsedForPoll		1020
#define DcErr_ServerMemoryAllocation		1021
#define DcErr_MaxNumberOfDevices		1022
#define DcErr_CmdNotInDataBuffer		1023
#define DcErr_NoDefaultDcForThisHost		1024
#define DcErr_CantGetClientHostInfo		1025
#define DcErr_DeviceNotDcImported		1026
#define DcErr_RPCTimedOut			1027
#define	DcErr_CantConvertDataToXDRFormat	1028
#define DcErr_PtrsTimerError			1029
#define DcErr_PtrsTimedOut			1030
#define DcErr_CellarTableFull			1031
#define DcErr_DeviceNotInPtrsMemory		1032
#define DcErr_DeviceAlreadyDefinedInPtrs	1033
#define DcErr_BadPtrsMemoryIndice		1034
#define DcErr_BadCommandForThisRec		1035
#define DcErr_CantContactServer			1036
#define DcErr_CantTestPSDevice			1037
#define DcErr_CantRegisterPSDevice		1038
#define DcErr_CantUnregisterPSDevice		1039

#define	DcErr_CantLoadSiglibSharedLibrary	1050
#define	DcErr_NoPlaceInClassArray		1051
#define DcErr_NoPlaceInDevArray			1052
#define	DcErr_CantResolveSymbol			1053
#define DcErr_SignalNotDefined			1054
#define DcErr_CommandArrayFull			1055
#define DcErr_SignalNotImported			1056
#define DcErr_CantFindDevInSortedArray		1057
#define DcErr_CantFindCmdInSortedArray		1058
#define DcErr_ErrDuringExtractFunction		1059
#define DcErr_CantRetrieveSignalClass		1060
#define DcErr_TooManyNethost			1061
#define DcErr_NethostNotDefined			1062
#define DcErr_SignalInitFuncFailed		1063
#define	DcErr_CantResolveInitFuncSymbol		1064
#define DcErr_CantResolveSigNbSymbol		1065
#define DcErr_CantResolveLoadTypeSymbol		1066


/* Numbers upto 1199 reserved for data acquisition */


/* Errors 1200-1210 for attenuators */
#define DevErr_FluorescentScreenPlaced          1200

/* Errors for Hbm system */
#define DevErr_Hbm			1211
#define DevErr_HbmBadData		1212

/* Errors for M4, four mirrors device */
#define DevErr_NoDevice			1220
#define DevErr_WrongData		1221
#define DevErr_ClutchOffNotAllowed	1222
#define DevErr_AnError			1223

/*
 **********************************
 *   List of device error strings *
 **********************************
 */


/*
 * device server error message strings
 *
 * The structure field for the error number was changed for the
 * LYNX port from errno to dev_errno. Due to compiler problems.
 */

#if !defined(FORTRAN)
typedef struct _DevErrListEntry {
				int  dev_errno;
				const char *message;
				} DevErrListEntry;

extern DevErrListEntry DevErr_List[];

extern long max_deverr;

#define MAX_DEVERR max_deverr
#endif /* FORTRAN */

#endif /* _DevErrors_h */
