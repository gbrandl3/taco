package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrDCCommandInfo XDR type (dev_cmd_query)
 */
public class XdrDCCommandInfo implements XdrAble {

  public int   cmdCode;
  public int   outType;
  public int   time;

  /**
   * Encode XdrDCCommandInfo
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(cmdCode);
    xdr.xdrEncodeInt(outType);
    xdr.xdrEncodeInt(time);

  }

  /**
   * Decode XdrDCCommandInfo
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    cmdCode = xdr.xdrDecodeInt();
    outType = xdr.xdrDecodeInt();
    time = xdr.xdrDecodeInt();

  }

}
