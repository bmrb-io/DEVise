
class TableAlias{

	// Jie, this class is the mirror of TableAlias in Core/myopt.h

	RelationId relId;
	String alias;

	DataSource dataSource;	

	public TableAlias(RelationId relId, String alias){

		this.relId = relId;
		this.alias = alias;
	}

	// implement toString

	void setDataSource(DataSource ds){
		dataSource = ds;
	}
	DataSource getDataSource(){
		return dataSource;
	}
}
