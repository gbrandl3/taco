package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_STATE_FLOAT_READPOINT XDR type
 */
public class XdrStateFloatReadPoint implements XdrAble {

  public short state;
  public float set;
  public float read;

  /**
   * Encode XdrStateFloatReadPoint
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeShort(state);
    xdr.xdrEncodeFloat(set);
    xdr.xdrEncodeFloat(read);

  }

  /**
   * Decode XdrStateFloatReadPoint
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    state = xdr.xdrDecodeShort();
    set   = xdr.xdrDecodeFloat();
    read  = xdr.xdrDecodeFloat();

  }

}
