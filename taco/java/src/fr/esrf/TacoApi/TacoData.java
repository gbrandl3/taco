/*+*******************************************************************

 File       : TacoData.java

 Project    : Device Servers with sun-rpc in Java

 Description:  An Helper class to handle TacoDevice.putGet() argin and
               argout type.

 Author(s)  :	JL Pons

 Original   :	June 2005

 Copyright (c) 2005-2013 by  European Synchrotron Radiation Facility,
			     Grenoble, France

*******************************************************************-*/
package fr.esrf.TacoApi;

import org.acplt.oncrpc.*;
import fr.esrf.TacoApi.TacoException;
import fr.esrf.TacoApi.xdr.*;

/**
 * An Helper class to handle putGet argin and argout type.
 */
public class TacoData implements TacoConst {

  private int     type;       //Object type
  private XdrAble value=null; //Object value

  /** Construct an argout DeviceData that will receive the data. */
  public TacoData() {
    type = D_VOID_TYPE;
    value = new XdrVoid();
  }

  /** Construct an empty argout DeviceData that will receive the data.
   * @param type Argout type
   */
  public TacoData(int type) {
    this.type = type;
  }

  /** Construct a D_SHORT_TYPE argument */
  public void insert(short value) {
    type = D_SHORT_TYPE;
    this.value = new XdrShort(value);
  }

  /** Construct a D_VAR_SHORTARR argument */
  public void insert(short[] value) {
    type = D_VAR_SHORTARR;
    this.value = new XdrShorts(value);
  }

  /** Construct a D_USHORT_TYPE argument */
  public void insertUShort(int value) {
    type = D_USHORT_TYPE;
    this.value = new XdrShort((short)value);
  }

  /** Construct a D_VAR_USHORTARR argument */
  public void insertUShortArray(int[] value) {
    type = D_VAR_USHORTARR;
    short[] tmp = new short[value.length];
    for(int i=0;i<value.length;i++)
      tmp[i] = (short)value[i];
    this.value = new XdrShorts(tmp);
  }

  /** Construct a D_LONG_TYPE argument */
  public void insert(int value) {
    type = D_LONG_TYPE;
    this.value = new XdrInt(value);
  }

  /** Construct a D_VAR_LONGARR argument */
  public void insert(int[] value) {
    type = D_VAR_LONGARR;
    this.value = new XdrLongs(value);
  }

  /** Construct a D_ULONG_TYPE argument */
  public void insertULong(long value) {
    type = D_ULONG_TYPE;
    this.value = new XdrInt((int)value);
  }

  /** Construct a D_VAR_ULONGARR argument */
  public void insertULongArray(long[] value) {
    type = D_VAR_ULONGARR;
    int[] tmp = new int[value.length];
    for(int i=0;i<value.length;i++)
      tmp[i] = (int)value[i];
    this.value = new XdrLongs(tmp);
  }

  /** Construct a D_FLOAT_TYPE argument */
  public void insert(float value) {
    type = D_FLOAT_TYPE;
    this.value = new XdrFloat(value);
  }

  /** Construct a D_VAR_FLOATARR argument */
  public void insert(float[] value) {
    type = D_VAR_FLOATARR;
    this.value = new XdrFloats(value);
  }

  /** Construct a D_DOUBLE_TYPE argument */
  public void insert(double value) {
    type = D_DOUBLE_TYPE;
    this.value = new XdrDouble(value);
  }

  /** Construct a D_VAR_DOUBLEARR argument */
  public void insert(double[] value) {
    type = D_VAR_DOUBLEARR;
    this.value = new XdrDoubles(value);
  }

  /** Construct a D_STRING_TYPE argument */
  public void insert(String value) {
    type = D_STRING_TYPE;
    this.value = new XdrString(value);
  }

  /** Construct a D_VAR_STRINGARR argument */
  public void insert(String[] value) {
    type = D_VAR_STRINGARR;
    this.value = new XdrStrings(value);
  }

  /** Construct a D_STATE_FLOAT_READPOINT argument */
  public void insert(short state,float read,float set) {
    type = D_STATE_FLOAT_READPOINT;
    XdrStateFloatReadPoint xdr = new XdrStateFloatReadPoint();
    xdr.state = state;
    xdr.read = read;
    xdr.set = set;
    this.value = xdr;
  }

  /** Extract a D_SHORT_TYPE argument */
  public short extractShort() throws TacoException {
    checkType(D_SHORT_TYPE);
    return ((XdrShort)value).shortValue();
  }

  /** Extract a D_VAR_SHORTARR argument */
  public short[] extractShortArray() throws TacoException {
    checkType(D_VAR_SHORTARR);
    return ((XdrShorts)value).shortsValue();
  }

  /** Extract a D_USHORT_TYPE argument */
  public int extractUShort() throws TacoException {
    checkType(D_USHORT_TYPE);
    int uShort = (int)((XdrShort)value).shortValue();
    return uShort & 0xFFFF;
  }

