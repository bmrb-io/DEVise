/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class EditCanvas.java
****************************************************************************/

//Packages 

package LayoutEditor;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>EditCanvas</CODE> class
 * @author  Shilpa Lawande
 * Implements the meat of the canvas editor
 */
/* Model used is that of rivets and edges
 * A edge has rivets at either ends.
 * The rivets are rigid but can be broken
 * Two edges connected by a rigid rivet cannot move relative to each other
 * A box is a rectangle with edges connected by rigid rivets at corners
*/

// 1) Create new box (by default create rivets where possible)
// 2) Delete box (break any rivets)
// 3) Move edges for horizontal or vertical motion
// 4) Move rivets for diagonal motion
// 5) Move entire box (breaks rivets if necessary)
// 6) Align boxes along X left or right (rivet by default)
// 7) Align boxes along Y top or bottom (rivet by default)
// 8) Create or break rivets

public class EditCanvas extends Canvas 
{
  /* constants - Colors */
  public static final int CREATE_RBNDCLR = 0;
  public static final int GROUP_RBNDCLR = 1;
  public static final int RIVET_RBNDCLR = 2;
  public static final int BOUNDING_BOXCLR = 3; 

  private static Color CanvasColors[] = new Color[4];
  static {
    CanvasColors[CREATE_RBNDCLR] =  Color.blue;
    CanvasColors[GROUP_RBNDCLR] = Color.cyan;
    CanvasColors[RIVET_RBNDCLR] = Color.white;
    CanvasColors[BOUNDING_BOXCLR] = Color.green;
  }
  public void configureRbndColors() {
    ConfigureColor dlg 
      = new ConfigureColor(new String[] { "Create Rubberband", 
                                          "Group Rubberband", 
                                          "Rivet Rubberband", 
                                          "BoundingBox Rubberband" }, 
                           CanvasColors); 
    dlg.doModal();
    CanvasColors[CREATE_RBNDCLR] = dlg.getColor(CREATE_RBNDCLR);
    CanvasColors[GROUP_RBNDCLR] = dlg.getColor(GROUP_RBNDCLR);
    CanvasColors[RIVET_RBNDCLR] = dlg.getColor(RIVET_RBNDCLR);
    CanvasColors[BOUNDING_BOXCLR] = dlg.getColor(BOUNDING_BOXCLR);
    repaint();
  } 
  public void configureEditColors() {
    ConfigureColor dlg 
      = new ConfigureColor(new String[] { "Normal Edge", 
                                          "Selected Edge",
                                          "Group Box",
                                          "RivetBar",
                                          "RivetBar HighLight" }, 
                           new Color[] { Edge.EdgeColors[Selectable.UNSELECTED], 
                                         Edge.EdgeColors[Selectable.SELECTED],
                                         GroupBox.GroupBoxColor,
                                         RivetBar.RivetColors[Selectable.UNSELECTED],
                                         RivetBar.RivetColors[Selectable.SELECTED]  
                                        });
                                        
    dlg.doModal();
    Edge.EdgeColors[Selectable.UNSELECTED] = dlg.getColor(0);
    Edge.EdgeColors[Selectable.SELECTED] = dlg.getColor(1);
    GroupBox.GroupBoxColor = dlg.getColor(2);
    RivetBar.RivetColors[Selectable.UNSELECTED] = dlg.getColor(3);
    RivetBar.RivetColors[Selectable.SELECTED] = dlg.getColor(4);
    repaint();
  }

  public static final int EDITLAYOUT = 0;
  public static final int DRAWRECTSINGLE = 1;
  public static final int DRAWRECTMULTI = 2;
  
  private static final int DEFAULT_HT = 50;
  private static final int DEFAULT_WD = 50;

  /* private members*/
  private int m_width;
  private int m_height;

  /* Variables to keep track of current state of the canvas */
  /* list of items on the canvas */
  
  protected LayoutState m_layoutState;  
                             // info about canvas state (stuff to be saved)
  
