/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseRecordLink.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import COM.objectspace.jgl.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseRecordLink</CODE> class represents a RecordLink in the
 * DEVise framework. A record link has a master view and one or more
 * slave views. The record link can be on any attribute of TData that
 * is common to the TData associated with the master and slave views.
 *
 * @author  Shilpa Lawande
 * @see DEViseObj
 * @see DEViseType
 * @see DEViseView
 */

public class DEViseRecordLink extends DEViseObj
{
  /* Private members */
  private DEViseView m_master;
  private DList m_slaves;
  private String m_attribute;

  /* Constructor */
  public DEViseRecordLink() {
    this("");
  }
  /** Creates a Record link with the given name and linking attribute.
    * @param name Name of the record link
    * @parsm attr The linking attribute name
    */
  public DEViseRecordLink(String name, String attr) {
    super(name, DEViseType.RecordLinkType);
    m_attribute = attr;
    m_master = null;
    m_slaves = new DList();
  }

  /** Creates a Record link with the given name, defaults the attribute
    * of the link to recId 
    * @param name Name of the record link
    */
  public DEViseRecordLink(String name) {
    this(name, "recId");
  }

  /* Getters and Setters */

  /** Disassociates from any existing master view and sets the new view
    * to master. Also if the desired master view is a slave it is 
    * removed from the slave list.
    * @param view The view desired to be master of the link
    */
  public void setMaster(DEViseView view) {
    if (m_master == view) {
      return;
    }
    unsetMaster();
    if (view != null) {
      m_master = view;
      if (m_slaves.indexOf(view)!= -1) {
        m_slaves.remove(view);
        return;
      }
      view.addRecordLink(this);
    }
  }
 /** Disassociates from any existing master view and sets the new view
   * to null
   */
  public void unsetMaster() {
    if (m_master != null) {
      m_master.removeRecordLink(this);
    }
    m_master = null;
  }
  public DEViseView getMaster() {
    return m_master;
  }
  public boolean isMaster(DEViseView view) {
    return (m_master == view);
  }
  /** @return Returns a iterator for the list of slave views.
    * @see COM.objectspace.jgl.DListIterator
    */
  public DListIterator slaves() {
    return m_slaves.begin();
  }
  /** Unsets the master if the given view is the master.
    * Add the view to the slave list.
    * @param view The view desired to be added as a slave.
    */
  public void addSlave(DEViseView view) {
    // check that the slave mapping is compatible with the record link attr
    m_slaves.add(view);
    if (m_master == view) {
      m_master = null; 
      return;
    }
    view.addRecordLink(this);
  }
  /** Removes the view from list of slaves if it exists
    * @param view The view to be removed from slave list
    */
  public void removeSlave(DEViseView view) {
    if (m_slaves.indexOf(view) != -1) {
      m_slaves.remove(view);
      view.removeRecordLink(this);
    }
  }

  public void setRecordLinkAttribute(String attr) {
    m_attribute = attr;
  }
  public String getRecordLinkAttribute() {
    return m_attribute;
  }
  /* Public methods */

  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("linkAttr = " + m_attribute + "\n");
    buf.append("Master: " + ((m_master == null)? "null" : m_master.getName()) + "\n");
    buf.append("Slaves:\n");
    for (DListIterator d = m_slaves.begin(); d.hasMoreElements(); ) {
      buf.append(((DEViseView)d.nextElement()).getName());
    }
    return buf.toString();
  }

 /** Serialization method
   * @see Util.AsciiSerializable
   */

  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);
    m_attribute = stream.readUTF();
    DEViseSession session = (DEViseSession)m_serializeInfo;
    if (session == null) {
      throw new IOException("Session object not available");
    }
    String masterName = stream.readUTF();
    DEViseView master;
    if (masterName.equals("null")) {
      master = null;
    } else {
      master = (DEViseView)session.myViews().find(masterName);
      if (master == null) {
        throw new IOException("Cannot find view: " + masterName);
      }
    }
    setMaster(master);
    int numSlaves = stream.readInt();
    for (int i = 0; i < numSlaves; i++) {
      String slaveName = stream.readUTF();
      DEViseView slave = (DEViseView)session.myViews().find(slaveName);
      if(slave == null) {
        throw new IOException("Cannot find view:" + slaveName);
      }
      addSlave(slave);
    }
  }

  /** Serialization method
    * @see Util.AsciiSerializable
    */

  public void writeAscii(PrintObjectOutput stream) throws IOException {
    super.writeAscii(stream);
    stream.writeChars(m_attribute + " ");
    stream.writeChars(m_master.getName() + " ");
    stream.writeChars(m_slaves.size() + "\n");
    for (DListIterator d = m_slaves.begin(); d.hasMoreElements();) {
      DEViseView slave = (DEViseView)d.nextElement();
      stream.writeChars(slave.getName() + "\n");
    }
  }
  public int numViewsInLink() {
    return m_slaves.size() + ((m_master != null) ? 1:0);
  }

  public void selfDestruct() {
    super.selfDestruct();
    unsetMaster();
    while(m_slaves.size() > 0) {
      DEViseView slave = (DEViseView)m_slaves.popFront();
      slave.removeRecordLink(this);
    }
  }
}
