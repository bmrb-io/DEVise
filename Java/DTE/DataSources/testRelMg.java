import java.io.*;
import java.util.*;
import java.lang.*;
import Types.*;
import DataSources.*;

class testRelMg{
 
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
    
    System.out.println("---Test of Constructor and Function registerNewRelation---");
  
    RelationManager relmgr = new RelationManager("idFile", "desFile");

    int i=0;
    for(; i<=9; i++){

    System.out.print("Register StandardTable [" + (5*i+1) +"]:");
    stdtbl = new StandardTable(5, " int cid int deptid string pname double avggpa boolean CR", "/u/relationFile0");
    System.out.println(stdtbl.getString());
    
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" + (5*i+1) +"]:");
    System.out.println(relid.getString());
    System.out.println();
    
    //******************************************************************* 
      
    System.out.print("Register StandardTable [" + (5*i+2) +"]:");
    stdtbl = new StandardTable(3, "string sname int sid double gpa", "/u/r/relationFile1");
    System.out.println(stdtbl.getString());
    
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" + (5*i+2) +"]:");
    System.out.println(relid.getString());
    System.out.println();
    
    //*******************************************************************
    System.out.print("Register StandardTable [" + (5*i+3) + "]:");
    stdtbl = new StandardTable(2, "int deptno string deptname", "/v/s/o/relationFile2");
    System.out.println(stdtbl.getString());
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" +(5*i+3) +"]:");
    System.out.println(relid.getString());
    System.out.println();

    //********************************************************************
    System.out.print("Register StandardTable [" + (5*i+4) + "]:");
    stdtbl = new StandardTable(1, "int hasRel", "/r/p/relationFile3");
    System.out.println(stdtbl.getString());
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" + (5*i+4) +"]:");
    System.out.println(relid.getString());
    System.out.println();

    //********************************************************************
    System.out.print("Register StandardTable [" +(5*i+5) +"]:");
    stdtbl = 
     new StandardTable(5, "int pid int deptno string pname double salary int hasRel", 
                       "/p/stat/relationFile4");
    System.out.println(stdtbl.getString());
    
    try{
      relid = relmgr.registerNewRelation(stdtbl);
    }catch(IOException e){
      System.out.println("OOPs, registerNewRelation failed...");
    };

    System.out.println("RelationId of Relation [" +(5*i+5) +"]:");
    System.out.println(relid.getString());
    System.out.println();
    }

    System.out.println();
    System.out.println("Totally registered " + (5*i)+" New DataSources");
    System.out.println("Register New Relations finished...");


    System.out.println();
    System.out.println("--------Test of Function createDataSource---------");
    System.out.println("DataSources fetched with RelationId chosen randomly.");
      
    RelationId id = new RelationId(0);
    
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
  
   
    StandardTable stbv = null;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource not found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println();
    
    //********************************************************************************
    id = new RelationId(3);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString() );
  
    
    //StandardTable stbv1 =null ;
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println();
   
    //********************************************************************************
    id = new RelationId(8);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
  
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
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

//********************************************************************************
    id = new RelationId(19);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

    
//********************************************************************************
    id = new RelationId(23);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 


    

//********************************************************************************
    id = new RelationId(27);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

//********************************************************************************
    id = new RelationId(36);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

//********************************************************************************
    id = new RelationId(41);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString() );
  
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 

    //********************************************************************************
    id = new RelationId(45);
    System.out.println("Fetch the DataSource with RelationId:" + id.toString());
   
    try{
      stbv = (StandardTable) relmgr.createDataSource(id);
      if(stbv == null)
	System.out.println("OOPs, DataSource not found");
      
    }catch(IOException e){
      System.out.println("DataSource Not Found or test failed");
    };

    System.out.println("DataSource Founded with Given RelationId:" + stbv.getString());
    System.out.println(); 


    System.out.println("Number of DataSources Found: 10");
    System.out.println("Test of RelationManager finished successfully...");
  }
}







