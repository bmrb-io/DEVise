/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseColor.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.awt.*;
import java.io.*;
import java.util.*;
import Util.*;

// Class
/**
 * The <CODE>DEViseColor</CODE> class is a placeholder class to handle
 * color stuff (such as background, foreground,GData color attr).
 *
 * @author  Shilpa Lawande
 * @see DEViseObj
 * @see Util.AsciiSerializable
 */

// This class will eventually be replaced by a Color Manager that provides
// a get-set color interface.
// For now, a fixed collection of colors is stored as an array.
public class DEViseColor implements AsciiSerializable
{
  // for now use java built in color class 
  private Color m_clr;
  private String m_name;

  public DEViseColor() {
  }

  /** Creates a <CODE>DEViseColor</CODE> corresponding to a java 
    * <CODE>Color</CODE> object. 
    * @param clr instance of Color
    * @name  name to associate with this color
    * @see java.awt.Color
    */
  public DEViseColor(Color clr, String name) {
    m_clr = clr;
    m_name = name;
  }

  /** Creates a <CODE>DEViseColor</CODE> corresponding to a java 
    * - internally generates a name from rgb values
    * <CODE>Color</CODE> object. 
    * @param clr instance of Color
    * @see java.awt.Color
    */
  public DEViseColor(Color clr) {
    this(clr,"clr:" + clr.getRed() + ":" + clr.getGreen() + ":" + clr.getBlue());
  }
  public static DEViseColor black = new DEViseColor(Color.black, "black");
  public static DEViseColor white = new DEViseColor(Color.white, "white");
  public static DEViseColor red = new DEViseColor(Color.red, "red");
  public static DEViseColor blue = new DEViseColor(Color.blue, "blue");
  public static DEViseColor green = new DEViseColor(Color.green, "green");
  public static DEViseColor yellow = new DEViseColor(Color.yellow, "yellow");
  public static DEViseColor orange = new DEViseColor(Color.orange, "orange");
  public static DEViseColor magenta = new DEViseColor(Color.magenta, "magenta");
  public static DEViseColor cyan = new DEViseColor(Color.cyan, "cyan");
  public static DEViseColor pink = new DEViseColor(Color.pink, "pink");

  public static String COLORS[] = {
    "black", "white", "red", "blue", "green", 
    "yellow", "orange", "magenta", "cyan", "pink"
  };
  public static Hashtable colorTable = new Hashtable();
  static {
    colorTable.put("black", DEViseColor.black);
    colorTable.put("white", DEViseColor.white);
    colorTable.put("red", DEViseColor.red);
    colorTable.put("blue", DEViseColor.blue);
    colorTable.put("green", DEViseColor.green);
    colorTable.put("yellow", DEViseColor.yellow);
    colorTable.put("orange", DEViseColor.orange);
    colorTable.put("magenta", DEViseColor.magenta);
    colorTable.put("cyan", DEViseColor.cyan);
    colorTable.put("pink", DEViseColor.pink);
  }
  
  /** Retrives a <CODE>DEViseColor</CODE> given a name
   * @param the name by which the color is identified
   */
  public static DEViseColor getColorByName(String name) {
    return (DEViseColor)colorTable.get(name);
  }

  public String toString() {
    return m_clr.toString();
  }
  public Color getColor() {
    return m_clr;
  }
  public String getColorName() {
    return m_name;
  }
  public void readAscii(ReadObjectInput stream) throws IOException{
    int red =  stream.readInt();
    int green = stream.readInt();
    int blue = stream.readInt();
    m_clr = new Color(red, green, blue);
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException{
    stream.writeChars(m_clr.getRed() + " " + 
                      m_clr.getGreen() + " " +
                      m_clr.getBlue() + " ");
  }
}
