/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class GroupBox.java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.awt.*;
import COM.objectspace.jgl.*;

// Class
/**
 * The <CODE>GroupBox</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class GroupBox extends Box 
{
  public static Color GroupBoxColor = Color.magenta;

  private DList m_members;   // boxes in this group
  
  public GroupBox(DList boxes) {
    super(getBoundingBox(boxes));
    m_members = new DList(boxes);
 }

  public static Rectangle getBoundingBox(DList boxes) {
    if (boxes.size() <= 0) {
      return null;
    }
    Rectangle r = ((Box)boxes.at(0)).bounds();
    for (DListIterator d = boxes.begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      r.add(b.bounds());
    }
    // return a slightly bigger bounding box
    return new Rectangle(r.x - 1, r.y - 1, r.width + 2, r.height + 2);
  }
  public void adjust() {
    Rectangle r = getBoundingBox(m_members);
    node[0].x = r.x - 1; node[0].y = r.y - 1;
    node[1].x = r.x - 1; node[1].y = r.y + r.height + 1;
    node[2].x = r.x + r.width + 1; node[2].y = r.y + r.height + 1;
    node[3].x = r.x + r.width + 1; node[3].y = r.y - 1;
  }
  public DList getMembers() { return m_members; }
  public Box copy(int dx, int dy) {
    DList copyMembers = new DList();
    for (DListIterator d = m_members.begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      Box bcopy = b.copy(dx, dy);
      copyMembers.add(bcopy);
    }
    return new GroupBox(copyMembers);
  }

  public void paint(Graphics g) {
    adjust();
    for (int i = 0; i < 4; i++) {
      edge[i].paint(g, GroupBoxColor);
    }
    if (isSelected()) {
      drawSizingHandles(g);
    }
  }

  public void paintXOR(Graphics g) {
    g.setXORMode(m_container.getBackground());
    // if selected draw sizing handles
    if (isSelected()) {
      drawSizingHandles(g);
    }
    for (int i = 0; i < 4; i++) {
      edge[i].paint(g, GroupBoxColor);
    }
    g.setPaintMode();
  }


  public void moveParallelBy(int dx, int dy, int rivetMode) {
    if (rivetMode != RIVET_OVERRIDE) {
	    DList extRivettedEdges = findExternallyRivettedEdges();
		  for (DListIterator d = extRivettedEdges.begin(); 
	         d.hasMoreElements();) {
		    Edge b = (Edge)d.nextElement();
	      RivetBar rb = b.getRivet();
        if (rb == null) {
          System.out.println("Something fishy!!" + b + "\n" + rb);
        }
	      if (rb.horizontal()) {
	        if (dy != 0) {
		        if (rivetMode == RIVET_PRESERVE) {
		          dy = 0;
		        } else {
		          rb.free(b);
		        }
		      }
	      } else {
	        if (dx != 0) {
		        if (rivetMode == RIVET_PRESERVE) {
		          dx = 0;
		        } else {
		          rb.free(b);
		        }
		      }
	      }
	    }
    }
    for (DListIterator d = m_members.begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      b.moveParallelBy(dx,dy, RIVET_OVERRIDE);
    }
    Graphics g = getGraphics();
    clear(g);
    for (int i = 0; i < 4; i++) {
      node[i].translate(dx, dy);
    }
    paintXOR(g);
    g.dispose();
  }
  
  public DList findExternallyRivettedEdges() {
    DList edgeList = new DList();
    for (DListIterator d = m_members.begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      for (int i = 0; i < 4; i++) {
        RivetBar rb = b.edge[i].getRivet();
        if (rb != null) {
          if (!isLocalRivet(rb)) {
            System.out.println("Adding ext edge");
	          edgeList.add(b.edge[i]);
          }
        }
      }
    }
    return edgeList;
  }

  public boolean isLocalRivet(RivetBar rb) {
    for (HashSetIterator s = rb.getRivettedEdges().begin();
         s.hasMoreElements(); ) {
      Edge b = (Edge)s.nextElement();
      if (m_members.indexOf(b.getContainer()) < 0) { // edge outside group
        return false;
      }
    }
    return true;
  }

  // what to do in presence of rivets!!??
  public void resize(GroupBox init, Edge bm, int dx, int dy) {
    int initW = init.node[2].x - init.node[0].x;
    int initH = init.node[2].y - init.node[0].y;
    int realdx = (node[2].x - node[0].x) - initW + dx;
    int realdy = (node[2].y - node[0].y) - initH + dy;
    float rW = (float)(initW  + realdx)/ (float)initW;
    float rH = (float)(initH  + realdy)/ (float)initH;
    System.out.println("realdx = " + realdx);
    System.out.println("realdy = " + realdy);

    System.out.println("rW = " + rW);
    System.out.println("rH = " + rH);
    int newX0, newY0;
    Graphics g = getGraphics();
    super.resizeSideways(bm, dx, dy);
    DListIterator dInit = init.m_members.begin();
    for (DListIterator d = m_members.begin(); d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      b.clear(g); 
   
      Box bInit = (Box)dInit.nextElement();
      int bx0 = (int)(node[0].x + (bInit.node[0].x - init.node[0].x) * rW);
      int by0 = (int)(node[0].y + (bInit.node[0].y - init.node[0].y) * rH);
      int bw = (int)((bInit.node[2].x - bInit.node[0].x) * rW);
      int bh = (int)((bInit.node[2].y - bInit.node[0].y) * rH);
      b.node[0].moveTo(bx0, by0);
      b.node[1].moveTo(bx0, by0 + bh);
      b.node[2].moveTo(bx0 + bw, by0 + bh);
      b.node[3].moveTo(bx0 + bw, by0);
      b.paintXOR(g); 
    }
    g.dispose();
  }
}
