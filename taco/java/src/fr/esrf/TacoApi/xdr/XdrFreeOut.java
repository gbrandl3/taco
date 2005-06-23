package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * RPC_DEV_FREE output XDR type
 */
public class XdrFreeOut implements XdrAble {

  public int status;
  public int error;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrFreeOut
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(status);
    xdr.xdrEncodeInt(error);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrFreeOut
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    status = xdr.xdrDecodeInt();
    error = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
