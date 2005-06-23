package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrAble;
import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;

import java.io.IOException;

/**
 * Taco Manager Register data.
 */
public class XdrRegisterData implements XdrAble {

  public String hostName;
  public int    progNumber;
  public int    versNumber;

  /**
   * Encode XdrRegisterData
   * @throws OncRpcException
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeString(hostName);
    xdr.xdrEncodeInt(progNumber);
    xdr.xdrEncodeInt(versNumber);

  }

  /**
   * Decode XdrRegisterData
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    hostName = xdr.xdrDecodeString();
    progNumber = xdr.xdrDecodeInt();
    versNumber = xdr.xdrDecodeInt();

  }

}
