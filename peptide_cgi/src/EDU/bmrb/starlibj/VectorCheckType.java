package EDU.bmrb.starlibj;

import java.util.*;
import java.lang.*;
import EDU.bmrb.starlibj.*;

/** VectorCheckType is essentially the exact same thing as the standard
  * Java class <TT>java.util.vector</TT>, but with the additional
  * provisions to ensure that only objects of a specific type will
  * be allowed to be put into the vector.  Anything else is deemed
  * an error and generates an exception.  Essentially, what you do is
  * you create an object of type <TT>VectorCheckType</TT>, then add the types
  * you want it to be able to hold using <TT>addType()</TT>.  Then prevent
  * any future types from being added with <TT>freezeTypes()</TT>.  Until you
  * have done this, you cannot insert anything into the vector.  Once you have
  * called <TT>freezeTypes()</TT>, you cannot call <TT>addType()</TT> again.
  * The idea is to provide a generic way to implement something like the
  * C++ concept of template classes - we want to have a vector that only
  * allows some types of object, not all types of object.  Typically,
  * <TT>addType()</TT> and <TT>freezeTypes()</TT> will have already been
  * called internally in the library before the user programmer gets to
  * use the vector.  For example, <TT>StarFileNode</TT> will use a 
  * VectorCheckType that has been set up to only hold BlockNodes.
  * <P>
  * A typical piece of code using VectorCheckType might look like this:
  * <TABLE BORDER=yes>
  *     <TR><TH><FONT SIZE=+2>Right</FONT></TH></TR>
  *     <TR>
  *         <TD><PRE>
  *             VectorCheckType aVect =
  *                 new VectorCheckType();
  *             <I>[...snip...]</I>
  *
  *             // Make the vector accept
  *             // only items and loops:
  *             aVect.addType(
  *                 Class.forName( StarValidity.clsNameDataItemNode) );
  *             aVect.addType( 
  *                 Class.forName( StarValidity.clsNameDataLoopNode) );
  *             aVect.freezeTypes();
  *
  *             aVect.addElement( 
  *                 new DataItemNode(<I>[...snip...]</I>);
  *         </PRE></TD>
  *     </TR>
  *     <TR>
  *         <TH><FONT SIZE=+2>Wrong</FONT></TH>
  *         <TH><FONT SIZE=+2>Wrong</FONT></TH>
  *     </TR>
  *     <TR>
  *         <TD><PRE>
  *             VectorCheckType aVect =
  *                 new VectorCheckType();
  *             <I>[...snip...]</I>
  *
  *             // Make the vector accept
  *             // only items and loops:
  *             aVect.addType(
  *                 Class.forName( StarValidity.clsNameDataItemNode) );
  *             aVect.addType( 
  *                 Class.forName( StarValidity.clsNameDataLoopNode) );
  *
  *             // This attempt to add an element
  *             // before the list was frozen
  *             // produces and exception.
  *             aVect.addElement( 
  *                 new DataItemNode(<I>[...snip...]</I>);
  *
  *             aVect.freezeTypes();
  *         </PRE></TD>
  *         <TD><PRE>
  *             VectorCheckType aVect =
  *                 new VectorCheckType();
  *             <I>[...snip...]</I>
  *
  *             // Make the vector accept
  *             // only items and loops:
  *             aVect.addType(
  *                 Class.forName( StarValidity.clsNameDataItemNode) );
  *             aVect.addType( 
  *                 Class.forName( StarValidity.clsNameDataLoopNode) );
  *             aVect.freezeTypes();
  *
  *             // This attempt to add an
  *             // element of the wrong type
  *             // produces an exception.
  *             aVect.addElement( SomeOtherType );
  *
  *             // This is also an exception:
  *             //   Attempting to add more
  *             //   types after freezeTypes()
  *             //   has been called.
  *             aVect.addType( SomeOtherType);
  *         </PRE></TD>
  *     </TR>
  * </TABLE>
  */

public class VectorCheckType
{
    protected Vector   types;  // List of Class names allowed in the vector.
    protected Vector   data;
    protected boolean  typesFrozen;

