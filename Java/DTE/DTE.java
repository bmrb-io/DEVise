import java.io.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import Parser.*;


/** This is a main DTE class */

public class DTE {
	Query query;
	void parse(){	// throws parse error

		// Jin has to implement this function and stuff closely related
		// to it.
		// To demonstrate that this function works, run it on bunch of
		// legal queries. It should print back the same query (up to
		// some formatting differences).

		SQLParser parser = new SQLParser(System.in);
		query = parser.query();
		System.out.println("Query was: " + query);
	}

	void typecheck(){ // throws type error

		// Hongju has to implement this part and related stuff.
		// To demonstrate that it works, build manually a bunch of queries,
		// typecheck them, and display them annotated with type information.
		// Example: select [int: [int: i.x] + [int: i.y]] ...
	}

	void optimize(){

		// Donko and Nengwu should implement this.
		// To demonstrate that it works, build manually some typechecked
		// queries, create plans for them and create executable from
		// the plans.
	}

	void execute(){
		
		// Gang has to implement this part and related stuff.
		// To demonstrate that it works, build manually bunch of iterator
		// trees and call getNext on the top one.
	}

	public static void main(String[] args) throws IOException {
		parse();	
		typecheck();
		optimize();
		execute();
	}
}
