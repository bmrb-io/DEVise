/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class .java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.io.*;
import java.awt.*;
import COM.objectspace.jgl.*;

// Class
/**
 * The <CODE></CODE> class
 *
 * @author  Shilpa Lawande
 */

public class LayoutState implements Externalizable
{
  protected DList m_boxList;
  protected DList m_groupList;          // list of groups
  protected DList m_rivetBarList;          // groups of rivets

  /* Constructor */
  public LayoutState() {
    m_boxList = new DList();
    m_groupList = new DList();
    m_rivetBarList = new DList();
  }
  public void clear() {
    m_boxList.clear();
    m_groupList.clear();
    m_rivetBarList.clear();
  }
  public void paint(Graphics g) {
    // draw all rivetbars
    for (DListIterator d = m_rivetBarList.begin(); d.hasMoreElements(); ) {
      ((RivetBar)d.nextElement()).paint(g);
    }
    // draw all boxes 
    for (DListIterator d = m_boxList.begin(); d.hasMoreElements(); ) {
      ((Box)d.nextElement()).paint(g); 
    } 
    // draw all groups
    for (DListIterator d = m_groupList.begin(); d.hasMoreElements(); ) {
      ((GroupBox)d.nextElement()).paint(g);
    }
  }
  public void addBox(Box b) {
    m_boxList.add(b);
  }
  public void addGroup(GroupBox g) {
    // remove subsumed groups from groupList
    for (DListIterator d = g.getMembers().begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (b instanceof GroupBox) {
        m_groupList.remove(b);
      }
      b.setGrouped(true);
    }
    m_groupList.add(g);
  }
  public void addRivetBar(RivetBar b) {
    m_rivetBarList.add(b);
  }
  public void removeBox(Box b) {
    m_boxList.remove(b);
  }
  public void removeGroup(GroupBox g) {
    // add subsumed groups to groupList
    for (DListIterator d = g.getMembers().begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (b instanceof GroupBox) {
        m_groupList.add(b);
      }
      b.setGrouped(false);
    }
    m_groupList.remove(g);
  }
  public void removeRivetBar(RivetBar b) {
    m_rivetBarList.remove(b);
  }
  public DListIterator boxList() { return m_boxList.begin(); }
  public DListIterator groupList() { return m_groupList.begin(); }
  public DListIterator rivetBarList() { return m_rivetBarList.begin(); }
  public void readExternal(ObjectInput stream) 
      throws IOException, ClassNotFoundException {
  }
  public void writeExternal(ObjectOutput stream) 
      throws IOException {
  }
}
