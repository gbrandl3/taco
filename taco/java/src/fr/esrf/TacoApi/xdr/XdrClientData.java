package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * TacoDevice.putGet argout XDR type
 */
public class XdrClientData implements XdrAble {

  public int             status;
  public int             error;
  public XdrVarArgument  argout;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrClientData
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(status);
    xdr.xdrEncodeInt(error);
    argout.xdrEncode(xdr);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrClientData
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    status = xdr.xdrDecodeInt();
    error = xdr.xdrDecodeInt();
    argout = new XdrVarArgument();
    argout.xdrDecode(xdr);
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);


  }

}
