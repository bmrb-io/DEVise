/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class RivetBar.java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.awt.*;
import AwtUtil.*;
import COM.objectspace.jgl.*;


// Class
/**
 * The <CODE>RivetBar</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class RivetBar implements Selectable
{
  public static Color RivetColors[] = new Color[2];
  static {
    RivetColors[UNSELECTED] = Color.white;
    RivetColors[SELECTED] = Color.red;
  }
  private HashSet m_edges;   // edges rivetted by this bar
  private Rectangle m_bounds; // rectangle that represents the rivet 
  private Component m_container; 
  private int m_state;     // 
  private int m_dir;

  public RivetBar(DList edgeList) {
    m_edges = new HashSet();
    for (DListIterator d = edgeList.begin(); d.hasMoreElements();) {
      Edge b = (Edge)d.nextElement();
      m_edges.add(b);
      b.setRivet(this); 
    }
    m_bounds = getBoundingBox(m_edges);
    Edge b0 = (Edge)edgeList.at(0);
    m_dir = b0.getDir();
    m_state = UNSELECTED;
  }
  public static Rectangle getBoundingBox(HashSet edgeSet) {
    if (edgeSet.size() <= 0) {
      return null;
    }
    int xMin = 99999;
    int xMax = -1;
    int yMin = 99999;
    int yMax = -1;
    for (HashSetIterator d = edgeSet.begin(); d.hasMoreElements(); ) {
      Edge b = (Edge)d.nextElement();
      if (b.node[0].x < xMin) {
        xMin = b.node[0].x;
      } 
      if (b.node[0].y < yMin) {
        yMin = b.node[0].y;
      }
      if (b.node[1].x > xMax) {
        xMax = b.node[1].x;
      }
      if (b.node[1].y > yMax) {
        yMax = b.node[1].y;
      }
    }
    return new Rectangle(xMin - 3, yMin - 3, 
                         xMax - xMin + 6, yMax - yMin + 6);
  }

  public HashSet getRivettedEdges() { return m_edges; }
  public boolean horizontal() { return (m_dir == Edge.CONST_Y); }
  public boolean vertical() { return (m_dir == Edge.CONST_X); }
  public boolean contains(int x, int y) {
    return m_bounds.contains(x,y);
  }
  public void setContainer(Component container) {
    m_container = container;
  }

  public Component getContainer() {
    return m_container;
  }
  public Graphics getGraphics() {
    return m_container.getGraphics();  
  }
  public void paint(Graphics g) {
    m_bounds = getBoundingBox(m_edges);
    g.setColor(RivetColors[Selectable.UNSELECTED]);
    g.fillRoundRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height,
                    10,10);
    if (isSelected()) {
      g.setColor(RivetColors[Selectable.SELECTED]);
      g.drawRoundRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height,
                      10,10);
    }
  }

  public void clear(Graphics g) {
    g.setColor(m_container.getBackground());
    g.fillRoundRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height,
                    10,10);
    g.drawRoundRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height,
                    10,10);
  }

  public void setState(int stateToSet) {
    m_state = stateToSet;
  }
  public int getState() {
    return m_state;
  }
  public boolean isSelected() {
    return (m_state == Selectable.SELECTED);
  }

  public void select() {
    setState(Selectable.SELECTED);
  }
  public void unselect() {
    setState(Selectable.UNSELECTED);
  }

  public void moveParallelBy(int dx, int dy) {
    Graphics g = getGraphics();
    clear(g);
    m_bounds.translate(dx, dy);
    paint(g);
//    System.out.println("moveParallelBy : " + dx + " " + dy);
    for (HashSetIterator d = m_edges.begin(); d.hasMoreElements(); ) {
      Edge bm = (Edge)d.nextElement();
      bm.getContainer().resizeSideways(bm, dx,dy);
    }
    g.dispose();
  }
  /*
  public void resizeSideways(Edge bm, int dx, int dy) {
    super.resizeSideways(bm, dx, dy);
    for (DListIterator d = m_members.begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      for (int i = 0; i < 4; i++) {
        if (edge[i] == bm) { // which edge is this ?
          b.resizeSideways(b.edge[i], dx, dy);
        }
      }
    }
  }
  */
  // return true if the specified rivet bar has an edge in common with
  // this
  public boolean intersects(RivetBar rb) {
    HashSet intersection = m_edges.intersection(rb.m_edges);
    return (intersection.size() > 0);
  }
  public void union(RivetBar rb) {
    m_edges = m_edges.union(rb.m_edges);
    for (HashSetIterator s = rb.m_edges.begin(); s.hasMoreElements();) {
      Edge b = (Edge)s.nextElement(); 
      b.setRivet(this);
    }
    m_bounds = getBoundingBox(m_edges);
  }
  void free(Edge b) {
    m_edges.remove(b);
    b.setRivet(null);
  }
}
