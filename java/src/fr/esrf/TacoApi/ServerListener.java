/*+*******************************************************************

 File       : ServerListener.java

 Project    : Device Servers with sun-rpc in Java

 Description:  An interface to receive events from a server.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005-2014 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

/**
 * An interface to receive events from a server.
 */
interface ServerListener {

 /** Trigerred when the server has lost the connection. */
 public void disconnectFromServer(ServerConnection source);

}
