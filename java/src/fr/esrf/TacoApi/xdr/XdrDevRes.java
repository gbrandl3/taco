package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.*;

import java.io.IOException;

/**
 * DB_GET_RES argout XDR type
 */
public class XdrDevRes extends XdrDbAble {

  public XdrStrings value;

  /**
   * Encode XdrDevRes
   * @throws org.acplt.oncrpc.OncRpcException
   * @throws java.io.IOException
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    value.xdrEncode(xdr);
    xdr.xdrEncodeInt(dbError);

  }

  /**
   * Decode XdrDevRes
   * @throws org.acplt.oncrpc.OncRpcException if an ONC/RPC error occurs.
   * @throws java.io.IOException if an I/O error occurs.
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    value = new XdrStrings();
    value.xdrDecode(xdr);
    dbError = xdr.xdrDecodeInt();

  }


}
