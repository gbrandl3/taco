package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * DevImportIn XDR type
 */
public class XdrImportIn implements XdrAble {

  public String devName;
  public int secAccessRight;
  public int secClientId;
  public int secConnId;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrImportIn
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeString(devName);
    xdr.xdrEncodeInt(secAccessRight);
    xdr.xdrEncodeInt(secClientId);
    xdr.xdrEncodeInt(secConnId);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrImportIn
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    devName = xdr.xdrDecodeString();
    secAccessRight = xdr.xdrDecodeInt();
    secClientId = xdr.xdrDecodeInt();
    secConnId = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
