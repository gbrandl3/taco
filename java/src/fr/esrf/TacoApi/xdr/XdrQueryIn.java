package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrQueryIn dev_cmd_query input
 */
public class XdrQueryIn implements XdrAble {

  public int             dsId;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrQueryIn
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(dsId);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrQueryIn
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    dsId = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
