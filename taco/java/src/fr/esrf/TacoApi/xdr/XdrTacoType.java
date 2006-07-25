/*+*******************************************************************

 File       : XdrTacoType.java

 Project    : Device Servers with sun-rpc in Java

 Description:  An Helper class for managing Taco Xdr Type

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.*;

import java.io.IOException;
import java.util.Vector;

import fr.esrf.TacoApi.TacoConst;

/**
 * An Helper class for managing Taco Xdr Type.
 */
public class XdrTacoType implements TacoConst {

  final static int LONG_NAME_SIZE = 80;
  final static int SHORT_NAME_SIZE = 32;

  /**
   * Creates an XdrAble object for the given type.
   * @param typeCode XdrTacoType type code
   * @return new created object
   * @throws IOException in case of failure
   */ 
  public static XdrAble createXdr(int typeCode) throws IOException {

    switch(typeCode) {

     case D_VOID_TYPE:
        return new XdrVoid();
      case D_BOOLEAN_TYPE:
        return new XdrBoolean();
      case D_USHORT_TYPE:
      case D_SHORT_TYPE:
        return new XdrShort();
      case D_ULONG_TYPE:
      case D_LONG_TYPE:
        return new XdrInt();
      case D_FLOAT_TYPE:
        return new XdrFloat();
      case D_DOUBLE_TYPE:
        return new XdrDouble();
      case D_STRING_TYPE:
        return new XdrString();
      case D_INT_FLOAT_TYPE:
        return new XdrIntFloat();
      case D_FLOAT_READPOINT:
        return new XdrFloatReadPoint();
      case D_STATE_FLOAT_READPOINT:
        return new XdrStateFloatReadPoint();
      case D_LONG_READPOINT:
        return new XdrLongReadPoint();
      case D_DOUBLE_READPOINT:
        return new XdrDoubleReadPoint();
      case D_VAR_CHARARR:
        return new XdrBytes();
      case D_VAR_STRINGARR:
        return new XdrStrings();
      case D_VAR_USHORTARR:
      case D_VAR_SHORTARR:
        return new XdrShorts();
      case D_VAR_ULONGARR:
      case D_VAR_LONGARR:
        return new XdrLongs();
      case D_VAR_FLOATARR:
        return new XdrFloats();
      case D_VAR_DOUBLEARR:
        return new XdrDoubles();
      case D_VAR_FRPARR:
        return new XdrFloatReadPoints();
      case D_VAR_SFRPARR:
        return new XdrStateFloatReadPoints();
      case D_VAR_LRPARR:
        return new XdrLongReadPoints();
      case D_OPAQUE_TYPE:
        return new XdrBytes();

      default:
        throw new IOException("XdrTacoType.createXdr(" + typeCode + ") : Unsupported type.");

    }

  }

  /**
   * Returns the name of the given type code.
   * @param typeCode code
   * @return Type name
   */
  public static String getTypeName(int typeCode) {

    switch(typeCode) {

     case D_VOID_TYPE:
        return "D_VOID_TYPE";
      case D_BOOLEAN_TYPE:
        return "D_BOOLEAN_TYPE";
      case D_USHORT_TYPE:
        return "D_USHORT_TYPE";
      case D_SHORT_TYPE:
        return "D_SHORT_TYPE";
      case D_ULONG_TYPE:
        return "D_ULONG_TYPE";
      case D_LONG_TYPE:
        return "D_LONG_TYPE";
      case D_FLOAT_TYPE:
        return "D_FLOAT_TYPE";
      case D_DOUBLE_TYPE:
        return "D_DOUBLE_TYPE";
      case D_STRING_TYPE:
        return "D_STRING_TYPE";
      case D_INT_FLOAT_TYPE:
        return "D_INT_FLOAT_TYPE";
      case D_FLOAT_READPOINT:
        return "D_FLOAT_READPOINT";
      case D_STATE_FLOAT_READPOINT:
        return "D_STATE_FLOAT_READPOINT";
      case D_LONG_READPOINT:
        return "D_LONG_READPOINT";
      case D_DOUBLE_READPOINT:
        return "D_DOUBLE_READPOINT";
      case D_VAR_CHARARR:
        return "D_VAR_CHARARR";
      case D_VAR_STRINGARR:
        return "D_VAR_STRINGARR";
      case D_VAR_USHORTARR:
        return "D_VAR_USHORTARR";
      case D_VAR_SHORTARR:
        return "D_VAR_SHORTARR";
      case D_VAR_ULONGARR:
        return "D_VAR_ULONGARR";
      case D_VAR_LONGARR:
        return "D_VAR_LONGARR";
      case D_VAR_FLOATARR:
        return "D_VAR_FLOATARR";
      case D_VAR_DOUBLEARR:
        return "D_VAR_DOUBLEARR";
      case D_VAR_FRPARR:
        return "D_VAR_FRPARR";
      case D_VAR_SFRPARR:
        return "D_VAR_SFRPARR";
      case D_VAR_LRPARR:
        return "D_VAR_LRPARR";
      case D_OPAQUE_TYPE:
        return "D_OPAQUE_TYPE";

      default:
        return "TypeCode" + typeCode;

    }

  }

