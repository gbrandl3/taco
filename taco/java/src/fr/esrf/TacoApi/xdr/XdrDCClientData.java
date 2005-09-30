package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * TacoDevice.dcDevGet argout XDR type
 */
public class XdrDCClientData implements XdrAble {

  public int           error;
  public int           type; // Not encoded
  public XdrAble       argout;

  /**
   * Encode XdrDCClientData
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(error);
    argout.xdrEncode(xdr);
    argout.xdrEncode(xdr);

  }

  /**
   * Decode XdrDCClientData
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    error = xdr.xdrDecodeInt();
    int ptrSize = xdr.xdrDecodeInt();  // Not used (should be 1)
    argout = XdrTacoType.createXdr(type);
    argout.xdrDecode(xdr);

  }

}
