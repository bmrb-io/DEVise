/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseView.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.util.*;
import java.io.*;
import COM.objectspace.jgl.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseView</CODE> class implements a view or a chart in DEVise
 * framework. A view has attributes such as x, y, width, height,
 * foreground and background color.
 * We implement a fully hierarchical structure of views i.e. a view 
 * can have child views. We maintain the relative position of the child
 * views with respect to the parent and the height and width as ratios
 * w.r.t. parent dimensions.  
 * 
 * A view also maintains a list of all its visual links, record links
 * and cursors. A view has a mapping and a tdata. It is necessary that
 * the schemas of the mapping and tdata associated with a view are
 * compatible (for now that means the same object).
 *
 * @author  Shilpa Lawande
 * @see     DEViseObj
 * @see     DEViseType
 * @see     DEViseVisualLink
 * @see     DEViseRecordLink
 * @see     DEViseCursor
 * @see     DEViseMapping
 * @see     DEViseTData
 * @see     DEViseSchema
 */

public class DEViseView extends DEViseObj 
{
  /* Private members */
  private double m_x;                          // relative x position
  private double m_y;                          // relative y position
  private double m_wRatio;                     // width as ratio to parent
  private double m_hRatio;                     // height as ratio to parent
  private DEViseColor m_fg;                   // foreground color
  private DEViseColor m_bg;                   // background color
  
  private boolean m_transparent;             
  DList m_visualLinks;                        // visual links for this view
  DList m_recordLinks;                        // record links for this view
  DList m_cursors;                            // cursors for this view

  /* Ascii variables */
  private DEViseView m_parent;                // parent of this DEViseView
  private DList m_children;                   // child views of this view

  private DEViseMapping m_mapping;            // mapping for this view
  private DEViseTData m_tdata;                // tdata for this view
  
  transient private boolean m_recursiveDelete; // view should recursively
                                             // delete children instead
                                             // of handling them to its
                                             // parent
  /* Constructor */
  public DEViseView() {
    this("",null);
  }
  public DEViseView(String name, DEViseView parent,
                    double x, double y, double wRatio, double hRatio,
                    DEViseColor fg, DEViseColor bg) 
  {
    super(name, DEViseType.ViewType);
    m_x = x;
    m_y = y;
    m_wRatio = wRatio;
    m_hRatio = hRatio;
    m_fg = fg;
    m_bg = bg;
    m_children = new DList();
    setParent(parent);
    m_visualLinks = new DList();
    m_recordLinks = new DList();
    m_cursors = new DList();
    m_recursiveDelete = false;
  }
  public DEViseView(String name, DEViseView parent) {
    this(name,parent, 0, 0, 0.5, 0.5, DEViseColor.black, DEViseColor.white);
  }
  
  // if view can be deleted 
  public boolean canBeDestroyed() {
    // check if has children
    if (m_children.size() > 0) {
      return false; 
    }
    if (m_visualLinks.size() > 0) {
      return false;
    }
    if (m_recordLinks.size() > 0) {
      return false;
    }
    if (m_cursors.size() > 0) {
      return false;
    }
    return true;
  }
  
  public void doRecursiveDelete(boolean flag) {
    m_recursiveDelete = flag;
  }
  /* IMPORTANT method to free the object from all its references , this
   * is called when the view is destroyed so that java * will GC it 
   */
  public void selfDestruct() {
    System.out.println("selfDestruct : view " + getName());
    // Since deletion messes up the iterators, we dont use iterators
    // but use popfront instead
    
    if (m_recursiveDelete) {
      m_children.clear();
      m_parent = null;  // dont call unsetParent here as it may mess
                        // up the parent's iterators 
    } else {
	    /* hand over ones children to parent if there is one, so as to maintain a 
	     * semblance of structure !! 
	     * note that if children were supposed to be destroyed also, the calling
	     * function should do a recursive destroy, first deleting all chilren and
	     * then calling destroy on the view. This gives more flexibility.
	     */
	    while(m_children.size() > 0) {
	      DEViseView child = (DEViseView)m_children.popFront();
	      // since one level of views will be gone the child's reference
	      // view for position and dimension changes so as to maintain
	      // the same absolute position and size
	      child.adjustBy(m_x, m_y, m_wRatio, m_hRatio);
	      child.setParent(m_parent); // internally deletes child from the view
	    }
	    // disassociate oneself from parent 
	    unsetParent();
    }
	  // disassociate from any other referencing structures 
    while(m_visualLinks.size() > 0) {
      DEViseVisualLink link = (DEViseVisualLink)m_visualLinks.popFront();
      link.removeView(this);
    }
    while(m_recordLinks.size() > 0) {
      DEViseRecordLink link = (DEViseRecordLink)m_recordLinks.popFront();
      if (link.isMaster(this)) {  
        link.unsetMaster();
      } else {
        link.removeSlave(this);
      }
    }
    while(m_cursors.size() > 0) {
      DEViseCursor cursor = (DEViseCursor)m_cursors.popFront();
      if (cursor.getSrc() == this) {
        cursor.unsetSrc();
      }
      if (cursor.getDst() == this) {
        cursor.unsetDst();
      }
    }
  }
  