  // transient info about layout operations
  Rectangle m_boundingBox;   // bounding box for drawing area
  Rubberband m_currentRbd;   // current rubberband being drawn
  Point m_clickPoint;        // used to store mouse location for some operations
  DList m_selectedNodes;     // selected nodes for resizing
  DList m_selectedEdges;     // selected edge in the selected box
  DList m_selectedBoxes;     // selected boxes
  DList m_selectedRivetBars; // selected rivetbars
  int m_mode;                // drawing mode
  Point m_copyOffset;        // default copy offset
  GroupBox m_movingGroup;       // moving group

  /* handlers */
  MouseHandler m_mouseHandler = new MouseHandler(); 
  MouseMotionHandler m_motionHandler = new MouseMotionHandler();
  Image m_offscreenBuf;

  /* Constructor */
  public EditCanvas(int width, int height) 
  {
    this.m_height = height;
    this.m_width = width;
    this.m_layoutState = new LayoutState();
    this.addMouseListener(m_mouseHandler);
    this.addMouseMotionListener(m_motionHandler);
    
    // transient state variables
    m_selectedBoxes = new DList();
    m_selectedEdges = new DList();
    m_selectedRivetBars = new DList();
    m_selectedNodes = new DList();
        
    m_currentRbd = new Rubberband(this);
    m_clickPoint = new Point(0,0);
    m_copyOffset = new Point(15,15);
    m_mode = EDITLAYOUT;
    m_movingGroup = null;

    m_boundingBox = null;
  }
  public void setBoundingBox(int x, int y, int width, int height) {
    m_boundingBox = new Rectangle(x, y, width, height);
  }
  public void setSize(int width, int height) {
    m_width = width;
    m_height = height;
  }
  public Dimension getPreferredSize() {
    return new Dimension(m_width, m_height);
  }
  public Dimension getMinimumSize() {
    return new Dimension(m_width, m_height);
  }
  public void setMode(int mode) { m_mode = mode; }
  public int getMode() { return m_mode; }

  public void clear() {
    if (m_layoutState != null) {
      m_layoutState.clear();
    }
    m_selectedBoxes.clear();
    m_selectedRivetBars.clear();
    m_selectedNodes.clear();
    m_selectedEdges.clear();
    repaint();
  }
  public void paint(Graphics g) {
    // Use double buffering
    if (m_offscreenBuf == null) { 
      m_offscreenBuf = createImage(getSize().width, getSize().height);
    }
    if ((m_offscreenBuf.getWidth(this) != getSize().width) ||
        (m_offscreenBuf.getHeight(this) != getSize().height)) {
      m_offscreenBuf = createImage(getSize().width, getSize().height); 
    } 
    Graphics g2 = m_offscreenBuf.getGraphics();
    g2.clearRect(0,0, getSize().width, getSize().height);
    super.paint(g2);
    
    if (m_boundingBox != null) {
      Color oldClr = g2.getColor();
      g2.setColor(CanvasColors[BOUNDING_BOXCLR]);
      g2.drawRect(m_boundingBox.x, m_boundingBox.y, 
                  m_boundingBox.width, m_boundingBox.height);
      g2.setColor(oldClr);
    }
    m_layoutState.paint(g2);
    g.drawImage(m_offscreenBuf, 0, 0, null);
    g2.dispose();
  }
  
  void createBox(int x, int y, Rubberband rbd) {
    Rectangle r;
    if (rbd.started()) {
      r = rbd.getBounds(); 
    } else {
      r = new Rectangle(x, y, DEFAULT_WD, DEFAULT_HT);
    }
    if (m_boundingBox != null) {
      if (!m_boundingBox.intersection(r).equals(r)) {
        System.out.println("Error!!Rectangle not contained within bounding box!!");
        return;
      }
    }
    Box b = new Box(r);
    b.setContainer(EditCanvas.this);
    m_layoutState.addBox(b);
  }

