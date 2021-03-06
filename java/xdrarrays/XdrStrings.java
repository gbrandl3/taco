/**
* Instances of the class <code>XdrStrings</code> represent (de-)serializeable
* array of floats.
*
* <p>Please note that this class is somewhat modelled after Java's primitive
* data type wrappers. As for these classes, the XDR data type wrapper classes
* follow the concept of values with no identity, so you are not allowed to
* change the value after you've created a value object.
*
*/

package org.acplt.oncrpc;

import java.io.IOException;

// Referenced classes of package org.acplt.oncrpc:
//            XdrAble, OncRpcException, XdrEncodingStream, XdrDecodingStream

public class XdrStrings
    implements XdrAble
{

    public XdrStrings(String as[])
    {
        value = as;
    }

    public XdrStrings()
    {
        value = new String[0];
    }

    public String[] stringsValue()
    {
        return value;
    }

    public void xdrEncode(XdrEncodingStream xdrencodingstream)
        throws OncRpcException, IOException
    {
        xdrencodingstream.xdrEncodeStringVector(value);
    }

    public void xdrDecode(XdrDecodingStream xdrdecodingstream)
        throws OncRpcException, IOException
    {
        value = xdrdecodingstream.xdrDecodeStringVector();
    }

    private String value[];
}