  /**
   * Returns the name of the given state code.
   * @param stateCode code
   * @return State name
   */
  public static String getStateName(int stateCode) {

    switch (stateCode) {

      case DEVUNKNOWN:
        return "DEVUNKNOWN";
      case DEVOFF:
        return "DEVOFF";
      case DEVON:
        return "DEVON";
      case DEVCLOSE:
        return "DEVCLOSE";
      case DEVOPEN:
        return "DEVOPEN";
      case DEVLOW:
        return "DEVLOW";
      case DEVHIGH:
        return "DEVHIGH";
      case DEVINSERTED:
        return "DEVINSERTED";
      case DEVEXTRACTED:
        return "DEVEXTRACTED";
      case DEVMOVING:
        return "DEVMOVING";
      case DEVWARMUP:
        return "DEVWARMUP";
      case DEVINIT:
        return "DEVINIT";
      case DEVSTANDBY:
        return "DEVSTANDBY";
      case DEVSERVICE:
        return "DEVSERVICE";
      case DEVRUN:
        return "DEVRUN";
      case DEVLOCAL:
        return "DEVLOCAL";
      case DEVREMOTE:
        return "DEVREMOTE";
      case DEVAUTOMATIC:
        return "DEVAUTOMATIC";
      case DEVRAMP:
        return "DEVRAMP";
      case DEVTRIPPED:
        return "DEVTRIPPED";
      case DEVHV_ENABLE:
        return "DEVHV_ENABLE";
      case DEVBEAM_ENABLE:
        return "DEVBEAM_ENABLE";
      case DEVBLOCKED:
        return "DEVBLOCKED";
      case DEVFAULT:
        return "DEVFAULT";
      case DEVSTARTING:
        return "DEVSTARTING";
      case DEVSTOPPING:
        return "DEVSTOPPING";
      case DEVSTARTREQUESTED:
        return "DEVSTARTREQUESTED";
      case DEVSTOPREQUESTED:
        return "DEVSTOPREQUESTED";
      case DEVPOSITIVEENDSTOP:
        return "DEVPOSITIVEENDSTOP";
      case DEVNEGATIVEENDSTOP:
        return "DEVNEGATIVEENDSTOP";
      case DEVBAKEREQUESTED:
        return "DEVBAKEREQUESTED";
      case DEVBAKEING:
        return "DEVBAKEING";
      case DEVSTOPBAKE:
        return "DEVSTOPBAKE";
      case DEVFORCEDOPEN:
        return "DEVFORCEDOPEN";
      case DEVFORCEDCLOSE:
        return "DEVFORCEDCLOSE";
      case DEVOFFUNAUTHORISED:
        return "DEVOFFUNAUTHORISED";
      case DEVONNOTREGULAR:
        return "DEVONNOTREGULAR";
      case DEVRESETTING:
        return "DEVRESETTING";
      case DEVFORBIDDEN:
        return "DEVFORBIDDEN";
      case DEVOPENING:
        return "DEVOPENING";
      case DEVCLOSING:
        return "DEVCLOSING";
      case DEVUNDEFINED:
        return "DEVUNDEFINED";
      case DEVCOUNTING:
        return "DEVCOUNTING";
      case STOPPED:
        return "STOPPED";
      case RUNNING:
        return "RUNNING";
      case DEVALARM:
        return "DEVALARM";
      case DEVDISABLED:
        return "DEVDISABLED";
      case DEVSTANDBY_NOT_REACHED:
        return "DEVSTANDBY_NOT_REACHED";
      case DEVON_NOT_REACHED:
        return "DEVON_NOT_REACHED";
      default:
        return "StateCode" + stateCode;

    }

  }

  /**
   * Convert the vector of string to a string array.
   */
  public static String[] toStrArr(Vector v) {

    int sz = v.size();
    String[] ret = new String[sz];
    for(int i=0;i<sz;i++) {
      ret[i] = (String)v.get(i);
    }
    return ret;

  }

  /**
   * Convert the given Opaque type to a string.
   */
  public static String opaqueToStr(XdrOpaque o) {

    byte[] b = o.opaqueValue();
    StringBuffer ret = new StringBuffer();
    for(int i=0;i<b.length && b[i]!=0;i++)
      ret.append((char)b[i]);
    return ret.toString();

  }

}
