/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ItemSelector.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import gjt.*;

// Class
/**
 * The <CODE>ItemSelector</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ItemSelector implements ItemSelectable
{
  boolean m_allowMultiple; // multiple selections allowed
  ItemListener m_listener;
  java.awt.Container m_parent;

  DList m_selectedItems;
  MouseListener m_mouseHandler;

  public ItemSelector(java.awt.Container parent) {
    m_parent = parent;
    m_listener = null;
    m_selectedItems = new DList();
    m_mouseHandler = new MouseHandler();
  }  
  
  class MouseHandler extends MouseAdapter { 
    public void mousePressed(MouseEvent e) {
      /*
      if (e.isAltDown() || e.isMetaDown()) {
        return;
      }
      */
      Object item = e.getSource();
      if (item instanceof Selectable) {
        if (!e.isShiftDown()) {
          selectExactlyOne((Selectable)item);
        } else { 
          setStateOne((Selectable)item, Selectable.SELECTED);
        }
      } else {
        setStateAll(Selectable.UNSELECTED);
      }
      repaint();
    }
    public void mouseReleased(MouseEvent e) {
      /*
      for(DListIterator d = m_selectedItems.begin(); d.hasMoreElements();) {
        System.out.println(((ImageItem)d.nextElement()).getLabel());
      }
      */
    }
  }

  // Itemselectable interface
  public void addItemListener(ItemListener l) {
    m_listener = AWTEventMulticaster.add(m_listener, l);  
  }
  public void removeItemListener(ItemListener l) {
    m_listener = AWTEventMulticaster.remove(m_listener, l);
  }
  public Object[] getSelectedObjects(){
    int numSelections = m_selectedItems.size();
    if (numSelections == 0) {
      return null;
    }
    Object selectedObjs[] = new Object[numSelections];
    int i = 0;
    for (DListIterator d = m_selectedItems.begin(); d.hasMoreElements(); i++){
      selectedObjs[i] = d.nextElement(); 
    }
    return selectedObjs;
  }

  public void setStateOne(Selectable item, int stateToSet) {
    if (item.getState() == stateToSet) {
      return;
    }
    ItemEvent e;
    item.setState(stateToSet);
    if (stateToSet == Selectable.SELECTED) {
      e = new ItemEvent(this, ItemEvent.ITEM_STATE_CHANGED, item, 
                        ItemEvent.SELECTED);
      m_selectedItems.add(item);
    } else {
      e = new ItemEvent(this, ItemEvent.ITEM_STATE_CHANGED, item,
                        ItemEvent.DESELECTED);
      m_selectedItems.remove(item);
    }
    if (m_listener != null) {
      m_listener.itemStateChanged(e);
    }
  }
  public void setStateAll(int stateToSet) {
    for (DListIterator d = m_selectedItems.begin(); d.hasMoreElements();){
      setStateOne((Selectable)d.nextElement(),stateToSet);
    }
  }
  public void selectExactlyOne(Selectable item) {
    for (Enumeration e = componentList(); e.hasMoreElements();) {
      Selectable s = (Selectable)e.nextElement();
      if (s == item) {
        setStateOne(s, Selectable.SELECTED);  
      } else {
        setStateOne(s, Selectable.UNSELECTED);
      }
    }
  }
  public void enableSelectorInterface(Component comp) {
    comp.addMouseListener(m_mouseHandler);
  }
  public void disableSelectorInterface(Component comp) {
    setStateOne((Selectable)comp, Selectable.UNSELECTED);
    comp.removeMouseListener(m_mouseHandler);
  }

  public void addMouseListener(MouseListener l) {
    m_mouseHandler = AWTEventMulticaster.add(m_mouseHandler, l);
  }
  public void removeMouseListener(MouseListener l) {
    m_mouseHandler = AWTEventMulticaster.remove(m_mouseHandler, l);
  }
  // unsafe with delete.
  public Enumeration componentList() {
    return new Enumeration() {
      int index = 0;
      int maxElts = m_parent.getComponentCount();
      public boolean hasMoreElements() {
        return (index < maxElts); 
      }
      public Object nextElement() {
        return m_parent.getComponents()[index++];
      }
    };
  }
  public void repaint() {
    m_parent.repaint();
  }
}
