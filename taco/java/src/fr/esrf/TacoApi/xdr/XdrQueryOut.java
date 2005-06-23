package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.*;

import java.io.IOException;

/**
 * XdrQueryOut dev_cmd_query output
 */
public class XdrQueryOut implements XdrAble {

  public XdrCommandInfos cmds;
  public String          className;
  public int             error;
  public int             status;
  public XdrVarArguments vArgs;

  /**
   * Encode XdrQueryOut
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    cmds.xdrEncode(xdr);
    XdrOpaque tmp = new XdrOpaque(XdrTacoType.SHORT_NAME_SIZE);
    tmp.xdrEncode(xdr);
    xdr.xdrEncodeInt(error);
    xdr.xdrEncodeInt(status);
    vArgs.xdrEncode(xdr);

  }

  /**
   * Decode XdrQueryOut
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    cmds = new XdrCommandInfos();
    cmds.xdrDecode(xdr);
    XdrOpaque tmp = new XdrOpaque(XdrTacoType.SHORT_NAME_SIZE);
    tmp.xdrDecode(xdr);
    className = XdrTacoType.opaqueToStr(tmp);
    error = xdr.xdrDecodeInt();
    status = xdr.xdrDecodeInt();    
    vArgs = new XdrVarArguments();
    vArgs.xdrDecode(xdr);

  }

}
