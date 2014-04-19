package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_LONG_READPOINT XDR type
 */
public class XdrLongReadPoint implements XdrAble {

  public int   set;
  public int   read;

  /**
   * Encode XdrLongReadPoint
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(set);
    xdr.xdrEncodeInt(read);

  }

  /**
   * Decode XdrLongReadPoint
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    set  = xdr.xdrDecodeInt();
    read = xdr.xdrDecodeInt();

  }

}
