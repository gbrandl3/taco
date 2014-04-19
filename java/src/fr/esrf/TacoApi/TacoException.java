/*+*******************************************************************

 File       : TacoException.java

 Project    : Device Servers with sun-rpc in Java

 Description:  Class for Taco Exception and error code handling.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005-2014 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

/**
 * Taco Exception management.
 */
public class TacoException extends Exception {

/*
 * device server error messages
 */

  public final static int DevErr_CannotCreateClientHandle = 1;
  public final static int DevErr_RPCTimedOut = 2;
  public final static int DevErr_RPCFailed = 3;
  public final static int DevErr_MsgImportFailed = 4;
  public final static int DevErr_BadXdrDataTypeDefinition = 5;
  public final static int DevErr_XdrTypeListOverflow = 6;
  public final static int DevErr_XdrDataTypeNotLoaded = 7;
  public final static int DevErr_DeviceNotImportedYet = 8;

/*
 *  Number 10-29 API-LIB client part
 */

  public final static int DevErr_DbImportFailed = 10;
  public final static int DevErr_ExceededMaximumNoOfServers = 11;
  public final static int DevErr_BadServerConnection = 12;
  public final static int DevErr_DevArgTypeNotRecognised = 13;
  public final static int DevErr_InsufficientMemory = 14;
  public final static int DevErr_NethostNotDefined = 15;
  public final static int DevErr_NoNetworkManagerAvailable = 16;
  public final static int DevErr_NetworkManagerNotResponding = 17;
  public final static int DevErr_SignalOutOfRange = 18;
  public final static int DevErr_CannotSetSignalHandler = 19;
  public final static int DevErr_DevArgTypeNotCorrectImplemented = 20;
  public final static int DevErr_UnknownInputParameter = 21;
  public final static int DevErr_CannotConnectToHost = 22;
  public final static int DevErr_CannotDecodeArguments = 23;

/*
 *  Number 30-49 API-LIB server part
 */

  public final static int DevErr_DISPLAYNotSet = 30;
  public final static int DevErr_DeviceOfThisNameNotServed = 31;
  public final static int DevErr_ExceededMaximumNoOfDevices = 32;
  public final static int DevErr_IncompatibleCmdArgumentTypes = 33;
  public final static int DevErr_CommandNotImplemented = 34;
  public final static int DevErr_ServerAlreadyExists = 35;
  public final static int DevErr_ServerRegisteredOnDifferentHost = 36;
  public final static int DevErr_XDRLengthCalculationFailed = 37;
  public final static int DevErr_NameStringToLong = 38;
  public final static int DevErr_DeviceNoLongerExported = 39;
  public final static int DevErr_ExceededMaximumNoOfClientConn = 40;
  public final static int DevErr_AsynchronousServerNotImported = 41;
  public final static int DevErr_AsynchronousCallsNotSupported = 42;
  public final static int DevErr_NoCallbackSpecified = 43;
  public final static int DevErr_ExceededMaxNoOfPendingCalls = 44;

/*
 * general error definitions
 */
  public final static int DevErr_DeviceSyntaxError = 50;
  public final static int DevErr_DeviceNoHardware = 51;
  public final static int DevErr_DeviceHardwareError = 52;
  public final static int DevErr_DeviceTypeNotRecognised = 53;
  public final static int DevErr_DeviceIllegalParameter = 54;
  public final static int DevErr_ValueOutOfBounds = 55;
  public final static int DevErr_DeviceTimedOut = 56;
  public final static int DevErr_SemaphoreTimedOut = 57;
  public final static int DevErr_AttemptToViolateStateMachine = 58;
  public final static int DevErr_UnrecognisedState = 59;
  public final static int DevErr_CommandIgnored = 60;
  public final static int DevErr_CommandFailed = 61;
  public final static int DevErr_FieldBusError = 62;
  public final static int DevErr_CantCreateCommPort = 63;
  public final static int DevErr_CannotForkProcess = 64;
  public final static int DevErr_BadChecksum = 65;
  public final static int DevErr_DeviceInFaultState = 66;
  public final static int DevErr_Reset = 67;
  public final static int DevErr_ConnectionError = 68;
  public final static int DevErr_BusError = 69;

