/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Rubberband.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;

// Class
/**
 * The <CODE>Rubberband</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class Rubberband 
{
  protected Point m_anchor;  // fixed point
  protected Point m_free;    // freely moving point
  protected Point m_lastfree; // last position of free point
  
  boolean m_started;         // Rubber band being drawn
  Component m_parent;
  Color m_rbndClr = Color.black;

  /* Public members */
  public Rubberband(Component parent) {
    m_parent = parent;
    m_anchor = new Point(0,0);
    m_free = new Point(0,0);
    m_lastfree = new Point(0,0);
  }
  public void start(int x, int y, Color clr) {
    m_rbndClr = clr;
    m_anchor.x = x; m_anchor.y = y;
    m_free.x = x; m_free.y = y;
    m_lastfree.x = x; m_lastfree.y = y; 
    m_started = true;
    m_parent.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
    draw();
  }
  public boolean started() {
    return m_started;
  }
  public void stretch(int x, int y) {
    if (!m_started) {
      return;
    }
    m_lastfree.x = m_free.x;
    m_lastfree.y = m_free.y;
    m_free.x = x;
    m_free.y = y;
    draw();
  }
  public void reset() {
    erase();
    m_anchor.x = m_anchor.y = 0;
    m_lastfree.x = m_lastfree.y = 0;
    m_free.x = m_free.y = 0;
    m_started = false;
    m_parent.setCursor(Cursor.getDefaultCursor());
  }

  public Rectangle getBounds() {
    return new Rectangle(m_anchor.x, m_anchor.y,
                         m_lastfree.x - m_anchor.x, m_lastfree.y - m_anchor.y);
  }
  public void erase() {
    Graphics g = m_parent.getGraphics();
	  g.setColor(m_rbndClr);
    g.setXORMode(m_parent.getBackground());
    g.drawRect(m_anchor.x, m_anchor.y,
               m_free.x - m_anchor.x, m_free.y - m_anchor.y);
    g.dispose();
  }
  public void draw() {
    if (m_started) {
      Graphics g = m_parent.getGraphics();
      Color oldClr = g.getColor();
	    g.setColor(m_rbndClr);
      g.setXORMode(m_parent.getBackground());
      g.drawRect(m_anchor.x, m_anchor.y,
                 m_lastfree.x - m_anchor.x, m_lastfree.y - m_anchor.y);
	    g.drawRect(m_anchor.x, m_anchor.y,
	               m_free.x - m_anchor.x, m_free.y - m_anchor.y);
      g.setPaintMode();
      g.setColor(oldClr);
    }
  }
}