  public void copySelection() {
    if (m_selectedBoxes.size() <= 0) {
      return;
    }
    Box b = (Box)m_selectedBoxes.at(0);
    if (b instanceof GroupBox) {
      GroupBox g2 = (GroupBox)
                    ((GroupBox)b).copy(m_copyOffset.x, m_copyOffset.y);
      g2.setContainer(EditCanvas.this);
      for (DListIterator d = g2.getMembers().begin(); d.hasMoreElements();) {
        Box bx = (Box)d.nextElement();
        bx.setContainer(EditCanvas.this); 
        m_layoutState.addBox(bx);
      }
      m_layoutState.addGroup(g2);
      g2.select();
      m_selectedBoxes.add(g2);
    } else {
      Box b2 = b.copy(m_copyOffset.x, m_copyOffset.y);
      b2.setContainer(EditCanvas.this);
      m_layoutState.addBox(b2);
      b2.select();
      m_selectedBoxes.add(b2);
    }
    b.unselect();
    m_selectedBoxes.remove(b);
    repaint();
  }

  public void deleteSelections() {
    for (DListIterator d = m_selectedBoxes.begin();
         d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (b instanceof GroupBox)  {
        GroupBox g = (GroupBox)b;
        for (DListIterator i = g.getMembers().begin(); i.hasMoreElements(); ) {
          Box bx = (Box)i.nextElement();
          bx.disconnect();
          m_layoutState.removeBox(bx);
        }
        g.disconnect();
        m_layoutState.removeGroup(g);
      } else {
        b.disconnect();
        m_layoutState.removeBox(b);
      }
    }
    m_selectedBoxes.clear();
    repaint();
  }
  public void align(int side) {
    if (m_selectedBoxes.size() <= 1) {
      return;
    }
    Box master = (Box)m_selectedBoxes.at(0);
    // TODO -> check for true...
    master.align(m_selectedBoxes, side);
    //unselectBoxes();
    repaint();
  }
  public void equalizeDimension(int dimension) {
    if (m_selectedBoxes.size() <= 1) {
      return;
    }
    Box master = (Box)m_selectedBoxes.at(0);
    // TODO -> check for true...
    master.equalizeDimension(m_selectedBoxes, dimension);
    //unselectBoxes();
    repaint();
  } 
  public void distribute(int dir) {
    if (m_selectedBoxes.size() <= 1) {
      return;
    }
    Box.distribute(m_selectedBoxes, dir);
    repaint();
  }
  public Box findBox(int x, int y) {
    // check if the current point is within a group
    for (DListIterator d = m_layoutState.groupList();
         d.hasMoreElements(); ) {
      GroupBox g = (GroupBox)d.nextElement();
      if (g.contains(x, y)) {
        return g;
      }
    }
    // check if the current point is within a box
    for (DListIterator d = m_layoutState.boxList();
         d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (b.contains(x, y)) {
        return b;
      }
    } 
    return null;
  }

  public boolean selectOneBox(int x, int y) {
    Box b = findBox(x,y);
    if (b != null) {
      if (m_selectedBoxes.remove(b) == 1) {
        b.unselect();
      } else {
        b.select();
        m_selectedBoxes.add(b);
      } 
      return true;
    } 
    return false;
  }
  public boolean selectBoxAt (int x, int y, boolean single) {
    if (single) {
      unselectBoxes();
    }
    return selectOneBox(x,y);
  }

  public void group() {
    if (m_selectedBoxes.size() == 0) {
      return;
    }
    System.out.println("Boxes to be grouped......\n");
    for (DListIterator d = m_selectedBoxes.begin(); d.hasMoreElements(); ) {
      System.out.println(d.nextElement());
    }
         
    GroupBox g = new GroupBox(m_selectedBoxes);
    if (g != null) {
      m_layoutState.addGroup(g);
      g.setContainer(EditCanvas.this);
      unselectBoxes();
      g.select();
      m_selectedBoxes.add(g);
    }
    repaint();
  }

  public void ungroup() {
    for (DListIterator d = m_layoutState.groupList();
         d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (b instanceof GroupBox) {
        m_layoutState.removeGroup((GroupBox)b);
        m_selectedBoxes.remove(b);
      }
    }
    repaint();
  }

  public void selectBoxesWithin (Rectangle r) {
    unselectBoxes();
    DList excludeBoxes = new DList();
    for (DListIterator d = m_layoutState.groupList(); 
         d.hasMoreElements(); ) {
      GroupBox g = (GroupBox)d.nextElement();
      if (g.containedBy(r)) {
        g.select();
        m_selectedBoxes.add(g);
      }
    }
    for (DListIterator d = m_layoutState.boxList();
         d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      if (b.containedBy(r) && !b.isGrouped()) {
        b.select();
        m_selectedBoxes.add(b);
      }
    }
  }
  public void unselectBoxes() {
    for (DListIterator d = m_selectedBoxes.begin();
         d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      b.unselect();
    } 
    m_selectedBoxes.clear();
  }
  
