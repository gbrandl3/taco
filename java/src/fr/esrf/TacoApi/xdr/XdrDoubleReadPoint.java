package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_DOUBLE_READPOINT XDR type
 */
public class XdrDoubleReadPoint implements XdrAble {

  public double set;
  public double read;

  /**
   * Encode XdrDoubleReadPoint
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeDouble(set);
    xdr.xdrEncodeDouble(read);

  }

  /**
   * Decode XdrDoubleReadPoint
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    set  = xdr.xdrDecodeDouble();
    read = xdr.xdrDecodeDouble();

  }

}
