package Iterators; 

import java.io.*;
import Types.*;
import Functions.*;
import Expressions.*;



public class MemoryLoader implements Iterator
{ 
  Tuple[] TupleArray;
  Iterator reln;
  int size, curr = 0;

  public MemoryLoader(int size1, Iterator r, TypeDesc[] typeStrings)
  {
    size = size1;

    TupleArray = new Tuple[size];
    /*    for(int i = 0; i < size; i++)
      TupleArray[i] = new Tuple(typeStrings);
      */
    reln = r;
  }

  //  public ~MemoryLoader(){}
  
  public void load() throws IOException
  {
    for(int i = 0; i < size; i++)
      TupleArray[i] = new Tuple(reln.getNext());
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




