/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseSession.java
****************************************************************************/

//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.util.*;
import java.io.*;
import COM.objectspace.jgl.*;
import Util.Tracer;
import Util.*;

// Class
/**
 * The <CODE>DEViseSession</CODE> class is the topmost entity in DEVise.
 * A session contains all the information needed to create and save
 * a visualization. A <CODE>DEViseSession</CODE> object consists of 
 * a number of categories to store instances of <CODE>DEViseView</CODE>, 
 * <CODE>DEViseVisualLink</CODE>, <CODE>DEViseRecordLink</CODE> etc.
 * By storing these instances in the session object we get a unique
 * name space for each session. There is also a category to store all
 * instances of <CODE>DEViseSession</CODE> which is a static member
 * of this class.
 *
 * @author  Shilpa Lawande
 * @see     DEViseObj
 * @see     DEViseType
 * @see     DEViseCategory
 * @see     DEViseView
 * @see     DEViseVisualLink
 * @see     DEViseRecordLink
 * @see     DEViseCursor
 * @see     DEViseTData
 * @see     DEViseSchema
 * @see     DEViseMapping
 */

public class DEViseSession extends DEViseObj
{
  /* Static members */
  private static DEViseCategory theSessionCategory
    = new DEViseCategory("Session", DEViseType.SessionType);
  static public DEViseCategory theSessionCategory() {
    return theSessionCategory;
  }

  /** Creates a new instance of DEVise session with a unique name  and
    * adds it to the session category.
    */
  public static DEViseSession createInstance() {
    return new DEViseSession(theSessionCategory.createUniqueName());
  }

  /** Destroys a session instance 
    * @param session The session instance to destroy
    */
  public static void destroyInstance(DEViseSession session) {
    theSessionCategory.destroy(session);
  }

  /* m_myCategories is a member of DEViseCategory type through which
   * all categories of this session can be queried 
   */
  private DEViseCategory m_myCategories;

  /* Constructor */
  public DEViseSession() {
    super(DEViseType.SessionType);
  }
  public DEViseSession(String name) {
    super(name, DEViseType.SessionType);
    Init();
  }
  private void Init() {
    createCategories();
    theSessionCategory.add(this);
  }
  /* Private methods */
  private void createCategories() {
    m_myCategories
      = new DEViseCategory(m_name + ".Categories", DEViseType.CategoryType);
    m_myCategories.add(new DEViseCategory("DEViseView",
                                          DEViseType.ViewType));
    m_myCategories.add(new DEViseCategory("DEViseVisualLink", 
                                          DEViseType.VisualLinkType));
    m_myCategories.add(new DEViseCategory("DEViseRecordLink", 
                                          DEViseType.RecordLinkType));
    m_myCategories.add(new DEViseCategory("DEViseCursor",
                                          DEViseType.CursorType));
    m_myCategories.add(new DEViseCategory("DEViseSchema", 
                                          DEViseType.SchemaType));
    m_myCategories.add(new DEViseCategory("DEViseTData", 
                                          DEViseType.TDataType));
    m_myCategories.add(new DEViseCategory("DEViseMapping", 
                                          DEViseType.MappingType));
  }
  /* Public methods */

