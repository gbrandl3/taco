package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrCommandInfos XDR type
 */
public class XdrDCCommandInfos implements XdrAble {

  public XdrDCCommandInfo[] value = new XdrDCCommandInfo[0];

  /**
   * Encode XdrDCCommandInfos
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(value.length);
    for(int i=0;i<value.length;i++)
      value[i].xdrEncode(xdr);

  }

  /**
   * Decode XdrDCCommandInfos
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    int lenght = xdr.xdrDecodeInt();
    value = new XdrDCCommandInfo[lenght];
    for(int i=0;i<value.length;i++) {
      value[i] = new XdrDCCommandInfo();
      value[i].xdrDecode(xdr);
    }

  }

}