  public final static int DevErr_DeviceNoInputAvailable = 71;
  public final static int DevErr_DeviceNotInRemote = 72;
  public final static int DevErr_DeviceOpen = 73;
  public final static int DevErr_DeviceWrite = 74;
  public final static int DevErr_DeviceRead = 75;

  public final static int DevErr_DevicePolarityWrong = 81;
  public final static int DevErr_DeviceErrorToCheck = 82;
  public final static int DevErr_NoInputVoltage = 83;
  public final static int DevErr_WrongDataType = 84;
  public final static int DevErr_RWAccessToDeviceFailed = 85;
  public final static int DevErr_OpenAccessToDeviceFailed = 86;

/*
 * errors for JCS
 */
  public final static int DevErr_JackUpperLimit = 98;
  public final static int DevErr_JackLowerLimit = 99;
  public final static int DevErr_JackLocked = 100;
  public final static int DevErr_JackMotorCard = 101;
  public final static int DevErr_JackMotorPower = 102;
  public final static int DevErr_JackCPU = 103;
  public final static int DevErr_JackBufferFull = 104;
  public final static int DevErr_GirderUpperRight = 105;
  public final static int DevErr_GirderLowerRight = 106;
  public final static int DevErr_GirderUpperLeft = 107;
  public final static int DevErr_GirderLowerLeft = 108;

/*
 * errors for Ribber Ion Pump Controller
 */
  public final static int DevErr_NoHighVoltage = 110;
  public final static int DevErr_ShortCircuit = 111;
  public final static int DevErr_HTPlugDisconnected = 112;
  public final static int DevErr_ThermalCutout = 113;
  public final static int DevErr_DUNotConnected = 114;
  public final static int DevErr_HTNotConnected = 115;
  public final static int DevErr_BadPumpVolume = 116;
  public final static int DevErr_BadVacuum = 117;
  public final static int DevErr_HTInUseByAnotherChannel = 118;
  public final static int DevErr_HTOn = 119;
  public final static int DevErr_DUOn = 120;
  public final static int DevErr_VoltageFalling = 121;
  public final static int DevErr_FailedToAchievePowerThreshold = 122;

/*
 *  errors for power supplies
 */
  public final static int DevErr_DeviceNotReady = 140;
  public final static int DevErr_AccessToDMFailed = 141;
  public final static int DevErr_UnknownResponse = 142;
  public final static int DevErr_NotMaster = 143;

/*
 *  errors for GADC
 */
  public final static int DevErr_DeviceBusy = 150;
  public final static int DevErr_DataStaleOrOverflow = 151;
  public final static int DevErr_NoData = 152;
  public final static int DevErr_ErroneousDataDiscarded = 153;
  public final static int DevErr_FBusFailed = 155;
  public final static int DevErr_FBusOverFlow = 156;
  public final static int DevErr_LinkToSignalFailed = 157;
  public final static int DevErr_InternalServerErrorDetected = 158;
  public final static int DevErr_DeviceNotActive = 159;

/*
 *  errors for pneum. Valves server
 */
  public final static int DevErr_CommandNotAvailableForManValve = 170;
  public final static int DevErr_ValveInFaultState = 171;
  public final static int DevErr_PenningGaugeInterlockActiv = 172;
  public final static int DevErr_PenningGaugeNotOperating = 173;
  public final static int DevErr_PlcPowerSupplyFault = 174;
  public final static int DevErr_PlcFault = 175;
  public final static int DevErr_ManValveStillClosed = 176;
  public final static int DevErr_UswitchIndicationWrong = 177;
  public final static int DevErr_ErrorModeNotImplemented = 178;
  public final static int DevErr_CommandIgnoredByValve = 179;
  public final static int DevErr_BitNotContainedInWord = 180;
  public final static int DevErr_ManValveIsMoving = 181;
  public final static int DevErr_ValveInBlockedState = 182;
  public final static int DevErr_AirPressureFailure = 183;
  public final static int DevErr_ValveInMovingState = 184;
  public final static int DevErr_PlcProgramStopped = 185;

/*
 *  errors for X-dev_menu
 */
  public final static int DevErr_OutParameterTypeUnknown = 190;
  public final static int DevErr_InParameterTypeUnknown = 191;


/*
 *  errors for VGC
 */
  public final static int DevErr_PressureTooHigh = 500;
  public final static int DevErr_AssociatedPiraniOff = 501;
  public final static int DevErr_Configuration = 502;
  public final static int DevErr_GaugeExternallyInhibited = 503;
  public final static int DevErr_GaugeSpecificError = 504;
  public final static int DevErr_VgcInLocalMode = 505;
  public final static int DevErr_VgcInFaultState = 506;
  public final static int DevErr_SerialLineInitFailed = 507;
  public final static int DevErr_BatteryLow = 508;
  public final static int DevErr_InternalSettingsLost = 509;
  public final static int DevErr_CommandNotReferenced = 510;
  public final static int DevErr_OpenCircuit = 511;
  public final static int DevErr_LowPressure = 512;
  public final static int DevErr_ParameterOutOfRange = 513;
  public final static int DevErr_CommandNotAccepted = 514;
  public final static int DevErr_HVSupplyLow = 515;
  public final static int DevErr_PiraniInterlock = 516;
  public final static int DevErr_MaxPressureExeeded = 517;

/*
 *  errors for NEG pumps
 */
  public final static int DevErr_CannotOpenDacAccess = 530;
  public final static int DevErr_VacuumInterlock = 531;
  public final static int DevErr_NegPumpInterlock = 532;
  public final static int DevErr_DeviceStillStarting = 533;

/*
 * errors for LINAC
 */
  public final static int DevErr_Interlock = 550;


/*
 *  Number 200-220 network manager error messages
 */
  public final static int DevErr_DSHOMENotSet = 200;

/*
 *  Number 250-299 security system error messages
 */
  public final static int DevErr_UserIdIsNotCorrect = 250;
  public final static int DevErr_NetworkAccessDenied = 251;
  public final static int DevErr_GroupIdIsNotCorrect = 252;
  public final static int DevErr_AccessDenied = 253;
  public final static int DevErr_UndefAccessRightInRes = 254;
  public final static int DevErr_UndefAccessRight = 255;
  public final static int DevErr_NoValidPasswdEntry = 256;
  public final static int DevErr_NoValidGroupEntry = 257;
  public final static int DevErr_NoValidHostEntry = 258;

