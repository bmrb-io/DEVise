/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class LMError.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>LMError</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class LMError 
{
  /* Public methods */
  public static int report(String msg, int errCode) {
    report(msg);
    return errCode;
  }
  public static void report(String msg) {
    MessageDialog dlg = new MessageDialog("Layout Manager Error",
                                          msg,
                                          MessageDialog.OK, 0);
    dlg.doModal();
  }
}
