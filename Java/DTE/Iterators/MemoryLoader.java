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

  public MemoryLoader(int size1, Iterator r)
  {
    size = size1;

    TupleArray = new Tuple[size];
    reln = r;
  }

  public void load() throws IOException
  {
    for(int i = 0; i < size; i++)
      TupleArray[i] = (Tuple)(reln.getNext()).clone();
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




