/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DEViseCategory.java
****************************************************************************/


//Packages 

package LayoutMgr.MetaData;

import java.lang.*;
import java.util.*;
import java.io.*;
import Util.*;

// Class
/** The <CODE>DEViseCategory</CODE> class
 * is the book-keeping class for objects which are
 * instances of first-class entities in the DEVise back-end. A category has 
 * a name and maintains a hashtable of instances of that category.
 * A DEViseCategory is homogeneous i.e. it can only contain entities of
 * the specified type. It also supports enumeration of all objects in the
 * category.
 * @author Shilpa Lawande
 * @see    DEViseObj
 * @see    DEViseType
 */

public class DEViseCategory extends DEViseObj
{
  /* Private members */
  private Hashtable  m_instances;  // table of instances of the category
  private DEViseType m_instanceType;  // type of objects that can be 
                                      // inserted into this category

  /* Constructor */
  /** Creates a <CODE>DEViseCategory</CODE> with the specified name
    * and instanceType
    * @param name Name of the category
    * @param instanceType Allowed type for inserted objects
    */
  public DEViseCategory(String name, DEViseType instanceType) {
    super(name, DEViseType.CategoryType); 
    m_instances = new Hashtable();
    m_instanceType = instanceType;
  }

  /* Public methods */
  /** Adds instance to category. The type of the instance must match
    * the type specified in the constructor of the category 
    * @param  obj  : the <CODE>DEViseObj</CODE> to insert
    * @return Returns true if insertion is successful, false if a type 
    *         mismatch occurred.
    */
  public boolean add(DEViseObj obj) {
    if (obj.getType() == m_instanceType) {
      m_instances.put(obj.getName(), obj);
      return true;
    } 
    return false;
  }

  /* Delete instance from category */
  /** Deletes instance from category. 
    * @param  name  : the name of the instance to be deleted.
    * @return Returns true if deletion is successful, false if no 
    *         instance was found.
    */
  /* internally calls delete which takes an object instead of string
   * as parameter
   */
  public boolean delete(String name) {
    DEViseObj obj = find(name);
    return delete(obj);
  }

  /** Deletes instance from category. 
    * @param  obj  : the instance to be deleted.
    * @return Returns true if deletion is successful, false if no 
    *         instance was found.
    */
  
  /* Delete instance from category */
  public boolean delete(DEViseObj obj) {
    if (obj != null) {
      if (m_instances.remove(obj.getName()) != null) {
        return true;
      }
    } 
    return false;
  }
  /** Deletes an instance.
    * Allows the instance to remove references to itself before removing
    * it from the category. If all's well, the instance should now be
    * a candidate for GC.
    * @param  name  : the name of the instance to be deleted.
    * @return Returns true if deletion is successful, false if no 
    *         instance was found.
    */
  /* remove all references to object and delete instance */
  public boolean destroy(String name) {
    DEViseObj obj = find(name);
    return destroy(obj);
  }
  /** Deletes an instance.
    * Allows the instance to remove references to itself before removing
    * it from the category. If all's well, the instance should now be
    * a candidate for GC.
    * @param  obj  : the instance to be deleted.
    * @return Returns true if deletion is successful, false if no 
    *         instance was found.
    */
  /* remove all references to object and delete instance */
  public boolean destroy(DEViseObj obj) {
    if (obj != null) {
      obj.selfDestruct();
      return delete(obj);
    }
    return false;
  }

  /** Creates a unique name by appending an integer to the category name
   */
  /* create unique name  composed of category name and an integer */
  public String createUniqueName() {
    int init = m_instances.size();
    String name;
    while (true) {
      name = getName() + init;
      if (m_instances.get(name) == null) {
        break;
      } 
      init++;
    }
    return name;
  }

  /** Finds the instance specified by the name 
    * @param  name Name of required instance
    * @return Returns the instance if found or null otherwise
    */
  /* find instance  */ 
  public DEViseObj find(String name) {
    return (DEViseObj)m_instances.get(name);
  }

  /** @return Returns the number of instances in the category */
  /* num instances */
  public int numInstances() {
    return m_instances.size();
  }

  /** Enumeration method for a category. <CODE>nextElement()</CODE> returns
    * the name of the next instance in the enumeration. The actual
    * object may retrieved using find.
    * @return A iterator which can be used to retrieve successively all
    *         instance names in the category.
    */
  /* get an enumeration for all objects in the list */
  public Enumeration list() {
    return new Enumeration() {
	    private Enumeration e = m_instances.keys();
	    public boolean hasMoreElements() { 
	      return e.hasMoreElements();
	    }
	    public Object nextElement() {
	      return find((String)e.nextElement());
	    }
	    public String nextKey() {
	      return (String)e.nextElement();
	    }
    };
  }
  /** Renames an instance 
    * @param oldName Existing name of the instance
    * @param newName New name for the instance
    * @return Returns true if instance of oldName exists, false otherwise.
    */
  /* rename */
  // BUG : does not check that instance of newName does not exist
  public boolean rename(String oldName, String newName) {
    DEViseObj obj = (DEViseObj)m_instances.remove(oldName);
    if (obj != null) {
      obj.setName(newName);
      m_instances.put(newName, obj);
      return true;
    }
    return false;
  }

  /** Serialization method
   *  @see Util.AsciiSerializable
   */
  public void readAscii(ReadObjectInput stream) throws IOException {
    super.readAscii(stream);
  }
  /** Serialization method
   *  @see Util.AsciiSerializable
   */
  public void writeAscii(PrintObjectOutput stream)
      throws IOException {
   writeCategoryHeader(stream);
   for (Enumeration l = list(); l.hasMoreElements(); ) {
      DEViseObj obj = (DEViseObj)l.nextElement(); 
      obj.writeAscii(stream);
    }
  }
  /** Serialization method
   *  function to save only the header separately from instances
   */
  //used to save view category
  public void writeCategoryHeader(PrintObjectOutput stream) 
              throws IOException {
    super.writeAscii(stream);
    stream.writeChars(numInstances() + "\n");
    stream.writeChars("#Instance List\n");
  }
}
