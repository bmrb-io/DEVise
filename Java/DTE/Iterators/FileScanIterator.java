package Iterators;

import Types.*;
import java.io.*;

/** FileScanIterator returns the stream of tuples from a ASCII file
    whose name is given in "fileName" and whose content is described
    by the array of type descriptors "types". */

public class FileScanIterator implements Iterator 
{
  Tuple tuple;
  FileReader fr;
  BufferedReader file;
  StreamTokenizer st;
  
  boolean first_call = true;
  String filename;
  
  public FileScanIterator(String fileName, TypeDesc[] types) 
       throws IOException //FileNotFoundException
  {
    fr = new FileReader(fileName);
    file = new BufferedReader(fr);
    st = new StreamTokenizer(file);
    tuple = new Tuple(types);
    filename = fileName;		
  }
  
  /** getFirst should be called before getNext */
  
  public Tuple getFirst() throws IOException
  {
    if(first_call)
      first_call=false;
    else
      {
	//fr.close();
	file.close();
	
	fr = new FileReader(filename);
	file = new BufferedReader(fr);
	st = new StreamTokenizer(file);
      }

    return getNext();
  }
  
  /** getNext returns a valid tuple object or null to signal the end of
	iterator */
  
  public Tuple getNext() throws IOException 
  {
    if(first_call)
      return getFirst();
          
    if(tuple.read(st))
      return tuple;
   
    else
      return null;
  }
}






