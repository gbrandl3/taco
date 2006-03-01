/*+*******************************************************************

 File       : TacoDevice.java

 Project    : Device Servers with sun-rpc in Java

 Description:  The high level class to connect to a Taco device.

 Author(s)  :	JL Pons

 Original   :	June 2005

 $Log: not supported by cvs2svn $
 Revision 1.11  2006/01/13 10:33:57  jlpons
 Fixed argout decoding bug

 Revision 1.10  2005/12/05 15:52:42  jlpons
 Fixed DC decoding bug

 Revision 1.9  2005/10/20 16:14:17  jlpons
 Fixed DC issue

 Revision 1.8  2005/10/19 16:37:18  jlpons
 Changed the SOURCE_CACHE_DEVICE algorithm

 Revision 1.7  2005/10/19 13:20:01  jlpons
 getInfo() does not throw an exception when not connected.

 Revision 1.6  2005/10/10 13:28:26  jlpons
 New algorithm for SOURCE_CACHE_DEVICE strategu

 Revision 1.5  2005/10/10 09:24:02  jlpons
 Fixed TacoDevice.getResource()

 Revision 1.4  2005/09/30 16:38:13  jlpons
 Implented DC access

 Revision 1.3  2005/06/27 09:31:49  jlpons
 Fixed argin encoding bug

 Revision 1.2  2005/06/24 08:41:33  jlpons
 Added full hostname syntax for nethost

 Revision 1.1  2005/06/23 16:19:45  jlpons
 Initial import


 Copyright (c) 2005 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

import  fr.esrf.TacoApi.xdr.*;

import java.util.HashMap;
import java.util.regex.Pattern;

/**
 * The high level object which provides an easy-to-use client interface to TACO devices.
 */
public class TacoDevice implements ServerListener {

  /** RPC UDP connection protocol */
  public final static int PROTOCOL_UDP = 0;
  /** RPC TCP connection protocol */
  public final static int PROTOCOL_TCP = 1;

  /** Security, No access */
  public final static int ACCESS_NO        = -20;
  /** Security, Read only access */
  public final static int ACCESS_READ      = -10;
  /** Security, Read and write access (Default) */
  public final static int ACCESS_WRITE     = 0;
  /** Security, Single user write access */
  public final static int ACCESS_SI_WRITE  = 10;
  /** Security, Read and write access and the rigth to execute super user commands */
  public final static int ACCESS_SU_ACCESS = 20;
  /** Security, Single user mode with super user rigths */
  public final static int ACCESS_SI_SU     = 30;
  /** Security, Single user administration */
  public final static int ACCESS_ADMIN     = 99;

  /** Device access */
  public final static int SOURCE_DEVICE = 0;
  /** Data collector access */
  public final static int SOURCE_CACHE  = 1;
  /** Cache access and Device access when DC fails */
  public final static int SOURCE_CACHE_DEVICE = 2;

  // Do not modify this line (it is used by the install script)
  public final String apiRelease = "$Revision: 1.12 $".substring(11,15);

  // Timeout before reinporting the device from the database
  private static long dbImportTimeout = 30000L; // 30 sec

  // Private member
  private String  deviceName;                // Full device name
  private String  shortName;                 // Device name without the nethost
  private int     protocol;                  // Connection protocol
  private int     source;                    // DC / Device access
  private HashMap commandList;               // The command list (from the device)
  private int     timeout;                   // RPC timeout
  private int     access;                    // Security access
  private int     deviceId;                  // Device ID (Server side)
  private String  devType="";                // Device type
  private String  devClass="";               // Device class
  private String  devProcessName="";         // processName
  private String  dcType="";                 // Device type
  private String  dcClass="";                // Device class
  private String  hostName="";               // hostname
  private int     progNumber;                // RPC prog number
  private String  hostDCReadName;            // hostname (dc_read)
  private int     progDCReadNumber;          // RPC prog number (dc_read)
  private String  hostDCWriteName;           // hostname (dc_write)
  private int     progDCWriteNumber;         // RPC prog number (dc_write)
  private boolean firstImport;               // First Import flag
  private long    lastDevImportTime;         // Time of the db_import(device)
  private TacoException lastDevImportError;  // Last device import error
  private long    lastDcImportTime;          // Time of the db_import(device)
  private TacoException lastDcImportError;   // Last device import error
  private ServerConnection dcHandle;         // DC server handle
  private ServerConnection dsHandle;         // Server handle
  private NethostConnection nhHandle;        // Nethost handle

