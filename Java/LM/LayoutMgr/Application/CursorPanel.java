/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class CursorPanel.java
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
 * The <CODE>CursorPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class CursorPanel extends PanelCard
{
  public static final String CURSOR_GIF= "/u/s/s/ssl/Work/LM/Icons/gifs/Cursor.gif";

  public static final int DEFAULT_WD = 20;
  public static final int DEFAULT_HT = 20;
  private Image m_image;

  /* Constructor */
  public CursorPanel(LMgr theApp, int width, int height) {
    super(theApp, "Cursors", width, height);
    m_image = createImage(ImageItem.getImageProducer(CURSOR_GIF));
    m_itemPanel.getSelector().addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        Object obj = e.getItem();
        if (!(obj instanceof CursorItem)) {
          return;
        }
        CursorItem item = (CursorItem)obj;
        if (e.getStateChange() == ItemEvent.SELECTED) {
          item.m_cursor.select();
        } else {
          item.m_cursor.unselect();
        }
      }
    });
  }
  /* Public methods */
  public void addItem(DEViseCursorPeer cursorPeer) {
    CursorItem item = new CursorItem(cursorPeer, DEFAULT_WD, DEFAULT_HT);
    m_itemPanel.add(item);

  }
  public CursorItem find(DEViseCursor cursor) {
    for (Enumeration e = m_itemPanel.getSelector().componentList();
         e.hasMoreElements(); ) {
      CursorItem item = (CursorItem)e.nextElement();
      if (item.m_cursor.getDEViseCursor() == cursor) {
        return item;
      }
    }
    return null;
  }
  public void delItem(DEViseCursor cursor) {
    m_itemPanel.remove(find(cursor));
  }
  /*
  public Insets getInsets() {
    return new Insets(30, 15, 15, 15);
  }
  */

  public Object[] getSelectedCursors() {
    Object  obj[] = m_itemPanel.getSelector().getSelectedObjects();
    if (obj == null) {
      return null;
    }
    for (int i = 0; i < obj.length; i++) {
      obj[i] = ((CursorItem)obj[i]).m_cursor.getDEViseCursor();
    }
    return obj;
  }

  private class CursorItem extends EditableImageItem {
    public DEViseCursorPeer m_cursor;
    public CursorItem(DEViseCursorPeer cursorPeer, int width, int height) {
      super(CursorPanel.this.m_image, cursorPeer.getDEViseCursor().getName(), 
            width, height);
      m_cursor = cursorPeer;
      addTextListener(new TextListener() {
        public void textValueChanged(TextEvent e) {
          String text = ((TextComponent)e.getSource()).getText();
          m_theApp.getCurrentSession().myCursors().rename(m_cursor.getDEViseCursor().getName(), text); 
        }
      });
    }
  }
}
