/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseVisualLinkPeer.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import LayoutMgr.MetaData.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>DEViseVisualLink</CODE> class
 *
 *
 author  Shilpa Lawande
 */

// A class that encapsulates all the code for drawing visual links
// in various modes (such as normal, thicker to indicate particular links
// with extra information like names etc....

public class DEViseVisualLinkPeer
{
  public static final int VISUAL_LINK_POINT = 7;
  public static int OVAL_RADIUS[] = new int[2];
  public static double VLINK_THICKNESS[] = new double[2];
  
  public static Color VLINK_COLORS[] = new Color[2];
  static {
    OVAL_RADIUS[Selectable.UNSELECTED] = 3;
    OVAL_RADIUS[Selectable.SELECTED] = 5;
    VLINK_COLORS[Selectable.UNSELECTED] =  Color.yellow;
    VLINK_COLORS[Selectable.SELECTED] = Color.pink;
    VLINK_THICKNESS[Selectable.UNSELECTED] = 1;
    VLINK_THICKNESS[Selectable.SELECTED] = 2;
  };

  // state of X-X, Y-Y links.
  private static int m_state[] = new int[] {   
    Selectable.UNSELECTED, Selectable.UNSELECTED, Selectable.UNSELECTED
  };

  private DEViseVisualLink m_link;
  private DEViseSessionPeer m_session;

  /* Constructor */
  public DEViseVisualLinkPeer(DEViseSessionPeer session, 
                              DEViseVisualLink link) {
    m_link = link;
    m_session = session;
  }

  public DEViseVisualLink getDEViseVisualLink() { return m_link; }

  public void paint(Graphics g) {
    System.out.println("Visual link peer paint - " + m_link.getName());
    for (DListIterator d1 = m_link.viewList(); d1.hasMoreElements(); ) {
      DEViseView view1 = m_link.getView(d1.nextElement());
      for (DListIterator d2 = m_link.viewList(); d2.hasMoreElements(); ) {
        DEViseView view2 = m_link.getView(d2.nextElement());
        if (d2.index() <= d1.index()) {
          continue;
        }
        draw(g, view1, view2);
      }
    }
  } 

  private void draw(Graphics g, DEViseView view1, DEViseView view2) {
    System.out.println("draw " + view1.getName() + " " + view2.getName());
    DEViseViewPeer peer1 = m_session.findViewPeerByName(view1.getName());
    DEViseViewPeer peer2 = m_session.findViewPeerByName(view2.getName());
    
    Rectangle bounds1 = peer1.getBounds();
    Rectangle bounds2 = peer2.getBounds();
    System.out.println("Bounds1 = " + bounds1);
    System.out.println("Bounds2 = " + bounds2);
    int x1, y1, x2, y2;
    
    boolean isPiled = false;
    DEViseViewPeer parent1 
      = m_session.findViewPeerByName(view1.getParent().getName());
    if (parent1.isPiled()) {
      isPiled = true;
      x1 = bounds1.x + bounds1.width - VISUAL_LINK_POINT;
      y1 = bounds1.y + bounds1.height - VISUAL_LINK_POINT;
    } else {
      x1 = bounds1.x + bounds1.width / 2 - VISUAL_LINK_POINT;
      y1 = bounds1.y + bounds1.height / 2 - VISUAL_LINK_POINT;
    }
    DEViseViewPeer parent2 
      = m_session.findViewPeerByName(view2.getParent().getName());
    if (parent2.isPiled()) {
      isPiled = true;
      x2 = bounds2.x + bounds2.width - VISUAL_LINK_POINT;
      y2 = bounds2.y + bounds2.height - VISUAL_LINK_POINT;
    } else {
      x2 = bounds2.x + bounds2.width / 2 - VISUAL_LINK_POINT;
      y2 = bounds2.y + bounds2.height / 2 - VISUAL_LINK_POINT;
    }
    int linkType = m_link.getLinkType();
    g.setColor(VLINK_COLORS[m_state[linkType]]);

    if (isPiled && (parent1 == parent2)) {
      MoreGraphics.drawLine(g, x1, y1, x2, y1, 
                            VLINK_THICKNESS[m_state[linkType]]);
      MoreGraphics.drawLine(g, x2, y1, x2, y2, 
                            VLINK_THICKNESS[m_state[linkType]]);
    } else {
      MoreGraphics.drawLine(g, x1, y1, x2, y2, 
                            VLINK_THICKNESS[m_state[linkType]]);
    }
    g.fillOval(x1-OVAL_RADIUS[m_state[linkType]],
               y1-OVAL_RADIUS[m_state[linkType]], 
               2 * OVAL_RADIUS[m_state[linkType]],
               2 * OVAL_RADIUS[m_state[linkType]]);
    g.fillOval(x2-OVAL_RADIUS[m_state[linkType]],
               y2-OVAL_RADIUS[m_state[linkType]], 
               2 * OVAL_RADIUS[m_state[linkType]],
               2 * OVAL_RADIUS[m_state[linkType]]);
  }

  public static void select(int linkType) {
    m_state[linkType] = Selectable.SELECTED;
  }
  public static void unselect(int linkType) {
    m_state[linkType] = Selectable.UNSELECTED;
  }
}
