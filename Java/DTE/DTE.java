import java.io.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import Parser.*;
import DataSources.*;
import Optimizer.*;


/** This is a main DTE class */

public class DTE {
	public static void main(String[] args) throws IOException {
		SQLParser parser = new SQLParser(System.in);
		Query query = null;
		try {
			query = parser.query();
		}
		catch (Parser.ParseException e){
			System.out.println(e);
			System.exit(1);
		}
		System.out.println("Query was: " + query);

		RelationManager relMngr = 
			new RelationManager("idFile.dte", "defFile.dte");
                try {
	        	query.typeCheck(relMngr);
                }
		catch( TypeCheckException e )
		{
			System.out.println(e);
			System.exit(1);
		}
		System.out.println("Type checked Query is: " + query.printTypes());
		PlanNode plan = query.optimize();
		Iterator topIter = plan.createIterator();
          for(Tuple t = topIter.getFirst(); t != null; t = topIter.getNext()){
               t.print(System.out);
          }

	}
}
