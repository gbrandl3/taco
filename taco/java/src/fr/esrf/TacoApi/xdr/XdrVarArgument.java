package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrAble;
import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;

import java.io.IOException;

import fr.esrf.TacoApi.TacoData;
import fr.esrf.TacoApi.TacoException;

/**
 * XDR variable length argument
 */
public class XdrVarArgument implements XdrAble {

  public int     type;
  public XdrAble vArg;

  // Default construct
  XdrVarArgument() {
    type = 0;
  }

  /**
   * Construct a variable argument for the given argin taco data.
   * @param argin TacoData
   */
  public XdrVarArgument(TacoData argin) throws TacoException {
     type = argin.getType();
     vArg = argin.getXdrValue();
  }

  /**
   * Encode XdrVarArgument
   * @throws org.acplt.oncrpc.OncRpcException
   * @throws java.io.IOException
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

    xdr.xdrEncodeInt(type);
    vArg.xdrEncode(xdr);

  }

  /**
   * Decode XdrVarArgument
   * @throws org.acplt.oncrpc.OncRpcException if an ONC/RPC error occurs.
   * @throws java.io.IOException if an I/O error occurs.
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

    type = xdr.xdrDecodeInt();
    int ptrSize = xdr.xdrDecodeInt();  // Not used (should be 1)
    vArg = XdrTacoType.createXdr(type);
    vArg.xdrDecode(xdr);

  }


}
