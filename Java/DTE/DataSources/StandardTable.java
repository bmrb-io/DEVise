pckage DataSources;

/** StandardTable is the simplest example of a DataSource. It is an ASCII
  * file with white-spaces as field separators and is stored on a local 
  * file system */

public class StandardTable implements DataSource{

  protected Schema schema;
  protected String urlString;

  //   AccessMethod[] relaccessMeth;
  //   Statistics relStat;
       
  public StandardTable(Schema schema){
    relschema = new Schema(schema.write());
  }

  public void read(StreamTokenizer st) throws IOException {
    schema.read(st);
    if(st.nextToken() == StreamTokenizer.TT_EOF){
      return;
    }
    urlString = (String) st.sval;
  }

  public Schema getSchema(){
    return schema;
  }

  /**
  public StandardTable(Schema schema, AccessMethod[] methods, 
		       Statistica stat){
    relSchema = new Schema() 
    relSchema = schema;
    
    relStat = new Statistics();
    relStat = stat;
    
    relaccessMeth = new AccessMethod[methods.length];
    for(int i=0; i<methods.length; i++){
      relaccessMeth[i] = methods[i];
    }
  }	

  public Schema getSchema(){
    return relSchema;
  }

  
  public AccessMethArray getAccessMethods(){
    AccessMethArray tmp = new AccessMethArray(relaccessMeth);
    return tmp;
  }
  
  public Statistics getStatistics(){
    return relStat;
  }
  */
}

/** This data source is used to represent view definitions */
/**
public class SQLView implements DataSource {
}
*/
