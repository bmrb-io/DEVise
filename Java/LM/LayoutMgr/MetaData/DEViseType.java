/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseType.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;

// Class
/**
 * The <CODE>DEViseType</CODE> class defines types for all first-class
 * entities in the session.
 *
 * @author  Shilpa Lawande
 */

public class DEViseType 
{
  private int m_type;
  private DEViseType(int type) {
    m_type = type;
  }
  /* Public members */
  public static final DEViseType CategoryType = new DEViseType(0);
  public static final DEViseType SessionType = new DEViseType(1);
  public static final DEViseType ViewType = new DEViseType(10);
  public static final DEViseType VisualLinkType = new DEViseType(11);
  public static final DEViseType CursorType = new DEViseType(12);
  public static final DEViseType RecordLinkType = new DEViseType(13);
  public static final DEViseType MappingType = new DEViseType(14);
  public static final DEViseType TDataType = new DEViseType(15);
  public static final DEViseType SchemaType = new DEViseType(16);
}
