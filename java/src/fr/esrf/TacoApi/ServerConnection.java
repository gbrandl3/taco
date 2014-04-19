/*+*******************************************************************

 File       : ServerConnection.java

 Project    : Device Servers with sun-rpc in Java

 Description:  This class is in charqe of centralizing connection
               to device servers.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005-2014 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

import org.acplt.oncrpc.*;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Vector;
import java.io.IOException;

import fr.esrf.TacoApi.xdr.*;

/**
 * Class which handle connection to a Taco server.
 */
class ServerConnection {

  // API timeouts (milliSec)
  static final int serverTimeout             = 3000;    // Default RPC timeout
  private static final int adminTimeout      = 4000;    // Timeout for RPC admin calls (milliSec)
  private static final int connectionUdpTimeout = 100;  // Timeout for connection to server (UDP protocol)

  // -----------------------------------------------------------------------------------
  // Public stuff
  // -----------------------------------------------------------------------------------

  /**
   * Constructs a server if not already exists else return the handle
   * of the existing one.
   * @param nethosName Nethost name
   * @param hostName Host where the server is running
   * @param progNumber RPC program number
   * @param versNumber RPC version number
   * @param protocol Inet protocol
   * @param sl Server listener (cannot be null)
   * @return A valid server handle
   * @throws TacoException in case of failure
   */
  static ServerConnection connectServer(String nethosName,String hostName,
                                        int progNumber,int versNumber,int protocol,
                                        ServerListener sl) throws TacoException {

    ServerConnection servInfo;
    String fullName = createKeyString(hostName,progNumber,protocol,nethosName);

    // Server connection are globaly serialized
    synchronized (serverMonitor) {

      // Check if this server has already been imported
      servInfo = getFromStringKey(fullName);
      if( servInfo==null ) {
        // Connect the server
        servInfo = new ServerConnection(hostName,progNumber,versNumber,protocol,fullName);
        serverList.add(servInfo);
      }
      // Register this device
      if(sl!=null)
        servInfo.registerListener(sl);

    }

    return servInfo;

  }

  /**
   * Gets device information from this server.
   * @param deviceName Device Name
   * @param access Desired access right
   * @param clientId Security stuff
   * @param connId Security stuff
   * @return XdrImportOut
   * @throws TacoException
   */
  XdrImportOut importDevice(String deviceName, int access, int clientId, int connId) throws TacoException {

    XdrImportIn impIn = new XdrImportIn();
    impIn.devName = deviceName;
    impIn.secAccessRight = access;
    impIn.secClientId = clientId;
    impIn.secConnId = connId;
    impIn.vArgs = new XdrVarArguments();
    XdrImportOut impOut = new XdrImportOut();
    rpcCall(RPC_DEV_IMPORT,impIn,impOut,adminTimeout);
    if(impOut.status < 0 )
       throw new TacoException("RpcDevImport("+deviceName+") failed with error:" , impOut.error);
    return impOut;

  }

  /**
   * Class RPC_DEV_FREE if asked and close the server if no longer referenced.
   * @param deviceId the deviceId return by importDevice
   * @param call True to call RPC_DEV_FREE , false otherwise
   * @param access access right
   * @param clientId Security stuff
   * @param sl ServerListener to be unregistered
   * close the server if no longer referenced.
   */
  void freeDevice(int deviceId,boolean call,int access,int clientId,ServerListener sl) throws TacoException {

    if (call) {
      XdrFreeIn in = new XdrFreeIn();
      in.dsId = deviceId;
      in.secAccessRight = access;
      in.secClientId = clientId;
      in.vArgs = new XdrVarArguments();
      XdrFreeOut out = new XdrFreeOut();
      rpcCall(RPC_DEV_FREE, in, out, adminTimeout);
      if (out.status < 0)
        throw new TacoException(out.error);
    }

    synchronized (this) {

      serverListener.remove(sl);
      // close the server if no longer referenced
      if(serverListener.size()==0) destroy(0);
      // The handle of this object should be unreferenced
      // after this call

    }

  }


