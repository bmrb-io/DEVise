/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class VisualLinkPanel.java
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
 * The <CODE>VisualLinkPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class VisualLinkPanel extends PanelCard
{
  public static final String XX_IMAGE_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/XLink.gif";
  public static final String XY_IMAGE_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/XYLink.gif";
  public static final String YY_IMAGE_GIF = "/u/s/s/ssl/Work/LM/Icons/gifs/YLink.gif";

  public static final int DEFAULT_WD = 20;
  public static final int DEFAULT_HT = 20;

   private Image m_image[]= new Image[3]; 

  /* Constructor */
  public VisualLinkPanel(LMgr theApp, int width, int height) {
    super(theApp, "Visual Links", width, height); 
    m_image[DEViseVisualLink.DEVISE_XX_LINK] 
      = createImage(ImageItem.getImageProducer(XX_IMAGE_GIF));
    m_image[DEViseVisualLink.DEVISE_XY_LINK] = 
      createImage(ImageItem.getImageProducer(XY_IMAGE_GIF));
    m_image[DEViseVisualLink.DEVISE_YY_LINK] = 
      createImage(ImageItem.getImageProducer(YY_IMAGE_GIF));
    addItem("X-X Link", DEViseVisualLink.DEVISE_XX_LINK);
    addItem("Y-Y Link", DEViseVisualLink.DEVISE_YY_LINK);
    m_itemPanel.getSelector().addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        Object obj = e.getItem();
        if (!(obj instanceof VisualLinkItem)) {
          return;
        }
        VisualLinkItem item = (VisualLinkItem)obj;
        if (e.getStateChange() == ItemEvent.SELECTED) {
          DEViseVisualLinkPeer.select(item.m_type);
          m_theApp.displayPanel().repaint();
        } else {
          DEViseVisualLinkPeer.unselect(item.m_type);
          m_theApp.displayPanel().repaint();
        }
      }
    });
  }
  /* Public methods */
  public void addItem(String label, int type) {
    VisualLinkItem item = new VisualLinkItem(type, label, 
                                             DEFAULT_WD, DEFAULT_HT);
    m_itemPanel.add(item);
  }
  public Object[] getSelectedLinks() {
    return m_itemPanel.getSelector().getSelectedObjects();
  }
  public int getType(Object item) {
    return ((VisualLinkItem)item).m_type;
  }

  private class VisualLinkItem extends ImageItem{
    public int m_type;
    VisualLinkItem(int type, String label, int width, int height) {
      super(VisualLinkPanel.this.m_image[type], label, width, height);
      m_type = type;
    }
  }
}
