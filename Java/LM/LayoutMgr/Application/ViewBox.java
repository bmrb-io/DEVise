/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ViewBox.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import COM.objectspace.jgl.*;
import LayoutEditor.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>ViewBox</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ViewBox extends Box
{
  private DEViseView m_view;
  /* Constructor */
  public ViewBox(DEViseView view, Rectangle rect) {
    super(rect);
    m_view = view;
  }
  public ViewBox(DEViseView view, Box b) {
    super();
    node = b.node;
    edge = b.edge;
    for (int i = 0; i < 4; i++) {
      edge[i].setContainer(this);
    }
    m_view = view;
  }

  public DEViseView getView() { return m_view; }
}