  /** Extract a D_VAR_USHORTARR argument */
  public int[] extractUShortArray() throws TacoException {
    checkType(D_VAR_USHORTARR);
    short[] tmpS = ((XdrShorts)value).shortsValue();
    int[]   tmpI = new int[tmpS.length];
    for(int i=0;i<tmpS.length;i++) {
      int uShort = (int)tmpS[i];
      tmpI[i] = uShort & 0xFFFF;
    }
    return tmpI;
  }

  /** Extract a D_LONG_TYPE argument */
  public int extractLong() throws TacoException {
    checkType(D_LONG_TYPE);
    return ((XdrInt)value).intValue();
  }

  /** Extract a D_VAR_LONGARR argument */
  public int[] extractLongArray() throws TacoException {
    checkType(D_VAR_LONGARR);
    return ((XdrLongs)value).longsValue();
  }

  /** Extract a D_ULONG_TYPE argument */
  public long extractULong() throws TacoException {
    checkType(D_ULONG_TYPE);
    long uLong = (long)((XdrInt)value).intValue();
    return uLong & 0xFFFFFFFF;
  }

  /** Extract a D_VAR_ULONGARR argument */
  public long[] extractULongArray() throws TacoException {
    checkType(D_VAR_ULONGARR);
    int[] tmpI = ((XdrLongs)value).longsValue();
    long[] tmpL = new long[tmpI.length];
    for(int i=0;i<tmpI.length;i++) {
      long uLong = (long)tmpI[i];
      tmpL[i] = uLong & 0xFFFFFFFF;
    }
    return tmpL;
  }

  /** Extract a D_FLOAT_TYPE argument */
  public float extractFloat() throws TacoException {
    checkType(D_FLOAT_TYPE);
    return ((XdrFloat)value).floatValue();
  }

  /** Extract a D_VAR_LONGARR argument */
  public float[] extractFloatArray() throws TacoException {
    checkType(D_VAR_FLOATARR);
    return ((XdrFloats)value).floatsValue();
  }

  /** Extract a D_DOUBLE_TYPE argument */
  public double extractDouble() throws TacoException {
    checkType(D_DOUBLE_TYPE);
    return ((XdrDouble)value).doubleValue();
  }

  /** Extract a D_VAR_DOUBLEARR argument */
  public double[] extractDoubleArray() throws TacoException {
    checkType(D_VAR_DOUBLEARR);
    return ((XdrDoubles)value).doublesValue();
  }

  /** Extract a D_STRING_TYPE argument */
  public String extractString() throws TacoException {
    checkType(D_STRING_TYPE);
    return ((XdrString)value).stringValue();
  }

  /** Extract a D_VAR_STRINGARR argument */
  public String[] extractStringArray() throws TacoException {
    checkType(D_VAR_STRINGARR);
    return ((XdrStrings)value).stringsValue();
  }

  /** Extract state value of a D_STATE_FLOAT_READPOINT argument */
  public short extractStateofSFR() throws TacoException {
    return ((XdrStateFloatReadPoint)value).state;
  }

  /** Extract Set/Read values of a D_STATE_FLOAT_READPOINT  or D_FLOAT_READPOINT rgument */
  public float[] extractStateFloatReadPoint() throws TacoException {
    checkType(D_STATE_FLOAT_READPOINT);
    return new float[] { ((XdrStateFloatReadPoint)value).set,
                         ((XdrStateFloatReadPoint)value).read };
  }

  /** Extract Set/Read values of a D_LONG_READPOINT argument */
  public int[] extractLongReadPoint() throws TacoException {
    checkType(D_LONG_READPOINT);
    return new int[]{ ((XdrLongReadPoint)value).set,
                      ((XdrLongReadPoint)value).read };
  }

  /** Extract Set/Read values of a D_FLOAT_READPOINT  or D_FLOAT_READPOINT rgument */
  public float[] extractFloatReadPoint() throws TacoException {
    checkType(D_FLOAT_READPOINT);
    return new float[] { ((XdrFloatReadPoint)value).set,
                         ((XdrFloatReadPoint)value).read };
  }

  /** Extract Set/Read values of a D_DOUBLE_READPOINT argument */
  public double[] extractDoubleReadPoint() throws TacoException {
    checkType(D_DOUBLE_READPOINT);
    return new double[]{ ((XdrDoubleReadPoint)value).set,
                         ((XdrDoubleReadPoint)value).read };
  }

  /**
   * Returns the type of this TacoData object.
   */
  public int getType() {
    return type;
  }

  /**
   * Returns the XdrAble of this TacoData object.
   */
  public XdrAble getXdrValue() {
    return value;
  }

  /**
   * Sets the value.
   * @param value XdrAble value
   */
  public void setXdrValue(XdrAble value) {
    this.value = value;
  }

  private void checkType(int typeToCheck) throws TacoException {

    if(value==null)
      throw new TacoException("Trying to extract an empty argument");

    if(type!=typeToCheck) {
      throw new TacoException("Trying to extract "+ XdrTacoType.getTypeName(typeToCheck) +
                              " but argument is " + XdrTacoType.getTypeName(type) );
    }

  }

}
