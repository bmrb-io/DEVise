/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Box.java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.awt.*;
import COM.objectspace.jgl.*;
import COM.objectspace.jgl.algorithms.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>Box</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class Box implements Selectable
{
  // Note : important that these numbers are from 0 - 3 as they are used
  // as indexes for the edges.
  public static final int ALIGN_LEFT = 0;
  public static final int ALIGN_BOTTOM = 1;
  public static final int ALIGN_RIGHT = 2;
  public static final int ALIGN_TOP = 3;
  public static final int ALIGN_CENTER = 4;
  public static final int ALIGN_MIDDLE = 5;

  public static final int DIMENSION_WIDTH = 5;
  public static final int DIMENSION_HEIGHT = 6;
  
  // rivet modes
  public static final int RIVET_PRESERVE = 0;
  public static final int RIVET_BREAK = 1;
  public static final int RIVET_OVERRIDE = 2;

  protected int m_state;
  /* Public members */
  public Node node[];
  public Edge  edge[];

  private boolean m_isGrouped; // Is this box a part of groupbox

  protected Component m_container;
  /* Constructor */
  public Box() {
    node = null;
    edge = edge;
    m_state = Selectable.UNSELECTED;
  }
  public Box(Rectangle rect) {
    int x1 = rect.x;
    int y1 = rect.y;
    int x2 = rect.x + rect.width;
    int y2 = rect.y + rect.height;    
    node = new Node[4];
    node[0] = new Node(x1, y1);
    node[1] = new Node(x1, y2);
    node[2] = new Node(x2, y2);
    node[3] = new Node(x2, y1);
  
    // try to have create a edge so that node[0] and node[1] are in sorted
    // order
    edge = new Edge[4];
    edge[0] = new Edge (node[0], node[1]);  // CONST_X
    edge[1] = new Edge (node[1], node[2]);  // CONST_Y
    edge[2] = new Edge (node[3], node[2]);  // CONST_X
    edge[3] = new Edge (node[0], node[3]);  // CONST_Y
    for (int i = 0; i < 4; i++) {
      edge[i].setContainer(this);
    }
    m_state = Selectable.UNSELECTED;
  }
  public Box copy(int dx, int dy) {
    return new Box(new Rectangle(node[0].x + dx, node[0].y + dy, 
                   node[2].x - node[0].x,
                   node[2].y - node[0].y));
  }
  
  public void setContainer(Component container) {
    m_container = container;
  }
  public Component getContainer() {
    return m_container;
  }
  public boolean isGrouped() { return m_isGrouped; }
  public void setGrouped(boolean flag) { m_isGrouped = flag; }

  public void paint(Graphics g) {
    // draw edges
    for (int i = 0; i < 4; i++) {
       edge[i].paint(g); 
    }
    // if selected draw sizing handles
    if (m_state == Selectable.SELECTED) {
      drawSizingHandles(g);
    }
  }
  public void clear(Graphics g) {
    paintXOR(g);  // clear previous rectangle
    // the foll is needed to get rid of the tiny dots of color left at
    // the corners due to XOR
    for (int i = 0; i < 4; i++) {
      edge[i].paint(g, m_container.getBackground());
    }
  }

  public void paintXOR(Graphics g) {
    g.setXORMode(m_container.getBackground());
    // if selected draw sizing handles
    if (m_state == Selectable.SELECTED) {
      drawSizingHandles(g);
    }
    for (int i = 0; i < 4; i++) {
      edge[i].paint(g);
    }
    g.setPaintMode();
  }

  protected void drawSizingHandles(Graphics g) {
    for (int i = 0; i < 4; i++) {
      node[i].paint(g);
    }
    Node.paint((node[0].x + node[1].x) / 2, (node[0].y + node[1].y) / 2, g);
    Node.paint((node[1].x + node[2].x) / 2, (node[1].y + node[2].y) / 2, g);
    Node.paint((node[2].x + node[3].x) / 2, (node[2].y + node[3].y) / 2, g);
    Node.paint((node[3].x + node[0].x) / 2, (node[3].y + node[0].y) / 2, g);
  }

  public Color getBackground() {
    return m_container.getBackground(); 
  }
  public Graphics getGraphics() {
    return m_container.getGraphics();  
  }
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
  public boolean containedBy(Rectangle r) {
    Rectangle myRect = bounds();
    if (r.intersection(myRect).equals(myRect)) {
      return true;
    } 
    return false;
  }
  public boolean contains(int x, int y) {
    // leave margin of 1 on all sides
    if ((node[0].x > x) || (node[2].x < x) || 
        (node[0].y > y) || (node[2].y < y)) {
      return false;
    }
    return true;
  }
  public Edge onEdge(int x, int y) {
    for (int i = 0; i < 4; i++) {
      if (edge[i].resizeHandle(x,y)) {
        return edge[i];
      }
    }
    return null;
  }
  public Node onNode(int x, int y) {
    for (int i = 0; i < 4; i++) {
      Rectangle r = new Rectangle(node[i].x - 3, node[i].y - 3, 6, 6);
      if (r.contains(x,y)) {
        return node[i];
      }
    }
    return null;
  }
  public boolean align(DList boxlist, int side) {
    if ((side != ALIGN_TOP) && (side != ALIGN_BOTTOM) &&
        (side != ALIGN_RIGHT) && (side != ALIGN_LEFT) && 
        (side != ALIGN_MIDDLE) && (side != ALIGN_CENTER)) {
      return false;   
    }
    for (DListIterator d = boxlist.begin();
          d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      int dx = 0;
      int dy = 0;
      if (side == ALIGN_CENTER) {
        dx = (node[0].x + node[2].x 
             -b.node[0].x - b.node[2].x) / 2;
      } else if (side == ALIGN_MIDDLE) {
        dy = (node[0].y + node[2].y 
             -b.node[0].y - b.node[2].y) / 2; 
      } else if ((side == ALIGN_TOP) || (side == ALIGN_BOTTOM)) {
        dy = edge[side].node[0].y - b.edge[side].node[0].y;
      } else {
        dx = edge[side].node[0].x - b.edge[side].node[0].x;
      }
      b.moveParallelBy(dx, dy); 
    }
    return true;
  }
  public boolean equalizeDimension(DList boxlist, int dimension) { 
    if ((dimension != DIMENSION_HEIGHT) && (dimension != DIMENSION_WIDTH)) {
      return false;
    }
    int width = node[2].x - node[0].x;
    int height = node[2].y - node[0].y;
    for (DListIterator d = boxlist.begin();
          d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (dimension == DIMENSION_WIDTH) {
        b.setWidth(width);
      } else {
        b.setHeight(height);
      }
    }
    return true;
  }
  public void disconnect() {
    for (int i = 0; i < 4; i++) {
      RivetBar rb = edge[i].getRivet();
      if (rb != null) {
        rb.free(edge[i]);
      }
    }
  }
  static public void distribute(DList boxList, int dir) {
    BinaryPredicate pred; 
    if (dir == DIMENSION_WIDTH) {
      Sorting.sort(boxList,new XComp());
      Box bMin = (Box)boxList.at(0);
      int minX = bMin.node[0].x;
      Box bMax = (Box)boxList.at(boxList.size() - 1);
      int maxX = bMax.node[2].x;
      int totalWidth = 0;
      for (DListIterator d = boxList.begin(); d.hasMoreElements();) {
        Box b = (Box)d.nextElement();
        totalWidth += ( b.node[2].x - b.node[0].x );
      }
      int spaceBetween = (maxX - minX - totalWidth) / (boxList.size() - 1);
      int start = minX;
      for (DListIterator d = boxList.begin(); d.hasMoreElements();) {
        Box b = (Box)d.nextElement();
        b.moveParallelBy(start - b.node[0].x, 0);
        start += (b.node[2].x - b.node[0].x) + spaceBetween;
      }
    } else if (dir == DIMENSION_HEIGHT) {
      Sorting.sort(boxList,new YComp());
      Box bMin = (Box)boxList.at(0);
      int minY = bMin.node[0].y;
      Box bMax = (Box)boxList.at(boxList.size() - 1);
      int maxY = bMax.node[2].y;
      int totalHeight = 0;
      for (DListIterator d = boxList.begin(); d.hasMoreElements();) {
        Box b = (Box)d.nextElement();
        totalHeight += ( b.node[2].y - b.node[0].y );
      }
      int spaceBetween = (maxY - minY - totalHeight) / (boxList.size() - 1);
      int start = minY;
      for (DListIterator d = boxList.begin(); d.hasMoreElements();) {
        Box b = (Box)d.nextElement();
        b.moveParallelBy(0, start - b.node[0].y);
        start += (b.node[2].y - b.node[0].y) + spaceBetween;
      }
    }
  }
  public void moveParallelBy(int dx, int dy) {
    moveParallelBy(dx, dy, RIVET_BREAK);
  }
  public void moveParallelBy(int dx, int dy, int rivetMode) {
    if (rivetMode != RIVET_OVERRIDE) {
	    for (int i = 0; i < 4; i++) {
	      RivetBar rb = edge[i].getRivet();
	      if (rb != null) {
	        if (rb.horizontal()) {
	          if (dy != 0) {
		          if (rivetMode == RIVET_PRESERVE) {
		            dy = 0;  // cannot move along y
		          } else {
		            rb.free(edge[i]);
		          }
	          }
	        } else {
	          if (dx != 0) {
	            if (rivetMode == RIVET_PRESERVE) {
	              dx = 0;
	            } else {
	              rb.free(edge[i]);
	            }
	          }
	        }
	      }
	    }
    }
    Graphics g = getGraphics();
    clear(g);
    for (int i = 0; i < 4; i++) {
      node[i].translate(dx, dy);
    }
    paintXOR(g);
    g.dispose();
  }

  public void resizeSideways(Edge b, int dx, int dy) {
    Graphics g = getGraphics();
    clear(g);
    b.moveParallelBy(dx, dy);
    paintXOR(g);
    g.dispose();
  }
  public void resizeDiagonally(Node n, int dx, int dy) {
    Graphics g = getGraphics();
    clear(g);
    if (n == node[0]) {
      edge[3].moveParallelBy(0, dy);
      edge[0].moveParallelBy(dx, 0);
    } else if (n == node[1]) {
      edge[0].moveParallelBy(dx, 0);
      edge[1].moveParallelBy(0, dy);
    } else if (n == node[2]) {
      edge[1].moveParallelBy(0, dy);
      edge[2].moveParallelBy(dx, 0);
    } else if (n == node[3]) {
      edge[2].moveParallelBy(dx, 0);
      edge[3].moveParallelBy(0, dy);
    }
    paintXOR(g);
    g.dispose();
  }
  public void setWidth(int width) {
    int x2 = node[0].x + width;
    node[2].x = x2;
    node[3].x = x2;
  }
  public void setHeight(int height) {
    int y2 = node[0].y + height;
    node[1].y = y2;
    node[2].y = y2;
  }
  public Rectangle getBounds() {
    return new Rectangle(node[0].x, node[0].y,
                         node[2].x - node[0].x,
                         node[2].y - node[0].y);
  }
  public Rectangle bounds() {
    // with some tolerance
    return new Rectangle(node[0].x - 3, node[0].y - 3,
                         node[2].x - node[0].x + 6, node[2].y - node[0].y + 6);
  }

  public DList getEdgesWithin(Rectangle r) {
    DList edgeList = new DList();
    for (int i = 0; i < 4; i++) {
      if (r.contains(edge[i].node[0]) && r.contains(edge[i].node[1])) {
        edgeList.add(edge[i]); 
      }
    }
    return edgeList;
  }
  public void init(int x1, int y1, int x2, int y2) {
    node[0].moveTo(x1, y1);
    node[1].moveTo(x1, y2);
    node[2].moveTo(x2, y2);
    node[3].moveTo(x2, y1);
  }
}

class XComp implements BinaryPredicate {
  public boolean execute(Object b1, Object b2) {
    if ((b1 instanceof Box) && (b2 instanceof Box)) {
      return ((Box)b1).node[0].x < ((Box)b2).node[0].x;
    }
    return false;
  }
}
class YComp implements BinaryPredicate {
  public boolean execute(Object b1, Object b2) {
    if ((b1 instanceof Box) && (b2 instanceof Box)) {
      return ((Box)b1).node[0].y < ((Box)b2).node[0].y;
    }
    return false;
  }
}
