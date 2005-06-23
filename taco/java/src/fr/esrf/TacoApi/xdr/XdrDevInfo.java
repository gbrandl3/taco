package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * DbDevInfo XDR type
 */
public class XdrDevInfo implements XdrAble {

  public String devName;
  public String hostName;
  public String devType;
  public String devClass;
  public int    progNumber;
  public int    versNumber;

  /**
   * Encode XdrDevInfo
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeString(devName);
    xdr.xdrEncodeString(hostName);
    xdr.xdrEncodeString(devType);
    xdr.xdrEncodeString(devClass);
    xdr.xdrEncodeInt(progNumber);
    xdr.xdrEncodeInt(versNumber);

  }

  /**
   * Decode XdrDevInfo
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    devName = xdr.xdrDecodeString();
    hostName = xdr.xdrDecodeString();
    devType = xdr.xdrDecodeString();
    devClass = xdr.xdrDecodeString();
    progNumber = xdr.xdrDecodeInt();
    versNumber = xdr.xdrDecodeInt();

  }

}
