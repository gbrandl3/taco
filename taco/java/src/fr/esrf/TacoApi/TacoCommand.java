/*+*******************************************************************

 File       : TacoCommand.java

 Project    : Device Servers with sun-rpc in Java

 Description:  Taco command info type.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

import fr.esrf.TacoApi.xdr.XdrTacoType;
import java.util.HashMap;
import java.util.Vector;

/**
 * Taco command info.
 */
public class TacoCommand {
  
  public int    cmdCode = 0;
  public String cmdName = "UnknownCommand";
  public String inName  = "Not specified";
  public int    inType  = 0;
  public String outName = "Not specified";
  public int    outType = 0;

  public String toString() {

    return "cmdCode:" + cmdCode + "\n" +
           "cmdName:" + cmdName + "\n" +
           "inType :" + XdrTacoType.getTypeName(inType) + "\n" +
           "inName :" + inName + "\n" +
           "outType:" + XdrTacoType.getTypeName(outType) + "\n" +
           "outName:" + outName;

  }

}
