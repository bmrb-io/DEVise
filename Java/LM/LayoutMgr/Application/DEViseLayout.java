/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseLayout.java
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
 * The <CODE>DEViseLayout</CODE> class
 *
 * @author  Shilpa Lawande
 */

public abstract class DEViseLayout implements AsciiSerializable
{
  // Calculate size and dimensions for each view and 
  // update the same in the corresponding DEViseView object
  public abstract void mapChildren(DEViseView view);

  public abstract void readAscii(ReadObjectInput stream) throws IOException;
  public abstract void writeAscii(PrintObjectOutput stream) throws IOException;
}