  /**
   * Gets the command list of the specified device
   * @param deviceId the deviceId return by importDevice
   * @param nh Nethost from where command name will be get (if not null).
   * @return Command array
   * @throws TacoException
   */
  TacoCommand[] commandQuery(int deviceId,NethostConnection nh) throws TacoException {

    int i;
    XdrQueryIn in = new XdrQueryIn();
    XdrQueryOut out = new XdrQueryOut();
    in.dsId = deviceId;
    in.vArgs = new XdrVarArguments();
    rpcCall(RPC_DEV_CMD_QUERY,in,out,adminTimeout);
    if(out.status < 0 )
       throw new TacoException(out.error);

    // Create TacoCommand Array
    int nbCmd = out.cmds.value.length;
    TacoCommand[] allCmds = new TacoCommand[nbCmd];
    for (i = 0; i < nbCmd; i++) {
      allCmds[i] = new TacoCommand();
      allCmds[i].cmdCode = out.cmds.value[i].cmdCode;
      allCmds[i].inType = out.cmds.value[i].inType;
      allCmds[i].outType = out.cmds.value[i].outType;
    }

    // Decode command names if present within the argout
    if (out.vArgs.value.length == nbCmd) {

      for (i = 0; i < nbCmd; i++) {
        XdrString xdr = (XdrString) out.vArgs.value[i].vArg;
        allCmds[i].cmdName = xdr.stringValue();
      }

    } else {

      if(nh!=null) {
        // Try to get them from the DB or from the internal hash map.
        Integer[] codes = new Integer[nbCmd];
        for (i = 0; i < nbCmd; i++) codes[i] = new Integer(allCmds[i].cmdCode);
        String[] cmdNames = nh.getCommandNames(codes);
        for (i = 0; i < nbCmd; i++) allCmds[i].cmdName = cmdNames[i];
      }

    }

    // TODO: Extract argin and argout description
    // String className = new String(out.className.opaqueValue());

    return allCmds;

  }

  /**
   * Executes a Taco command a on device.
   * @param in Input args
   * @param timeout timeout in millisec
   * @return output args
   */
  XdrClientData putGet(XdrServerData in,int timeout) throws TacoException {

    XdrClientData out = new XdrClientData();
    rpcCall(RPC_DEV_PUTGET,in,out,timeout);
    if (out.status < 0) {
      if (out.vArgs.value.length == 1) {
        // We have an extended error Message
        String err = ((XdrString) out.vArgs.value[0].vArg).stringValue();
        throw new TacoException(err);
      }
      throw new TacoException(out.error);
    }
    return out;

  }

  /**
   * Executes a Taco command from the data collector.
   * @param in Input args
   * @param timeout timeout in millisec
   * @return output args
   */
  XdrDCClientData dcPutGet(XdrDCServerData in,int timeout) throws TacoException {

    XdrDCClientData out = new XdrDCClientData();
    out.type = in.argoutType;
    rpcCall(RPC_DC_DEVGET,in,out,timeout);
    if (out.error != 0)
      throw new TacoException(out.error);
    return out;

  }

  TacoCommand[] commandQueryDC(String devName,NethostConnection nh) throws TacoException {

    //XdrOutPar info = new XdrOutPar();
    //rpcCall(7,new XdrStrings(new String[]{devName}),info,adminTimeout);
    //TacoCommand[] allCmds = new TacoCommand[nbCmd];
    //return allCmds;

    XdrDCInfo info = new XdrDCInfo();
    rpcCall(RPC_DC_DEVINFO,new XdrString(devName),info,adminTimeout);

    if(info.errCode!=0)
      throw new TacoException(info.errCode);

    int nbCmd = info.cmdInfo.value.length;
    int i;
    TacoCommand[] allCmds = new TacoCommand[nbCmd];
    for (i = 0; i < nbCmd; i++) {
      allCmds[i] = new TacoCommand();
      allCmds[i].cmdCode = info.cmdInfo.value[i].cmdCode;
      allCmds[i].inType  = 0; // VOID
      allCmds[i].outType = info.cmdInfo.value[i].outType;
      allCmds[i].hasCache = true;
    }

    if(nh!=null) {
      // Try to get them from the DB or from the internal hash map.
      Integer[] codes = new Integer[nbCmd];
      for (i = 0; i < nbCmd; i++) codes[i] = new Integer(allCmds[i].cmdCode);
      String[] cmdNames = nh.getCommandNames(codes);
      for (i = 0; i < nbCmd; i++) allCmds[i].cmdName = cmdNames[i];
    }

    return allCmds;

  }