  /**
   * Constructs the specified device.
   * @param name Name of the device to be imported
   * @param protocol Protocol type (TCP/UDP)
   * @throws TacoException in case of major Taco failure (No Manager or NETHOST not defined)
   */
  public TacoDevice(String name,int protocol,int source) throws TacoException {

    this.protocol        = protocol;
    this.commandList     = null;
    this.dsHandle        = null;
    this.dcHandle        = null;
    this.firstImport     = true;
    this.lastDevImportTime  = 0;
    this.timeout         = ServerConnection.serverTimeout;
    this.access          = ACCESS_WRITE;
    this.devProcessName     = "";
    this.lastDevImportError = null;
    this.lastDcImportError  = null;
    this.source             = source;

    if(name.startsWith("taco:"))
      deviceName = name.substring(5).toLowerCase();
    else
      deviceName = name.toLowerCase();

    // Check the syntax
    checkDeviceSyntax(deviceName);

    // Extract '//nethost/' if detected
    String netHost = "";
    if(deviceName.startsWith("//")) {

      int i = deviceName.indexOf('/',2);
      if(i<0)
        throw new TacoException("Wrong device name syntax");
      shortName = deviceName.substring(i+1);
      netHost = deviceName.substring(2,i);

    } else {
      shortName = deviceName;
    }

    // Connect to the nethost, this will fire TacoException
    // on major Taco error (Manager not responding).
    nhHandle = NethostConnection.connectNethost(netHost);

  }

  /**
   * Constructs the specified device (UDP protocol).
   * @param name Name of the device to be imported
   * @throws TacoException in case of major Taco failure (No Manager or NETHOST not defined)
   */
  public TacoDevice(String name) throws TacoException {
    this(name,PROTOCOL_UDP,SOURCE_DEVICE);
  }

  /**
   * Constructs the specified device (UDP protocol).
   * @param name Name of the device to be imported
   * @throws TacoException in case of major Taco failure (No Manager or NETHOST not defined)
   */
  public TacoDevice(String name,int protocol) throws TacoException {
    this(name,protocol,SOURCE_DEVICE);
  }

  /**
   * Sets the security access level for this device.
   * @param access Security access
   */
  public void setSecurityAccess(int access) {
    this.access = access;
  }

  /**
   * Returns the security access level for this device.
   * @return
   */
  public int getSecurityAccess() {
    return access;
  }

  /**
   * Sets the source for this device and force a reimport.
   * @param s Source
   * @see #SOURCE_DEVICE
   * @see #SOURCE_CACHE_DEVICE
   * @see #SOURCE_CACHE
   */
  synchronized public void setSource(int s) throws TacoException {
    if (s != this.source) {
      this.source = s;
      free();
    }
  }

  /**
   * Return the current source.
   * @return Source value
   * @see #setSource
   */
  public int getSource() {
    return source;
  }

  /**
   * Returns the name of this device.
   */
  public String getName() {
    return deviceName;
  }

  /**
   * Returns the name of this device (without the nethost if specified).
   */
  public String getShortName() {
    return shortName;
  }

  /**
   * Set the RPC timeout for this device (in milliSec).
   * @param milliSec timeout in milliSec
   */
  public void setTimeout(int milliSec) {
    timeout = milliSec;
  }

  /**
   * Returns the RPC of this device (in milliSec)
   */
  public int getTimeout() {
    return timeout;
  }

  /**
   * Dynamicaly change the protocol of this device and force a reimport
   * @param protocol protocol
   * @see #PROTOCOL_TCP
   * @see #PROTOCOL_UDP
   */
  synchronized public void setProtocol(int protocol) throws TacoException {

    if (protocol != this.protocol) {
      this.protocol = protocol;
      free();
    }

  }

  /**
   * Return the protocol used by this device.
   */
  public int getProtocol() {
    return protocol;
  }

  /**
   * Return the code associated to the command name (for this device).
   * @param name Command name
   * @return Command code
   * @throws TacoException in case of failure
   */
  public int getCommandCode(String name) throws TacoException {

    TacoCommand[] cmds = commandQuery();
    for(int i=0;i<cmds.length;i++) {
      if(cmds[i].cmdName.equalsIgnoreCase(name))
        return cmds[i].cmdCode;
    }
    if(source!=SOURCE_CACHE)
      throw new TacoException("Device command " + name + " not found for this device.");
    else
      throw new TacoException("DC command " + name + " not found for this device.");

  }