  /* Getters and Setters */
  public double getX() { return m_x; }
  public void setX(double x) { m_x = x; }

  public double getY() { return m_y; }
  public void setY(double y) { m_y = y; }

  public double getWidthRatio() { return m_wRatio; } 
  public void setWidthRatio(double wRatio) { m_wRatio = wRatio; }

  public double getHeightRatio() { return m_hRatio; } 
  public void setHeightRatio(double hRatio) { m_hRatio = hRatio; } 

  public DEViseColor getBgColor() {
    return m_bg;
  }
  public DEViseColor getFgColor() {
    return m_fg;
  }
  public void setBgColor(DEViseColor bg) {
    m_bg = bg;
  }
  public void setFgColor(DEViseColor fg) {
    m_fg = fg;
  }
  public double getAbsX() {
    if (m_parent == null) {
      return m_x;
    } else {
      return m_x + m_parent.getAbsX();
    }
  }
  public double getAbsY() {
    if (m_parent == null) {
      return m_y;
    } else {
      return m_y + m_parent.getAbsY();
    }
  }
  public double getWidth() {
    if (m_parent == null) {
      return m_wRatio;
    } else {
      return m_wRatio * m_parent.getWidth();
    }
  }
  public double getHeight() {
    if (m_parent == null) {
      return m_hRatio;
    } else {
      return m_hRatio * m_parent.getHeight();
    }
  }
  public void setWidth(double width) {
    if (m_parent == null) {
      m_wRatio = width;
    } else {
      double parentWidth = m_parent.getWidth();
      m_wRatio = width / parentWidth;
    }
  }
  public void setHeight(double height) {
    if (m_parent == null) {
      m_hRatio = height;
    } else {
      double parentHeight = m_parent.getHeight();
      m_hRatio = height / parentHeight;
    }
  }
  public void setTransparent(boolean isTransparent) {
    m_transparent = isTransparent;
  }
  // adjust position by xBy, yBy
  // multiply dimensions by wRatioBy, hRatioBy
  // useful for move/resize
  public void adjustBy(double xBy, double yBy, double wRatioBy, double hRatioBy)
  {
    m_x += xBy;
    m_y += yBy;
    m_wRatio *= wRatioBy;
    m_hRatio *= hRatioBy;
  }
  public void adjustTo(double xTo, double yTo, double wRatioTo, double hRatioTo)
  {
    m_x = xTo;
    m_y = yTo;
    m_wRatio = wRatioTo;
    m_hRatio = hRatioTo;
  }
  public DEViseView getParent() { return m_parent;}
  public void setParent(DEViseView parent) {
    unsetParent();
    if (parent != null) {
      parent.addChild(this); 
      m_parent = parent;
    }
  }
  public void unsetParent() {
    if (m_parent != null) {
      m_parent.delChild(this);
      m_parent = null;
    }
  }
  public void addChild(DEViseView child) {
    if (!m_children.contains(child)) {
      m_children.add(child); 
    }
  }

  public boolean delChild(DEViseView child) {
    if (m_children.remove(child) > 0 ) {
      return true;
    }
    return false;
  }

  public int numChildren() {
    return m_children.size();
  }
  
  public DListIterator childList() {
    return m_children.begin();
  }
  
