package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_FLOAT_READPOINT XDR type
 */
public class XdrFloatReadPoint implements XdrAble {

  public float set;
  public float read;

  /**
   * Encode XdrFloatReadPoint
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeFloat(set);
    xdr.xdrEncodeFloat(read);

  }

  /**
   * Decode XdrFloatReadPoint
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    set  = xdr.xdrDecodeFloat();
    read = xdr.xdrDecodeFloat();

  }

}
