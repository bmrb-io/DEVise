/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseMapping.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseMapping</CODE> class represents the Mapping entity in
 * DEVise. Note that in the Layout Manager, unlike DEVise, a Mapping is
 * a first-class entity and has a name.
 *
 * @author  Shilpa Lawande
 * @see DEViseObj
 * @see DEViseType
 * @see DEViseSchema
 * @see DEViseColor
 * @see DEViseSymbol
 * @see DEVisePattern
 * @see DEViseOrient
 */

// Does not implement expressions yet!

public class DEViseMapping extends DEViseObj
{
  public static final int GDATA_ATTR_X = 0;
  public static final int GDATA_ATTR_Y = 1;
  public static final int GDATA_ATTR_Z = 2;
  public static final int GDATA_ATTR_COLOR = 3;
  public static final int GDATA_ATTR_SIZE = 4;
  public static final int GDATA_ATTR_SYMBOL = 5;
  public static final int GDATA_ATTR_PATTERN = 6;
  public static final int GDATA_ATTR_ORIENT =7;
  public static final int GDATA_ATTR_SHAPEATTR = 8;
  
  private static final int NUM_GDATA_ATTR = 9;
  private static final int MAX_SHAPE_ATTR = 10;

  private int m_attrMapping[] = new int[NUM_GDATA_ATTR + MAX_SHAPE_ATTR]; 
                                            // mapping of GData attributes to 
  private double m_defaultX;
  private double m_defaultY;
  private double m_defaultZ;
  private DEViseColor m_defaultColor;
  private double m_defaultSize;
  private DEVisePattern m_defaultPattern;
  private DEViseOrient m_defaultOrient;
  private DEViseSymbol m_defaultSymbol;
  private double m_defaultShapeAttrs[] = new double[MAX_SHAPE_ATTR];
  
  private DEViseSchema m_schema;
  public DEViseMapping() {
    this("", null);
  }
  /** Creates a <CODE>DEViseMapping</CODE> with the given name and 
    * schema
    * @param name The name of the mapping
    * @param schema The <CODE>DEViseSchema</CODE> object of the schema
    *               used for the mapping
    */
  public DEViseMapping(String name, DEViseSchema schema) {
    super(name, DEViseType.MappingType);
    m_schema = schema;
    for (int i = 0; i < m_attrMapping.length; i++) {
      m_attrMapping[i] = -1;
    }
  }
  
  public void setAttrMapping(int index, int attrNum) {
    m_attrMapping[index] = attrNum;
  }
  public int getAttrMapping(int index) {
    return m_attrMapping[index];
  }
  /** Gets the attribute name if the GData attribute at the given
    * index is mapped to an attribute
    * @param index The index of the GData attribute whose mapping is desired
    */
  public String getAttrMappingName(int index) {
    if(m_attrMapping[index] != -1) {
      return m_schema.getAttrName(m_attrMapping[index]);
    }
    return "";
  }
  public DEViseSchema getSchema() { return m_schema; } 
  public void setDefaultX(double x) { m_defaultX = x; }
  public void setDefaultY(double y) { m_defaultY = y; }
  public void setDefaultZ(double z) { m_defaultZ = z; }
  public void setDefaultColor(DEViseColor clr) { m_defaultColor = clr; }
  public void setDefaultSize(double size) { m_defaultSize = size; }
  public void setDefaultPattern(DEVisePattern pat) { 
    m_defaultPattern = pat;
  }
  public void setDefaultOrient(DEViseOrient pat) { 
    m_defaultOrient = pat;
  }
  public void setDefaultSymbol(DEViseSymbol sym) {  
    m_defaultSymbol = sym;
  }
  public void setDefaultShapeAttr(int index, double val) {
    m_defaultShapeAttrs[index] = val;
  }
  public double getDefaultX() { return m_defaultX; }
  public double getDefaultY() { return m_defaultY; }
  public double getDefaultZ() { return m_defaultZ; }
  public DEViseColor getDefaultColor() { return m_defaultColor; }
  public double getDefaultSize() { return m_defaultSize; }
  public DEVisePattern getDefaultPattern() { return m_defaultPattern; }
  public DEViseOrient getDefaultOrient() { return m_defaultOrient; }
  public DEViseSymbol getDefaultSymbol() { return m_defaultSymbol; }
  public double getDefaultShapeAttr(int i) { 
    return m_defaultShapeAttrs[i]; 
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */
 
  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);
    DEViseSession session = (DEViseSession)m_serializeInfo;
    if (session == null) {
      throw new IOException("Unable to create link without session info");
    }
    String schemaName = stream.readUTF();
    m_schema = (DEViseSchema)session.mySchemas().find(schemaName);
    if (m_schema == null) {
      throw new IOException("Error in reading schema info from file");
    }
    for(int i = 0; i < m_attrMapping.length; i++) {
      m_attrMapping[i] = stream.readInt();
    }
    m_defaultX = stream.readDouble();
    m_defaultY = stream.readDouble();
    m_defaultZ = stream.readDouble();
    String colorName = stream.readUTF();
    m_defaultColor = DEViseColor.getColorByName(colorName);
    m_defaultSize = stream.readDouble();
    String symbolName = stream.readUTF();
    m_defaultSymbol = getSymbolByName(symbolName);
    String patternName = stream.readUTF();
    m_defaultPattern = DEVisePattern.getPatternByName(patternName);
    double angle = stream.readDouble();
    m_defaultOrient = new DEViseOrient(angle);

    System.out.println(this);
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */
 
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    super.writeAscii(stream);
    stream.writeChars(m_schema.getName() + "\n");
    for (int i = 0; i < m_attrMapping.length; i++) {
      stream.writeChars(m_attrMapping[i] + " ");
    }
    stream.writeChars("\n");
    stream.writeChars("# Defaults in mapping\n");
    stream.writeChars(m_defaultX + " ");
    stream.writeChars(m_defaultY + " ");
    stream.writeChars(m_defaultZ + " ");
    stream.writeChars(m_defaultColor.getColorName() + " ");
    stream.writeChars(m_defaultSize + " ");
    stream.writeChars(m_defaultSymbol.getSymbolName() + " ");
    stream.writeChars(m_defaultPattern.getPatternName() + " ");
    stream.writeChars(m_defaultOrient.getAngle() + "\n");
  }
  
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("Schema:" + m_schema);
    buf.append("Attribute Mappings: \n");
    for (int i = 0; i < m_attrMapping.length; i++) {
      buf.append("mapping[" + i + "]=" + getAttrMappingName(i) + "\n");
    }
    buf.append("Defaults:\n");
    buf.append("X: " + getDefaultX()+ "\n");
    buf.append("Y: " + getDefaultY()+ "\n");
    buf.append("Z: " + getDefaultZ()+ "\n");
    buf.append("Size: " + getDefaultSize() + "\n");
    buf.append("Symbol: " + getDefaultSymbol().getSymbolName() + "\n");
    buf.append("Pattern: " + getDefaultPattern() + "\n");
    buf.append("Orient: " + getDefaultOrient() + "\n");
    return buf.toString();
  }
  // This function is here because Java doesnt let me put it in DEViseSymbol
  // interface
  public static DEViseSymbol getSymbolByName(String name) {
    if (name.equals("Rect")) {  
      return DEViseSymbol.RectSymbol;
    } else if (name.equals("Bar")) {  
      return DEViseSymbol.BarSymbol;
    }
    return null;
  }
}
