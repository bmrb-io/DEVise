/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DualModeStickyImageButtonListener.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

// Class
/**
 * The <CODE>DualModeStickyImageButtonListener</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DualModeStickyImageButtonListener 
             extends StickyImageButtonListener
{
  public void mousePressed(MouseEvent me) {
    System.out.println("mousePressed " + me);
    DualModeStickyImageButton ib = (DualModeStickyImageButton)me.getSource();
    if(!ib.isDisabled()) {
      int clicks = me.getClickCount();
      if (clicks == 1) {
        super.mousePressed(me);
        ib.setSingleMode();
      } else if (clicks == 2) {
        ib.setMultiMode();
        arm(ib);
      }
    }
  }
}
