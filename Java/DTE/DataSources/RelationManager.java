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

    int d_length = 0;
    try{
      d_length =(int) d_fout.length();
    }catch(IOException e){
      throw new IOException();};
    
    try{
      d_fout.seek((long) d_length ); //write append to tail of def file
    }catch(IOException e){
      throw new IOException();};

    try{
      d_fout.writeChars(dataSrc.getString());
    }catch(IOException e){
      throw new IOException();};

    try{
      d_fout.close();
    }catch(IOException e){
      throw new IOException("Failed to close description file:"+ defStream);};
    
   
    int i_length = 0;
    RandomAccessFile i_fout = null;
   
    try{
      i_fout = new RandomAccessFile(idStream, "rw");
    }catch(IOException e){
      throw new IOException("Failed to open idfile:"+ idStream);};

   
    try{
      i_length =(int) i_fout.length();
    }catch(IOException e){
      throw new IOException();};

    try{
      i_fout.seek((long) i_length);  //write appends to the tail 
    }catch(IOException e){
      throw new IOException();};
 
    try{
      i_fout.writeInt( d_length);
    }catch(IOException e){
      throw new IOException();};

    try{
      i_fout.close();
    }catch(IOException e){
      throw new IOException("Failed to open idfile:"+ idStream);};

    RelationId relid = new RelationId(i_length/4); //localId starts from 0
    return relid; 
  }


   

  public DataSource createDataSource(RelationId relId)
         throws IOException
  {

    int lcid = relId.getLocalId();
    if( relId.getServerId() != 0)    //DTE_SERVER_ID
      throw new IOException("Remote ServerID not Implemented");

    RandomAccessFile i_fin = null;
    try{
      i_fin = new RandomAccessFile(idStream, "rw");
    }catch(IOException e){
       throw new IOException("Failed to open idfile:"+ idStream);};
   
    try{
      i_fin.seek(0);
    }catch(IOException e){};
    

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
      d_fin = new RandomAccessFile(defStream, "r");
    }catch(IOException e){
       throw new IOException("Failed to open description file:"+ defStream);};
    
    try{
      d_fin.seek(0);
    }catch(IOException e){};

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

System.out.println("Test of RelationManager starts:");
    System.out.println();

    StandardTable stdtbl = null;
    RelationId relid = null;

    System.out.println("------Test of Constructor and Function registerNewRelation-----");

    RelationManager relmgr = new RelationManager("idFile", "desFile");
    int i=0;
    for(; i<=4; i++){
     

    System.out.print("Register StandardTable [" + (4*i+1) +"]:");
    stdtbl = new StandardTable(3, "string sname int sid double gpa", "relationFile1");
    System.out.println(stdtbl.getString());
    

    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation {" + (4*i+1) +"]:");
    System.out.println(relid.getString());
    System.out.println();
    
    //*******************************************************************
    System.out.print("Register StandardTable [" + (4*i+2) + "]:");
    stdtbl = new StandardTable(2, "int deptno string deptname", "relationFile2");
    System.out.println(stdtbl.getString());
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" +(4*i+2) +"]:");
    System.out.println(relid.getString());
    System.out.println();

    //********************************************************************
    System.out.print("Register StandardTable [" + (4*i+3) + "]:");
    stdtbl = new StandardTable(1, "int hasRel", "relationFile3");
    System.out.println(stdtbl.getString());
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" + (4*i+3) +"]:");
    System.out.println(relid.getString());
    System.out.println();

    //********************************************************************
    System.out.print("Register StandardTable [" +(4*i+4) +"]:");
    stdtbl = 
     new StandardTable(5, "int pid int deptno string pname double salary int hasRel", 
                       "relationFile4");
    System.out.println(stdtbl.getString());
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" +(4*i+4) +"]:");
    System.out.println(relid.getString());
    System.out.println();
    }

    System.out.println();
    System.out.println("Totally registered " + (4*i)+"New DataSources");
    System.out.println("Register New Relations finished...");


    System.out.println();
    System.out.println("--------Test of Function createDataSource---------");

      
    RelationId id = new RelationId(0);
    
    System.out.println("Fetch the DataSource with RelationId:");
    System.out.println( id.getString());
   
    StandardTable stbv = null;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("something wrong");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println();
    
    //********************************************************************************
    id = new RelationId(2);
    System.out.println("Fetch the DataSource with RelationId:");
    System.out.println( id.getString());
    
    //StandardTable stbv1 =null ;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("test of createDataSource failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println();
   
    //********************************************************************************
    id = new RelationId(8);
    System.out.println("Fetch the DataSource with RelationId:");
    System.out.println( id.getString());
    
    //StandardTable stbv =null ;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("test of createDataSource failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

//********************************************************************************
    id = new RelationId(15);
    System.out.println("Fetch the DataSource with RelationId:");
    System.out.println( id.getString());
    
    //StandardTable stbv =null ;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("test of createDataSource failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

//********************************************************************************
    id = new RelationId(19);
    System.out.println("Fetch the DataSource with RelationId:");
    System.out.println( id.getString());
    
    //StandardTable stbv =null ;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("test of createDataSource failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 


    System.out.println("Test of RelationManager finished successfully...");

  }
}







