package DataSources;

import Types.*;
import java.io.*;

/** StandardTable is an implementation of a DataSource. It is an ASCII
  * file with white-spaces as field separators and is stored on a local 
  * file system */

/** This class implements the DataSource. Each StandardTable consists 
  * of the schema of the relation and the filename to store the relation
  * table. */

public class StandardTable implements DataSource{

  protected Schema schema;
  protected String urlString;

  /** constructor with no arguments. */
  public StandardTable(){
    schema = new Schema();
    urlString = new String();
  }
       
  /** constructor with the given schema and the relation file. */
  public StandardTable(Schema sch, String url){
    int nmflds = sch.getnumFlds();
    String st  = sch.getTypeAttr();

    schema = new Schema(nmflds, st);
    urlString = new String(url);
  }

  /** constructor(int, String, String) with number of fields of schema, 
      types and corresponding attribute names, and the relation filename.*/
  public StandardTable(int nmflds, String s, String url){
    schema = new Schema(nmflds, s);
    urlString = new String( url);
  }

  /** This method returns the relation filename. */
  public String getFileName(){
    return urlString;
  }

  /** This method returns the typename of this StandardTable. */
  public String getTypeNm(){
    String str = "StandardTable";
    return str;
  }

  /** This method reads the schema and relation filename from the given 
      StreamTokenizer. */
  public boolean read(StreamTokenizer st) throws IOException {
   
    if(schema.read(st)==false)
      throw new IOException("Schema not correctly read, check your input");
	     
    if(st.nextToken() != StreamTokenizer.TT_EOF && st.ttype == StreamTokenizer.TT_WORD ){
     
      urlString += st.sval; 
      return true;
    }
    else throw new IOException("RelationFileName Expected...");
  }

  /** This method returns the string representation of this StandardTable.*/
  public String getString(){
    String str = getTypeNm();
    str += " " + schema.getString();
    str += " " + urlString + ";";
    return str;
  }

  /** This method returns the Schema of the StandardTable. */
  public Schema getSchema(){
    return schema;
  }

}

 