  /**
   * Return the string associated to the code.
   * @param code command code
   */
  public String getCommandString(int code) {

     // We do not need to be imported to do this call
     return nhHandle.getCommandNames(new Integer[]{new Integer(code)})[0];

  }

  /**
   * Return device info.
   */
  public synchronized String getInfo() {

    // Note: All procedure that call import device must be synchronized
    try {
      importDevice();
    } catch(TacoException e) {
      return e.getErrorString();
    }

    switch(source) {

      case SOURCE_DEVICE:
        return "Devname:   " + deviceName + "\n" +
               "DevClass:  " + devClass + "\n" +
               "DevType:   " + devType + "\n" +
               "DsName:    " + devProcessName + "\n" +
               "Host:      " + dsHandle.getHostName() + "\n" +
               "Nethost:   " + nhHandle.getName();

      case SOURCE_CACHE_DEVICE:
        if (dcHandle != null) {
          return "Devname:   " + deviceName + "\n" +
                 "DevClass:  " + dcClass + "\n" +
                 "DevType:   " + dcType + "\n" +
                 "DsName:    " + devProcessName + "\n" +
                 "Host:      " + dcHandle.getHostName() + "\n" +
                 "Nethost:   " + nhHandle.getName();
        } else {
          return "Devname:   " + deviceName + "\n" +
                 "DevClass:  " + dcClass + "\n" +
                 "DevType:   " + dcType + "\n" +
                 "DsName:    " + devProcessName + "\n" +
                 "Host:      " + dsHandle.getHostName() + "\n" +
                 "Nethost:   " + nhHandle.getName();
        }
      case SOURCE_CACHE:
        return "Devname:   " + deviceName + "\n" +
               "DevClass:  " + dcClass + "\n" +
               "DevType:   " + dcType + "\n" +
               "DsName:    " + devProcessName + "\n" +
               "Host:      " + dcHandle.getHostName() + "\n" +
               "Nethost:   " + nhHandle.getName();
    }

    return "No info";

  }

  /**
   * Returns the command list of this device.
   * @throws TacoException in case of failure
   */
  public synchronized TacoCommand[] commandQuery() throws TacoException {

    // Note: All procedure that call import device must be synchronized
    importDevice();
    // Convert Taco command list to array
    Object[] vec = commandList.values().toArray();
    TacoCommand[] ret = new TacoCommand[vec.length];
    for(int i=0;i<commandList.size();i++) ret[i] = (TacoCommand)vec[i];
    return ret;

  }

  /**
   * Executes a command on a device.
   * @param cmd Command
   * @param argin Argin value (null can be passed for D_VOID_TYPE)
   * @return Data got from the device (if any)
   */
  private TacoData devPutGet(TacoCommand cmd,TacoData argin) throws TacoException {

    if (argin == null) argin = new TacoData();
    TacoData argout = new TacoData(cmd.outType);

    // Build ServerDara structure
    XdrServerData dataIn = new XdrServerData();
    dataIn.dsId = deviceId;
    dataIn.cmdCode = cmd.cmdCode;
    dataIn.arginType = argin.getType();
    dataIn.argoutType = argout.getType();
    dataIn.argin = new XdrVarArgument(argin);
    dataIn.vArgs = new XdrVarArguments();
    dataIn.secAccessRight = access;
    dataIn.secClientId = 0; // TODO: Manage security
    XdrClientData dataOut = dsHandle.putGet(dataIn, timeout);
    argout.setXdrValue(dataOut.argout.vArg);
    return argout;

  }

  /**
   * Executes a command on a device via the data collector.
   * @param cmd Command
   * @return Data got from the DC (if any)
   */
  private TacoData dcDevGet(TacoCommand cmd) throws TacoException {

    // Build ServerDara structure
    TacoData argout = new TacoData(cmd.outType);
    XdrDCServerData dataIn = new XdrDCServerData();
    dataIn.cmdCode = cmd.cmdCode;
    dataIn.devName = shortName;
    dataIn.argoutType = argout.getType();
    XdrDCClientData dataOut = dcHandle.dcPutGet(dataIn, timeout);
    argout.setXdrValue(dataOut.argout);
    return argout;

  }