  public final static int DevErr_DeviceIsLockedInAdminMode = 259;
  public final static int DevErr_DeviceIsLockedInSiMode = 260;
  public final static int DevErr_SecurityKeyNotValid = 261;
  public final static int DevErr_CmdAccessDenied = 262;
  public final static int DevErr_SIAccessWasCanceled = 263;
  public final static int DevErr_AdminAccessWasCanceled = 264;
  public final static int DevErr_CannotStoreSecKey = 265;
  public final static int DevErr_CantChangeProtWithOpenSIAccess = 266;
  public final static int DevErr_NoSingleUserModeOnVersion3 = 267;

/*
 *  Number 300-320 message server error messages
 */
  public final static int DevErr_CannotOpenErrorFile = 300;
  public final static int DevErr_CannotOpenPipe = 301;
  public final static int DevErr_CannotWriteToPipe = 302;

/*
 *  Number 400-500 database server error messages
 */
  public final static int DbErr_NoDatabase = 400;
  public final static int DbErr_CannotCreateClientHandle = 401;
  public final static int DbErr_RPCreception = 402;
  public final static int DbErr_DatabaseAccess = 403;
  public final static int DbErr_BooleanResource = 404;
  public final static int DbErr_MemoryFree = 405;
  public final static int DbErr_ClientMemoryAllocation = 406;
  public final static int DbErr_ServerMemoryAllocation = 407;
  public final static int DbErr_DomainDefinition = 408;
  public final static int DbErr_ResourceNotDefined = 409;
  public final static int DbErr_DeviceServerNotDefined = 410;
  public final static int DbErr_MaxNumberOfDevice = 411;
  public final static int DbErr_DeviceNotDefined = 412;
  public final static int DbErr_HostName = 413;
  public final static int DbErr_DeviceNotExported = 414;
  public final static int DbErr_FamilyDefinition = 415;
  public final static int DbErr_TimeCriterion = 416;
  public final static int DbErr_BooleanDefinition = 417;
  public final static int DbErr_BadValueCriterion = 418;
  public final static int DbErr_BadBooleanCriterion = 419;
  public final static int DbErr_BadStringCriterion = 420;
  public final static int DbErr_MemberDefinition = 421;
  public final static int DbErr_DataDefinition = 422;
  public final static int DbErr_BadStructureDefinition = 423;
  public final static int DbErr_BadParameters = 424;
  public final static int DbErr_OS9_FileAccess = 425;
  public final static int DbErr_NethostNotDefined = 426;
  public final static int DbErr_NoManagerAvailable = 427;
  public final static int DbErr_ManagerNotResponding = 428;
  public final static int DbErr_ManagerReturnError = 429;
  public final static int DbErr_DbServerNotExported = 430;
  public final static int DbErr_MaxDeviceForUDP = 431;
  public final static int DbErr_OtherProcessOnDb = 432;
  public final static int DbErr_TooManyInfoForUDP = 433;
  public final static int DbErr_BadResourceType = 434;
  public final static int DbErr_StringTooLong = 435;
  public final static int DbErr_DatabaseNotConnected = 436;
  public final static int DbErr_NameAlreadyUsed = 437;
  public final static int DbErr_CannotEncodeArguments = 438;
  public final static int DbErr_CannotDecodeResult = 439;
  public final static int DbErr_CantGetContent = 440;
  public final static int DbErr_IndTooLarge = 441;
  public final static int DbErr_BadContSyntax = 442;
  public final static int DbErr_BadDevSyntax = 443;
  public final static int DbErr_CantBuildKey = 444;
  public final static int DbErr_BadKeySyntax = 445;
  public final static int DbErr_BadResSyntax = 446;
  public final static int DbErr_CantOpenResFile = 447;
  public final static int DbErr_DoubleTupleInNames = 448;
  public final static int DbErr_DeviceAlreadyRegistered = 449;
  public final static int DbErr_DoubleTupleInRes = 449;
  public final static int DbErr_NoPassword = 450;
  public final static int DbErr_NoPollerFound = 451;
  public final static int DbErr_BadServerSyntax = 452;
  public final static int DbErr_CantInitMapCache = 453;


/*
 *  Number 1000-1100 data collector server error messages
 */
  public final static int DcErr_CannotCreateClientHandle = 1001;
  public final static int DcErr_RPCreception = 1002;
  public final static int DcErr_BadParameters = 1003;
  public final static int DcErr_ClientMemoryAllocation = 1004;
  public final static int DcErr_CantBuildStaDbConnection = 1005;
  public final static int DcErr_CantGetDcHostInfo = 1006;
  public final static int DcErr_CantGetDcResources = 1007;
  public final static int DcErr_CantGetDcServerNetInfo = 1008;
  public final static int DcErr_TooManyCmdForDevice = 1009;
  public final static int DcErr_NoCmdForDevice = 1010;
  public final static int DcErr_DatabaseError = 1011;
  public final static int DcErr_DeviceNotDefined = 1012;
  public final static int DcErr_BadCmdNumber = 1013;
  public final static int DcErr_CmdNotDefinedForDevice = 1014;
  public final static int DcErr_DatBufAllocError = 1015;
  public final static int DcErr_CantFreeDataBuffer = 1016;
  public final static int DcErr_DataNotUpdated = 1017;
  public final static int DcErr_IncompatibleCmdArgumentTypes = 1018;
  public final static int DcErr_DataNotYetAvailable = 1019;
  public final static int DcErr_CommandNotUsedForPoll = 1020;
  public final static int DcErr_ServerMemoryAllocation = 1021;
  public final static int DcErr_MaxNumberOfDevices = 1022;
  public final static int DcErr_CmdNotInDataBuffer = 1023;
  public final static int DcErr_NoDefaultDcForThisHost = 1024;
  public final static int DcErr_CantGetClientHostInfo = 1025;
  public final static int DcErr_DeviceNotDcImported = 1026;
  public final static int DcErr_RPCTimedOut = 1027;
  public final static int DcErr_CantConvertDataToXDRFormat = 1028;
  public final static int DcErr_PtrsTimerError = 1029;
  public final static int DcErr_PtrsTimedOut = 1030;
  public final static int DcErr_CellarTableFull = 1031;
  public final static int DcErr_DeviceNotInPtrsMemory = 1032;
  public final static int DcErr_DeviceAlreadyDefinedInPtrs = 1033;
  public final static int DcErr_BadPtrsMemoryIndice = 1034;
  public final static int DcErr_BadCommandForThisRec = 1035;
  public final static int DcErr_CantContactServer = 1036;
  public final static int DcErr_CantTestPSDevice = 1037;
  public final static int DcErr_CantRegisterPSDevice = 1038;
  public final static int DcErr_CantUnregisterPSDevice = 1039;