  /**
   * Destroy this connection.
   */
  void destroy() {
    try {
      destroy(0);
    }  catch (TacoException e) {}
  }

  /**
   * Returns the host from where this server has been exported.
   */
  String getHostName() {
    return hostName;
  }

  /**
   * Returns the portNumber of this device.
   * @return progNumber.
   */
  int getProgNumber() {
    return progNumber;
  }

  // -----------------------------------------------------------------------------------
  // Private stuff
  // -----------------------------------------------------------------------------------

  private String          hostName;           // Host of the server
  private int             protocol;           // RPC protocol
  private long            lastConnection;     // last success connection time
  private int             progNumber;         // RPC server prog number
  private Vector          serverListener;     // Client device registered to this server

  private String          fullName;           // Name used as search key
  private InetAddress     hostIp;             // Inet adress of the host
  private OncRpcClient    client;             // RPC Handle to the server

  // Internal management stuff
  private static Object serverMonitor = new Object();  // Global monitor for server list
  private static Vector serverList    = new Vector();  // Global static server list (to make conn unique)

  // Generic RPC server command
  private static final int RPC_DEV_IMPORT    = 1;
  private static final int RPC_DEV_FREE      = 2;
  private static final int RPC_DEV_PUTGET    = 3;
  private static final int RPC_DEV_PUT       = 4;
  private static final int RPC_DEV_CMD_QUERY = 5;

  // DC commands
  private final static int RPC_DC_DEVGET      = 4;
  private final static int RPC_DC_DEVINFO     = 6;

  /**
   * Connect a Server.
   * Make the connection and call the NULLPROC.
   */
  private ServerConnection(String hostName,int progNumber,int versNumber,int protocol,
                           String key) throws TacoException {

    this.hostName   = hostName;
    this.protocol   = protocol;
    this.lastConnection = 0;
    this.serverListener = new Vector();
    this.progNumber = progNumber;

    // This name will be used as search key
    fullName = key;

    try {
      hostIp = InetAddress.getByName(hostName);
    } catch(UnknownHostException e) {
      throw new TacoException("Cannot resolve server hostname : " + hostName + "\n" + e.getMessage() );
    }

    // Connect to the server
    try {

      // Contact the portmapper
      switch (protocol) {
        case TacoDevice.PROTOCOL_UDP:
          client = new OncRpcUdpClient(hostIp, progNumber, versNumber, 0);
          break;
        case TacoDevice.PROTOCOL_TCP:
          client = new OncRpcTcpClient(hostIp, progNumber, versNumber, 0);
          break;
      }

      if(protocol==TacoDevice.PROTOCOL_UDP) {
        // Call the NULLPROC with a low timeout
        setTimeout(connectionUdpTimeout);
        client.call(0,new XdrVoid(),new XdrVoid());
      } else {
        // In TCP the socket should not make a timeout
        // So let's call with the default ADMIN timeout
        setTimeout(adminTimeout);
        client.call(0,new XdrVoid(),new XdrVoid());
      }

    } catch (Exception e) {

      throw new TacoException(TacoException.DevErr_BadServerConnection);

    }

    // We are now connected
    lastConnection = System.currentTimeMillis();

  }

