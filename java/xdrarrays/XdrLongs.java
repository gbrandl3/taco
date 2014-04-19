/**
* Instances of the class <code>XdrLongs</code> represent (de-)serializeable
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

public class XdrLongs
    implements XdrAble
{

    public XdrLongs(int ai[])
    {
        value = ai;
    }

    public XdrLongs()
    {
        value = new int[0];
    }

    public int[] longsValue()
    {
        return value;
    }

    public void xdrEncode(XdrEncodingStream xdrencodingstream)
        throws OncRpcException, IOException
    {
        xdrencodingstream.xdrEncodeIntVector(value);
    }

    public void xdrDecode(XdrDecodingStream xdrdecodingstream)
        throws OncRpcException, IOException
    {
        value = xdrdecodingstream.xdrDecodeIntVector();
    }

    private int value[];
}