/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseLayoutPeer.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.awt.*;
import java.lang.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>DEViseLayoutPeer</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DEViseLayoutPeer 
             extends Object implements LayoutManager2
{
  /* Constructor */
  public DEViseLayoutPeer() {
    super();
  }

  /* Public methods */
  // adds the component to the layout
  public void addLayoutComponent(String name, Component comp) {
  }
  // removes the component from the layout
  public void removeLayoutComponent(Component comp) {
  }
  public void layoutContainer(java.awt.Container parent) {
    //System.out.println("layoutContainer");
    if (parent instanceof DEViseViewPeer) {
      mapTree((DEViseViewPeer)parent);
    } else {
      throw new IllegalArgumentException("This layout manager can be used only for container which are instances of DEViseViewPeer class");
    }
  }
  public Dimension minimumLayoutSize(java.awt.Container parent) {
    if (parent instanceof DEViseViewPeer) {
      return parent.getMinimumSize();
    } else {
      throw new IllegalArgumentException("This layout manager can be used only for container which are instances of DEViseViewPeer class");
    }
  }
  public Dimension preferredLayoutSize(java.awt.Container parent) {
    if (parent instanceof DEViseViewPeer) {
      return parent.getPreferredSize();
    } else {
      throw new IllegalArgumentException("This layout manager can be used only for container which are instances of DEViseViewPeer class");
    }
  }
  public Dimension maximumLayoutSize(java.awt.Container parent) 
                   throws IllegalArgumentException {
    if (parent instanceof DEViseViewPeer) {
      return parent.getMaximumSize();
    } else {
      throw new IllegalArgumentException("This layout manager can be used only for container which are instances of DEViseViewPeer class");
    }
  }
  public void addLayoutComponent(Component comp, Object constraints) {
  } 
  public float getLayoutAlignmentX(java.awt.Container parent) {
    System.out.println("getAlignmentX : Not implemented");
    return 0.5f;
  }
  public float getLayoutAlignmentY(java.awt.Container parent) {
    System.out.println("getAlignmentY : Not implemented");
    return 0.5f;
  }
  public void invalidateLayout(java.awt.Container parent) {
    System.out.println("invalidateLayout: " + parent);
  }
  private static void mapTree(DEViseViewPeer view) {
    view.getDEViseLayout().mapChildren(view.getDEViseView());
    int count = view.getComponentCount();
    Component comp[] = view.getComponents();
    for (int i = 0; i < count; i++) {
      DEViseViewPeer child = (DEViseViewPeer)comp[i];
      mapTree(child);
    }
  }
}