  /* print function */
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("Position: " + m_x + " " + m_y + "\n");
    buf.append("Dimensions: " + getWidth() + " " + getHeight()+ "\n");
    buf.append("Color: " + "Fg:" + m_fg + " Bg:" + m_bg + "\n");
    if (m_parent != null) {
      buf.append("Parent: " + m_parent.getName() + "\n");
    } else {
      buf.append("Parent: none\n");
    }
    buf.append("Children:\n");
    for (DListIterator children = m_children.begin();
         children.hasMoreElements(); ) {
      buf.append("\t" + ((DEViseView)children.nextElement()).getName() + "\n");
    }
    buf.append("Visual Links:\n");
    for (DListIterator d = m_visualLinks.begin();
         d.hasMoreElements();) {
      buf.append("\t" + ((DEViseVisualLink)d.nextElement()).getName() + "\n");
    }
    buf.append("Record Links:\n");
    for (DListIterator d = m_recordLinks.begin();
         d.hasMoreElements();) {
      DEViseRecordLink link = (DEViseRecordLink)d.nextElement();
      buf.append("\t" + link.getName() + "(" + link.isMaster(this) +")\n");
    }
    return buf.toString();
  }

  public void exchange(DEViseView child1, int child1Index, 
                       DEViseView child2, int child2Index) {
    if ((child1Index != -1) && (child1Index != -1)) {
      m_children.put(child1Index, child2);
      m_children.put(child2Index, child1);
    }
  }
  
  // first -> last, second -> first
  public void flipBackward() {
    DEViseView front = (DEViseView)m_children.popFront();
    m_children.pushBack(front);
  }
  
  // first -> second, last ->first
  public void flipForward() {
    DEViseView back = (DEViseView)m_children.popBack();
    m_children.pushFront(back);
  }
  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);
    m_x = stream.readDouble();
    m_y = stream.readDouble();
    m_wRatio = stream.readDouble();
    m_hRatio = stream.readDouble();
    m_fg = new DEViseColor();
    m_fg.readAscii(stream);
    m_bg = new DEViseColor();
    m_bg.readAscii(stream);
    m_parent = null;

    DEViseSession session = (DEViseSession)m_serializeInfo;
    if (session == null) {
      throw new IOException("Session object not available");
    }
    String parentName = stream.readUTF();
    DEViseView parent;
    if (parentName.equals("null")) {
      parent = null;
    } else {
      parent = (DEViseView)session.myViews().find(parentName); 
      if (parent == null) {
        throw new IOException("Cannot find view:" + parentName);
      }
    }
    setParent(parent);
    String tdataName = stream.readUTF();
    if (tdataName.equals("null")) {
      m_tdata = null;
    } else {
      m_tdata = (DEViseTData)session.myTDatas().find(tdataName);
      if (m_tdata == null) {
        throw new IOException("Cannot find tdata: " + tdataName);
      }
    }
    String mappingName = stream.readUTF();
    if (mappingName.equals("null")) {
      m_mapping = null;
    } else {
      m_mapping = (DEViseMapping)session.myMappings().find(mappingName);
      if (m_mapping == null) {
        throw new IOException("Cannot find mapping:" + mappingName);
      }
    }
    System.out.println("Created view :" + this);
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    super.writeAscii(stream);
    stream.writeChars(m_x + " ");
    stream.writeChars(m_y + " ");
    stream.writeChars(m_wRatio + " ");
    stream.writeChars(m_hRatio + " ");
    m_fg.writeAscii(stream);
    stream.writeChars(" ");
    m_bg.writeAscii(stream);
    if (m_parent != null) {
      stream.writeChars(m_parent.getName() + " ");
    } else {
      stream.writeChars("null ");
    }
    if (m_tdata != null) {
      stream.writeChars(m_tdata.getName() + " ");
    } else {
      stream.writeChars("null ");
    }
    if (m_mapping != null) {
      stream.writeChars(m_mapping.getName() + " ");
    } else {
      stream.writeChars("null ");
    }
    stream.writeChars("\n");
  }

  public void addVisualLink(DEViseVisualLink link) {
    m_visualLinks.add(link);
  }
  public void removeVisualLink(DEViseVisualLink link) {
    m_visualLinks.remove(link);
  }
  public DEViseVisualLink findVisualLink(int type) {
    for (DListIterator d = m_visualLinks.begin(); d.hasMoreElements(); ) {
      DEViseVisualLink link = (DEViseVisualLink)d.nextElement();
      if (link.getLinkType() == type) {
        return link;
      }
    }
    return null;
  }
  public void addRecordLink(DEViseRecordLink link) {
    m_recordLinks.add(link);
  }
  public void removeRecordLink(DEViseRecordLink link) {
    m_recordLinks.remove(link);
  }
  public void addCursor(DEViseCursor cursor) {
    m_cursors.add(cursor);
  }
  public void removeCursor(DEViseCursor cursor) {
    m_cursors.remove(cursor);
  }
  public DEViseTData getTData() { return m_tdata; }
  public DEViseMapping getMapping() { return m_mapping; }
  public boolean setTData(DEViseTData tdata) {
    if (m_mapping != null) {
      if (m_mapping.getSchema() != tdata.getSchema()) {
        return false;
      }
    }
    m_tdata = tdata;
    return true;
  }
  public boolean setMapping(DEViseMapping mapping) {
    if (m_tdata != null) {
      if (m_tdata.getSchema() != mapping.getSchema()) {
        return false;
      }
    }
    m_mapping = mapping;
    return true;
  }
  public void unsetTData() {
    m_tdata = null;
  }
  public void unsetMapping() {
    m_mapping = null;
  }
}