    /** makes an empty vector */
    public VectorCheckType( )
    {
	data = new Vector();
	types = new Vector();
	typesFrozen = false;
    }
    /** makes an empty vector with a starting capacity */
    public VectorCheckType( int startCap )
    {
	data = new Vector(startCap);
	types = new Vector();
	typesFrozen = false;
    }
    /** Constructs an empty vector with starting capacity and amount to
      * increment it by when it is overflown.
      */
    public VectorCheckType( int startCap, int incr )
    {
	data = new Vector(startCap,incr);
	types = new Vector();
	typesFrozen = false;
    }
    /** Adds another type to the list of types that the class
      * will allow to be inserted.  This must be done before
      * the vector can have any values inserted into it.
      * @see freezeTypes
      */
    public void addType( Class typ )
	throws TypesAreFrozen
    {
	if( typesFrozen )
	    throw new TypesAreFrozen();
	else
	    types.addElement( typ.getName() );
    }

    /** Freezes the class like it is such that no more types can be added
      * to the list of acceptable types for this vector to hold.
      * Until this is done, none of the insertion functions for this
      * vector will be allowed.
      */
    public void freezeTypes( )
    {
	typesFrozen = true;
    }
    
    // -------------------------------------------
    //    Things designed to mimic Vector:
    // -------------------------------------------


    /** Just like the Vector method of the same name.
      * @see java.util.Vector.setSize
      */
    public void setSize(int newSize)
    {
	data.setSize(newSize);
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.capacity
      */
    public int capacity()
    {
	return data.capacity();
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.size
      */
    public int size()
    {
	return data.size();
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.isEmpty
      */
    public boolean isEmpty()
    {
	return data.isEmpty();
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.Enumeration
      */
    public Enumeration elements()
    {
	return data.elements();
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.contains
      */
    public boolean contains(Object obj)
    {
	return data.contains( obj );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj)
    {
	return data.indexOf( obj );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.indexOf
      */
    public int indexOf(Object obj,
                                       int index)
    {
	return data.indexOf( obj, index );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj)
    {
	return data.lastIndexOf( obj );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.lastIndexOf
      */
    public int lastIndexOf(Object obj,
                                           int index)
    {
	return data.lastIndexOf( obj, index );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.elementAt
      */
    public Object elementAt(int index)
    {
	return data.elementAt( index );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.firstElement
      */
    public Object firstElement()
    {
	return data.firstElement();
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.lastElement
      */
    public Object lastElement()
    {
	return data.lastElement();
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.setElementAt
      */
    public void setElementAt(Object obj,
                                             int index)
	throws WrongElementType, TypesNotFrozenYet
    {
	if( ! typesFrozen )
	    throw new TypesNotFrozenYet();
	if( isObjectAllowed( obj ) )
	    data.setElementAt( obj, index );
	else
	    throw new WrongElementType( types, obj.getClass().getName() );
    }
    /** Similar to the Vector method of the same name.
      * @see java.util.Vector.removeElementAt
      */
    public void removeElementAt(int index)
    {
	data.removeElementAt( index );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.insertElementAt
      */
    public void insertElementAt(Object obj,
                                                int index)
	throws WrongElementType, TypesNotFrozenYet
    {
	if( ! typesFrozen )
	    throw new TypesNotFrozenYet();
	if( isObjectAllowed( obj ) )
	    data.insertElementAt( obj, index );
	else
	    throw new WrongElementType( types, obj.getClass().getName() );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.addElement
      */
    public void addElement(Object obj)
	throws WrongElementType, TypesNotFrozenYet
    {
	if( ! typesFrozen )
	    throw new TypesNotFrozenYet();
	if( isObjectAllowed( obj ) )
	    data.addElement( obj );
	else
	    throw new WrongElementType( types, obj.getClass().getName() );
    }
    /** Just like the Vector method of the same name.
      * @see java.util.Vector.removeElement
      */
    public boolean removeElement( Object obj)
    {
	return data.removeElement( obj );
    }

    /** Used to ask "is this object allowed in this class?"
      * (In other words, "Was there a previous call to
      * <TT>addType()</TT> that allowed it to handle this kind
      * of class?")
      * @param o the object to check for.
      * @see addType()
      */
    public boolean isObjectAllowed( Object o )
    {
	int i;
	try
	{
	for( i = 0 ; i < types.size() ; i++ )
	    if(  Class.forName( (String)(types.elementAt(i)) ).isInstance( o ) )
		return true;
	}
	catch( ClassNotFoundException e )
	{
	    // Can't happen.  The string name we are checking
	    // came from doing a Class.getName(), so we already
	    // know it's the name of a real class.
	}
	return false;
    }
}


