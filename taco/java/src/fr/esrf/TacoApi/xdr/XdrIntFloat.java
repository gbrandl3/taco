package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_INT_FLOAT_TYPE XDR type
 */
public class XdrIntFloat implements XdrAble {

  public int   state;
  public float value;

  /**
   * Encode XdrIntFloat
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(state);
    xdr.xdrEncodeFloat(value);

  }

  /**
   * Decode XdrIntFloat
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    state = xdr.xdrDecodeInt();
    value = xdr.xdrDecodeFloat();

  }

}