  public boolean selectEdgeNear(int x, int y) {
    Box b = (Box)m_selectedBoxes.at(0);
    Edge edge = b.onEdge(x, y);
    if (edge != null) {
      edge.select();
      m_selectedEdges.add(edge);
      m_clickPoint.x = ( edge.node[0].x  + edge.node[1].x ) / 2;
      m_clickPoint.y = ( edge.node[0].y  + edge.node[1].y ) / 2;
      return true;
    }
    return false;
  }

  public void unselectEdges() {
    for (DListIterator d = m_selectedEdges.begin();
         d.hasMoreElements(); ) {
      Edge b = (Edge)d.nextElement();
      b.unselect();
    }
    m_selectedEdges.clear();
  }
  public void selectEdgesWithin(Rectangle r) {
    for (DListIterator d = m_layoutState.boxList(); 
         d.hasMoreElements(); ) {
      Box b = (Box)d.nextElement();
      DList edges = b.getEdgesWithin(r);
      if (edges.size() > 1) {
        System.out.println("Ambiguous rivet");
        return;  // ambiguous rivet - ignore!!
      } else if (edges.size() == 1) {
        Edge edge = (Edge)edges.at(0);
        edge.select();
        m_selectedEdges.add(edge);
      }
    }
  }
  public void rivetEdges() {
    if (m_selectedEdges.size() > 0) {
      RivetBar rbar = new RivetBar(m_selectedEdges);
      // check for intersection with existing rivet bars and union them
      // if there is 
      DList intersectingBars = new DList();
      for (DListIterator d = m_layoutState.rivetBarList(); d.hasMoreElements();) {
        RivetBar rb2 = (RivetBar)d.nextElement();
        if (rbar.intersects(rb2)) {
          intersectingBars.add(rb2);
        }
      }
      for (DListIterator d = intersectingBars.begin(); d.hasMoreElements();) {
        RivetBar rb2 = (RivetBar)d.nextElement();
        rbar.union(rb2);
        m_layoutState.removeRivetBar(rb2);
      }
      rbar.setContainer(EditCanvas.this);
      m_layoutState.addRivetBar(rbar);
      intersectingBars.clear();
    }
    repaint();
  }
  public void unrivetEdges() {
    for (DListIterator d = m_selectedEdges.begin(); d.hasMoreElements();) {
      Edge e = (Edge)d.nextElement();
      RivetBar rb = e.getRivet();
      if (rb != null) {
        rb.free(e);
      }
      if (rb.getRivettedEdges().size() == 0) {
        m_layoutState.removeRivetBar(rb);
      }
    }
    repaint();
  }

  public void alignEdges() {
    if (m_selectedEdges.size() <= 1) {
      return;
    }
    Edge master = (Edge)m_selectedEdges.at(0);
    master.align(m_selectedEdges);
    //unselectEdges();
    repaint();
  }

  public RivetBar findRivetBar(int x, int y) {
    // check if the current point is within a rivetbar
    for (DListIterator d = m_layoutState.rivetBarList();
         d.hasMoreElements(); ) {
      RivetBar rb = (RivetBar)d.nextElement();
      if (rb.contains(x, y)) {
        return rb;
      }
    } 
    return null;
  }

