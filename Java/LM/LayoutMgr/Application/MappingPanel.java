/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class MappingPanel.java
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
 * The <CODE>MappingPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class MappingPanel extends PanelCard
{
  public static final String MAPPING_GIF= "/u/s/s/ssl/Work/LM/Icons/gifs/Mapping.gif";
  public static final int DEFAULT_WD = 20;
  public static final int DEFAULT_HT = 20;

  Image m_image;

  /* Constructor */
  public MappingPanel(LMgr theApp, int width, int height) {
    super(theApp, "Mappings", width, height);
    m_image
      = createImage(ImageItem.getImageProducer(MAPPING_GIF));
    m_itemPanel.getSelector().addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        Object obj = e.getItem();
        if (!(obj instanceof MappingItem)) {
          return;
        }
        MappingItem item = (MappingItem)obj;
        if (e.getStateChange() == ItemEvent.SELECTED) {
          item.m_mapping.select();
        } else {
          item.m_mapping.unselect();
        }
      }
    });
  }
  /* Public methods */
  public void addItem(DEViseMappingPeer mappingPeer) {
    MappingItem item = new MappingItem(mappingPeer, DEFAULT_WD, DEFAULT_HT);
    m_itemPanel.add(item);
  }
  private MappingItem find(DEViseMapping mapping) {
    for (Enumeration e = m_itemPanel.getSelector().componentList();
         e.hasMoreElements(); ) {
      MappingItem item = (MappingItem)e.nextElement();
      if (item.m_mapping.getDEViseMapping() == mapping) {
        return item;
      }
    }
    return null;
  }

  public void delItem(DEViseMapping mapping) {
    m_itemPanel.remove(find(mapping));
  }
  public Object[] getSelectedMappings() {
    Object  obj[] = m_itemPanel.getSelector().getSelectedObjects();
    if (obj == null) {
      return null;
    }
    for (int i = 0; i < obj.length; i++) {
      obj[i] = ((MappingItem)obj[i]).m_mapping.getDEViseMapping();
    }
    return obj;
  }
  private class MappingItem extends EditableImageItem {
    public DEViseMappingPeer m_mapping;
    public MappingItem(DEViseMappingPeer mappingPeer, int width, int height) {
      super(MappingPanel.this.m_image, mappingPeer.getDEViseMapping().getName(), width, height);
      m_mapping = mappingPeer;
      addTextListener(new TextListener() {
        public void textValueChanged(TextEvent e) {
          String text = ((TextComponent)e.getSource()).getText();
          m_theApp.getCurrentSession().
          myMappings().rename(m_mapping.getDEViseMapping().getName(), text); 
        }
      });
    }
  }
}
