package Iterators; 

import java.io.*;
import Types.*;
import Expressions.*;


/** This class is used to store outer relation tuple when using NLjoin. */
 
public class MemoryLoader implements Iterator
{ 
  Tuple[] TupleArray;
  Iterator reln;
  int size, curr = 0;

  /* Argument:
     size1: integer, specify how many outer relation tuples can be store in memory.
     r: Iterator on outer relation.
     types: TypeDesc array, specify the type of attributes of the outer relation tuple.
     */

  public MemoryLoader(int size1, Iterator r, TypeDesc[] types)
  {
    size = size1;

    TupleArray = new Tuple[size];
    for(int i = 0; i < size; i++)
	TupleArray[i] = new Tuple(types);
   
    reln = r;
  }

  public void load() throws IOException
  {
    Tuple t;
    curr = 0;

    for(int i = 0; i < size; i++)
      {
	if((t = reln.getNext()) != null)
	    t.copyTo(TupleArray[i]);
	else
	  {
	    for(int j = i; j < size; j++)
	      TupleArray[j] = null;
	    return;
	  }
      }
  }
  
 
  public Tuple getFirst() throws IOException
  {
    curr = 0;
    return TupleArray[curr++];
  }

  public Tuple getNext() throws IOException
  {
    if( curr < size )
      return TupleArray[curr++]; 
    return null;
  }
}










