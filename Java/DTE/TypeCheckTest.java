import java.io.*;
import java.util.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import Parser.*;


/** This is a main DTE class */

public class DTE {

	private RelationManager relationMngr;
	private Query query;

	void parse(){	// throws parse error

		Vector selectClause = new Vector();
		Vector fromClause = new Vector();
		Expression whereClause = null;


		// Hongju, create some fictitious relId here (like 1.1)
		// Then, when you typecheck, don't ask relationManager for
		// the schema but assume it is "int i, double d" or 
		// something

		RelationId relId(1, 1);
		fromClause.put(new TableAlias(relId, "t");	// alias is "t"

		selectClause.put(new Selection("t", "i"));
		query = new Query(selectClause, fromClause, whereClause);
		System.out.println("Query was: " + query);
	}

	void typecheck(){ // throws type error

		// Hongju has to implement this part and related stuff.
		// To demonstrate that it works, build manually a bunch of queries,
		// typecheck them, and display them annotated with type information.
		// Example: select [int: [int: i.x] + [int: i.y]] ...
		
		// Also, use the setDataSource method on each TableAlias
		// in from clause to set the dataSource returned by relationMngr
	}

	public static void main(String[] args) throws IOException {
		parse();	
		typecheck();
	}
}
