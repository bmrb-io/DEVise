/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Edge.java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.awt.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>Edge</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class Edge implements Selectable
{ 
  /* constants */
  public static final int CONST_X = 0;
  public static final int CONST_Y = 1;
  
  private int m_state;
  /* constants - Colors */
  public static final Color EdgeColors[] = new Color[2];
  static {
    EdgeColors[UNSELECTED] = Color.black;
    EdgeColors[SELECTED] = Color.red;
  }
  
  /* Public members */
  public Node node[] = new Node[2];
  private int m_dir;
  private Box m_container;
  
  RivetBar m_rivet;   // null if free edge, else the RivetBar which controls
                      // this edge. Note that an edge can have only one 
                      // RivetBar
  /* Constructor */
  Edge(Node r1, Node r2) {
    node[0] = r1;
    node[1] = r2;
    if (r1.x == r2.x) {
      m_dir = CONST_X;
    } else if (r1.y == r2.y) {
      m_dir = CONST_Y;
    } else {
      System.out.println("Error in Edge()");
      return;
    }
    m_state = Selectable.UNSELECTED;
    m_container = null;
  }
  
  /* Public methods */
  public int getDir() {
    return m_dir;
  }
  public RivetBar getRivet() { return m_rivet; }
  public void setRivet(RivetBar rb) { m_rivet = rb; }
  public void setContainer(Box container) {
    m_container = container;
  }
  public Box getContainer() {
    return m_container;
  }
  public boolean coincides(Edge b) {
    return (node[0].coincides(b.node[0]) && node[1].coincides(b.node[1]));
  }
  boolean resizeHandle(int x, int y) {
   int midX = ( node[0].x  + node[1].x ) / 2;
   int midY = ( node[0].y  + node[1].y ) / 2;
  
   Rectangle midR = new Rectangle(midX - 3, midY - 3, 6, 6);
   return midR.contains(x, y);
  }

  boolean parallel(Edge b) {
    return (m_dir == b.m_dir);
  }
  
  // aligning a list of edges makes them colinear
  public boolean align(DList edgelist) {
    for (DListIterator d = edgelist.begin();
         d.hasMoreElements(); ) {
      Edge b = (Edge)d.nextElement();
      if (!align(b)) {
        return false;
      }
    }
    return true;
  }
  
  // align one edge to this one
  public boolean align(Edge b) {
    if (m_dir != b.m_dir) {  // not parallel
      return false;
    }
    int dx = 0;
    int dy = 0;
    if (m_dir == CONST_Y) {   // y coordinate constant
      dy = node[0].y - b.node[0].y;
    } else {              // x coordinate constant
      dx = node[0].x - b.node[0].x;
    }
    b.m_container.moveParallelBy(dx, dy);
    return true;
  }


  // general move function
  private void translate(int dx, int dy) {
    //System.out.println("translate " + dx + ", " + dy);
    node[0].translate(dx, dy);
    node[1].translate(dx, dy);
  }
  
  // move to a position parallel to itself  
  // one of dx and dy must be zero
  public void moveParallelBy(int dx, int dy) {
    if (((m_dir == CONST_Y) && (dx != 0)) ||
        ((m_dir == CONST_X) && (dy != 0))) {
      System.out.println("moveBy Error...");
      return;
    }
    translate(dx, dy);
  }
  protected Point getDelta(int toX, int toY) {
    int dx = 0;
    int dy = 0;
    if (m_dir == CONST_Y) {
      dy = toY - node[0].y; 
    } else {
      dx = toX - node[0].x;
    }
    return new Point(dx, dy);  
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
    paint(getGraphics()); 
  }
  public void unselect() { 
    setState(UNSELECTED); 
    paint(getGraphics()); 
  }

  public boolean isSelected() {
    return (m_state == Selectable.SELECTED);
  }
  public Graphics getGraphics() {
    return m_container.getGraphics();
  }
  public void paint(Graphics g, Color clr) {
    Color oldClr = g.getColor();
    g.setColor(clr);
    g.drawLine(node[0].x, node[0].y, node[1].x, node[1].y); 
    g.setColor(oldClr);
  }
  public void paint(Graphics g) {
    paint(g, EdgeColors[m_state]);
  }
}
