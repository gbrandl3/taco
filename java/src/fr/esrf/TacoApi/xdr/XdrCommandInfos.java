package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrCommandInfos XDR type
 */
public class XdrCommandInfos implements XdrAble {

  public XdrCommandInfo[] value = new XdrCommandInfo[0];

  /**
   * Encode XdrCommandInfos
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(value.length);
    for(int i=0;i<value.length;i++)
      value[i].xdrEncode(xdr);

  }

  /**
   * Decode XdrCommandInfos
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    int lenght = xdr.xdrDecodeInt();
    value = new XdrCommandInfo[lenght];
    for(int i=0;i<value.length;i++) {
      value[i] = new XdrCommandInfo();
      value[i].xdrDecode(xdr);
    }

  }

}
