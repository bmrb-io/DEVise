/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseOrient.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;

// Class
/**
 * The <CODE>DEViseOrient</CODE> class is a place holder class to handle
 * orientation information.
 * @author  Shilpa Lawande
 */

public class DEViseOrient 
{
  private double m_angle;
  public DEViseOrient(double angle) {
    m_angle = angle;
  }
  public double getAngle() { return m_angle; }
  public String toString() {
    return "Orientation: " + m_angle;
  }
}