  /**
   * Executes a command om a device.
   * @param cmdCode Command code
   * @param argin Argin value (null can be passed for D_VOID_TYPE)
   * @return Data got from the device (if any)
   */
  public synchronized TacoData putGet(int cmdCode,TacoData argin) throws TacoException {

    // Note: All procedure that call import device must be synchronized
    importDevice();

    // Get command info
    TacoCommand cmd = (TacoCommand)commandList.get( new Integer(cmdCode) );

    switch(source) {

      case SOURCE_DEVICE:

        if(cmd==null) throw new TacoException("Device command not found " + cmdCode);
        return devPutGet(cmd,argin);

      case SOURCE_CACHE_DEVICE:

        if(cmd==null) throw new TacoException("Device command not found " + cmdCode);
        if(cmd.hasCache && dcHandle!=null) {
          try {
            TacoData argout = dcDevGet(cmd);
            return argout;
          } catch (TacoException e) {
            // Cache failed
            if(dsHandle!=null)
              return devPutGet(cmd,argin);
            else
              throw e;
          }
        } else {
          if(dsHandle!=null)
            return devPutGet(cmd,argin);
          else
            throw new TacoException("Failed to execute " + cmd.cmdName );
        }

      case SOURCE_CACHE:

        if(cmd==null) throw new TacoException("DC command not found " + cmdCode);
        if(cmd.hasCache)
          return dcDevGet(cmd);
        else
          throw new TacoException("The command " + cmd.cmdName + " is not polled by the DC");

    }

    throw new TacoException("Wrong source parameter");

  }

  /**
   * Executes a simple command (VOID,VOID) on a device.
   * @param cmdCode Command code
   */
  public void command(int cmdCode) throws TacoException {
    putGet(cmdCode,null);
  }

  /**
   * Executes a command that returns data.
   * @param cmdCode Command code
   */
  public TacoData get(int cmdCode) throws TacoException {
    return putGet(cmdCode,null);
  }

  /**
   * Executes a command that gets input data.
   * @param cmdCode Command code
   * @param argin Argin value
   */
  public void put(int cmdCode,TacoData argin) throws TacoException {
    putGet(cmdCode,argin);
  }

  /**
   * Retreive resource of this device from the static database.
   * @param  resName resource name
   * @return Resource value or an empty array if the resource does not
   * exists
   */
  public String[] getResource(String resName) throws TacoException {

    // We do not need to be imported to do this call
    // Build argin
    String[] fullName = new String[]{ shortName + "/" + resName.toLowerCase() };
    String result = nhHandle.dbGetResource(fullName)[0];

    if(result.equals("N_DEF"))
      return new String[0];

    if( result.charAt(0) == 5 ) {

      // We have an array
      return nhHandle.extractResourceArray(result);

    } else {

      // Simple resource
      return new String[]{result};

    }

  }

  /**
   * Free the connection to this device. A call to a command
   * after this call will force a reimport.
   */
  public synchronized void free() throws TacoException {

     if( dsHandle!=null ) {
       // TODO: Manage security
       dsHandle.freeDevice(deviceId,true,access,0,this);
       dsHandle = null;
     }

    if( dcHandle!=null ) {
       dcHandle.destroy();
       dcHandle = null;
    }

     // Deimported this device
    firstImport = true;
    lastDevImportTime = 0;

  }

  /**
   * Retreive server info from the database.
   */
  private void dbImportDevice() throws TacoException {

    long now = System.currentTimeMillis();

    if ((now - lastDevImportTime) > dbImportTimeout) {
      lastDevImportTime = now;

      //  -- (re)import the device here -------------------------------------

      try {

        String[] devNames;
        devNames = new String[]{shortName};

        XdrDevInfos argout = nhHandle.dbImportDevice(devNames);
        devType = argout.value[0].devType;
        devClass = argout.value[0].devClass;
        hostName = argout.value[0].hostName;
        progNumber = argout.value[0].progNumber;
        lastDevImportError = null;

      } catch (TacoException e) {
        lastDevImportError = e;
      }

    }

    if (lastDevImportError != null)
      throw lastDevImportError;

  }

