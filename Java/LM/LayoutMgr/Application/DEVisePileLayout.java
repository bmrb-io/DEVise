/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEVisePileLayout.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.io.*;
import COM.objectspace.jgl.*;
import Util.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>DEVisePileLayout</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class DEVisePileLayout extends DEViseLayout
{
  private DEViseLayout m_origLayout; // layout to use on unpiling

  /* Constructor */
  public DEVisePileLayout() {
    m_origLayout = new DEViseGridLayout();
  }
  public DEVisePileLayout(DEViseLayout oldLayout) {
    m_origLayout = oldLayout;
  }
  public DEViseLayout getOrigLayout() {
    return m_origLayout;
  }
  /* Interface implementation */
  public void mapChildren(DEViseView parent) {
    //System.out.println("mapChildren(" + parent.getName() + ")");
    for (DListIterator l = parent.childList(); 
         l.hasMoreElements(); ) {
      DEViseView child = (DEViseView)l.nextElement();
      child.setX(0); 
      child.setY(0);
      child.setWidthRatio(1);
      child.setHeightRatio(1);
      System.out.println(child);
    }
  }
  public void readAscii(ReadObjectInput stream) throws IOException {
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
  }
}