  /** @return Returns the Category object containing all 
    *         <CODE>DEViseView</CODE> objects in this session
    */
  public DEViseCategory myViews() { 
    return (DEViseCategory)m_myCategories.find("DEViseView");
  }
  /** @return Returns the Category object containing all 
    *         <CODE>DEViseVisualLink</CODE> objects in this session
    */
  public DEViseCategory myVisualLinks() {
    return (DEViseCategory)m_myCategories.find("DEViseVisualLink");
  }
  /** @return Returns the Category object containing all 
    *         <CODE>DEViseRecordLink</CODE> objects in this session
    */
  public DEViseCategory myRecordLinks() {
    return (DEViseCategory)m_myCategories.find("DEViseRecordLink");
  }
  /** @return Returns the Category object containing all 
    *         <CODE>DEViseCursor</CODE> objects in this session
    */
  public DEViseCategory myCursors() {
    return (DEViseCategory)m_myCategories.find("DEViseCursor");
  }
  /** @return Returns the Category object containing all 
    *         <CODE>DEViseTData</CODE> objects in this session
    */
  public DEViseCategory myTDatas() {  // Active tDatas
    return (DEViseCategory)m_myCategories.find("DEViseTData");
  }
  /** @return Returns the Category object containing all 
    *         <CODE>DEViseSchema</CODE> objects in this session
    */
  public DEViseCategory mySchemas() {  // Active schemas
    return (DEViseCategory)m_myCategories.find("DEViseSchema");
  }
  /** @return Returns the Category object containing all 
    *         <CODE>DEViseMapping</CODE> objects in this session
    */
  public DEViseCategory myMappings() {
    return (DEViseCategory)m_myCategories.find("DEViseMapping");
  }
  /* Print function */
  public String toString() {
    StringBuffer buf = new StringBuffer();
    buf.append(super.toString());
    buf.append("Views ->\n");
    for (Enumeration viewList = myViews().list(); viewList.hasMoreElements();)
    {
      buf.append(viewList.nextElement());
    }
    buf.append("VisualLinks ->\n");
    for (Enumeration linkList = myVisualLinks().list(); linkList.hasMoreElements(); ) {
      buf.append(linkList.nextElement());
    }
    return buf.toString();
  }
 /** Serialization method
   * @see Util.AsciiSerializable
   */

  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream); // reads name
    Init();
    // for each of the categories in the session
    // serialize objects of that category according to type
    while(true) {
      String categoryName = stream.readUTF();
      System.out.println("CategoryName = " + categoryName);
      if (categoryName.equals("EndOfDEViseObjects")) {
        break;
      }
      DEViseCategory cat = (DEViseCategory)m_myCategories.find(categoryName);
      if (cat == null) {
        throw new IOException("CategoryName: " + categoryName + " not found\n");
      }
      int numInstances = stream.readInt();
      for (int i = 0; i < numInstances; i++) {
        String fullQualName = "LayoutMgr.MetaData." + categoryName;
        try {
          DEViseObj obj = (DEViseObj)Class.forName(fullQualName).newInstance();
          obj.setSerializeInfo(this); // set session info in case the object 
                                      // needs cross category info to recreate i
                                      // itself
          obj.readAscii(stream);
          
          ((DEViseCategory)m_myCategories.find(categoryName)).add(obj);
        } catch(ClassNotFoundException e) {
          throw new IOException("Class " + fullQualName + " not found\n");
        } catch(IllegalAccessException e) {
          System.out.println(e);
          return;
        } catch(InstantiationException e) {
          System.out.println(e);
          return;
        }
      }
    }
    System.out.println("Done with DEViseSession:readAscii\n");
  }
 /** Serialization method
   * @see Util.AsciiSerializable
   */
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    super.writeAscii(stream); // writes name
    stream.writeChars("#CategoryName NumberofInstances\n");
    // for each of the categories in the session
    // serialize objects of that category according to type
    mySchemas().writeAscii(stream);
    myTDatas().writeAscii(stream);
    myMappings().writeAscii(stream);
   
    // views have to saved in an manner preserving order of children
    myViews().writeCategoryHeader(stream);
    DEViseView display = (DEViseView)myViews().find("Display");
    writeViews(display, stream);

    myVisualLinks().writeAscii(stream);
    myRecordLinks().writeAscii(stream);
    myCursors().writeAscii(stream);
    stream.writeChars("EndOfDEViseObjects\n");
  }

  private void writeViews(DEViseView view, PrintObjectOutput stream) 
               throws IOException {
    view.writeAscii(stream);

    for (DListIterator d = view.childList(); d.hasMoreElements(); ) {
      DEViseView child = (DEViseView)d.nextElement();
      writeViews(child, stream);
    }
  }
}
