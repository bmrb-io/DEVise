/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseRecordLinkPeer.java
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
 * The <CODE>DEViseRecordLink</CODE> class
 *
 * @author  Shilpa Lawande
 */

// A class that encapsulates all the code for drawing record links
// in various modes (such as normal, thicker to indicate particular links
// with extra information like names etc....

public class DEViseRecordLinkPeer implements Selectable
{
  public static final int RECORD_LINK_POINT = 7;

  public static Color RLINK_COLORS[] = new Color[2];
  public static double RLINK_THICKNESS[] = new double[2];
  public static double RLINK_THETA[] = new double[2];
  public static int OVAL_RADIUS[] = new int[2];
  public static int RLINK_SIDE[] = new int[2];
  public static boolean ARROW_FILL[] = new boolean[2];

  static {
    RLINK_COLORS[UNSELECTED] =  Color.cyan;
    RLINK_COLORS[SELECTED] = Color.magenta;
    RLINK_THICKNESS[UNSELECTED] = 1;
    RLINK_THICKNESS[SELECTED] = 2;
    OVAL_RADIUS[UNSELECTED] = 3;
    OVAL_RADIUS[SELECTED] = 5;
    RLINK_THETA[UNSELECTED] = Math.PI/12;
    RLINK_THETA[SELECTED] = Math.PI/6;
    ARROW_FILL[UNSELECTED] = false;
    ARROW_FILL[SELECTED] = true;
  };

  private DEViseRecordLink m_link;
  private DEViseSessionPeer m_session;
  private int m_state;

  /* Constructor */
  public DEViseRecordLinkPeer(DEViseSessionPeer session, 
                              DEViseRecordLink link) {
    m_link = link;
    m_session = session;
    m_state = UNSELECTED;
  }

  public DEViseRecordLink getDEViseRecordLink() { return m_link; }

  public void paint(Graphics g) {
    DEViseView master = m_link.getMaster();
    if (master == null) {
      return;
    }
    for (DListIterator d = m_link.slaves(); d.hasMoreElements(); ) {
      DEViseView slave = (DEViseView)d.nextElement();
      draw(g, master, slave);
    }
  } 

  public void draw(Graphics g, DEViseView master, DEViseView slave) {
    DEViseViewPeer peer1 = m_session.findViewPeerByName(master.getName());
    DEViseViewPeer peer2 = m_session.findViewPeerByName(slave.getName());

    Rectangle bounds1 = peer1.getBounds();
    Rectangle bounds2 = peer2.getBounds();
    int x1, y1, x2, y2;

    boolean isPiled = false;
    DEViseViewPeer parent1 
      = m_session.findViewPeerByName(master.getParent().getName());
    if (parent1.isPiled()) {
      isPiled = true;
      x1 = bounds1.x + bounds1.width - RECORD_LINK_POINT;
      y1 = bounds1.y + RECORD_LINK_POINT;
    } else {
      x1 = bounds1.x + bounds1.width / 2 + RECORD_LINK_POINT;
      y1 = bounds1.y + bounds1.height / 2 + RECORD_LINK_POINT;
    }
    DEViseViewPeer parent2 
      = m_session.findViewPeerByName(slave.getParent().getName());
    if (parent2.isPiled()) {
      isPiled = true; 
      x2 = bounds2.x + bounds2.width - RECORD_LINK_POINT;
      y2 = bounds2.y + RECORD_LINK_POINT;
    } else {
      x2 = bounds2.x + bounds2.width / 2 + RECORD_LINK_POINT;
      y2 = bounds2.y + bounds2.height / 2 + RECORD_LINK_POINT;
    }
    g.setColor(RLINK_COLORS[m_state]);
    if (isPiled && (parent1 == parent2)) {
      MoreGraphics.drawLine(g, x1, y1, x2, y1,
                            RLINK_THICKNESS[m_state]);
      MoreGraphics.drawLine(g, x2, y1, x2, y2, 
                            RLINK_THICKNESS[m_state]);
      MoreGraphics.drawArrow(g, x2, y1, x2, y2, 10, RLINK_THETA[m_state], 
                             ARROW_FILL[m_state]);
    } else {
      MoreGraphics.drawLine(g, x1, y1, x2, y2, 
                            RLINK_THICKNESS[m_state]);
      MoreGraphics.drawArrow(g, x1, y1, x2, y2, 
                             10, RLINK_THETA[m_state],
                             ARROW_FILL[m_state]);
    }
    g.fillOval(x1- OVAL_RADIUS[m_state], 
               y1- OVAL_RADIUS[m_state], 
               2 * OVAL_RADIUS[m_state], 
               2 * OVAL_RADIUS[m_state]);
  }

  public void setState(int stateToSet) {
    m_state = stateToSet;
  }
  public int getState() {
    return m_state;
  }
  public void select() {
    setState(Selectable.SELECTED);
    m_session.repaint();
  }
  public void unselect() {
    setState(Selectable.UNSELECTED);
    m_session.repaint();
  }
}
