package DataSources;

import Types.*;
import java.io.*;

/** StandardTable is the simplest example of a DataSource. It is an ASCII
  * file with white-spaces as field separators and is stored on a local 
  * file system */

public class StandardTable implements DataSource{

  protected Schema schema;
  protected String urlString;

  public StandardTable(){
    schema = new Schema();
    urlString = new String();
  }
       
  public StandardTable(Schema sch, String url){
    int nmflds = sch.getnumFlds();
    String st  = sch.getTypeAttr();

    schema = new Schema(nmflds, st);
    urlString = new String(url);
  }

  public StandardTable(int nmflds, String s, String url){
    schema = new Schema(nmflds, s);
    urlString = new String( url);
  }

  public String getFileName(){
    return urlString;
  }

  public String getTypeNm(){
    String str = "StandardTable";
    return str;
  }

  public boolean read(StreamTokenizer st) throws IOException {
    try{
        schema.read(st);
    }catch(IOException e){
      throw new IOException();};
     
    if(st.nextToken() != StreamTokenizer.TT_EOF && st.ttype == StreamTokenizer.TT_WORD ){
     
      urlString += st.sval; 
      return true;
    }
    else throw new IOException("RelFileName Expected");
  }

  public String getString(){
    String str = getTypeNm();
    str += " " + schema.getString();
    str += " " + urlString + ";";
    return str;
  }

  public Schema getSchema(){
    return schema;
  }

}

 



