/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseMappingPeer.java
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
 * The <CODE>DEViseMappingPeer</CODE> class
 *
 * @author  Shilpa Lawande
 */

// A class that encapsulates all the code for indicating a mapping
// for a view

public class DEViseMappingPeer implements Selectable
{
  private DEViseMapping m_mapping;
  private DEViseSessionPeer m_session;
  private int m_state;

  /* Constructor */
  public DEViseMappingPeer(DEViseSessionPeer session, DEViseMapping mapping) {
    m_mapping = mapping;
    m_session = session;
    m_state = Selectable.UNSELECTED;
  }

  public DEViseMapping getDEViseMapping() { return m_mapping; }

  public void paint(Graphics g, int x, int y, Color color) {
    if (m_state == SELECTED) {
      g.setColor(color);
      g.drawString(m_mapping.getName(), x, y);
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
