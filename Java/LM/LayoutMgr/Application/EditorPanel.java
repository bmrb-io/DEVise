/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class EditorPanel.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import gjt.*;

// Class
/**
 * The <CODE>EditorPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class EditorPanel extends Panel
{
  private LMgr m_theApp; // the layout manager app this is part of
  DEViseEditCanvas m_canvas;
  MenuMaker  m_menuMaker;  // stuff that creates menus
  ToolBarMaker m_toolbarMaker; // stuff that creates toolbars

  MenuBar    m_menuBar;  // the menu bar for the editor
  NiceToolbar m_editToolBar; 
  NiceToolbar m_viewToolBar;
  private int m_width;
  private int m_height;

  /* Constructor */
  public EditorPanel(LMgr theApp, int width, int height) {
    super(new BorderLayout());
    m_theApp = theApp;
    ScrollPane scrollPane = new ScrollPane();
    scrollPane.setSize(width, height);
    m_canvas = new DEViseEditCanvas(theApp, width, height);

    scrollPane.add(m_canvas);
    
    m_menuMaker = new MenuMaker(m_canvas, "DEViseEditCanvas.menus");
                                                              // create menubar
    m_toolbarMaker = new ToolBarMaker(m_canvas, "DEViseEditCanvas.toolbars"); 
                                                             // create toolbars
    m_editToolBar = m_toolbarMaker.getToolBarByName("Edit");
    m_viewToolBar = m_toolbarMaker.getToolBarByName("View");

    m_canvas.addMouseListener(new MouseAdapter() {
    // Raise toolbar button for New view 
      public void mouseReleased(MouseEvent e) {
        if (m_canvas.getMode() == DEViseEditCanvas.EDITLAYOUT) {
          ((DualModeStickyImageButton)m_viewToolBar.getButtonByName("New")).raise();  
        }
      }
    });
    add(scrollPane, "Center");
    add(m_editToolBar, "South");
    add(m_viewToolBar, "North");
  }

  /* Public methods */
  /*
  public Dimension getPreferredSize() {
    return new Dimension(m_width, m_height);
  }
  */
  public void createMenuBar(MenuBar menuBar) {
    int count = menuBar.getMenuCount();
    for (int i = 0; i < count; i++) {
      menuBar.remove(0);
    }
    m_menuMaker.createMenuBar(menuBar, null);
  }
  public DEViseEditCanvas editCanvas() { return m_canvas; }
  public void Init(DEViseViewPeer currentView) { 
    m_canvas.Init(currentView);
  }
}
