/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseViewPeer.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;
import LayoutEditor.*;

// Class
/**
 * The <CODE>DEViseViewPeer</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DEViseViewPeer extends java.awt.Container implements Selectable
//public class DEViseViewPeer extends Panel implements Selectable
{
  static final Color HIGHLIGHTCLR = Color.blue;
  static final int THICKNESS = 3;
  public static final int X_PILEOFFSET = 15;
  public static final int Y_PILEOFFSET = 15;

  private DEViseView m_view;   // actual devise view this represents
  private DEViseLayout m_layout; // the layout mechanism for this container
  private DEViseSessionPeer m_thisSession; // the session to which this view 
                                           // belongs
  private LayoutState m_layoutState;  // the layout state for this view - 
                                      // stores any groups, rivets etc 
                                      // created during editing; 
  int m_state;
  boolean m_isCurrent;
  PopupMenu m_popupMenu;

  /* Constructor */
  DEViseViewPeer(DEViseSessionPeer session, 
                 DEViseView view, DEViseLayout layout) {
    super();
    setLayout(new DEViseLayoutPeer());
    m_view = view;
    m_layout = layout;
    m_thisSession = session;
    addMouseListener(new MouseHandler());
    //setBackground(m_view.getBgColor().getColor());
    MenuMaker menuMaker = new MenuMaker(session, "View.popupmenu");
    m_popupMenu = menuMaker.getPopupMenuByName("View");
    m_popupMenu.setLabel(m_view.getName());
    add(m_popupMenu);
    m_state = UNSELECTED;
  }
  public void selfDestruct() {
    System.out.println("DEViseViewPeer : selfDestruct\n");
    m_thisSession = null;
    m_view = null;
    m_layout = null;
    m_popupMenu = null;
    m_layoutState = null;
  }
  public void finalize() {
    System.out.println("GoodBye to ->\n" + this + "\n");
  }
  /* Getters and Setters */
  public void setCurrent(boolean val) { m_isCurrent = val; }
  public DEViseView getDEViseView() { return m_view; }
  public void setDEViseView(DEViseView view) { m_view = view; }
  public DEViseLayout getDEViseLayout() { return m_layout; }
  public void setDEViseLayout(DEViseLayout layout) { 
    m_layout = layout; 
  }
  public boolean isPiled() {
    return (m_layout instanceof DEVisePileLayout);
  }
  public DEViseSessionPeer getSession() { return m_thisSession; }
  public void setLayoutState(LayoutState layoutState) {
    m_layoutState = layoutState;
  }
  public LayoutState getLayoutState() {
    return m_layoutState;
  }
  /* Public methods */
  public Dimension getPreferredSize() {
    return new Dimension((int)(m_view.getWidth() * LMgr.getGlobalScaleX()),
                         (int)(m_view.getHeight() * LMgr.getGlobalScaleY())); 
  }
  public Dimension getMinimumSize() {
    return new Dimension((int)(m_view.getWidth() * LMgr.MINSCALEX),
                         (int)(m_view.getHeight() * LMgr.MINSCALEY)); 
  }
  public Dimension getMaximumSize() {
    return new Dimension((int)m_view.getWidth(), (int)m_view.getHeight());
  }

  
  public Rectangle getLayoutBounds() {
    int off = 0;
    int x = (int)(m_view.getX() * LMgr.getGlobalScaleX()) + off;
    int y = (int)(m_view.getY() * LMgr.getGlobalScaleY()) + off;
    int width = (int)(m_view.getWidth() * LMgr.getGlobalScaleX()) - 2 * off- 1;
    int height = (int)(m_view.getHeight() * LMgr.getGlobalScaleY()) - 2 * off - 1;
    return new Rectangle(x, y, width, height);
  }

  double getAbsX() {
    System.out.println("getAbsX(" + m_view.getName() + ")");
    if (m_view.getParent() == null) {
      return getBounds().x;
    } else {
      System.out.println(getParent());
      return ((DEViseViewPeer)getParent()).getAbsX() + getBounds().x;
    }
  }
  double getAbsY() {
    if (m_view.getParent() == null) {
      return getBounds().y;
    } else {
      return ((DEViseViewPeer)getParent()).getAbsY() + getBounds().y;
    }
  }
  
  public void paint(Graphics g) {
    if (m_view.getParent() == null) {  // Display
      setBounds(getLayoutBounds());
    }
    paint(g, (int)getAbsX(), (int)getAbsY());
    m_thisSession.showVisualLinks(g);
    m_thisSession.showRecordLinks(g);
    m_thisSession.showCursors(g);
    m_thisSession.showTDataMap(g);
  }

  public void paint(Graphics g, int absX, int absY) {
    draw(g, absX, absY);
    
    Rectangle r = getBounds();
    //System.out.println("paint " + this + " " + r);
    // set bounds for children
    // Special case the piled layout
    // we pass dimensions here as the pile displayed is not the real
    // layout but a pseudo visual representation. So when this is 
    // called recursively, we would need to use the "false" coordinates
    if (m_layout instanceof DEVisePileLayout) {
      paintChildrenPiled(g, absX, absY);
      return;
    } 
    //System.out.println("doing children of " + this);
    Insets insets = getInsets();
    double parentX = m_view.getX();
    double parentY = m_view.getY();
    double parentW = m_view.getWidth();
    double parentH = m_view.getHeight();
    
    // calculate actual bounds where children can be drawn
    int newParentX = absX + insets.left;
    int newParentY = absY + insets.top;
    int newParentW = r.width - insets.left - insets.right;
    int newParentH = r.height - insets.top - insets.bottom;

    Component comp[] = getComponents();
    int count = getComponentCount();
    // to get represented dimensions of child,  scale the actual dimensions 
    // for the child by the ratio of represented to actual dimensions of the parent
    for (int i = 0; i < count; i++) {
      DEViseViewPeer childP = (DEViseViewPeer)comp[i];
      DEViseView child = childP.getDEViseView();
      double relXRatio = (double)child.getX() / (double)parentW;
      double relYRatio = (double)child.getY() / (double)parentH;
      System.out.println("relXRatio = " + relXRatio + 
                         " relYRatio = " + relYRatio); 
      int childX = (int)(newParentX + newParentW * relXRatio);
      int childY = (int)(newParentY + newParentH * relYRatio);
      int childW = (int)(newParentW * child.getWidthRatio());
      int childH = (int)(newParentH * child.getHeightRatio());
      
      childP.setBounds(new Rectangle(childX, childY, 
                                     childW, childH));
      childP.paint(g, childX, childY);
    //  childP.repaint();
    }
  }

  private void paintChildrenPiled(Graphics g, int absX, int absY) {
    int count = getComponentCount();
    Component comp[] = getComponents();

    Insets insets = getInsets();
    Rectangle r = getBounds();

    int childWidth = r.width  - insets.left - insets.right 
                    - ((count - 1) * X_PILEOFFSET);
    int childHeight = r.height  - insets.top - insets.bottom 
                    - ((count - 1) * Y_PILEOFFSET);
    for (int i = count - 1; i >=0; i--) {
      DEViseViewPeer view = (DEViseViewPeer)comp[i];
      int relX = insets.left + X_PILEOFFSET * i;
      int relY = insets.top + Y_PILEOFFSET * i;
      view.setBounds(new Rectangle(absX + relX, 
                                   absY + relY, childWidth, childHeight));
      view.paint(g, absX + relX, absY + relY);
    }
  }

  public void draw(Graphics g, int absX, int absY) {
    Rectangle r = getBounds();
    Color oldclr = g.getColor();
    g.setColor(m_view.getBgColor().getColor());
    g.fillRect(absX + 1, absY + 1, r.width - 1, r.height - 1);
    if (m_state == Selectable.UNSELECTED) {
      g.setColor(m_view.getFgColor().getColor());
      g.drawRect(absX, absY, r.width - 1, r.height - 1);
    } else {
      g.setColor(HIGHLIGHTCLR);
      int h = r.height;
      int w = r.width;
      g.setColor(HIGHLIGHTCLR);
      int x[] = new int[] { 
        absX, 
        absX,
        absX + w, 
        absX + w, 
        absX,  
        absX + THICKNESS, 
        absX + THICKNESS, 
        absX + w - THICKNESS,  
        absX + w - THICKNESS,
        absX + THICKNESS
      };
      int y[] = new int[] {
        absY, 
        absY + h, 
        absY + h, 
        absY, 
        absY, 
        absY + THICKNESS, 
        absY + h - THICKNESS, 
        absY + h - THICKNESS, 
        absY + THICKNESS, 
        absY + THICKNESS
      };
      g.fillPolygon(x, y, 10);
    }
    g.setColor(oldclr);
  } 

  public void setBounds(Rectangle r) {
    System.out.println("Setting bounds for " + this + " to " + r);
    super.setBounds(r);
  }
  public Insets getInsets() {
    return new Insets(3,3,3,3);
  }
  /* 
  public Rectangle getScaledBounds() {
    Insets insets = getParent().getInsets();
    int x = (int)(m_view.getX() * LMgr.getGlobalScaleX()) + insets.top;
    int y = (int)(m_view.getY() * LMgr.getGlobalScaleY()) + insets.left;
    int width = (int)(m_view.getWidth() * LMgr.getGlobalScaleX());
    int height = (int)(m_view.getHeight() * LMgr.getGlobalScaleY());
    return new Rectangle(x, y, width, height);
  }

  public void paint(Graphics g) {
    if (m_view.getParent() == null) {
      setBounds(getScaledBounds());
    } 
    mypaint(g);
    if (m_view.getParent() == null) {
      m_thisSession.showVisualLinks(g);
      m_thisSession.showRecordLinks(g);
      m_thisSession.showCursors(g);
    }
  }
  public void mypaint(Graphics g) {
    draw(g);
    // Special case the piled layout
    // we pass dimensions here as the pile displayed is not the real
    // layout but a pseudo visual representation. So when this is 
    // called recursively, we would need to use the "false" coordinates
    if (m_layout instanceof DEVisePileLayout) {
      paintChildrenPiled();
      return;
    }

    Rectangle r = getBounds();
    Insets insets = getInsets();
    double parentX = m_view.getX();
    double parentY = m_view.getY();
    double parentW = m_view.getWidth();
    double parentH = m_view.getHeight();
    
    // calculate actual bounds where children can be drawn
    int newParentX = insets.left;
    int newParentY = insets.top;
    int newParentW = r.width - insets.left - insets.right;
    int newParentH = r.height - insets.top - insets.bottom;

    Component comp[] = getComponents();
    
    int count = getComponentCount();
    // to get represented dimensions of child,  scale the actual dimensions 
    // for the child by the ratio of represented to actual dimensions of the parent
    for (int i = 0; i < count; i++) {
      DEViseViewPeer childP = (DEViseViewPeer)comp[i];
      DEViseView child = childP.getDEViseView();
      double relXRatio = (double)child.getX() / (double)parentW;
      double relYRatio = (double)child.getY() / (double)parentH;
      System.out.println("relXRatio = " + relXRatio + 
                         " relYRatio = " + relYRatio); 
      int childX = (int)(newParentX + newParentW * relXRatio);
      int childY = (int)(newParentY + newParentH * relYRatio);
      int childW = (int)(newParentW * child.getWidthRatio());
      int childH = (int)(newParentH * child.getHeightRatio());
      
      childP.setBounds(new Rectangle(childX, childY, childW, childH));
      childP.mypaint(g);
    //  childP.repaint();
    }
  }

  public void draw(Graphics g) {
    Rectangle r = getBounds();
    Color oldclr = g.getColor();
    //g.setColor(m_view.getBgColor().getColor());
    //g.fillRect(r.x + 1, r.y + 1, r.width - 1, r.height - 1);
    if (m_state == Selectable.UNSELECTED) {
      g.setColor(m_view.getFgColor().getColor());
    //  g.drawRect(0, 0, r.width - 1, r.height - 1);
      g.drawRect(r.x, r.y, r.width - 1, r.height - 1);
    } else {
      int h = r.height - 1;
      int w = r.width - 1;
      g.setColor(HIGHLIGHTCLR);
      int x[] = new int[] { 
        0, 0, w, w, 0,  THICKNESS,  THICKNESS,  w - THICKNESS,  w - THICKNESS,
        THICKNESS
      };
      int y[] = new int[] {
        0, h, h, 0, 0,  THICKNESS, h - THICKNESS, h - THICKNESS, THICKNESS, 
        THICKNESS
      };
      g.fillPolygon(x, y, 10);
    }
    System.out.println("Drawing rectangle:" + r);
    //g.drawRect(r.x, r.y, r.width - 1, r.height - 1);
    g.setColor(oldclr);
    g.dispose();
  } 
  private void paintChildrenPiled() {
    int count = getComponentCount();
    Component comp[] = getComponents();

    Insets insets = getInsets();
    Rectangle r = getBounds();

    int childWidth = r.width  - insets.left - insets.right 
                    - ((count - 1) * X_PILEOFFSET);
    int childHeight = r.height  - insets.top - insets.bottom 
                    - ((count - 1) * Y_PILEOFFSET);
    for (int i = count - 1; i >=0; i--) {
      DEViseViewPeer view = (DEViseViewPeer)comp[i];
      int relX = insets.left + X_PILEOFFSET * i;
      int relY = insets.top + Y_PILEOFFSET * i;
      view.setBounds(new Rectangle(relX, relY, childWidth, childHeight));
      view.repaint();
    } 
  }
  */

  public void exchange(DEViseViewPeer child1, DEViseViewPeer child2) {
    int child1Index = -1;
    int child2Index = -1;
    int count = getComponentCount();
    Component comp[] = getComponents();
    for (int i = 0; i < count; i++) {
      if (comp[i] == child1) {
        child1Index = i;
      } else if (comp[i] == child2) {
        child2Index = i;
      }
    }
    /*
    comp[child1Index] = child2;
    comp[child2Index] = child1;
    */
    System.out.println("!!!!!!!!!!!!!!!!!!!!!child1Index = " + child1Index);
    System.out.println("!!!!!!!!!!!!!!!!!!!!!child2Index = " + child2Index);

    // Weird way to do a swap but is necessary :)
    remove(child1);
    remove(child2);
    if (child1Index < child2Index) {
      add(child2, child1Index);
      add(child1, child2Index);
    } else {
      add(child1, child2Index);
      add(child2, child1Index);
    }
    m_view.exchange(child1.m_view, child1Index, 
                    child2.m_view, child2Index);
  }
  

  // first -> last, second -> first
  public void flipBackward() {
    int count = getComponentCount();
    Component comp[] = getComponents();
    if (count == 0) {
      return;
    }
    DEViseViewPeer first = (DEViseViewPeer)comp[0];
    for (int i = 0; i < count - 1; i++) {
      comp[i] = comp[i+1];
    }
    comp[count - 1] = first;
    m_view.flipBackward();
  }
  public void flipForward() {
    int count = getComponentCount();
    Component comp[] = getComponents();
    if (count == 0) {
      return;
    }
    DEViseViewPeer last = (DEViseViewPeer)comp[count - 1];
    for (int i = count - 1; i > 0 ; i--) {
      comp[i] = comp[i - 1];
    }
    comp[0] = last;
    m_view.flipForward();
  }
  
  /* Event handlers */
  private class MouseHandler extends MouseAdapter {
    public void mousePressed(MouseEvent e) {
      if (e.isAltDown() || e.isMetaDown()) {
        /*
	      System.out.println("Mouse Press " + e.getSource() + ", " + e.getPoint());
	      Rectangle r = getBounds();
	      Graphics g = getGraphics();
        g.setColor(m_view.getFgColor().getColor());
	      int x = r.width / 2;
	      int y = r.height / 2;
	      g.drawString(m_view.getName(), x, y);
	      g.dispose();
        */
        m_popupMenu.show((Component)e.getSource(), e.getX(), e.getY());
      }
    }
    public void mouseReleased(MouseEvent e) {
      repaint();
    }
  }
  public String toString() {
    return m_view.getName();
  }
  public void setState(int stateToSet) {
    m_state = stateToSet;
    repaint();
  }
  public int getState() {
    return m_state;
  }
  public void rename(String newname) {
    m_thisSession.getCurrentSession().myViews().rename(m_view.getName(), newname);
  }

  public void getViews(DList list) {
    list.add(this);
    int count = getComponentCount();
    Component comp[] = getComponents();
    for (int i = 0; i < count; i++) {
      ((DEViseViewPeer)comp[i]).getViews(list);
    }
    /*
    for (DListIterator d = list.begin(); d.hasMoreElements();  ) {
      System.out.println(d.nextElement());
    }
    */
  } 
  /*
  public void getLeafViews(DList list) {
    int count = getComponentCount();
    if (count == 0) {
      list.add(this);
    } else {
      Component comp[] = getComponents();
      for (int i = 0; i < count; i++) {
        ((DEViseViewPeer)comp[i]).getLeafViews(list);
      }
    }
  } 
  */
}
