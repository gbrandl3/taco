package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrAble;
import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;

import java.io.IOException;

/**
 * Taco Manager configuration data.
 */
public class XdrManagerData implements XdrAble {

  public XdrRegisterData  msgInfo;
  public XdrRegisterData  dbInfo;
  public int              status;
  public int              error;
  public int	            security;

  public XdrVarArguments vArgs;

  /**
   * Encode XdrManagerData
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    msgInfo.xdrEncode(xdr);
    dbInfo.xdrEncode(xdr);
    xdr.xdrEncodeInt(status);
    xdr.xdrEncodeInt(error);
    xdr.xdrEncodeInt(security);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrManagerData
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    msgInfo = new XdrRegisterData();
    msgInfo.xdrDecode(xdr);
    dbInfo = new XdrRegisterData();
    dbInfo.xdrDecode(xdr);
    status = xdr.xdrDecodeInt();
    error = xdr.xdrDecodeInt();
    security = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