  /**
   * Retreive DC info from the database.
   */
  private void dbImportDc() throws TacoException {

    long now = System.currentTimeMillis();

    if ((now - lastDcImportTime) > dbImportTimeout) {
      lastDcImportTime = now;

      //  -- (re)import the device here -------------------------------------

      try {

        String dcWriteName = nhHandle.getDcServerWriteName();
        String dcReadName = nhHandle.getDcServerReadName();
        String[] devNames = new String[]{dcWriteName,dcReadName};
        XdrDevInfos argout = nhHandle.dbImportDevice(devNames);
        dcType  = argout.value[1].devType;
        dcClass = argout.value[1].devClass;
        hostDCWriteName = argout.value[0].hostName;
        progDCWriteNumber = argout.value[0].progNumber;
        hostDCReadName = argout.value[1].hostName;
        progDCReadNumber = argout.value[1].progNumber;
        lastDcImportError = null;

      } catch (TacoException e) {
        lastDcImportError = e;
      }

    }

    if (lastDcImportError != null) {
      throw new TacoException("DC system import failed.\nHint: Use SOURCE_DEVICE to avoid this message\n"+lastDcImportError.getErrorString());
    }

  }

  /**
   * Retreive DC server info from the database.
   */
  private void initDc() throws TacoException {

    TacoCommand[]    allCmds;
    ServerConnection dcRead;
    ServerConnection dcWrite;

    // Connect the DC write server to get the polling config
    try {
      dcWrite = ServerConnection.connectServer(nhHandle.getName(),
                                               hostDCWriteName,
                                               progDCWriteNumber,
                                               1,
                                               PROTOCOL_TCP,
                                               null);
    } catch (TacoException e) {
      throw new TacoException("DC write server "+nhHandle.getDcServerWriteName()+" not responding");
    }

    // Get the list of polled commands
    allCmds = dcWrite.commandQueryDC(shortName, nhHandle);
    dcWrite.destroy();

    if( commandList==null ) {
      // Build the command hashMap
      commandList = new HashMap();
      for (int i = 0; i < allCmds.length; i++)
        commandList.put(new Integer(allCmds[i].cmdCode), allCmds[i]);
    } else {
      // Update cache field
      for (int i = 0; i < allCmds.length; i++) {
        TacoCommand cmd = (TacoCommand)commandList.get( new Integer(allCmds[i].cmdCode) );
        if(cmd!=null) cmd.hasCache = true;
      }
    }

    // Now connect to the dc_read
    dcRead = ServerConnection.connectServer(nhHandle.getName(),
                                            hostDCReadName,
                                            progDCReadNumber,
                                            1,
                                            PROTOCOL_TCP,
                                            this);

    // We are now ready to exectute commands via DC
    dcHandle = dcRead;

  }

  /**
   * Init device access.
   * @throws TacoException
   */
  private void initDev() throws TacoException {

    TacoCommand[] allCmds;
    ServerConnection newDs;

    try {

      // Connect the server
      newDs = ServerConnection.connectServer(nhHandle.getName(),
                                             hostName,
                                             progNumber,
                                             4, /* Current Taco server release */
                                             /* argout.value[0].versNumber,*/
                                             protocol,
                                             this);

      // Retreive deviceId from the server
      // TODO:Manage secutiry
      XdrImportOut impOut = newDs.importDevice(shortName, access, 0, 0);
      deviceId = impOut.dsId;
      devProcessName = impOut.serverMame;

      // Get the command list to be able to manage
      // putGet argin and argout type
      allCmds = newDs.commandQuery(deviceId, nhHandle);

    } catch (TacoException e) {

      if (firstImport) {
        throw new TacoException(TacoException.DevErr_DeviceNotImportedYet);
      } else {
        throw e;
      }

    }

    // Build the command hashMap
    commandList = new HashMap();
    for (int i = 0; i < allCmds.length; i++)
      commandList.put(new Integer(allCmds[i].cmdCode), allCmds[i]);

    // We are now ready to exectute commands
    dsHandle = newDs;
    firstImport = false;

  }

