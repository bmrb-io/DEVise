package DataSources;


import java.io.*;
import java.lang.*;
import Types.*;

/* DataSource was called Interface in C++ source 
 * This class is implemented in C++ in file RelationManager.[ch] 
 * In summary, we have two files: idStream and defStream.
 * idStream is a sequence of integers that represent offsets into defFile.
 * defFile contains sequence of DataSource definitions.
*/

public class RelationManager {
  private String idStream;
  private String defStream;

  public RelationManager(String istr, String dstr ){
    idStream = new String(istr);
    defStream = new String(dstr);
  }

  /** Return immutable (unique and nonchangable) relation id */

  public RelationId registerNewRelation(DataSource dataSrc)
         throws IOException
  {
    RandomAccessFile d_fout = null;
    try{
      d_fout = new RandomAccessFile(defStream, "rw");
    }catch(IOException e){
      throw new IOException("Failed to open description file:"+ defStream);};

    long d_length = 0;
    try{
      d_length = d_fout.length();
      d_fout.seek( d_length ); //write append to tail of def file
      d_fout.writeChars(dataSrc.getString());
    }catch(IOException e){
      throw new IOException();};
   

    try{
      d_fout.close();
    }catch(IOException e){
      throw new IOException("Failed to close description file:"+ defStream);};
    
   
    long i_length = 0;
    RandomAccessFile i_fout = null;
   
    try{
      i_fout = new RandomAccessFile(idStream, "rw");
    }catch(IOException e){
      throw new IOException("Failed to open idfile:"+ idStream);};

   
    try{
      i_length = i_fout.length();
      i_fout.seek( i_length );  //write appends to the tail
      i_fout.writeInt((int)d_length);
    }catch(IOException e){
      throw new IOException();};

    try{
      i_fout.close();
    }catch(IOException e){
      throw new IOException("Failed to open idfile:"+ idStream);};

    RelationId relid = new RelationId((int)i_length/4); //localId starts from 0
    return relid; 
  }


   

  public DataSource createDataSource(RelationId relId)
         throws IOException
  {

    int lcid = relId.getLocalId();
    if( relId.getServerId() != 1)    //DTE_SERVER_ID
      throw new IOException("Remote ServerID yet-to-be Implemented");

    RandomAccessFile i_fin = null;
    try{
      i_fin = new RandomAccessFile(idStream, "rw");
    }catch(IOException e){
       throw new IOException("Failed to open idfile:"+ idStream);};
 
    try{
      i_fin.seek( (long)(lcid * 4) );   
    }catch(IOException e){
      throw new IOException();};

    int offset = 0;            //offset to description file 

    try{
      offset = i_fin.readInt();
    }catch(IOException e){
      throw new IOException();};

    try{
      i_fin.close();
    }catch(IOException e){
       throw new IOException("Failed to close idfile:"+ idStream);};



    RandomAccessFile d_fin = null;
    try{
      d_fin = new RandomAccessFile(defStream, "rw");
    }catch(IOException e){
       throw new IOException("Failed to open description file:"+ defStream);};

    try{
      d_fin.seek((long)offset);
    }catch(IOException e){
      throw new IOException();};

    String schstr = new String();
    char strch = 'a';   //dummy initial value

    try{
      strch = d_fin.readChar();
    }catch(IOException e){
      throw new IOException();};

  
    while( strch != ';' ){
      schstr += strch;

      try{
	strch = d_fin.readChar();
      }catch(IOException e){
	throw new IOException();};
    }

    try{
      d_fin.close();
    }catch(IOException e){
       throw new IOException("Failed to close description file:"+ defStream);};

    StringReader strrd = new StringReader(schstr);
    StreamTokenizer stk = new StreamTokenizer(strrd);
    stk.wordChars(40,127);     //to include "/" for StreamToken

    StandardTable standtable = new StandardTable();
    try{
      standtable.read(stk);
    }catch(IOException e){
      return null;};

    return standtable;
  }
  

  
  public static void main(String[] args){

	// this is the test code

	// create initial versions of id_file and def_file
	// they should be empty in the begining

	// register a new relation (use StandardTable)

	// lookup this relation by the returned relId

    
  }
   
}







