/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ImageItemPanel.java
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
 * The <CODE>ImageItemPanel</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ItemPanel extends Panel 
{
  /* Test code */
  public static void main(String args[]) {
    Frame f = new Frame();
    f.setSize(150, 700);
    
    ItemPanel ip = new ItemPanel(100,700);
    f.add(ip, "Center");
    for (int i = 0; i < 10; i++) {
      Image img = ip.createImage(ImageItem.getImageProducer("/u/s/s/ssl/Work/LM/Icons/gifs/RLink.gif"));
      EditableImageItem item 
        = new EditableImageItem(img, "Image" + i, 20, 20);
      ip.add(item);
      item.addTextListener(new TextListener() {
        public void textValueChanged(TextEvent e) {
          System.out.println("text value changed to " 
                            + ((TextComponent)e.getSource()).getText());
        }
      });
    }
    f.pack();
    f.show();
  }
  protected ItemSelector m_selector;
  int m_width, m_height;
  public ItemPanel(int width, int height) {
    super(new ColumnLayout());
    m_width = width;
    m_height = height;
    m_selector = new ItemSelector(this);
    m_selector.enableSelectorInterface(this);
    setSize(width, height);
  }  
  public ItemSelector getSelector() { return m_selector; } 
  public Dimension getPreferredSize() {
    return new Dimension(m_width, m_height);
  }
  public void addImpl(Component comp, Object constraints, int index) {
    super.addImpl(comp, constraints, index);
    m_selector.enableSelectorInterface(comp);
    comp.invalidate();
    validate();
    repaint();
  }

  public void remove(Component comp){ 
    m_selector.setStateOne((Selectable)comp, Selectable.UNSELECTED);
    super.remove(comp);
    m_selector.disableSelectorInterface(comp);
    validate();
    repaint();
  }
}
