/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseSymbol.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.util.*;

// Class
/**
 * The <CODE>DEViseSymbol</CODE> class is a place holder class to 
 * implement symbol used in a Mapping.
 *
 * @author  Shilpa Lawande
 */

public interface DEViseSymbol 
{
  public int numAttrs();
  public String getSymbolName();
  // Draw routine
  // public void drawSymbol(...);

  /* Public members */
  // The following are sample symbols...notall are implemented here for
  // lack of time. Also the draw routine is not needed for the front end.

  public static final DEViseSymbol RectSymbol = new DEViseRectSymbol();
  public static final DEViseSymbol BarSymbol = new DEViseBarSymbol();

  public static String SYMBOLS[] = { 
    "Rect", "Bar"
  };

  class DEViseRectSymbol implements DEViseSymbol {
    public DEViseRectSymbol() { }
    public String getSymbolName() { return "Rect"; }
    public int numAttrs() { return 2; }
  }
  class DEViseBarSymbol implements DEViseSymbol {
    public DEViseBarSymbol() { }
    public String getSymbolName() { return "Bar"; }
    public int numAttrs() { return 1; }
  }
  // Note : DEViseView should also be made a Symbol by implementing this 
  // interface
}
