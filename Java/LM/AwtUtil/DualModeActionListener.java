/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DualModeActionListener.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import gjt.*;

// Class
/**
 * The <CODE>DualModeActionListener</CODE> class
 *
 * @author  Shilpa Lawande
 */

public abstract class DualModeActionListener 
                      implements ActionListener
{
  public final void actionPerformed(ActionEvent ae) {
    DualModeStickyImageButton ib = (DualModeStickyImageButton)ae.getSource();
    if (!ib.isRaised()) {
	    if (ib.inSingleMode()) {
	      doSingleModeActivated(); 
	    } else {
	      doMultiModeActivated();
	    } 
    } else {
      doRaised();
    }
  }
  abstract public void doSingleModeActivated();
  abstract public void doMultiModeActivated();
  abstract public void doRaised();
}