  public boolean selectOneRivetBar(int x, int y) {
    RivetBar rb = findRivetBar(x,y);
    if (rb != null) {
      if (m_selectedRivetBars.remove(rb) == 1) {
        rb.unselect();
      } else {
        rb.select();
        m_selectedRivetBars.add(rb);
      } 
      return true;
    } 
    return false;
  }
  public boolean selectRivetBarAt(int x, int y, boolean single) {
    if (single) {
      unselectRivetBars();
    }
    return selectOneRivetBar(x,y);
  }
  public void unselectRivetBars() {
    for (DListIterator d = m_selectedRivetBars.begin();
         d.hasMoreElements(); ) {
      RivetBar rb = (RivetBar)d.nextElement();
      rb.unselect();
    } 
    m_selectedRivetBars.clear();
  }
  public boolean selectNodeNear(int x, int y) {
    Box b = (Box)m_selectedBoxes.at(0);
    Node nd = b.onNode(x, y);
    if (nd !=  null) {
      nd.select();
      m_selectedNodes.add(nd);
      m_clickPoint.x = nd.x;
      m_clickPoint.y = nd.y;
      return true;
    }
    return false;
  }
  public void unselectNodes() {
    for (DListIterator d = m_selectedNodes.begin();
         d.hasMoreElements(); ) {
      Node n = (Node)d.nextElement();
      n.unselect();
    }
    m_selectedNodes.clear();
  }
  // check that this is the first mouse button
  private boolean button1(MouseEvent e) {
    return (!e.isMetaDown() && !e.isAltDown());
  }
  private boolean button2(MouseEvent e) {
    return e.isAltDown();
  }
  private boolean button3(MouseEvent e) {
    return e.isMetaDown();
  }
  /* Handle mouse events */
  private class MouseMotionHandler extends MouseMotionAdapter {
    public void mouseDragged(MouseEvent e) {
      if (button2(e)) {  // select edges
        m_currentRbd.stretch(e.getX(), e.getY());
      }
      if ((m_mode == DRAWRECTSINGLE) || (m_mode == DRAWRECTMULTI)) {
        if (!m_currentRbd.started()) {
	        m_currentRbd.start(e.getX(), e.getY(), CanvasColors[CREATE_RBNDCLR]);
        } else {
          m_currentRbd.stretch(e.getX(), e.getY());
        }
      } else if (m_mode == EDITLAYOUT) {  // move or group
        if (!m_currentRbd.started()) { // not a grouping operation
          doMove(e);
        } else {
          m_currentRbd.stretch(e.getX(), e.getY());
        }
      }
      /*
      else if (m_mode == RIVETEDGES) {  // stretch rivet rubberband
        m_currentRbd.stretch(e.getX(), e.getY());
      }
      */
    }
  }
  private class MouseHandler extends MouseAdapter {
    public void mousePressed(MouseEvent e) {
      if (button2(e)) {
        unselectBoxes();
        unselectEdges();
        unselectNodes();
        m_currentRbd.start(e.getX(), e.getY(), CanvasColors[RIVET_RBNDCLR]);
        return;
      }
      if (!button1(e)) {
        return;
      }
      if (m_mode == EDITLAYOUT) {
        // check for rivets first
        boolean selection = false;
        m_clickPoint.x = e.getX();
        m_clickPoint.y = e.getY();
        if (selectRivetBarAt(e.getX(), e.getY(), true)) {
          repaint();
          return;
        }
        // now check for box selection
        if (!e.isShiftDown()) {
	        selection = selectBoxAt(e.getX(), e.getY(), true);
          if (selection) {
            if (e.isControlDown()) {
              copySelection();
            } else {
              if (!selectNodeNear(e.getX(), e.getY())) {
                selectEdgeNear(e.getX(), e.getY());
              }
            }
          }
	      } else {
	        selection = selectBoxAt(e.getX(), e.getY(), false);
	      }
        repaint();
        // if no box is selected then start a grouping rubberband
        if (!selection) {
          // rubberband for grouping 
          m_currentRbd.start(e.getX(), e.getY(), CanvasColors[GROUP_RBNDCLR]);
        }
      } 
      /*
      else if (m_mode == RIVETEDGES) {
        unselectBoxes();
        unselectEdges();
        m_currentRbd.start(e.getX(), e.getY(), CanvasColors[RIVET_RBNDCLR]);
      } 
      */
    }
    public void mouseReleased(MouseEvent e) {
      if (button2(e)) {
        if (m_currentRbd.started()) {
          selectEdgesWithin(m_currentRbd.getBounds());
        }
        m_currentRbd.reset(); 
        repaint();
        return;
      }
      if ((m_mode == DRAWRECTSINGLE) || (m_mode == DRAWRECTMULTI)) {
        createBox(e.getX(), e.getY(), m_currentRbd);
        if (m_mode == DRAWRECTSINGLE) {
          m_mode = EDITLAYOUT;
        } 
      } else if (m_mode == EDITLAYOUT) {
        if (m_currentRbd.started()) {
          selectBoxesWithin(m_currentRbd.getBounds());
        } 
        unselectEdges();
        unselectNodes();
        unselectRivetBars();
        m_movingGroup = null;
      }
      /*
      else if (m_mode == RIVETEDGES) {
        if (m_currentRbd.started()) {
          rivetSelectedEdges(m_currentRbd.getBounds());
        }
      }
      */
      m_currentRbd.reset(); 
      repaint();
      setCursor(Cursor.getDefaultCursor());
    }
  }

