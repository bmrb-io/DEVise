/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseTDataPeer.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import LayoutMgr.MetaData.*;
import AwtUtil.*;

// Class
/**
 * The <CODE>DEViseTDataPeer</CODE> class
 *
 * @author  Shilpa Lawande
 */

// A class that encapsulates all the code for indicating a tdata
// for a view

public class DEViseTDataPeer implements Selectable
{
  private DEViseTData m_tdata;
  private DEViseSessionPeer m_session;
  private int m_state;

  /* Constructor */
  public DEViseTDataPeer(DEViseSessionPeer session, DEViseTData tdata) {
    m_tdata = tdata;
    m_session = session;
    m_state = Selectable.UNSELECTED;
  }

  public DEViseTData getDEViseTData() { return m_tdata; }

 
  public void paint(Graphics g, int x, int y, Color color) {
    if (m_state == SELECTED) {
      g.setColor(color);
      g.drawString(m_tdata.getName(), x, y);
    }
  } 
  public void setState(int stateToSet) {
    m_state = stateToSet;
  }
  public int getState() {
    return m_state;
  }
  public void select() {
    setState(Selectable.SELECTED);
    m_session.repaint();
  }
  public void unselect() {
    setState(Selectable.UNSELECTED);
    m_session.repaint();
  }
}
