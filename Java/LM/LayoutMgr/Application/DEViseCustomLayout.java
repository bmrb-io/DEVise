/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseCustomLayout.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.io.*;
import Util.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>DEViseCustomLayout</CODE> class
 *
 * @author  Shilpa Lawande
 */

// This class actually does nothing!! However, we do want to maintain
// DEViseLayout as the interface in case we want to do some auto stuff
// in the custom layout!
public class DEViseCustomLayout extends DEViseLayout
{
  /* Constructor */
  public DEViseCustomLayout() {
  }
  /* Interface implementation */
  public void mapChildren(DEViseView parent) {
  }
  public void readAscii(ReadObjectInput stream) throws IOException {
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
  }
}
