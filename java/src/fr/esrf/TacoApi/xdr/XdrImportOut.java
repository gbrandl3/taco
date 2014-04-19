package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.*;

import java.io.IOException;

/**
 * DevImportOut XDR type
 */
public class XdrImportOut implements XdrAble {

  public String          serverMame;
  public int             dsId;
  public int             status;
  public int             error;
  public XdrVarArguments vArgs;

  /**
   * Encode DevImportOut
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    XdrOpaque tmp = new XdrOpaque(XdrTacoType.LONG_NAME_SIZE);
    tmp.xdrEncode(xdr);
    xdr.xdrEncodeInt(dsId);
    xdr.xdrEncodeInt(status);
    xdr.xdrEncodeInt(error);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode DevImportOut
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    XdrOpaque tmp = new XdrOpaque(XdrTacoType.LONG_NAME_SIZE);
    tmp.xdrDecode(xdr);
    serverMame = XdrTacoType.opaqueToStr(tmp);
    dsId = xdr.xdrDecodeInt();
    status = xdr.xdrDecodeInt();
    error = xdr.xdrDecodeInt();
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
