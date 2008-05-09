package EDU.bmrb.starlibj;

import java.util.*;
import java.lang.*;
import EDU.bmrb.starlibj.*;

/** This interface is declared by any object in the starlibj that
  * wants to announce to the world that it will behave a lot like
  * a java.util.vector.  Anyone familiar with the java.util.vector
  * class should have no problem understanding how yo use a class
  * that implements the VectorLike interface.
  * @see java.util.vector
  */

public interface StarVectorLike
{
    public int size();
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.isEmpty
      */
    public boolean isEmpty();
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.Enumeration
      */
    public Enumeration elements();
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.contains
      */
    public boolean contains(Object obj);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj,
                                       int index);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj,
                                           int index);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.elementAt
      */
    public Object elementAt(int index);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.firstElement
      */
    public Object firstElement();
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.lastElement
      */
    public Object lastElement();
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.setElementAt
      */
    public void setElementAt(Object obj,
                                             int index)
	throws WrongElementType;
    /** Similar to the Vector method of the same name.
      * @see java.util.Vector.removeElementAt
      */
    public void removeElementAt(int index);
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.insertElementAt
      */
    public void insertElementAt(Object obj,
                                                int index)
	throws WrongElementType;
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.addElement
      */
    public void addElement(Object obj)
	throws WrongElementType;
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.removeElement
      */
    public boolean removeElement( Object obj);
}

