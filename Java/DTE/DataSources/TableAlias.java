
class TableAlias{

	// Jie, this class is the mirror of TableAlias in Core/myopt.h

	DataSource dataSource;

	RelationId relId;
	String alias;

	public TableAlias(RelationManager relationMngr, RelationId relId, 
		String alias){

		this.relId = relId;
		this.alias = alias;
		dataSource = relationMngr.createDataSource(relId);
	}
	public TableAlias(DataSource dataSource, String alias){

		// for testing purposes only

		this.alias = alias;
		this.dataSource = dataSource;
	}

	// implement toString
	Schema getSchema(){
		Schema tmp = dataSource.getSchema();	

		// prepend the alias to the attribute names;
	}
}
