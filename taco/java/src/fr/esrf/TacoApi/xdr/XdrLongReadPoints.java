package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_VAR_LRPARR XDR type
 */
public class XdrLongReadPoints implements XdrAble {

  public XdrLongReadPoint[] value = new XdrLongReadPoint[0];

  /**
   * Encode XdrFloatReadPoints
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(value.length);
    for(int i=0;i<value.length;i++)
      value[i].xdrEncode(xdr);

  }

  /**
   * Decode XdrFloatReadPoints
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    int lenght = xdr.xdrDecodeInt();
    value = new XdrLongReadPoint[lenght];
    for(int i=0;i<value.length;i++) {
      value[i] = new XdrLongReadPoint();
      value[i].xdrDecode(xdr);
    }


  }

}
