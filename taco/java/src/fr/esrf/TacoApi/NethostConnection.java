/*+*******************************************************************

 File       : NethostConnection.java

 Project    : Device Servers with sun-rpc in Java

 Description:  This class is in charge of managing and centralizing the couple
               Manager/DatabaseDs per nethost.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005-2013 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

import fr.esrf.TacoApi.xdr.*;

import java.util.Vector;
import java.util.HashMap;
import java.net.InetAddress;
import java.net.UnknownHostException;

import org.acplt.oncrpc.*;

/**
 * Class which handle connection to Taco manager and dbserver.
 */
class NethostConnection {

  // API timeouts (milliSec)
  private final static int managerTimeout  = 3000;            // Manager server RPC timeout
  private final static int dbaseTimeout    = 4000;            // Database server RPC timeout
  private final static int dbaseRetryCount = 2;               // Retry count on DbErr_DatabaseNotConnected error
  private final static int dbaseReconnectionTimeout = 10000;  // Time after database is reimported on connection lost

  // --------------------------------------------------------------
  // Public stuff
  // --------------------------------------------------------------

  /**
   * Connects to the default nethost or to the one specified
   * if not already done , returns a valid Nethost handle.
   * @param hostName Nethost name
   * @throws TacoException in case of failure
   */
  static NethostConnection connectNethost(String hostName) throws TacoException {

    // Connection to nethost are global serialized
    synchronized (nethostMonitor) {

      // Starts by connecting to the default nethost if not already done
      connectDefaultNehost();
      if(hostName.length()==0)
        return defNethost;

      // Check that the nethost name is not the default one
      if(!hostName.equals(defNethost.name)) {

          NethostConnection newNethost = getNethostByName(hostName);
          if(newNethost==null) {
            // We have to connect a new nethost
            newNethost = new NethostConnection(hostName);
            netHostList.add(newNethost);
          }
          return newNethost;

      }

      return defNethost;

    }

  }

  /**
   * Return a valid handle to the default nethost.
   * @throws TacoException in case of failre
   */
  static NethostConnection getDefaultNethost() throws TacoException {

    synchronized (nethostMonitor) {
      connectDefaultNehost();
    }
    return defNethost;

  }

  /**
   * Get the error message associated to the code for this nethost
   * if not found in the private hash map
   * @param code Error code
   */
  synchronized String getErrorString(int code) {

    if (code <= 0)
      return "No error message stored in the database for negative error: " + code;

    Object err;

    // Search the hashMap
    Integer iCode = new Integer(code);
    err = errorList.get(iCode);

    if (err == null) {

      // We have to get it from the database
      // Decode the error number into the fields:
      // team, server and error_ident.
      int team = (code >> DS_TEAM_SHIFT);
      team = team & DS_TEAM_MASK;
      int server = (code >> DS_IDENT_SHIFT);
      server = server & DS_IDENT_MASK;
      int error_ident = (code & DS_ERRORNUMBER_NASK);

      // Contact the DB
      try {

        String[] argin = new String[]{"error/" + team + "/" + server + "/" + error_ident};
        // Retreive error from the default nethost
        String[] result = dbGetResource(argin);
        String errDbStr;
        if (result.length == 0 || result[0].length() == 0) {
          // We are sure here that no error for this code is stored in the database
          // So let's store the following message into the hashMap
          errDbStr = "No error message stored in the database for code " + code;
        } else {
          errDbStr = result[0];
        }

        // Add this error to the hashMap
        errorList.put(iCode, errDbStr);
        err = errDbStr;

      } catch (Exception e) {
        return "Failed to get error message from database for code " + code + "\n" + e.getMessage();
      }

    }

    return err.toString();

  }

  /**
   * Returns the command name associated to the code given code
   * if not found in the private hash map.
   * @param code List of code
   */
  String getCommandName(int code) {
    return getCommandNames(new Integer[]{new Integer(code)})[0];
  }

