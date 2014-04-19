package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * TacoDevice.dcDevGet argin XDR type
 */
public class XdrDCServerData implements XdrAble {

  public String          devName;
  public int             cmdCode;
  public int             argoutType;

  /**
   * Encode XdrDCServerData
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeString(devName);
    xdr.xdrEncodeInt(cmdCode);
    xdr.xdrEncodeInt(argoutType);

  }

  /**
   * Decode XdrDCServerData
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    devName = xdr.xdrDecodeString();
    cmdCode = xdr.xdrDecodeInt();
    argoutType = xdr.xdrDecodeInt();

  }

}
