/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class .java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.io.*;
import java.awt.*;
import java.util.*;
import LayoutEditor.*;
import LayoutMgr.MetaData.*;
import COM.objectspace.jgl.*;
import Util.*;

// Class
/**
 * The <CODE></CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DEViseLayoutState extends LayoutState implements AsciiSerializable
{
  private DList m_box_view_pairs;
  private DList m_markForDeletion = new DList();

  /* Constructor */
  public DEViseLayoutState() {
    super();
    m_box_view_pairs = new DList();
  }
  private void addPair(Box b, DEViseView view) {
    m_box_view_pairs.add(new Pair(b, view));
  }
  private void removePair(Pair p) {
    m_box_view_pairs.remove(p);
  }
  public void clear() {
    super.clear();
    for (DListIterator d = m_box_view_pairs.begin(); d.hasMoreElements(); ) {
      Pair p = (Pair)d.nextElement();
      if (p.second == null) {
        markForDeletion(p);
      }
      p.first = null;
    }
  }
  public void markForDeletion(Pair p) {
    m_markForDeletion.add(p); 
  }
  public void deleteMarkedEntries() {
    for (DListIterator d = m_markForDeletion.begin(); d.hasMoreElements(); ) {
      Pair p = (Pair)d.nextElement();
      System.out.println("Deleting..." + p);
      removePair(p);
      if (p.first != null) {
        super.removeBox((Box)p.first);
      }
      p.second = null;
    }     
    m_markForDeletion.clear();
  }

  public void addBox(Box b) {
    addBox(b, null);
  }
  public void addBox(Box b, DEViseView v) {
    super.addBox(b);
    addPair(b, v);
  }
  public void removeBox(Box b) {
    super.removeBox(b);
    Pair p = findPair(b);
    if (p.second == null) {
      markForDeletion(p);
    }
    p.first = null;
  }
  public Pair findPair(Box b) {
    for (DListIterator d = getPairs(); d.hasMoreElements(); ) {
      Pair p = (Pair)d.nextElement();
      if (p.first == b) {
        return p;
      }
    } 
    return null;
  }
  public Pair findPair(DEViseView view) {
    for (DListIterator d = getPairs(); d.hasMoreElements(); ) {
      Pair p = (Pair)d.nextElement();
      if (p.second == view) {
        return p;
      }
    } 
    return null;
  }
  public Box findBox(DEViseView view) {
    Pair p = (Pair)findPair(view);
    if (p != null) {
      return (Box)p.first;
    } 
    return null;
  }
  public DEViseView findView(Box b) {
    Pair p = (Pair)findPair(b);
    if (p != null) {
      return (DEViseView)p.second;
    } 
    return null;
  }
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append("Boxes->\n");
    for (DListIterator d = m_boxList.begin(); d.hasMoreElements(); ) {
      buf.append(d.nextElement() + "\n");
    }
    buf.append("Pairs->\n");
    for (DListIterator d = getPairs(); d.hasMoreElements(); ) {
      Pair p = (Pair)d.nextElement();
      buf.append(p);
    }
    return buf.toString();
  }

  public DListIterator getPairs() { 
    deleteMarkedEntries();
    return m_box_view_pairs.begin(); 
  }

  public void readAscii(ReadObjectInput stream) throws IOException {
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
  }
}