  public final static int DcErr_CantLoadSiglibSharedLibrary = 1050;
  public final static int DcErr_NoPlaceInClassArray = 1051;
  public final static int DcErr_NoPlaceInDevArray = 1052;
  public final static int DcErr_CantResolveSymbol = 1053;
  public final static int DcErr_SignalNotDefined = 1054;
  public final static int DcErr_CommandArrayFull = 1055;
  public final static int DcErr_SignalNotImported = 1056;
  public final static int DcErr_CantFindDevInSortedArray = 1057;
  public final static int DcErr_CantFindCmdInSortedArray = 1058;
  public final static int DcErr_ErrDuringExtractFunction = 1059;
  public final static int DcErr_CantRetrieveSignalClass = 1060;
  public final static int DcErr_TooManyNethost = 1061;
  public final static int DcErr_NethostNotDefined = 1062;
  public final static int DcErr_SignalInitFuncFailed = 1063;
  public final static int DcErr_CantResolveInitFuncSymbol = 1064;
  public final static int DcErr_CantResolveSigNbSymbol = 1065;
  public final static int DcErr_CantResolveLoadTypeSymbol = 1066;


/*
 * Errors for TACO/TANGO attribute handling
 */
  public final static int DevErr_AttributeNotFound = 1100;
  public final static int DevErr_TangoAccessFailed = 1101;
  public final static int DevErr_CannotConvertAttributeDataType = 1102;

/*
 * Errors 1200-1210 for attenuators
 */
  public final static int DevErr_FluorescentScreenPlaced = 1200;

/*
 * Errors for Hbm system
 */
  public final static int DevErr_Hbm = 1211;
  public final static int DevErr_HbmBadData = 1212;

/*
 * Errors for M4, four mirrors device
 */
  public final static int DevErr_NoDevice = 1220;
  public final static int DevErr_WrongData = 1221;
  public final static int DevErr_ClutchOffNotAllowed = 1222;
  public final static int DevErr_AnError = 1223;

