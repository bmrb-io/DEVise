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
import java.util.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>TDataPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class TDataPanel extends PanelCard
{
  public static final String TABLE_GIF= "/u/s/s/ssl/Work/LM/Icons/gifs/TData.gif";

  public static final int DEFAULT_WD = 20;
  public static final int DEFAULT_HT = 20;
  private Image m_image;

  /* Constructor */
  public TDataPanel(LMgr theApp, int width, int height) {
    super(theApp, "Active TDatas", width, height);
    m_image
      = createImage(ImageItem.getImageProducer(TABLE_GIF));
    m_itemPanel.getSelector().addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        Object obj = e.getItem();
        if (!(obj instanceof TDataItem)) {
          return;
        }
        TDataItem item = (TDataItem)obj;
        if (e.getStateChange() == ItemEvent.SELECTED) {
          item.m_tdata.select();
        } else {
          item.m_tdata.unselect();
        }
      }
    }); 
  }
  /* Public methods */
  public void addItem(DEViseTDataPeer tdataPeer) {
    TDataItem item = new TDataItem(tdataPeer, DEFAULT_WD, DEFAULT_HT);
    m_itemPanel.add(item);
  }

  public TDataItem find(DEViseTData tdata) {
    for (Enumeration e = m_itemPanel.getSelector().componentList(); 
         e.hasMoreElements(); ) {
      TDataItem item = (TDataItem)e.nextElement();
      if (item.m_tdata.getDEViseTData() == tdata) {
        return item;
      }
    }
    return null; 
  }

  public void delItem(DEViseTData tdata) {
    m_itemPanel.remove(find(tdata));
  }

  public Object[] getSelectedTDatas() {
    Object obj[] = m_itemPanel.getSelector().getSelectedObjects();
    if (obj == null) {
      return null;
    }
    for (int i = 0; i < obj.length; i++) {
      System.out.println(obj[i]);
      obj[i] = ((TDataItem)obj[i]).m_tdata.getDEViseTData();
    }
    return obj;
  }

  private class TDataItem extends ImageItem {
    public DEViseTDataPeer m_tdata;
    TDataItem(DEViseTDataPeer tdataPeer, int width, int height) {
      super(TDataPanel.this.m_image, tdataPeer.getDEViseTData().getName(), width, height);
      m_tdata = tdataPeer;
    }
  }
}
