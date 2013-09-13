/*+*******************************************************************

 File       : TacoConst.java

 Project    : Device Servers with sun-rpc in Java

 Description:  An interface containing various Taco constants.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005-2013 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

/**
 * An interface containing various Taco onstants.
 * @see fr.esrf.TacoApi.xdr.XdrTacoType
 */
public interface TacoConst {

  /*
   * Taco Commands code (Only the most used are specified)
   */

  public final static int DevOn = 1;
  public final static int DevOff = 2;
  public final static int DevReset = 3;
  public final static int DevState = 4;
  public final static int DevStandby = 5;
  public final static int DevRun = 6;
  public final static int DevLocal = 7;
  public final static int DevStatus = 8;
  public final static int DevOpen = 9;
  public final static int DevClose = 10;
  public final static int DevSetValue = 11;
  public final static int DevReadValue = 12;
  public final static int DevInsert = 13;
  public final static int DevExtract = 14;
  public final static int DevReadPos = 15;
  public final static int DevWritePos = 16;
  public final static int DevSetParam = 17;
  public final static int DevReadParam = 18;
  public final static int DevUnlock = 19;
  public final static int DevSetConversion = 20;
  public final static int DevSelectCh = 21;
  public final static int DevSetProtThresh = 22;
  public final static int DevReadPressure = 23;
  public final static int DevResetInputBuffer = 24;
  public final static int DevChangeCalibration = 25;
  public final static int DevNAve = 26;
  public final static int DevHello = 27;
  public final static int DevRead = 28;
  public final static int DevWrite = 29;
  public final static int DevGetSigConfig = 238;
  public final static int DevUpdateSigConfig = 199;
  public final static int DevReadSigValues = 239;

  /*
   * XDR Taco type
   */
  public final static int D_VOID_TYPE = 0;
  public final static int D_BOOLEAN_TYPE = 1;
  public final static int D_USHORT_TYPE = 70;
  public final static int D_SHORT_TYPE = 2;
  public final static int D_ULONG_TYPE = 71;
  public final static int D_LONG_TYPE = 3;
  public final static int D_FLOAT_TYPE = 4;
  public final static int D_DOUBLE_TYPE = 5;
  public final static int D_STRING_TYPE = 6;
  public final static int D_INT_FLOAT_TYPE = 27;
  public final static int D_FLOAT_READPOINT = 7;
  public final static int D_STATE_FLOAT_READPOINT = 8;
  public final static int D_LONG_READPOINT = 22;
  public final static int D_DOUBLE_READPOINT = 23;
  public final static int D_VAR_CHARARR = 9;
  public final static int D_VAR_STRINGARR = 24;
  public final static int D_VAR_USHORTARR = 72;
  public final static int D_VAR_SHORTARR = 10;
  public final static int D_VAR_ULONGARR = 69;
  public final static int D_VAR_LONGARR = 11;
  public final static int D_VAR_FLOATARR = 12;
  public final static int D_VAR_DOUBLEARR = 68;
  public final static int D_VAR_FRPARR = 25;
  public final static int D_VAR_SFRPARR = 73;
  public final static int D_VAR_LRPARR = 45;
  public final static int D_OPAQUE_TYPE = 47;

  /*
   * Taco state
   */
  public final static int DEVUNKNOWN = 0;
  public final static int DEVOFF = 1;
  public final static int DEVON = 2;
  public final static int DEVCLOSE = 3;
  public final static int DEVOPEN = 4;
  public final static int DEVLOW = 5;
  public final static int DEVHIGH = 6;
  public final static int DEVINSERTED = 7;
  public final static int DEVEXTRACTED = 8;
  public final static int DEVMOVING = 9;
  public final static int DEVWARMUP = 10;
  public final static int DEVINIT = 11;
  public final static int DEVSTANDBY = 12;
  public final static int DEVSERVICE = 13;
  public final static int DEVRUN = 14;
  public final static int DEVLOCAL = 15;
  public final static int DEVREMOTE = 16;
  public final static int DEVAUTOMATIC = 17;
  public final static int DEVRAMP = 18;
  public final static int DEVTRIPPED = 19;
  public final static int DEVHV_ENABLE = 20;
  public final static int DEVBEAM_ENABLE = 21;
  public final static int DEVBLOCKED = 22;
  public final static int DEVFAULT = 23;
  public final static int DEVSTARTING = 24;
  public final static int DEVSTOPPING = 25;
  public final static int DEVSTARTREQUESTED = 26;
  public final static int DEVSTOPREQUESTED = 27;
  public final static int DEVPOSITIVEENDSTOP = 28;
  public final static int DEVNEGATIVEENDSTOP = 29;
  public final static int DEVBAKEREQUESTED = 30;
  public final static int DEVBAKEING = 31;
  public final static int DEVSTOPBAKE = 32;
  public final static int DEVFORCEDOPEN = 33;
  public final static int DEVFORCEDCLOSE = 34;
  public final static int DEVOFFUNAUTHORISED = 35;
  public final static int DEVONNOTREGULAR = 36;
  public final static int DEVRESETTING = 37;
  public final static int DEVFORBIDDEN = 38;
  public final static int DEVOPENING = 39;
  public final static int DEVCLOSING = 40;
  public final static int DEVUNDEFINED = 41;
  public final static int DEVCOUNTING = 42;
  public final static int STOPPED = 43;
  public final static int RUNNING = 44;
  public final static int DEVALARM = 45;
  public final static int DEVDISABLED = 46;
  public final static int DEVSTANDBY_NOT_REACHED = 47;
  public final static int DEVON_NOT_REACHED = 48;



}
