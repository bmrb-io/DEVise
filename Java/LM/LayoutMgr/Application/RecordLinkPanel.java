/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class RecordLinkPanel.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import LayoutMgr.MetaData.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>RecordLinkPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class RecordLinkPanel extends PanelCard
{
  public static final String RLINK_GIF= "/u/s/s/ssl/Work/LM/Icons/gifs/RLink.gif";

  public static final int DEFAULT_WD = 20;
  public static final int DEFAULT_HT = 20;

  private Image m_image;

  /* Constructor */
  public RecordLinkPanel(LMgr theApp, int width, int height) {
    super(theApp, "Record Links", width, height);
    m_image = createImage(ImageItem.getImageProducer(RLINK_GIF));
    m_itemPanel.getSelector().addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        Object obj = e.getItem();
        if (!(obj instanceof RecordLinkItem)) {
          return;
        }
        RecordLinkItem item = (RecordLinkItem)obj;
        if (e.getStateChange() == ItemEvent.SELECTED) {
          item.m_link.select();
        } else {
          item.m_link.unselect();
        }
      }
    });
  }

  /* Public methods */
  public void addItem(DEViseRecordLinkPeer linkPeer) {
    RecordLinkItem item = new RecordLinkItem(linkPeer, DEFAULT_WD, DEFAULT_HT);
    m_itemPanel.add(item);
  }

  public RecordLinkItem find(DEViseRecordLink link) {
    for (Enumeration e = m_itemPanel.getSelector().componentList(); 
         e.hasMoreElements(); ) {
      RecordLinkItem item = (RecordLinkItem)e.nextElement();
      if (item.m_link.getDEViseRecordLink() == link) {
        return item;
      }
    }
    return null;
  }

  public void delItem(DEViseRecordLink link) {
    m_itemPanel.remove(find(link));
  }
  /*
  public Insets getInsets() {
    return new Insets(30, 15, 15, 15);
  }
  */ 
  public Object[] getSelectedLinks() {
    Object obj[] = m_itemPanel.getSelector().getSelectedObjects();
    if (obj == null) {
      return null;
    }
    for (int i = 0; i < obj.length; i++) {
      System.out.println(obj[i]);
      obj[i] = ((RecordLinkItem)obj[i]).m_link.getDEViseRecordLink();
    }
    return obj;
  }
  private class RecordLinkItem extends EditableImageItem {
    public DEViseRecordLinkPeer m_link;
    RecordLinkItem(DEViseRecordLinkPeer linkPeer, int width, int height) {
      super(RecordLinkPanel.this.m_image, linkPeer.getDEViseRecordLink().getName(), 
            width, height);
      m_link = linkPeer;
      addTextListener(new TextListener() {
        public void textValueChanged(TextEvent e) {
          String text = ((TextComponent)e.getSource()).getText();
          m_theApp.getCurrentSession().myRecordLinks().rename(m_link.getDEViseRecordLink().getName(), text); 
        }
      });
    }
  }
}
