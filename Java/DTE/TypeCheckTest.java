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
		Vector whereClause = new Vector();

		// make standard table with attributes int i, double d

		StandardTable table = new StandardTable(2, "int i double d", "junk");

		fromClause.put(new TableAlias(table, "t");	// alias is "t"

		selectClause.put(new Selection("t", "i"));
		query = new Query(selectClause, fromClause, whereClause);
		System.out.println("Query was: " + query);
	}

	void typecheck(){ // throws type error

		// Hongju has to implement this part and related stuff.
		// To demonstrate that it works, build manually a bunch of queries,
		// typecheck them, and display them annotated with type information.
		// Example: select [int: [int: i.x] + [int: i.y]] ...
	}

	public static void main(String[] args) throws IOException {
		parse();	
		typecheck();
	}
}
