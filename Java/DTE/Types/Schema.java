package DataSources;

import java.util.*;

public class Schema {
  private TypeDesc[] typeIDs;
  private String[] attributeNames;

  public Schema(){
  }

  /*
  public Schema( String str){
    schemastr = new String(str);
  }
  */
  
  public boolean read(StreamTokenizer st) throws IOException {
    if(st.nextToken() == StreamTokenizer.TT_EOF){
      return false;
    }
    int numFlds = (int) st.nval;

    // read type attrName pairs, such as "int a1 double a2"

    return true;
  }

  String toString(){
  }
}
    
