package DataSources;

/* DataSource was called Interface in C++ source 
 * This class is implemented in C++ in file RelationManager.[ch] 
 * In summary, we have two files: idStream and defStream.
 * idStream is a sequence of integers that represent offsets into defFile.
 * defFile contains sequence of DataSource definitions.
*/

public class RelationManager {
     fstream idStream;
     fstream defStream;

	public RelationManager(){
	}

	/** Return immutable (unique and nonchangable) relation id */
     public RelationId registerNewRelation(DataSource dataSrc);

     public DataSource createDataSource(RelationId relId);
}
