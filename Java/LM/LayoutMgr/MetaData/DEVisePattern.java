/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEVisePattern.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.util.*;

// Class
/**
 * The <CODE>DEVisePattern</CODE> class is a place holder class to handle
 * information about patterns in a mapping
 *
 * @author  Shilpa Lawande
 */

public class DEVisePattern 
{
  private String m_name;
  private DEVisePattern(String name) {
    m_name = name;
  }
  public static String PATTERNS[] = {
    "Pattern0", "Pattern1", "Pattern2", "Pattern3", "Pattern4", 
    "Pattern5", "Pattern6", "Pattern7", "Pattern8", "Pattern9"
  };
  public static final DEVisePattern Pattern0 = new DEVisePattern("Pattern0");
  public static final DEVisePattern Pattern1 = new DEVisePattern("Pattern1");
  public static final DEVisePattern Pattern2 = new DEVisePattern("Pattern2");
  public static final DEVisePattern Pattern3 = new DEVisePattern("Pattern3");
  public static final DEVisePattern Pattern4 = new DEVisePattern("Pattern4");
  public static final DEVisePattern Pattern5 = new DEVisePattern("Pattern5");
  public static final DEVisePattern Pattern6 = new DEVisePattern("Pattern6");
  public static final DEVisePattern Pattern7 = new DEVisePattern("Pattern7");
  public static final DEVisePattern Pattern8 = new DEVisePattern("Pattern8");
  public static final DEVisePattern Pattern9 = new DEVisePattern("Pattern9");
  /* Public members */
  public static Hashtable patternTable = new Hashtable();
  static {
    patternTable.put("Pattern0", DEVisePattern.Pattern0); 
    patternTable.put("Pattern1", DEVisePattern.Pattern1); 
    patternTable.put("Pattern2", DEVisePattern.Pattern2); 
    patternTable.put("Pattern3", DEVisePattern.Pattern3); 
    patternTable.put("Pattern4", DEVisePattern.Pattern4); 
    patternTable.put("Pattern5", DEVisePattern.Pattern5); 
    patternTable.put("Pattern6", DEVisePattern.Pattern6); 
    patternTable.put("Pattern7", DEVisePattern.Pattern7); 
    patternTable.put("Pattern8", DEVisePattern.Pattern8); 
    patternTable.put("Pattern9", DEVisePattern.Pattern9); 
  }
  public static DEVisePattern getPatternByName(String name) {
    return (DEVisePattern) patternTable.get(name); 
  }
  public String getPatternName() {
    return m_name;
  }
  public String toString() {
    return m_name; 
  }
}