  /**
   * Destroy the server and fire TacoException(errorToFire if != 0).
   * No call or access to this object should be done after a
   * call to this function.
   * @param errorToFire TacoException to be fired
   * @throws TacoException Fire TacoException(errorToFire)
   */
   private void destroy(int errorToFire) throws TacoException {

    // We have to be globaly serialized here
    // Do not make slow or blocking calls inside this block
    synchronized (serverMonitor) {

      // Disconnect all registered devices
      for (int i = 0; i < serverListener.size(); i++)
        ((ServerListener) serverListener.get(i)).disconnectFromServer(this);
      // Mark this server as destroyed
      client = null;
      // Remove from the global list
      serverList.remove(this);

    }

    // Throw the exception
    if(errorToFire!=0)
      throw new TacoException(errorToFire);

  }

  /**
   * Register the specified listener to this server.
   * @param l Listener that will receive server envent
   */
  private void registerListener(ServerListener l) {
    serverListener.add(l);
  }


  /**
   * Set the timeout for this server.
   * @param timeout timeout in milliSec
   */
  private void setTimeout(int timeout) {

    client.setTimeout(timeout);
    switch (protocol) {
      case TacoDevice.PROTOCOL_UDP:
        ((OncRpcUdpClient)client).setRetransmissionTimeout(timeout);
        break;
      case TacoDevice.PROTOCOL_TCP:
        ((OncRpcTcpClient)client).setTransmissionTimeout(timeout);
        break;
    }

  }

  /**
   * Performs RPC operations
   * @param fCode RPC function code
   * @param in Input arg
   * @param out Output arg
   * @param timeout timeout for this call (milliSec)
   * @throws TacoException in case of RPC failure
   */
  // We serialize by server here, making parralell calls on
  // the same server is not possible
  private synchronized void rpcCall(int fCode, XdrAble in, XdrAble out, int timeout) throws TacoException {

    if (client == null)
      // Already destroyed
      throw new TacoException(TacoException.DevErr_BadServerConnection);

    try {

      // ONC RPC Call
      setTimeout(timeout);
      client.call(fCode, in, out);
      lastConnection = System.currentTimeMillis();

    } catch (OncRpcException e) {

      switch (e.getReason()) {

        case OncRpcException.RPC_CANTDECODEARGS:
        case OncRpcException.RPC_CANTENCODEARGS:
        case OncRpcException.RPC_BUFFEROVERFLOW:
        case OncRpcException.RPC_BUFFERUNDERFLOW:
          // Xdr problem ,not a connection problem
          throw new TacoException(TacoException.DevErr_CannotDecodeArguments);

        case OncRpcException.RPC_VERSMISMATCH:

          // TODO: Implement older version of RPC call
          throw new TacoException("RPC server version number mismatch");

        case OncRpcException.RPC_TIMEDOUT:

          // Check the maximum RPC_TIMEDOUT time allowed and force reconnection
          //long now = System.currentTimeMillis();
          //if ((now - lastConnection) > 15000)
          //  destroy(TacoException.DevErr_RPCTimedOut);
          //throw new TacoException(TacoException.DevErr_RPCTimedOut);

          if(protocol == TacoDevice.PROTOCOL_UDP) {
            // In UDP we destroy the server to force
            // a reconnection.
            destroy(TacoException.DevErr_RPCTimedOut);
          } else {
            // In TCP, Simply fire timedout
            throw new TacoException(TacoException.DevErr_RPCTimedOut);
          }

        default:
          // Major RPC failure
          // We destroy the server to force the reconnection
          destroy(TacoException.DevErr_BadServerConnection);

      }

    }

  }

  // Create an identifiant string for the server
  private static String createKeyString(String hostName,int progNumber,int protocol,String nethostName) {

    StringBuffer key = new StringBuffer();
    key.append(Integer.toString(progNumber)).append("/");
    key.append(hostName).append("/");
    key.append(Integer.toString(protocol)).append("/");
    key.append(nethostName);
    return key.toString().toLowerCase();

  }

  // Retreive a server according to its string identifier
  // Important, this function must be call from a globaly
  // synchronized block.
  private static ServerConnection getFromStringKey(String key) {

    int nb = serverList.size();
    for(int i=0;i<nb;i++) {
      ServerConnection si = (ServerConnection)serverList.get(i);
      if(key.equals(si.fullName)) return si;
    }
    return null;

  }

}
