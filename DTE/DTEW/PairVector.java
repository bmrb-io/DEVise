// PairVector.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.util.*;

// A pair of vectors.
public class PairVector
{
  // The first and second vector.
  private Vector firstVector = new Vector ();
  private Vector secondVector = new Vector ();
  
  // Add two elements into the two vectors respectively.
  public final void addElement (Object f, Object s)
    {
      firstVector.addElement (f);
      secondVector.addElement (s);
    }
  
  // Set the two elements at a certain index.
  public final void setElementAt (Object f, Object s, int i)
    {
      firstVector.setElementAt (f, i);
      secondVector.setElementAt (s, i);
    }
  
  // Return the element in the first vector at a given index.
  public final Object elementAtFirst (int i)
    {
      return firstVector.elementAt (i);
    }
  
  // Return the element in the second vector at a given index.
  public final Object elementAtSecond (int i)
    {
      return secondVector.elementAt (i);
    }

  // Return the size of the two vectors.
  public final int size ()
    {
      return firstVector.size ();
    }

  // Find the second element based on the value of the corresponding first element.
  public final Object find (Object o)
    {
      for (int i = 0; i < firstVector.size (); i ++)
	if (firstVector.elementAt (i).equals (o))
	  return secondVector.elementAt (i);
      
      return null;
    }
  
}
