package fr.esrf.TacoApi.xdr;

import org.acplt.oncrpc.XdrEncodingStream;
import org.acplt.oncrpc.OncRpcException;
import org.acplt.oncrpc.XdrDecodingStream;
import org.acplt.oncrpc.XdrAble;

import java.io.IOException;

/**
 * XdrDCInfo XDR type
 */
public class XdrDCInfo implements XdrAble {

  public int               errCode;
  public XdrDCCommandInfos cmdInfo;
  public int pollx;
  public int diff_time;
  public int ptr_offset;
  public int data_offset;
  public int data_base;
  public int delat1;
  public int delat2;
  public int delat3;
  public int delat4;
  public int delat5;

  /**
   * Encode XdrDCInfo
   */
  public void xdrEncode(XdrEncodingStream xdr)
          throws OncRpcException, IOException {

     xdr.xdrEncodeInt(errCode);
     cmdInfo.xdrEncode(xdr);
     xdr.xdrEncodeInt(pollx);
     xdr.xdrEncodeInt(diff_time);
     xdr.xdrEncodeInt(ptr_offset);
     xdr.xdrEncodeInt(data_base);
     xdr.xdrEncodeInt(delat1);
     xdr.xdrEncodeInt(delat2);
     xdr.xdrEncodeInt(delat3);
     xdr.xdrEncodeInt(delat4);
     xdr.xdrEncodeInt(delat5);

  }

  /**
   * Decode XdrDCInfo
   */
  public void xdrDecode(XdrDecodingStream xdr)
          throws OncRpcException, IOException {

     errCode = xdr.xdrDecodeInt();
     cmdInfo = new XdrDCCommandInfos();
     cmdInfo.xdrDecode(xdr);
     pollx = xdr.xdrDecodeInt();
     diff_time = xdr.xdrDecodeInt();
     ptr_offset = xdr.xdrDecodeInt();
     data_base = xdr.xdrDecodeInt();
     delat1 = xdr.xdrDecodeInt();
     delat2 = xdr.xdrDecodeInt();
     delat3 = xdr.xdrDecodeInt();
     delat4 = xdr.xdrDecodeInt();
     delat5 = xdr.xdrDecodeInt();

  }

}
