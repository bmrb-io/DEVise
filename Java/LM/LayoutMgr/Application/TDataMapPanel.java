/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class TDataPanel.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>TDataPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class TDataMapPanel extends Panel
{
  private LMgr m_theApp;
  private TDataPanel m_tdataPanel;
  private MappingPanel m_mapPanel;

  /* Constructor */
  public TDataMapPanel(LMgr theApp, int width, int height) {
    super(new GridLayout());
    m_tdataPanel = new TDataPanel(theApp, 2 * width/3, height);
    m_mapPanel = new MappingPanel(theApp, 2 * width/3, height);
    add(m_tdataPanel);
    add(m_mapPanel);
    m_theApp = theApp;
  }
  public TDataPanel tdataPanel() { return m_tdataPanel; }
  public MappingPanel mappingPanel() { return m_mapPanel; }
}

