package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrDevInfo array XDR type
 */
public class XdrDevInfos extends XdrDbAble {

  public XdrDevInfo[] value = new XdrDevInfo[0];

  /**
   * Encode XdrDevInfos
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(value.length);
    for(int i=0;i<value.length;i++)
      value[i].xdrEncode(xdr);
    xdr.xdrEncodeInt(dbError);

  }

  /**
   * Decode XdrDevInfos
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    int lenght = xdr.xdrDecodeInt();
    value = new XdrDevInfo[lenght];
    for(int i=0;i<value.length;i++) {
      value[i] = new XdrDevInfo();
      value[i].xdrDecode(xdr);
    }
    dbError = xdr.xdrDecodeInt();

  }

}
