/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Node.java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>Node</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class Node extends Point implements Selectable
{
  /* constants */
  private static final Color NodeColors[] = new Color[2];
  
  static {
    NodeColors[UNSELECTED] = Color.white;
    NodeColors[SELECTED] = Color.red;
  }

  public static void setNodeColor(int index, Color clr) {
    NodeColors[index] = clr;
  }
  int m_state;

  /* Constructor */
  Node(int x1, int y1) {
    super(x1,y1);
    m_state = UNSELECTED;
  }

  /* Public methods */
  boolean coincides(Object p) {
    return (p instanceof Node) && (((Node)p).x == x) && (((Node)p).y == y);
  }
  
  public void moveTo(int toX, int toY) {
    x = toX; y = toY;
  }
  public static void paint(int x, int y, Graphics g) {
    Color oldClr = g.getColor();
    //g.setColor(Color.black);
    g.setColor(Color.black);
    g.drawRect(x - 3, y - 3, 6, 6);
    g.setColor(NodeColors[UNSELECTED]);
    g.fillRect(x - 2, y - 2, 4, 4);
    g.setColor(oldClr);
  }

  public void paint(Graphics g) {
    Color oldClr = g.getColor();
    g.setColor(Color.black);
    g.drawRect(x - 3, y - 3, 6, 6);
    g.setColor(NodeColors[m_state]);
    g.fillRect(x - 2, y - 2, 4, 4);
    g.setColor(oldClr);
  }

  // Selectable interface
  public void setState(int stateToSet) {
    m_state = stateToSet;
  }
  public int getState() {
    return m_state;
  }
  public void select() { 
    setState(SELECTED); 
  }
  public void unselect() { 
    setState(UNSELECTED); 
  }

  public boolean isSelected() {
    return (m_state == Selectable.SELECTED);
  }
}
