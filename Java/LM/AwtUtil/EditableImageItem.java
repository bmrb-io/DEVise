/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class EditableImageItem.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.awt.image.*;

// Class
/**
 * The <CODE>EditableImageItem</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class EditableImageItem extends ImageItem
{
  protected TextListener m_textListener;
  private TextField m_text;

  public EditableImageItem(Image image, String label, int imgW, int imgH) {
    super(image, label, imgW, imgH);
    m_textListener = null;

    setLayout(new BorderLayout());
    m_text = new TextField(m_label);
    m_text.setBackground(getBackground());
    m_text.setEditable(false);
    add(m_text, "East");
    m_text.addMouseListener(new MouseHandler());
    m_text.addKeyListener(new KeyHandler());
  }

  public void addTextListener(TextListener l) {
    m_textListener = AWTEventMulticaster.add(m_textListener, l);
  }
  public void removeTextListener(TextListener l) {
    m_textListener = AWTEventMulticaster.remove(m_textListener, l);
  }
  public void paint(Graphics g) {
    if (m_state == Selectable.SELECTED) {
      g.setColor(ItemColors[TEXT_HGLTCLR]);
    } else {
      g.setColor(ItemColors[TEXT_CLR]);
    } 
    g.drawImage(m_image, 5, 10, m_image.getWidth(this), 
                m_image.getHeight(this), this);
    if (m_state == Selectable.SELECTED) {
      g.drawRect(1, 1, m_width - 3, m_height - 3);
    }
  }
  public Insets getInsets() {
    return new Insets(5, 0,5,5);
  }

  class MouseHandler extends MouseAdapter {
    public void mouseClicked(MouseEvent e) {
      if (!e.isAltDown() && !e.isMetaDown() && (e.getClickCount()==2)) {
        m_text.setEditable(true);
      }
    }
  }
  class KeyHandler extends KeyAdapter {
    public void keyPressed(KeyEvent e) {
      if (e.getKeyCode() == KeyEvent.VK_ENTER) {
        String newLabel = m_text.getText();
        if (newLabel.equals("")) {
          m_text.setText(m_label);
          return;
        }
        m_label = newLabel;
        m_text.setEditable(false);
        if (m_textListener != null) {
          TextEvent t = new TextEvent(m_text, 
                                    TextEvent.TEXT_VALUE_CHANGED);
          m_textListener.textValueChanged(t);
        }
      }
    }
  }
}
