/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class AsciiSerializable.java
****************************************************************************/

//Packages 

package Util;

import java.lang.*;
import java.io.*;

// Class
/**
 * The <CODE>AsciiSerializable</CODE> interface 
 *
 * @author  Shilpa Lawande
 */

// an interface to be implemented by any class that is externalizable
// into ascii format. Forces the object output/input streams to be
// PrintObjectOutput and ReadObjectInput classes respectively
public interface AsciiSerializable 
{
  public void writeAscii(PrintObjectOutput ps) throws IOException;
  public void readAscii(ReadObjectInput rs) throws IOException;
}