  /**
   * Retrieve server information from the database and connect to the server.
   * !!! Important Note: All procedure that call importDevice must be synchronized to this !!!
   * @throws TacoException in case of failure
   */
   private void importDevice() throws TacoException {

    // Check if we are imported
    switch(source) {

      case SOURCE_DEVICE:

        if(dsHandle!=null) return;
        dbImportDevice();
        initDev();
        break;

      case SOURCE_CACHE:

        if(dcHandle!=null) return;
        dbImportDc();
        initDc();
        break;

      case SOURCE_CACHE_DEVICE:

        TacoException devErr = null;
        if (dsHandle != null || dcHandle != null) return;

        try {
          dbImportDevice();
          initDev();
        } catch (TacoException e) {
          devErr = e;
        }

        try {
          dbImportDc();
          initDc();
        } catch (TacoException e) {}

        if(dsHandle == null && dcHandle == null)
          throw devErr;

        break;

    }

  }

  /**
   * (Expert usage) Should not be called.
   * Fired when a server lost the connection.
   */
  synchronized public void disconnectFromServer(ServerConnection source) {

    // No calls to the the API or other slow calls
    // can be made here else we will face deadlock
    // problems.

    // Unreference the server and force a reimport
    // on the next call to a command
    if(source == dsHandle)
      dsHandle = null;

    if(source == dcHandle)
      dcHandle = null;

  }

  /**
   * Checks the device syntax.
   * @param name Device name (whithout the taco:)
   */
  private void checkDeviceSyntax(String name) throws TacoException {

    boolean ret;

    // Check full syntax: //hostName/domain/family/member
    ret = Pattern.matches("//[a-zA-Z_0-9]+/[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+", name);

    // Check full syntax: //xxx.xxx.xxx/hostName/domain/family/member
    if (ret == false)
      ret = Pattern.matches("//[a-zA-Z_0-9]+\\.[a-zA-Z_0-9]+\\.[a-zA-Z_0-9]+/[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+", name);

    // Check full syntax: //ipAddress/domain/family/member
    if (ret == false)
       ret = Pattern.matches("//[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+/[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+", name);

    // Check classic syntax: domain/family/member
    if (ret == false)
       ret = Pattern.matches("[a-zA-Z_0-9[-]]+/[a-zA-Z_0-9[-]]+/[[a-zA-Z_0-9][-]]+", name);

    if(ret == false )
      throw new TacoException("Wrong device name syntax for " + name);

  }

  /**
   * Main test function.
   * @param args Not yet used
   */
  public static void main(String[] args) {

    try {

      TacoDevice dev1  = new TacoDevice("sr/m-hp/1");
//        TacoDevice dev1  = new TacoDevice("sr/d-ct/1");
//      TacoDevice dev2  = new TacoDevice("sr/jlp/1");
//      TacoDevice dev3  = new TacoDevice("//aries/sr/d-fbpm/peak-h");
        dev1.setSource(SOURCE_CACHE_DEVICE);
//        dev1.setProtocol(PROTOCOL_TCP);
//      dev2.setSource(SOURCE_CACHE_DEVICE);
//      dev3.setSource(SOURCE_CACHE_DEVICE);

      while(true) {

        long t0 = System.currentTimeMillis();
        try {

          int cmdCode = dev1.getCommandCode("DevReadDisplay");
          TacoData values = dev1.get(cmdCode);
          float statusValue = values.extractFloat();
          System.out.println("Display = " + statusValue );

//          TacoData state2 = dev2.get(TacoConst.DevState);
//          stateValue2 = state2.extractShort();
//          System.out.println("State (sr/jlp/1)= " + stateValue2 + " [" + XdrTacoType.getStateName(stateValue2) + "]");

 //         cmdCode = dev3.getCommandCode("DevUpdate");
 //         TacoData values3 = dev3.get(cmdCode);
 //         float[] dvalues3 = values3.extractStateFloatReadPoint();
 //         System.out.println("Values sr/d-fbpm/peak-h read " + dvalues3[0] + " , " + dvalues3[1]);

        } catch (TacoException e1) {
          System.out.println("TacoException: " + e1.getErrorString());
        }
        System.out.println("Time: " + (System.currentTimeMillis() - t0) + " ms");

        try { Thread.sleep(2000); } catch (Exception e) {};

      }

      //TacoData setPoint = new TacoData();
      //setPoint.insertFloat(200.5f);
      //dev.put(TacoConst.DevSetValue,setPoint);
      //dev.free();

    } catch(TacoException e2) {

      System.out.println("TacoException: " + e2.getErrorString());
      System.out.println("\n-----------------------\n");
      e2.printStackTrace();

    }

  }


}
