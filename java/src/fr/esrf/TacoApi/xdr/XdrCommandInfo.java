package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrCommandInfo XDR type (dev_cmd_query)
 */
public class XdrCommandInfo implements XdrAble {

  public int   cmdCode;
  public int   inType;
  public int   outType;

  /**
   * Encode XdrCommandInfo
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(cmdCode);
    xdr.xdrEncodeInt(inType);
    xdr.xdrEncodeInt(outType);

  }

  /**
   * Decode XdrCommandInfo
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    cmdCode = xdr.xdrDecodeInt();
    inType = xdr.xdrDecodeInt();
    outType = xdr.xdrDecodeInt();

  }

}
