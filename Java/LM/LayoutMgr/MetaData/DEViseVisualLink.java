/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseVisualLink.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.io.*;
import COM.objectspace.jgl.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseVisualLink</CODE> class implements a visual link.
 * An instance of this class represents an equivalence class of views
 * linked by this link. The visual link can be of type DEVISE_XX_LINK,
 * DEVISE_YY_LINK or DEVISE_XY_LINK. All views linked by a XX (YY) link
 * have the linking attribute as X (Y). If the link is of type XY then
 * the linking attribute of the view must be specified when linking the
 * view.
 *
 * @author  Shilpa Lawande
 * @see     DEViseObj
 * @see     DEViseType
 * @see     DEViseView
 */

public class DEViseVisualLink extends DEViseObj
{
  public static final int DEVISE_XX_LINK = 0;
  public static final int DEVISE_YY_LINK = 1;
  public static final int DEVISE_XY_LINK = 2;
  public static final int DEVISE_X_ATTR = 0x1;
  public static final int DEVISE_Y_ATTR = 0x2;

  private DList m_viewList;  // list of views in link
  private int m_linkType;    // XX, XY, YY
  
  private DEViseSession m_session; // this session

  /* Constructor */
  public DEViseVisualLink() {
    super(DEViseType.VisualLinkType);
    m_viewList = new DList();
  }
  public DEViseVisualLink(String name, int type) {
    super(name, DEViseType.VisualLinkType);
    if ((type != DEVISE_XX_LINK) || (type != DEVISE_XY_LINK) ||
        (type != DEVISE_YY_LINK)) {
      m_linkType = type;
    }
    m_viewList = new DList();
  }
  public DListIterator viewList() { return m_viewList.begin(); }

  public DEViseView getView(Object obj) {
    if (obj instanceof DEViseVisualLinkEntry) {
      return ((DEViseVisualLinkEntry)obj).m_view;
    } else {
      return null;
    }
  }
  public int getLinkType() {
    return m_linkType;
  }
  public void addView(DEViseView view) {
    if (m_linkType == DEVISE_XX_LINK) {
      addView(view, DEVISE_X_ATTR);
    } else if (m_linkType == DEVISE_YY_LINK) {
      addView(view, DEVISE_Y_ATTR);
    } else {
      System.out.println("Assuming attribute to be X");
      addView(view, DEVISE_X_ATTR);
    }
  }
  public void addView(DEViseView view, int attr) {
    DEViseVisualLinkEntry entry = new DEViseVisualLinkEntry(view, attr);
    m_viewList.add(entry);
    view.addVisualLink(this);
  }
  private int find(DEViseView view) {
    int i = 0;
    for (DListIterator d = m_viewList.begin(); d.hasMoreElements(); i++) {
      DEViseVisualLinkEntry entry = (DEViseVisualLinkEntry)d.nextElement();
      if (entry.m_view == view) {
        return i;
      }
    }
    return -1;
  }
  public void removeView(DEViseView view) {
    int index = find(view);
    m_viewList.remove(index);
    view.removeVisualLink(this);
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    super.writeAscii(stream);
    stream.writeChars(m_linkType + " ");
    stream.writeChars(m_viewList.size() + "\n");
    for (DListIterator d = m_viewList.begin(); d.hasMoreElements(); ) {
      DEViseVisualLinkEntry entry = (DEViseVisualLinkEntry)d.nextElement();
      stream.writeChars(entry.m_view.getName() + " " + entry.m_attr + "\n");
    }
  }
  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);
    m_linkType = stream.readInt();
    int numViews = stream.readInt();
    
    DEViseSession session = (DEViseSession)m_serializeInfo;
    if (session == null) {
      throw new IOException("Session object not available");
    }
    for (int i = 0; i < numViews; i++) {
      String viewName = stream.readUTF();
      int attr = stream.readInt();
      DEViseView view = (DEViseView)session.myViews().find(viewName);
      if (view == null) {
        throw new IOException("Cannot find view: " + viewName);
      }
      addView(view, attr);
    }
  }
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("linkType = " + m_linkType + "\n");
    for (DListIterator d = m_viewList.begin(); d.hasMoreElements(); ) {
      buf.append(d.nextElement());
    }
    return buf.toString();
  }
  // add all views from link to this link 
  // (effectively maintaining an equivalence relationship between 
  // views in a link).
  public void expand(DEViseVisualLink link) {
    if (m_linkType != link.m_linkType) {
      System.out.println("Cannot combine link types!!");
    }
    while(link.m_viewList.size() > 0) {
      DEViseVisualLinkEntry entry 
        = (DEViseVisualLinkEntry)link.m_viewList.popFront();
      System.out.println("Adding view " + entry.m_view.getName() + " to " + 
                         link.getName());
      m_viewList.add(entry);
      entry.m_view.addVisualLink(this);
      entry.m_view.removeVisualLink(link);
    }
  }
  public void selfDestruct() {
    super.selfDestruct();
    while(m_viewList.size() > 0) {
      DEViseVisualLinkEntry e  = (DEViseVisualLinkEntry)m_viewList.popFront();
      e.m_view.removeVisualLink(this);
    }
  }
}

class DEViseVisualLinkEntry {
  public DEViseView m_view;
  public int m_attr;
  public DEViseVisualLinkEntry(DEViseView view, int attr) {
    m_view = view;
    m_attr = attr;
  }

  public String toString() {
    return m_view.getName() + " : " + m_attr + "\n";
  }
}
