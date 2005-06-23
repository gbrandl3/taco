package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrAble;
import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;

import java.io.IOException;

/**
 * XDR variable argument
 */
public class XdrVarArguments implements XdrAble {

  public XdrVarArgument[] value = new XdrVarArgument[0];

  /**
   * Encode XdrVarArgument
   * @throws org.acplt.oncrpc.OncRpcException
   * @throws java.io.IOException
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(value.length);
    for(int i=0;i<value.length;i++)
      value[i].xdrEncode(xdr);

  }

  /**
   * Decode XdrVarArgument
   * @throws org.acplt.oncrpc.OncRpcException if an ONC/RPC error occurs.
   * @throws java.io.IOException if an I/O error occurs.
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    int lenght = xdr.xdrDecodeInt();
    value = new XdrVarArgument[lenght];
    for(int i=0;i<value.length;i++) {
      value[i] = new XdrVarArgument();
      value[i].xdrDecode(xdr);
    }

  }


}
