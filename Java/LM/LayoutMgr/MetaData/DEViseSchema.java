/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseSchema.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseSchema</CODE> class represents the DEViseSchema
 * object corresponding to the actual schema stored in the 
 * schema directory. Note that currently this is a place holder class
 * as the schema information should actually come from the DTE/catalog
 * server.
 *
 * @author  Shilpa Lawande
 * @see     DEViseObj
 * @see     DEViseType
 * @see     Util.AsciiSerializable
 */

public class DEViseSchema extends DEViseObj
{
  /* Private members */
  private String m_schemaFile;
  private String m_attrList[];

  /* Constructor */
  public DEViseSchema() {
    super("", DEViseType.SchemaType);
    m_schemaFile = null;
    m_attrList = null;
  }

  /** Creates a <CODE>DEViseSchema</CODE> given a name, file and 
    * attribute list. (Note that the schema should also have type
    * information but for now we dont have it).
    * @param schemaName The name of the schema
    * @param schemaFile The full path name for the file with the schema
    * @param attrList   A list of names of attribute names in the schema
    */                   

  public DEViseSchema(String schemaName,
                      String schemaFile, String attrList[]) {
    super(schemaName,DEViseType.SchemaType);
    m_schemaFile = schemaFile;
    m_attrList = attrList;
    saveSchema();
  }

  public DEViseSchema(String schemaFile) {
    super("", DEViseType.SchemaType);
    m_schemaFile = schemaFile;
    loadSchema();
  }

  public String[] getAttrList() { return m_attrList; }
  public String getAttrName(int index) { return m_attrList[index];}
  
  private void loadSchema() {
    try {
      FileInputStream fs = new FileInputStream(m_schemaFile);
      ReadObjectInput rs = new ReadObjectInput(fs);
      super.readAscii(rs); 
      int numAttrs = rs.readInt();
      m_attrList = new String[numAttrs];
      for (int i = 0; i < numAttrs; i++) {
        m_attrList[i] = rs.readUTF();
      }
    } 
    catch (IOException e) {
      System.out.println("Error in reading schema from file " + e);
      return;
    } 
    /*
    catch (ClassNotFoundException e) {
      System.out.println("Error in reading schema from file " + e);
      return;
    } 
    */
  }

  private void saveSchema() {
    try {
      FileOutputStream fs = new FileOutputStream(m_schemaFile);
      PrintObjectOutput ps = new PrintObjectOutput(fs);
      super.writeAscii(ps);
      ps.writeChars(m_attrList.length + "\n");
      for (int i = 0; i < m_attrList.length; i++) {
        ps.writeChars(m_attrList[i] + "\n");
      }
      ps.close();
    } 
    catch(IOException e) {
      System.out.println("Error in saving schema " + getName());
    }
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */

  public void readAscii(ReadObjectInput stream) throws IOException {
    m_schemaFile = stream.readUTF();
    loadSchema();
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */

  public void writeAscii(PrintObjectOutput stream) throws IOException {
    stream.writeChars(m_schemaFile + "\n");
  }

  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("Schema File: " + m_schemaFile);
    buf.append("Attributes:\n");
    for (int i = 0; i < m_attrList.length; i++) {
      buf.append(m_attrList[i] + "\n");
    }
    return buf.toString();
  }
}