  /**
   * Returns the command names associated to the code given codes
   * if not found in the private hash map
   * @param codes List of code
   */
  synchronized String[] getCommandNames(Integer[] codes) {

    String ret[] = new String[codes.length];
    int ids[] = new int[codes.length];
    Vector toDB = new Vector();

    // Starts by checking all code we already have in memory
    int k = 0;
    for (int i = 0; i < codes.length; i++) {
      String cmdName = (String) commandList.get(codes[i]);
      if (cmdName == null) {
        // We have to get it from the db
        toDB.add(buildCommandResString(codes[i].intValue()));
        ids[k++] = i;
        // Set a default value in case of failure with DB
        ret[i] = "Cmd" + codes[i];
      } else {
        ret[i] = cmdName;
      }
    }

    if (toDB.size() > 0) {

      try {

        // Now contact the DB
        String[] allRes = XdrTacoType.toStrArr(toDB);
        String[] result = dbGetResource(allRes);
        if (result.length == allRes.length) {
          for (int i = 0; i < result.length; i++) {

            String cmdName;
            if (result[i].length() > 0) {
              // We get the result
              cmdName = result[i];
            } else {
              // No message stored in DB for this code
              cmdName = "Cmd" + codes[ids[i]];
            }
            // Add it to the hashMap
            commandList.put(codes[ids[i]], cmdName);
            ret[ids[i]] = cmdName;

          }
        }

      } catch (TacoException e) {
      }

    }

    return ret;

  }

  /**
   * Check whether security is enabled for this NetHost
   */
  boolean isSecurityEnabled() {
    return securityEnabled;
  }

  /**
   * Returns the name of this nethost.
   */
  String getName() {
    return name;
  }

  /**
   * Get device server information from the database of this nethost.
   * @param devNames device names (without the nethost)
   * @return XdrDevInfos
   * @throws TacoException
   */
  XdrDevInfos dbImportDevice(String[] devNames) throws TacoException {

    XdrStrings argin = new XdrStrings(devNames);
    XdrDevInfos argout = new XdrDevInfos();
    dataBaseCall(RPC_DB_DEVIMP,argin,argout);
    return argout;

  }

  /**
   * Get the resources from the static database of this nethost.
   * @param resNames List of resource name (4 fields lowercase)
   * @return Array of value
   */
  String[] dbGetResource(String[] resNames) throws TacoException {

    XdrStrings argin  = new XdrStrings(resNames);
    XdrDevRes  argout = new XdrDevRes();
    dataBaseCall(RPC_DB_GETRES,argin,argout);
    return argout.value.stringsValue();

  }

  /**
   * Convert a string array returned by the DB to
   * a java string array.
   * @param resValue resource value
   * @return The string array.
   */
  String[] extractResourceArray(String resValue) {

    // The first bytes contains the number of items as string (Not used here)
    int idx = resValue.indexOf((char)2);
    resValue = resValue.substring(idx+1);
    String splitElt = new String(new char[]{2});
    String[] splitted = resValue.split(splitElt);
    return splitted;

  }


  /** Return the DC server name */
  String getDcServerReadName() throws TacoException {

    if (lastDCError == null && dcServerReadName == null)
      initDC();

    if (dcServerReadName != null)
      return dcServerReadName;

    throw lastDCError;

  }

  String getDcServerWriteName() throws TacoException {

    if (lastDCError == null && dcServerReadName == null)
      initDC();

    if (dcServerWriteName != null)
      return dcServerWriteName;

    throw lastDCError;

  }

  // --------------------------------------------------------------
  // Private stuff
  // --------------------------------------------------------------

  // Manager version/program number
  private final static int NMSERVER_PROG = 100;
  private final static int NMSERVER_VERS = 4;

  // Manager commands
  private final static int RPC_GET_CONFIG = 1;

