package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * RPC_DEV_FREE input XDR type
 */
public class XdrFreeIn implements XdrAble {

  public int dsId;
  public int secAccessRight;
  public int secClientId;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrFreeIn
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(dsId);
    xdr.xdrEncodeInt(secAccessRight);
    xdr.xdrEncodeInt(secClientId);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrFreeIn
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    dsId = xdr.xdrDecodeInt();
    secAccessRight = xdr.xdrDecodeInt();
    secClientId = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
