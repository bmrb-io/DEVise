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
    String urlString = null;
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

  public String getTypeNm(){
    String str = "StandardTable";
    return str;
  }

  public boolean read(StreamTokenizer st) throws IOException {
    if(schema.read(st)){
      {
	if(st.nextToken() == StreamTokenizer.TT_WORD){
	  urlString = (String) st.sval; 
	  return true;
	}
	else throw new IOException("RelFileName Expected");
      }
    else return false;   
    }
  }

  public String getString(){
    String str = getTypeNm();
    str += " " + schema.getString();
    str += " " + urlString + ";";
    return str;
  }

  public void printDatasource(){
    System.out.println( getString());
  }

  public Schema getSchema(){
    return schema;
  }

}

 