  // Database commands
  private final static int RPC_DB_GETRES = 1;
  private final static int RPC_DB_DEVIMP = 4;

  // Decoding stuff
  private final static int DS_TEAM_SHIFT =	26;
  private final static int DS_IDENT_SHIFT = 18;

  private final static int DS_TEAM_MASK = 63;       /* value = 0x3f */
  private final static int DS_IDENT_MASK = 255;     /* value = 0xff */

  private static int DS_ERRORNUMBER_NASK = 0xfff;
  private static int CMD_NUMBER_MASK = 0x3ffff;

  // Host name
  private String          name;
  private OncRpcUdpClient manager = null;
  private boolean         securityEnabled;

  // Database server stuff
  private InetAddress     dbHostName;
  private int             dbProgNumber;
  private int             dbVersNumber;
  private OncRpcUdpClient dbClient = null;
  private long            lastConnection;
  private HashMap         errorList;        // Database cache for errors
  private HashMap         commandList;      // Database chace for commands

  // DC stuff
  private String          dcServerReadName = null;
  private String          dcServerWriteName = null;
  private String          dcHostName = null;
  private TacoException   lastDCError = null;

  // Static variables
  private static Object            nethostMonitor = new Object();   // Global nethost monitor
  private static Vector            netHostList = new Vector();      // Global nethost list
  private static NethostConnection defNethost = null;               // Default nethost (METHOST environemet)

  /**
   * Connect to the nethost.
   * @param name Nethost name
   * @throws TacoException in case of failure
   */
  private NethostConnection(String name) throws TacoException {

      // We have to connect to the manager to retreive
      // nethost configuration info.
      InetAddress netHostAdress;
      OncRpcUdpClient manager;

      if(name.length() == 0)
        throw new TacoException("Environment variable NETHOST not defined");

      try {
        netHostAdress = InetAddress.getByName(name);
      } catch (UnknownHostException e) {
        throw new TacoException("Cannot resolve the nethost ip address: " + name);
      }

      try {

        manager = new OncRpcUdpClient(netHostAdress,
                                      NMSERVER_PROG,
                                      NMSERVER_VERS,
                                      0 /*Use the portmapper*/);
        manager.setTimeout(managerTimeout);
        manager.setRetransmissionTimeout(managerTimeout);

      } catch (Exception e) {
        throw new TacoException("No network manager available on " + name);
      }

      // All is OK, we can store netHost information
      this.name = name;
      this.dbClient = null;
      this.manager = manager;
      this.lastConnection = 0;

      initDBChache();

  }

  // Initialise DC for this host
  private void initDC() {

    try {

      // Search the dc_host/dc_read which handle the device name.

      String[] dcHost = dbGetResource(new String[]{"class/dc/1/host"});
      if (dcHost[0].equals("N_DEF"))
        throw new TacoException("No DC system on " + getName());

      if (dcHost[0].charAt(0) == 5) {
        // We have an array
        throw new TacoException("Multi host for DC not supported");
      }

      InetAddress dcHostAdd = null;

      // Resolve the dc host
      dcHostName = dcHost[0];

      try {
        dcHostAdd = InetAddress.getByName(dcHostName);
      } catch (UnknownHostException e) {
        throw new TacoException("Cannot resolve the DC Host ip address: " + dcHost[0]);
      }

      // Create the pseudo DC device name and select the less loaded

      int subNet = (int) dcHostAdd.getAddress()[3];
      subNet &= 0xFF;
      int min = Integer.MAX_VALUE;
      int idx = 0;

      String[] resNames = new String[10];
      for (int j = 0; j < 10; j++)
        resNames[j] = "sys/dc_rd_" + subNet + "/request/" + (j + 1);

      // Select the less loaded

      String[] requests = dbGetResource(resNames);
      if (requests[0].equals("N_DEF")) {
        throw new TacoException("Cannot select the dc_read server");
      }

      for (int j = 0; j < 10; j++) {
        int callNumber = Integer.MAX_VALUE;
        try {
          callNumber = Integer.parseInt(requests[j]);
          if(callNumber==0)
            // This dc_rd is probaly not running, ignore it
            callNumber = Integer.MAX_VALUE;
        } catch (Exception e) {}
        if (callNumber < min) {
          min = callNumber;
          idx = j;
        }
      }

      // Create the pseudo device name
      dcServerReadName = "sys/dc_rd_" + subNet + "/" + (idx + 1);
      // Select always the dc_wr_1
      dcServerWriteName = "sys/dc_wr_" + subNet + "/1";

    } catch (TacoException e) {
      dcServerReadName = null;
      dcHostName = null;
      lastDCError = e;
    }

  }

