/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseObj.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import Util.*;
// Class
/**
 * The <CODE>DEViseObj</CODE> class is the base class for all first-class
 * entities in the DEVise framework. The DEViseObj has a name and a type
 * as necessary attributes. 
 * @author  Shilpa Lawande
 * @see DEViseType
 */

public abstract class DEViseObj implements AsciiSerializable
{
  /* Private members */
  protected String  m_name;                     // Name of the object
  protected DEViseType m_type;                  // Type of the object 
    
  protected transient Object m_serializeInfo;   // any info that may temporarily
                                                // be necessary for an
                                                // object to serialize itself
   
  /* Constructor */
  public DEViseObj(DEViseType type) {
    this("", type);
  }
  public DEViseObj(String name, DEViseType type) {
    m_name = name;
    m_type = type;
    m_tracer = new Tracer();
  }
  public void finalize() {
    cerr("GoodBye to ->\n" + this + "\n");
  }
  /* Getters and Setters */
  public final String getName() {
    return m_name;
  }
  public final void setName(String name) {
    m_name = name; 
  }
  public final DEViseType getType() {
    return m_type;
  }
  public final void setType(DEViseType type) {
    m_type = type;
  }
  /* Overrides */
  public String toString() {
    // print the name of the object
    return "Name:" + m_name + "\n";
  }
  /** This function must be overridden by every  subclass to remove all 
   * references to itself, which it added explicitly during creation,
   * so that garbage collection of objects happens upon destruction
   */
  public void selfDestruct() {
    System.out.println("self destruct " + this);
  }

  // Debugging members
  private Tracer m_tracer;
  public void cout(String str) {
    m_tracer.cout(str);
  }
  public void cerr(String str) {
    m_tracer.cerr(str);
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */
  public void readAscii(ReadObjectInput stream) throws IOException {
    m_name = stream.readUTF();
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    stream.writeChars(m_name + "\n");
  }
  public void setSerializeInfo(Object obj) {
    m_serializeInfo = obj;
  }
}
