/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class PanelCard.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>PanelCard</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class PanelCard extends Panel
{
  protected LMgr m_theApp;
  protected ScrollPane m_scrollPane;
  protected ItemPanel m_itemPanel;

  /* Constructor */
  public PanelCard(LMgr theApp, String title, int width, int height) {
    super(new BorderLayout());
    m_itemPanel = new ItemPanel(width,height);
    m_scrollPane = new ScrollPane();
    m_scrollPane.setSize(width, height);
    m_scrollPane.add(m_itemPanel);
    Adjustable vadjustable = m_scrollPane.getVAdjustable();
    if (vadjustable != null) {
      vadjustable.setBlockIncrement(1);
      vadjustable.setUnitIncrement(1);
    }
    Adjustable hadjustable = m_scrollPane.getHAdjustable();
    if (hadjustable != null) {
      hadjustable.setBlockIncrement(1);
      hadjustable.setUnitIncrement(1);
    }
    Label titleL = new Label(title);
    add(titleL, "North");
    add(m_scrollPane, "Center");
    m_theApp = theApp;
  }
}
