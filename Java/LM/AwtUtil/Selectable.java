/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Selectable.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;

// Class
/**
 * The <CODE>Selectable</CODE> class
 *
 * @author  Shilpa Lawande
 */

public interface Selectable 
{
  public static final int SELECTED = 0;
  public static final int UNSELECTED = 1;

  public void setState(int stateToSet);
  public int getState();
}
