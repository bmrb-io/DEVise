/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseCursor.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseCursor</CODE> class represents a cursor in DEVise.
 * A cursor is associated with a source view and a destination view.
 * A cursor can be on X,Y or X and Y attributes of GData.
 *
 * @author  Shilpa Lawande
 * @see DEViseObj
 * @see DEViseType
 * @see DEViseView
 */

public class DEViseCursor extends DEViseObj
{
  public static final int CURSOR_X = 0;  
  public static final int CURSOR_Y = 1;
  public static final int CURSOR_XY = 2;

  /* Private members */
  private DEViseView m_source;
  private DEViseView m_destination;
  private int m_cursorType;

  /* Constructor */
  public DEViseCursor() {
    this("");
  }

  /** Creates a cursor with given name. Source and destination are set to 
   *  null and type is set to CURSOR_XY
   *  @param name Name of the cursor instance
   */
  public DEViseCursor(String name) {
    this(name, CURSOR_XY, null, null);
  }

  /** Creates a cursor with given name, type, source and destination
   *  @param name Name of the cursor instance
   *  @param type Type of cursor - CURSOR_X, CURSOR_Y or CURSOR_XY
   *  @param src  The source view of the cursor
   *  @param dst  The destination view of the cursor
   */
  public DEViseCursor(String name, int type, 
                      DEViseView src, DEViseView dst) {
    super(name, DEViseType.CursorType);
    m_source = src;
    m_destination = dst;
    setCursorType(type);
  }
  /* Getters and Setters */
  /** Sets the source to the specified view and dissociates from any previous
   *  source view.
   *  @param view Source view of the cursor
   */
  public void setSrc(DEViseView view) {
    if (m_source == view) {
      return;
    }
    unsetSrc();
    m_source = view;
    if (view != null) {
      view.addCursor(this);
    }
  }
  /** Sets the destination to the specified view and dissociates from 
   *  any previous destination view
   *  @param view Destination view of the cursor
   */
  public void setDst(DEViseView view) {
    if (m_destination == view) {
      return;
    }
    unsetDst();
    m_destination = view;
    if (view != null) {
      view.addCursor(this);
    }
  }
  /** Dissociates from  the source view  and sets it to null 
   */
  public void unsetSrc() {
    if (m_source != null) {
      m_source.removeCursor(this);
    }
    m_source = null;
  }
  /** Dissociates from  the destination view  and sets it to null 
   */
  public void unsetDst() {
    if (m_destination != null) {
      m_destination.removeCursor(this);
    }
  }
  /** Retrieves the source view of the cursor
   */
  public DEViseView getSrc() {
    return m_source;
  }
  /** Retrieves the destination view of the cursor
   */
  public DEViseView getDst() {
    return m_destination;
  }
  /** Sets the cursor type to the given type
   *  @param type Type of the cursor CURSOR_X, CURSOR_Y or CURSOR_XY
   */
  public void setCursorType(int type) {
    if ((type != CURSOR_X) && (type != CURSOR_Y) && (type != CURSOR_XY)) {
      System.out.println("Invalid cursor type");
    } else {
      m_cursorType = type;
    }
  }
  public int getCursorType() {
    return m_cursorType;
  }
  /** Serialization method
    * @see Util.AsciiSerializable
    */
  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);  
    DEViseSession session = (DEViseSession)m_serializeInfo;
    if (session == null) {
      throw new IOException("Session object not found");
    }
    setCursorType(stream.readInt());
    String srcName = stream.readUTF();
    m_source = (DEViseView)session.myViews().find(srcName);
    String dstName = stream.readUTF();
    m_destination = (DEViseView)session.myViews().find(dstName);
    if (m_source == null) {
      throw new IOException("Cannot find view: " + srcName);
    }
    if (m_destination == null) {
      throw new IOException("Cannot find view:" + dstName);
    }
  } 

  /** Serialization method
    * @see Util.AsciiSerializable
    */
  public void writeAscii(PrintObjectOutput stream) 
              throws IOException  {
    super.writeAscii(stream);            
    stream.writeChars(m_cursorType + " " + m_source.getName() + " " +
                      m_destination.getName() + "\n");
  }
  /* print function */
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("Cursor Type : " + m_cursorType + " "  + 
               "Src: " +((m_source == null)? "null" : m_source.getName())+ " "+ 
               "Dst: " +((m_destination == null)? "null": m_destination.getName()) + "\n");
    return buf.toString();
  }
}