  // Database RPC function call
  private synchronized void dataBaseCall(int fCode,XdrAble in,XdrDbAble out) throws TacoException {

    // Database has been destroyed
    if(dbClient==null)
      importDataBase();

    try {

      // ONC RPC Call
      dbClient.call(fCode, in, out);

      // Check DB error
      if (out.dbError != 0) {

        switch (out.dbError) {

          case TacoException.DbErr_DatabaseNotConnected:

            /* If the server is not connected to the database (because a database update
               is just going on), sleep a while (20 mS) and redo the call */
            int retryCount = dbaseRetryCount;
            boolean ok = false;
            while (retryCount > 0 && !ok) {
              try {
                Thread.sleep(20);
                dbClient.call(fCode, in, out);
                ok = (out.dbError == 0);
              } catch (Exception e2) {
                retryCount = 0;
              }
              retryCount--;
            }

            if (!ok)
              throw new TacoException("NethostConnection.dataBaseCall(" + fCode + ") failed after " + dbaseRetryCount + " retries",
                                      out.dbError);

            break;

          case TacoException.DbErr_TooManyInfoForUDP:
            // TODO: Connect via TCP on DbErr_TooManyInfoForUDP
            throw new TacoException("NethostConnection.dataBaseCall(" + fCode + ") TCP not yet upported", out.dbError);

          default:
            throw new TacoException(out.dbError);

        }

      }

    } catch (OncRpcException e) {

      switch (e.getReason()) {

        case OncRpcException.RPC_CANTDECODEARGS:
        case OncRpcException.RPC_CANTENCODEARGS:
        case OncRpcException.RPC_BUFFEROVERFLOW:
        case OncRpcException.RPC_BUFFERUNDERFLOW:
          // Xdr problem, not a connection problem
          throw new TacoException(TacoException.DevErr_CannotDecodeArguments);

        case OncRpcException.RPC_VERSMISMATCH:
          // TODO: Implement older version of RPC calls
          throw new TacoException("RPC databse server version number mismatch for function " + fCode);

        case OncRpcException.RPC_TIMEDOUT:
          // TODO: Better implementation of reconnection on RPC_TIMEDOUT

        default:
          // Major RPC failure
          // We force the reconnection on the next call to getNethostInfo
          dbClient = null;
          throw new TacoException(e.getMessage(), TacoException.DbErr_ManagerNotResponding);

      }

    }


  }

  // Build the resource value corresponding to the command code
  private String buildCommandResString(int code) {

    int team   = (code >> DS_TEAM_SHIFT);
    team   = team & DS_TEAM_MASK;
    int server = (code >> DS_IDENT_SHIFT);
    server = server & DS_IDENT_MASK;
    int cmd_ident = (code & CMD_NUMBER_MASK);

    return "cmds/" + team + "/" + server + "/" + cmd_ident;

  }

