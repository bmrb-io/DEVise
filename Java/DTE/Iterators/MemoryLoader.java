package Iterators; 

import java.io.*;
import Types.*;
import Expressions.*;



public class MemoryLoader implements Iterator
{ 
  Tuple[] TupleArray;
  Iterator reln;
  int size, curr = 0;

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
    for(int i = 0; i < size; i++)
      {
	if((t = reln.getNext()) != null)
	    t.copyTo(TupleArray[i]);
	else
	  TupleArray[i] = null;
      }

    curr = 0;
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










