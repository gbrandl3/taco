package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrAble;
import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;

import java.io.IOException;

/**
 * XDR TacoDevice.putGet argin type
 */
public class XdrServerData implements XdrAble {

  public int dsId;
  public int cmdCode;
  public int arginType;
  public int argoutType;
  public XdrVarArgument argin;
  public int secAccessRight;
  public int secClientId;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrIntFloat
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(dsId);
    xdr.xdrEncodeInt(cmdCode);
    xdr.xdrEncodeInt(arginType);
    xdr.xdrEncodeInt(argoutType);
    argin.xdrEncode(xdr);
    xdr.xdrEncodeInt(secAccessRight);
    xdr.xdrEncodeInt(secClientId);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrIntFloat
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    dsId = xdr.xdrDecodeInt();
    cmdCode = xdr.xdrDecodeInt();
    arginType = xdr.xdrDecodeInt();
    argoutType = xdr.xdrDecodeInt();
    argin = new XdrVarArgument();
    argin.xdrDecode(xdr);
    secAccessRight = xdr.xdrDecodeInt();
    secClientId = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}

