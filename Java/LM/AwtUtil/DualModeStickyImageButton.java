/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DualModeStickyImageButton.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

// Class
/**
 * The <CODE>DualModeStickyImageButton</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DualModeStickyImageButton 
             extends ImageButton
{
  DualModeActionListener m_actionListener;
  private static final int SINGLE_MODE = 0;
  private static final int MULTI_MODE = 1;
  
  private int m_mode = SINGLE_MODE;
  public boolean inSingleMode() { return m_mode == SINGLE_MODE; }
  public boolean inMultiMode() { return m_mode == MULTI_MODE; }
  public void setSingleMode() { m_mode = SINGLE_MODE; }
  public void setMultiMode() { m_mode = MULTI_MODE; }
  
  public DualModeStickyImageButton() {
    super();
  }
  public DualModeStickyImageButton(Image image, int thickness) {
    super(image, thickness);
  }
  public DualModeStickyImageButton(Image image, int thickness, ImageButtonListener listener) {
    super(image, thickness, listener);
  }
  public void setListener(ImageButtonListener listener) {
    if (!(listener instanceof DualModeStickyImageButtonListener)) {
      System.out.println("DualModeStickyImageButtonListener required");
    } else {
      super.setListener(listener);
    }
  }
  public void addActionListener(ActionListener listener) {
    if (!(listener instanceof DualModeActionListener)) {
      System.out.println("DualModeActionListener required");
    } else {
      super.addActionListener(listener);
      m_actionListener = (DualModeActionListener)listener;
    }
  }
  public void raise() {
    if (inSingleMode() && !isRaised()) {
      paintRaised();
      m_actionListener.doRaised();
    }
  }
}
