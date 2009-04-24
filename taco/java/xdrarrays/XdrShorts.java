/**
* Instances of the class <code>XdrShorts</code> represent (de-)serializeable
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

public class XdrShorts
    implements XdrAble
{

    public XdrShorts(short aword0[])
    {
        value = aword0;
    }

    public XdrShorts()
    {
        value = new short[0];
    }

    public short[] shortsValue()
    {
        return value;
    }

    public void xdrEncode(XdrEncodingStream xdrencodingstream)
        throws OncRpcException, IOException
    {
        xdrencodingstream.xdrEncodeShortVector(value);
    }

    public void xdrDecode(XdrDecodingStream xdrdecodingstream)
        throws OncRpcException, IOException
    {
        value = xdrdecodingstream.xdrDecodeShortVector();
    }

    private short value[];
}