  void doMove(MouseEvent e) {
    if (m_selectedRivetBars.size() > 0) {
      doMoveRivetBar(e);
      return;
    }
    if (m_selectedBoxes.size() <= 0) {  // no box has been selected
      return;
    }
    // check if click point is within any sizing handle so edge was
    // selected
    Box b = (Box)m_selectedBoxes.at(0);
    if (m_selectedNodes.size() == 1) {
      Node nd = (Node)m_selectedNodes.at(0);
      int dx = m_clickPoint.x - nd.x;
      int dy = m_clickPoint.y - nd.y;
      b.resizeDiagonally(nd, dx, dy);
      if ((dx > 0) && (dy > 0)) {
        setCursor(Cursor.getPredefinedCursor(Cursor.SE_RESIZE_CURSOR));
      } else if ((dx > 0) && (dy < 0)) {
        setCursor(Cursor.getPredefinedCursor(Cursor.NE_RESIZE_CURSOR));
      } else if ((dx < 0) && (dy < 0)) {
        setCursor(Cursor.getPredefinedCursor(Cursor.NW_RESIZE_CURSOR));
      } else if ((dx < 0) && (dy > 0)) {
        setCursor(Cursor.getPredefinedCursor(Cursor.SW_RESIZE_CURSOR));
      }
    } else if (m_selectedEdges.size() == 1) {  
      Edge edge = (Edge)m_selectedEdges.at(0); 
      Point delta = edge.getDelta(m_clickPoint.x, m_clickPoint.y);
      if (b instanceof GroupBox) {
        if (m_movingGroup == null) {
          m_movingGroup = (GroupBox)((GroupBox)b).copy(0,0);
          return;
        } else {
          ((GroupBox)b).resize(m_movingGroup,edge, delta.x, delta.y);
        } 
      } else {
        b.resizeSideways(edge, delta.x, delta.y);
      }
      if (delta.x > 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR));
      } else if (delta.x < 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.W_RESIZE_CURSOR));
      } else if (delta.y < 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.N_RESIZE_CURSOR));
      } else if (delta.y > 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR));
      }
    } else {
      if (b instanceof GroupBox) {
        ((GroupBox)b).moveParallelBy(e.getX() - m_clickPoint.x, e.getY() - m_clickPoint.y, Box.RIVET_PRESERVE); 
      } else {
         b.moveParallelBy(e.getX() - m_clickPoint.x, e.getY() - m_clickPoint.y,
                          Box.RIVET_PRESERVE); 
      }
      setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
    }
    m_clickPoint.x = e.getX();
    m_clickPoint.y = e.getY();
  }
  void doMoveRivetBar(MouseEvent e) {
    if (m_selectedRivetBars.size() == 1) {
      RivetBar rb = (RivetBar)m_selectedRivetBars.at(0);
      int dx = 0;
      int dy = 0;
      if (rb.horizontal()) {
        dy = e.getY() - m_clickPoint.y;
      } else {
        dx = e.getX() - m_clickPoint.x;
      }
      if (dx > 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR));
      } else if (dx < 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.W_RESIZE_CURSOR));
      } else if (dy < 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.N_RESIZE_CURSOR));
      } else if (dy > 0) {
        setCursor(Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR));
      }
      if ((dx != 0) || (dy != 0)) {
        rb.moveParallelBy(dx,dy);
        m_clickPoint.x = e.getX();
        m_clickPoint.y = e.getY();
      }
    }
    repaint();
  }
}