  // Contact the manager and retreive Dbserver info
  private void importDataBase() throws TacoException {

    long now = System.currentTimeMillis();
    if ((now - lastConnection) > dbaseReconnectionTimeout) {

      lastConnection = now;

      // TODO: Should we try to reconnect to the manager ?

      XdrRegisterData argin = new XdrRegisterData();
      argin.hostName   = name;
      argin.progNumber = 0; // TODO: We should have pid here
      argin.versNumber = 0;
      XdrManagerData argout = new XdrManagerData();

      try {
        manager.call(RPC_GET_CONFIG, argin, argout);
      } catch (Exception e) {
        throw new TacoException(e.getMessage(), TacoException.DevErr_NetworkManagerNotResponding);
      }

      if (argout.status < 0) {
        String str = "Namager error code: " + argout.error;
        throw new TacoException(str, TacoException.DevErr_NetworkManagerNotResponding);
      }

      try {
        this.dbHostName = InetAddress.getByName(argout.dbInfo.hostName);
      } catch (UnknownHostException e) {
        throw new TacoException("Cannot retrieve the databse host ip address\n" + e.getMessage());
      }
      dbProgNumber = argout.dbInfo.progNumber;
      dbVersNumber = argout.dbInfo.versNumber;
      securityEnabled = (argout.security==1);

      try {

        dbClient = new OncRpcUdpClient(dbHostName,
                                       dbProgNumber,
                                       dbVersNumber,
                                       0 /*Use the portmapper*/);
        dbClient.setTimeout(dbaseTimeout);
        dbClient.setRetransmissionTimeout(dbaseTimeout);
        dbClient.call(0,new XdrVoid(),new XdrVoid());

      } catch (Exception e) {

        dbClient = null;
        throw new TacoException(e.getMessage(),TacoException.DevErr_DbImportFailed);

      }

    } else {

      throw new TacoException(TacoException.DevErr_DbImportFailed);

    }

  }

  // Connect to the default nethost
  // !!! Important, this function must be call from a globaly
  // synchronized block. !!!
  private static void connectDefaultNehost() throws TacoException {

    // Get the environement variable
    if( defNethost==null ) {

      String netHost = System.getProperty("NETHOST", "null");
      if( netHost.equals("null") )
        throw new TacoException("Environment variable NETHOST not defined");

      defNethost = new NethostConnection(netHost.toLowerCase());

    }

  }

  // Retreive a nethost according to its name
  // !!! Important, this function must be call from a globaly
  // synchronized block. !!!
  private static NethostConnection getNethostByName(String key) {

    int nb = netHostList.size();
    for(int i=0;i<nb;i++) {
      NethostConnection ni = (NethostConnection)netHostList.get(i);
      if(key.equals(ni.name)) return ni;
    }
    return null;

  }

