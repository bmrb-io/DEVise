/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DisplayPanel.java
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
 * The <CODE>DisplayPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DisplayPanel extends Panel
{
  private Panel m_displayPanel;
  private ToolBarPanel m_toolBarPanel;
  
  /* Constructor */
  public DisplayPanel(LMgr theApp, int width, int height) {
    super(new BorderLayout());
    ScrollPane scrollPane = new ScrollPane();
    m_displayPanel = new InsetPanel();
    m_displayPanel.setSize(width, height);
    scrollPane.add(m_displayPanel);
    m_toolBarPanel = new ToolBarPanel(theApp, "LMgr.toolbars");
    add(m_toolBarPanel, "North");
    scrollPane.setSize(width, height);
    Adjustable vadjustable = scrollPane.getVAdjustable();
    if (vadjustable != null) {
      vadjustable.setBlockIncrement(1);
      vadjustable.setUnitIncrement(1);
    }
    Adjustable hadjustable = scrollPane.getHAdjustable();
    if (hadjustable != null) {
      hadjustable.setBlockIncrement(1);
      hadjustable.setUnitIncrement(1);
    }
    add(scrollPane, "Center");
  }

  /* Public methods */
  public Panel displayPanel() { return m_displayPanel; }
  private class ToolBarPanel extends Panel{
    private LMgr m_theApp;
    private ToolBarMaker m_toolBarMaker;
    private NiceToolbar m_sessionToolBar;
    private NiceToolbar m_viewToolBar;
    private NiceToolbar m_pileToolBar;
    private NiceToolbar m_linkToolBar;
    public ToolBarPanel(LMgr theApp, String toolbarFile) {
      super(new BorderLayout());
      Panel panel = new Panel();
      m_theApp = theApp;
      m_toolBarMaker = new ToolBarMaker(theApp, toolbarFile);
      m_sessionToolBar = m_toolBarMaker.getToolBarByName("Session");
      m_viewToolBar = m_toolBarMaker.getToolBarByName("View");
      m_pileToolBar = m_toolBarMaker.getToolBarByName("Pile");
      m_linkToolBar = m_toolBarMaker.getToolBarByName("Link");
      
      panel.add(m_sessionToolBar);
      panel.add(m_viewToolBar);
      panel.add(m_pileToolBar);
      panel.add(m_linkToolBar);
      add(panel, "Center");
      add(new Separator(), "South");
    }
  }
  private class InsetPanel extends Panel {
    public InsetPanel() {
      super();
    }
    public Insets getInsets() {
      return new Insets(10,10,0,0);
    }
  }
}
