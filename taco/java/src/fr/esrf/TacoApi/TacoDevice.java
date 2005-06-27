/*+*******************************************************************

 File       : TacoDevice.java

 Project    : Device Servers with sun-rpc in Java

 Description:  The high level class to connect to a Taco device.

 Author(s)  :	JL Pons

 Original   :	June 2005

 $Log: not supported by cvs2svn $
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

  // Do not modify this line (it is used by the install script)
  public final String apiRelease = "$Revision: 1.3 $".substring(11,15);

  // Private member
  private String  deviceName;         // Full device name
  private String  shortName;          // Device name without the nethost
  private boolean firstImport;        // First Import flag
  private int     protocol;           // Connection protocol
  private HashMap commandList;        // The command list
  private int     timeout;            // RPC timeout
  private int     access;             // Security access
  private int     deviceId;           // Device ID (Server side)
  private String  devType;            // Device type
  private String  devClass;           // Device class
  private String processName;         // processName
  private ServerConnection dsHandle;  // Server handle
  private NethostConnection nhHandle; // Nethost handle

  /**
   * Constructs the specified device.
   * @param name Name of the device to be imported
   * @param protocol Protocol type (TCP/UDP)
   * @throws TacoException in case of major Taco failure (No Manager or NETHOST not defined)
   */
  public TacoDevice(String name,int protocol) throws TacoException {

    this.protocol      = protocol;
    this.commandList   = null;
    this.dsHandle      = null;
    this.firstImport   = true;
    this.timeout       = ServerConnection.serverTimeout;
    this.access        = ACCESS_WRITE;
    this.processName   = "";

    if(name.startsWith("taco:"))
      deviceName = name.substring(5).toLowerCase();
    else
      deviceName = name.toLowerCase();

    // Check the syntax
    checkDeviceSyntax(deviceName);

    // Extract '//nethost/' of detected
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
    this(name,PROTOCOL_UDP);
  }

  /**
   * Sets the security access level for this device.
   * @param access Security access
   */
  public void setAccess(int access) {
    this.access = access;
  }

  /**
   * Returns the security access level for this device.
   * @return
   */
  public int getAccess() {
    return access;
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
   * @throws TacoException in case of failure
   */
  public int getTimeout() throws TacoException {
    return timeout;
  }

  /**
   * Dynamicaly change the protocol of this device
   * @param protocol protocol
   */
  synchronized public void setProtocol(int protocol) throws TacoException {

    if (protocol != this.protocol) {
      this.protocol = protocol;
      // Unregister the connection
      if (dsHandle != null)
        dsHandle.freeDevice(deviceId, false, 0, 0, this);
      // Force reimport of next command
      dsHandle = null;
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
    throw new TacoException("Command " + name + " not found for this device.");

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
  public synchronized String getInfo() throws TacoException {

    // Note: All procedure that call import device must be synchronized
    importDevice();
    return "Devname:   " + deviceName + "\n" +
           "DevClass:  " + devClass + "\n" +
           "DevType:   " + devType + "\n" +
           "DsName:    " + processName + "\n" +
           "Host:      " + dsHandle.getHostName() + "\n" +
           "Nethost:   " + nhHandle.getName();

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
    if(cmd==null) throw new TacoException("Device command not found " + cmdCode);

    if(argin==null)  argin  = new TacoData();
    TacoData         argout = new TacoData(cmd.outType);

    // Build ServerDara structure
    XdrServerData dataIn = new XdrServerData();
    dataIn.dsId       = deviceId;
    dataIn.cmdCode    = cmdCode;
    dataIn.arginType  = argin.getType();
    dataIn.argoutType = argout.getType();
    dataIn.argin      = new XdrVarArgument(argin);
    dataIn.vArgs      = new XdrVarArguments();
    dataIn.secAccessRight = access;
    dataIn.secClientId    = 0; // TODO: Manage security
    XdrClientData dataOut = dsHandle.putGet(dataIn,timeout);
    argout.setXdrValue(dataOut.argout.vArg);
    return argout;

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
   * @param resNames Array of resource name
   * @return Resource values
   */
  public String[] getResources(String[] resNames) throws TacoException {

    // We do not need to be imported to do this call
    // Build argin
    String[] fullNames = new String[resNames.length];
    for(int i=0;i<resNames.length;i++)
      fullNames[i] = shortName + "/" + resNames[i].toLowerCase();
    return nhHandle.dbGetResource(fullNames);

  }

  /**
   * Free the connection to this device. A call to a command
   * after this call will force a reimport.
   */
  public synchronized void free() throws TacoException {

     if( dsHandle!=null )
       // TODO: Manage security
       dsHandle.freeDevice(deviceId,true,access,0,this);

     // Deimported this device
    firstImport = true;
    dsHandle = null;

  }

  /**
   * Retrieve server information from the database and connect to the server.
   * !!! Important Note: All procedure that call importDevice must be synchronized to this !!!
   * @throws TacoException in case of failure
   */
   private void importDevice() throws TacoException {

    // Check if we are already imported
    if(dsHandle!=null) return;

    TacoCommand[] allCmds;
    ServerConnection newDs;

    //TODO: Better handling of reconnection
    // We should try to reconnect first to the server
    // if we have already been imported

    // Get server info from the DB
    XdrDevInfos argout = nhHandle.dbImportDevice(shortName);
    devType = argout.value[0].devType;
    devClass = argout.value[0].devClass;

    try {

      // Connect the server
      newDs = ServerConnection.connectServer(nhHandle.getName(),
                                             argout.value[0].hostName,
                                             argout.value[0].progNumber,
                                             argout.value[0].versNumber,
                                             protocol,
                                             this);

      // Retreive deviceId from the server
      // TODO:Manage secutiry
      XdrImportOut impOut = newDs.importDevice(shortName, access,0,0);
      deviceId = impOut.dsId;
      processName = impOut.serverMame;

      // Get the command list to be able to manage
      // putGet argin and argout type
      allCmds = newDs.commandQuery(deviceId,nhHandle);

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
    dsHandle    = newDs;
    firstImport = false;

  }

  /**
   * (Expert usage) Should not be called.
   * Fired when a server lost the connection.
   */
  synchronized public void disconnectFromServer() {

    // No calls to the the API or other slow calls
    // can be made here else we will face deadlock
    // problems.

    // Unreference the server and force a reimport
    // on the next call to a command
    dsHandle = null;

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

      TacoDevice dev1  = new TacoDevice("test/jlp/1");
      TacoDevice dev2  = new TacoDevice("sr/testdev/2");
      TacoDevice dev3  = new TacoDevice("//aries.esrf.fr/sr/rf-tra/tra3");

      while(true) {

        long t0 = System.currentTimeMillis();
        try {

          TacoData setPoint = new TacoData();
          setPoint.insert((int)2);
          dev1.put(8727,setPoint);

          TacoData state = dev1.get(TacoConst.DevState);
          short stateValue = state.extractShort();
          System.out.println("State = " + stateValue + " [" + XdrTacoType.getStateName(stateValue) + "]");

          TacoData state2 = dev2.get(TacoConst.DevState);
          short stateValue2 = state2.extractShort();
          System.out.println("State = " + stateValue2 + " [" + XdrTacoType.getStateName(stateValue2) + "]");

          TacoData strArr = dev3.get(TacoConst.DevGetSigConfig);
          String[] cfgSig = strArr.extractStringArray();
          System.out.println("Config = Array of " + cfgSig.length);


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