  /**
   * Construct a Taco exception containing the message associated to the given error code.
   * @param code
   */
  public TacoException(int code) {
    // Set e default message
    super("Taco error code: " + code);
    this.code = code;
  }

  /**
   * Construct a Taco exception containing the given message.
   * @param message Exception message
   */
  public TacoException(String message) {
    super(message);
    this.msgStr = message;
  }

  /**
   * Construct a Taco exception containing the given message +
   * the message associated to the given error code.
   * @param message Exception message
   */
  public TacoException(String message,int code) {
    super(message);
    this.msgStr = message;
    this.code   = code;
  }

  /**
   * Get the full error message of this exception.
   */
  public String getErrorString() {

    if(msgStr==null) {

      // Only code
      errStr = getErrorString(code);
      return errStr;

    } else {

      if(code!=Integer.MAX_VALUE) {
        // Code + message
        errStr = getErrorString(code);
        return errStr + "\n" + msgStr;
      } else {
        // Message only
        return msgStr;
      }

    }

  }

  /**
   * Return the error code associated with this Exception.
   * @return Integer.MAX_VALUE when not specified, error code otherwise.
   */
  public int getCode() {
    return code;
  }

  // Contact the default database (default NETHOST) and try to get the error message.
  private static String getErrorString(int code) {

    try {
      return NethostConnection.getDefaultNethost().getErrorString(code);
    } catch (TacoException e) {
      return "Failed to get error message from database for code " + code +"\n" + e.getMessage();
    }

  }

  private int    code    = Integer.MAX_VALUE;
  private String msgStr  = null;
  private String errStr  = "Unknown error";

}
