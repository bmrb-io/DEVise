import java.io.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import Parser.*;


/** This is a main DTE class */

public class DTE {
	private Query query;

	void makeQuery(){

		// this should create a typechecked query by hand.

          Vector selectClause = new Vector();
          Vector fromClause = new Vector();
          Expression whereClause = null;


          // Hongju, create some fictitious relId here (like 1.1)
          // Then, when you typecheck, don't ask relationManager for
          // the schema but assume it is "int i, double d" or
          // something

          RelationId relId(1, 1);
          fromClause.put(new TableAlias(relId, "t");   // alias is "t"

          selectClause.put(new Selection("t", "i", new IntDesc()));
          query = new Query(selectClause, fromClause, whereClause);
          System.out.println("Query was: " + query);
	}

	void optimize(){

		// Donko and Nengwu should implement this.
		// To demonstrate that it works, build manually some typechecked
		// queries, create plans for them and create executable from
		// the plans.

		// Walk through the from clause list
		// Create file scans from each element and hook them 
		// together with a join.
		// Create executable Expressions along the way.
		// At the end, add select-project iterator.
	}

	public static void main(String[] args) throws IOException {
		makeQuery();	
		optimize();
	}
}
