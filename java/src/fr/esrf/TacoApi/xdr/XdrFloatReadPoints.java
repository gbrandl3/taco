package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * D_VAR_FRPARR XDR type
 */
public class XdrFloatReadPoints implements XdrAble {

  public XdrFloatReadPoint[] value = new XdrFloatReadPoint[0];

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
    value = new XdrFloatReadPoint[lenght];
    for(int i=0;i<value.length;i++) {
      value[i] = new XdrFloatReadPoint();
      value[i].xdrDecode(xdr);
    }


  }

}
