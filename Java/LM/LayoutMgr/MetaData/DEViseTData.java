/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseTData.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseTData</CODE> class implements the TData object in the
 * DEVise framework. A TData basically consists of a schema object and
 * a data file
 * @author  Shilpa Lawande
 * @see     DEViseObj
 * @see     DEViseType
 * @see     DEViseSchema
 */

public class DEViseTData extends DEViseObj
{
  private DEViseSchema m_schema;
  private String m_dataFile;
  
  public DEViseTData() {
    this("", null, null);
  }
  /* Creates a <CODE>DEViseTData</CODE> object with the given name
   * schema and data file
   * @param name Name of the Tdata
   * @param schema Schema object for the tdata
   * @param datafile for the tdata
   */
  public DEViseTData(String name, DEViseSchema schema, String dataFile) {
    super(name, DEViseType.TDataType);  
    m_schema = schema;
    m_dataFile = dataFile;
  }
  public DEViseSchema getSchema() { return m_schema; }
  public String getAttrName(int index) {
    return m_schema.getAttrList()[index];
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */

  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);
    DEViseSession session = (DEViseSession)m_serializeInfo;
    if (session == null) {
      throw new IOException("Cannot find session info");
    }
    String schemaName = stream.readUTF();
    m_schema = (DEViseSchema)session.mySchemas().find(schemaName);
    if (m_schema == null) {
      throw new IOException("Cannot find schema: " + schemaName);
    }
    m_dataFile = stream.readUTF();
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */

  public void writeAscii(PrintObjectOutput stream) throws IOException {
    super.writeAscii(stream);
    stream.writeChars(m_schema.getName() + " " + m_dataFile + "\n");  
  }
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append(" Schema:");
    buf.append(m_schema.toString());
    buf.append(" DataFile:" + m_dataFile);
    return buf.toString();
  }
}
