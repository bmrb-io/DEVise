/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseCursorPeer.java
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
 * The <CODE>DEViseCursor</CODE> class
 *
 * @author  Shilpa Lawande
 */

// A class that encapsulates all the code for drawing cursors
// in various modes (such as normal, thicker to indicate particular links
// with extra information like names etc....

public class DEViseCursorPeer implements Selectable
{
  public static final int CURSOR_POINT = 7;

  
  public static Color CURSOR_COLORS[] = new Color[2];
  public static Color CURSOR_COLORS_DST[] = new Color[2];
  public static double CURSOR_THICKNESS[] = new double[2];
  public static int OVAL_RADIUS[] = new int[2];
  public static int CURSOR_SIDE[] = new int[2];

  static {
    CURSOR_COLORS[UNSELECTED] =  Color.orange;
    CURSOR_COLORS[SELECTED] = Color.red;
    CURSOR_COLORS_DST[UNSELECTED] = Color.green;
    CURSOR_COLORS_DST[SELECTED] = Color.blue;
    CURSOR_THICKNESS[UNSELECTED] = 1;
    CURSOR_THICKNESS[SELECTED] = 2;
    OVAL_RADIUS[UNSELECTED] = 3;
    OVAL_RADIUS[SELECTED] = 5;
    CURSOR_SIDE[UNSELECTED] = 3;
    CURSOR_SIDE[SELECTED] = 5;
  };

  private DEViseCursor m_cursor;
  private DEViseSessionPeer m_session;
  private int m_state;

  /* Constructor */
  public DEViseCursorPeer(DEViseSessionPeer session, DEViseCursor cursor) {
    m_cursor = cursor;
    m_session = session;
    m_state = Selectable.UNSELECTED;
  }

  public DEViseCursor getDEViseCursor() { return m_cursor; }

  public void paint(Graphics g) {
    DEViseView src = m_cursor.getSrc();
    DEViseView dst = m_cursor.getDst();
    if ((src != null) && (dst != null)) {
      draw(g, src, dst);
    }
  } 

  public void draw(Graphics g, DEViseView src, DEViseView dst) {
    DEViseViewPeer peer1 = m_session.findViewPeerByName(src.getName());
    DEViseViewPeer peer2 = m_session.findViewPeerByName(dst.getName());

    Rectangle bounds1 = peer1.getBounds();
    Rectangle bounds2 = peer2.getBounds();

    int x1, y1, x2, y2;
    boolean isPiled = false;
    DEViseViewPeer parent1 = m_session.findViewPeerByName(src.getParent().getName());
    if (parent1.isPiled()) {
      isPiled = true;
      x1 = bounds1.x + bounds1.width - CURSOR_POINT;
      y1 = bounds1.y + bounds1.height / 2 - CURSOR_POINT;
    } else {
      x1 = bounds1.x + bounds1.width / 2;
      y1 = bounds1.y + bounds1.height / 2;
    }
    DEViseViewPeer parent2 = m_session.findViewPeerByName(dst.getParent().getName());
    if (parent2.isPiled()) {
      isPiled = true;
      x2 = bounds2.x + bounds2.width - CURSOR_POINT;
      y2 = bounds2.y + bounds2.height / 2 - CURSOR_POINT;
    } else {
      x2 = bounds2.x + bounds2.width / 2;
      y2 = bounds2.y + bounds2.height / 2;
    }
    
    g.setColor(CURSOR_COLORS[m_state]);
    if (isPiled && (parent1 == parent2)) {
      MoreGraphics.drawLine(g, x1, y1, x2, y1, CURSOR_THICKNESS[m_state]);
      MoreGraphics.drawLine(g, x2, y1, x2, y2, CURSOR_THICKNESS[m_state]);
    } else {
      MoreGraphics.drawLine(g, x1, y1, x2, y2, CURSOR_THICKNESS[m_state]);
    }
    g.fillOval(x1-OVAL_RADIUS[m_state], 
               y1-OVAL_RADIUS[m_state], 
               2 * OVAL_RADIUS[m_state], 
               2 * OVAL_RADIUS[m_state]);
    g.setColor(CURSOR_COLORS_DST[m_state]);
    g.fillRect(x2 - CURSOR_SIDE[m_state], 
               y2 - CURSOR_SIDE[m_state], 
               2 * CURSOR_SIDE[m_state],
               2 * CURSOR_SIDE[m_state]);
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
