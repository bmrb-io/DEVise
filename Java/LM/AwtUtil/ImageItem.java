/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ImageItem.java
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
 * The <CODE>ImageItem</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ImageItem extends Container implements Selectable
{
  public static final int TEXT_CLR = 0;
  public static final int TEXT_HGLTCLR = 1;

  protected Image m_image;
  protected String m_label;
  protected int m_state = Selectable.UNSELECTED;
  protected int m_width, m_height;
  protected static Color ItemColors[] = new Color[2];
   
  public ImageItem(Image image, String label, int imgW, int imgH) {
    super();
    m_image = image.getScaledInstance(imgW, imgH, Image.SCALE_AREA_AVERAGING);
    m_label = label;
    m_width = imgW + 20 + label.length() * 10;
    m_height = imgH + 20;
    addMouseListener(new MouseAdapter() {
      public void mouseClicked(MouseEvent e) {
        System.out.println("ImageItem : " + m_label + " " + m_state);
        repaint();
      }
    });
    ItemColors[TEXT_CLR] = Color.black;
    ItemColors[TEXT_HGLTCLR] = Color.blue;
  }
  public void setState(int stateToSet) {
    m_state = stateToSet;
  }
  public int getState() {
    return m_state;
  }
  public String getLabel() {
    return m_label;
  }
  public void paint(Graphics g){
    super.paint(g);
    if (m_state == Selectable.SELECTED) {
      g.setColor(ItemColors[TEXT_HGLTCLR]);
    } else {
      g.setColor(ItemColors[TEXT_CLR]);
    } 
    g.drawImage(m_image, 5, 5, m_image.getWidth(this), 
                m_image.getHeight(this), this);
    if (m_state == Selectable.SELECTED) {
      g.drawRect(1, 1, m_width - 3, m_height - 3);
    }
    g.drawString(m_label, m_image.getWidth(this) + 5,
                 m_image.getHeight(this) + 2);
  }

  public Dimension getPreferredSize() {
    return new Dimension(m_width, m_height);
  }
  /* Public methods */
  public static ImageProducer getImageProducer(String imagePath) {
    URL url;
    try { 
      url = new URL("file:" + imagePath);
    } catch (Exception e) {
      System.out.println("Malformed url");
      return null;
    }
    ImageProducer ip;
    try {
      ip = (ImageProducer)url.getContent();
    } catch (Exception e) {
      System.out.println("Io Exception");
      return null;
    }
    return ip;
  }
}
