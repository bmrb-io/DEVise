package DataSources;

public class TableAlias{

	// Jie, this class is the mirror of TableAlias in Core/myopt.h

	RelationId relId;
	String alias;

	DataSource dataSource;	

	public TableAlias(RelationId relId, String alias){

		this.relId = relId;
		this.alias = alias;
	}

	public String getAlias(){
		return alias;
	}

	public RelationId getRelationId(){
		return relId;
	}

	// implement toString
     
        public String toString()
        {
           return relId.toString() + " AS " + alias;
        }

	public void setDataSource(DataSource ds){
		dataSource = ds;
	}
	public DataSource getDataSource(){
		return dataSource;
	}
}