  // Initialisation of database caches
  private void initDBChache() {

    // Default command message ------------------------------------------------------

    commandList = new HashMap();
    commandList.put(new Integer(TacoConst.DevOn),"DevOn");
    commandList.put(new Integer(TacoConst.DevOff),"DevOff");
    commandList.put(new Integer(TacoConst.DevReset),"DevReset");
    commandList.put(new Integer(TacoConst.DevState),"DevState");
    commandList.put(new Integer(TacoConst.DevStandby),"DevStandby");
    commandList.put(new Integer(TacoConst.DevRun),"DevRun");
    commandList.put(new Integer(TacoConst.DevLocal),"DevLocal");
    commandList.put(new Integer(TacoConst.DevStatus),"DevStatus");
    commandList.put(new Integer(TacoConst.DevOpen),"DevOpen");
    commandList.put(new Integer(TacoConst.DevClose),"DevClose");
    commandList.put(new Integer(TacoConst.DevSetValue),"DevSetValue");
    commandList.put(new Integer(TacoConst.DevReadValue),"DevReadValue");
    commandList.put(new Integer(TacoConst.DevInsert),"DevInsert");
    commandList.put(new Integer(TacoConst.DevExtract),"DevExtract");
    commandList.put(new Integer(TacoConst.DevReadPos),"DevReadPos");
    commandList.put(new Integer(TacoConst.DevWritePos),"DevWritePos");
    commandList.put(new Integer(TacoConst.DevSetParam),"DevSetParam");
    commandList.put(new Integer(TacoConst.DevReadParam),"DevReadParam");
    commandList.put(new Integer(TacoConst.DevUnlock),"DevUnlock");
    commandList.put(new Integer(TacoConst.DevSetConversion),"DevSetConversion");
    commandList.put(new Integer(TacoConst.DevSelectCh),"DevSelectCh");
    commandList.put(new Integer(TacoConst.DevSetProtThresh),"DevSetProtThresh");
    commandList.put(new Integer(TacoConst.DevReadPressure),"DevReadPressure");
    commandList.put(new Integer(TacoConst.DevResetInputBuffer),"DevResetInputBuffer");
    commandList.put(new Integer(TacoConst.DevChangeCalibration),"DevChangeCalibration");
    commandList.put(new Integer(TacoConst.DevNAve),"DevNAve");
    commandList.put(new Integer(TacoConst.DevHello),"DevHello");
    commandList.put(new Integer(TacoConst.DevRead),"DevRead");
    commandList.put(new Integer(TacoConst.DevWrite),"DevWrite");
    commandList.put(new Integer(TacoConst.DevGetSigConfig),"DevGetSigConfig");
    commandList.put(new Integer(TacoConst.DevUpdateSigConfig),"DevUpdateSigConfig");
    commandList.put(new Integer(TacoConst.DevReadSigValues),"DevReadSigValues");

    // Default error messages --------------------------------------------------------------

    errorList = new HashMap();
    errorList.put(new Integer(TacoException.DevErr_CannotCreateClientHandle), "Cannot create RPC client handle");
    errorList.put(new Integer(TacoException.DevErr_RPCTimedOut), "RPC client call timed out");
    errorList.put(new Integer(TacoException.DevErr_RPCFailed), "RPC client call failed");
    errorList.put(new Integer(TacoException.DevErr_MsgImportFailed), "Import of message server failed");
    errorList.put(new Integer(TacoException.DevErr_BadXdrDataTypeDefinition),"The XDR data type is not correctly defined");
    errorList.put(new Integer(TacoException.DevErr_XdrTypeListOverflow), "The list of possible XDR data types is full");
    errorList.put(new Integer(TacoException.DevErr_XdrDataTypeNotLoaded), "The requested XDR data type is not loaded");
    errorList.put(new Integer(TacoException.DevErr_DeviceNotImportedYet), "Device has not been fully imported yet, (hint: start the device server)");
    errorList.put(new Integer(TacoException.DevErr_DbImportFailed), "Import of database server failed");
    errorList.put(new Integer(TacoException.DevErr_ExceededMaximumNoOfServers), "Exceeded maximum number of servers");
    errorList.put(new Integer(TacoException.DevErr_BadServerConnection),"Lost connection to the device server");
    errorList.put(new Integer(TacoException.DevErr_DevArgTypeNotRecognised), "Unrecognised device argument type");
    errorList.put(new Integer(TacoException.DevErr_InsufficientMemory), "Device server ran out of memory");
    errorList.put(new Integer(TacoException.DevErr_NethostNotDefined), "Environment variable NETHOST not defined");
    errorList.put(new Integer(TacoException.DevErr_NoNetworkManagerAvailable), "No network manager available");
    errorList.put(new Integer(TacoException.DevErr_NetworkManagerNotResponding), "Network manager is not responding");
    errorList.put(new Integer(TacoException.DevErr_SignalOutOfRange), "Not a valid signal value");
    errorList.put(new Integer(TacoException.DevErr_CannotSetSignalHandler), "Cannot setup signal handler function");
    errorList.put(new Integer(TacoException.DevErr_DevArgTypeNotCorrectImplemented), "Device argument type is not correctly implemented");
    errorList.put(new Integer(TacoException.DevErr_UnknownInputParameter), "Input parameter is unknown");
    errorList.put(new Integer(TacoException.DevErr_CannotConnectToHost), "Cannot establish connection to host");
    errorList.put(new Integer(TacoException.DevErr_CannotDecodeArguments), "XDR: Cannot decode arguments");
    errorList.put(new Integer(TacoException.DevErr_DISPLAYNotSet), "Environment variable DISPLAY has not been setup");
    errorList.put(new Integer(TacoException.DevErr_DeviceOfThisNameNotServed),"Device of this name unknown to the server");
    errorList.put(new Integer(TacoException.DevErr_ExceededMaximumNoOfDevices),"The maximum of devices which can be served has been exceeded");
    errorList.put(new Integer(TacoException.DevErr_IncompatibleCmdArgumentTypes),"Incompatible arguments types for command function");
    errorList.put(new Integer(TacoException.DevErr_CommandNotImplemented),"Tried to execute a command which is not implemented");
    errorList.put(new Integer(TacoException.DevErr_ServerAlreadyExists), "The same Device Server is already running");
    errorList.put(new Integer(TacoException.DevErr_ServerRegisteredOnDifferentHost),"The Device Server is still registered on a different host");
    errorList.put(new Integer(TacoException.DevErr_XDRLengthCalculationFailed),"Calculation of the XDR length of the data type failed");
    errorList.put(new Integer(TacoException.DevErr_NameStringToLong), "String exceeded length of character field");
    errorList.put(new Integer(TacoException.DevErr_DeviceNoLongerExported), "The device is no longer exported");
    errorList.put(new Integer(TacoException.DevErr_ExceededMaximumNoOfClientConn), "Exceeded the maximum number of client connections");
    errorList.put(new Integer(TacoException.DevErr_DSHOMENotSet), "Environment variable DSHOME has not been setup");
    errorList.put(new Integer(TacoException.DevErr_CannotOpenErrorFile),"Message server cannot open error file");
    errorList.put(new Integer(TacoException.DevErr_CannotOpenPipe), "Message server cannot open named pipe");
    errorList.put(new Integer(TacoException.DevErr_CannotWriteToPipe), "Message server cannot write to named pipe");
    errorList.put(new Integer(TacoException.DevErr_AsynchronousCallsNotSupported), "Asynchronous calls not supported (hint: relink with V6 and/or use a remote device)");
    errorList.put(new Integer(TacoException.DevErr_AsynchronousServerNotImported), "Asynchronous server not imported yet");
    errorList.put(new Integer(TacoException.DevErr_NoCallbackSpecified), "No callback specified for asynchronous reply");
    errorList.put(new Integer(TacoException.DevErr_ExceededMaxNoOfPendingCalls), "Exceeded maximum no. of pending asynchronous calls (hint: try calling dev_synch() to clear pending calls)");
    errorList.put(new Integer(TacoException.DbErr_NoDatabase), "No database therefore the process cannot query it (hint: try running with database)");
    errorList.put(new Integer(TacoException.DbErr_CannotCreateClientHandle), "Cannot create RPC client handle to database server");
    errorList.put(new Integer(TacoException.DbErr_RPCreception), "Problem during data reception from server");
    errorList.put(new Integer(TacoException.DbErr_DatabaseAccess), "Something wrong during a database access function");
    errorList.put(new Integer(TacoException.DbErr_BooleanResource), "Bad definition for a boolean resource");
    errorList.put(new Integer(TacoException.DbErr_MemoryFree), "Problem occurs during XDR memory freeing");
    errorList.put(new Integer(TacoException.DbErr_ClientMemoryAllocation),"Impossible to allocate memory in the function client part");
    errorList.put(new Integer(TacoException.DbErr_ServerMemoryAllocation),"Impossible to allocate memory in the function server part");
    errorList.put(new Integer(TacoException.DbErr_DomainDefinition),"Bad domain definition in a resource definition");
    errorList.put(new Integer(TacoException.DbErr_ResourceNotDefined),"Resource not defined in the database");
    errorList.put(new Integer(TacoException.DbErr_DeviceServerNotDefined),"Device server not defined in the database");
    errorList.put(new Integer(TacoException.DbErr_MaxNumberOfDevice),"Too much devices for this device server");
    errorList.put(new Integer(TacoException.DbErr_DeviceNotDefined),"Device not defined in the database");
    errorList.put(new Integer(TacoException.DbErr_HostName),"The host name used in the db_dev_export function is not the same than the host name defined in the database");
    errorList.put(new Integer(TacoException.DbErr_DeviceNotExported),"Try to import a device which has not been previously exported");
    errorList.put(new Integer(TacoException.DbErr_FamilyDefinition),"Bad family definition in the data_key definition");
    errorList.put(new Integer(TacoException.DbErr_TimeCriterion),"Wrong time criterion for retrieval function");
    errorList.put(new Integer(TacoException.DbErr_BooleanDefinition),"Bad boolean definition in the research criterions structure");
    errorList.put(new Integer(TacoException.DbErr_BadValueCriterion),"Wrong criterion used for numrical data");
    errorList.put(new Integer(TacoException.DbErr_BadBooleanCriterion),"Bad boolean criterion for retrieval function");
    errorList.put(new Integer(TacoException.DbErr_BadStringCriterion),"Wrong string criterion for retrieval function");
    errorList.put(new Integer(TacoException.DbErr_MemberDefinition),"Bad member definition in the data_key definition");
    errorList.put(new Integer(TacoException.DbErr_DataDefinition),"Wrong data definition in the data_key definition");
    errorList.put(new Integer(TacoException.DbErr_BadStructureDefinition),"Bad structure definition for data");
    errorList.put(new Integer(TacoException.DbErr_BadParameters),"Bad parameters initialization");
    errorList.put(new Integer(TacoException.DbErr_OS9_FileAccess),"Problem during file access in OS-9 stand alone system");
    errorList.put(new Integer(TacoException.DbErr_NethostNotDefined),"Environment varaible NETHOST not defined");
    errorList.put(new Integer(TacoException.DbErr_NoManagerAvailable),"No database server available");
    errorList.put(new Integer(TacoException.DbErr_ManagerNotResponding),"Database server is not responding");
    errorList.put(new Integer(TacoException.DbErr_ManagerReturnError),"Network manager returned error");
    errorList.put(new Integer(TacoException.DbErr_DbServerNotExported),"No data available for this database server in the network manager");
    errorList.put(new Integer(TacoException.DbErr_MaxDeviceForUDP), "Too many exported devices for a UDP packet");
    errorList.put(new Integer(TacoException.DbErr_OtherProcessOnDb), "The database server is not the only process which has connection to the database");
    errorList.put(new Integer(TacoException.DbErr_TooManyInfoForUDP),  "The size of information sent back to the client is too big for UDP packet");
    errorList.put(new Integer(TacoException.DbErr_BadResourceType),"The resource is not initialised as an array");
    errorList.put(new Integer(TacoException.DbErr_StringTooLong),"The resource is too long to be stored in the database");
    errorList.put(new Integer(TacoException.DbErr_NameAlreadyUsed),"The pseudo device name is already used for a device");
    errorList.put(new Integer(TacoException.DbErr_CannotEncodeArguments),"XDR : Cannot encode arguments");
    errorList.put(new Integer(TacoException.DbErr_CannotDecodeResult),"XDR : Cannot decode results from db server");
    errorList.put(new Integer(TacoException.DevErr_AttributeNotFound),"Attribute name is neither defined for TACO nor for TANGO");
    errorList.put(new Integer(TacoException.DevErr_CannotConvertAttributeDataType),"Cannot cast attribute data type to the requested type");

  }

}
