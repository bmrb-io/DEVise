import java.io.*;
import java.util.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import DataSources.*;
import Parser.*;


/** This is a main DTE class */

public class OptimizerTest {
	static private Iterator topIter;
	static private Query query;

	static void makeQuery(){

		// this should create a typechecked query by hand.

          Vector selectClause = new Vector();
          Vector fromClause = new Vector();
          Expression whereClause = null;


          // Hongju, create some fictitious relId here (like 1.1)
          // Then, when you typecheck, don't ask relationManager for
          // the schema but assume it is "int i, double d" or
          // something

          RelationId relId = new RelationId(1, 1);
		TableAlias ta = new TableAlias(relId, "t");   // alias is "t"
		ta.setDataSource(new StandardTable(
			new Schema(2, "int i double d"), "./test.txt"));
          fromClause.addElement(ta);

          selectClause.addElement(new Selection("t", "i", new IntDesc()));
          query = new Query(selectClause, fromClause, whereClause);
          System.out.println("Query was: " + query);
	}

	static void optimize() throws IOException {

		// Donko and Nengwu should implement this.
		// To demonstrate that it works, build manually some typechecked
		// queries, create plans for them and create executable from
		// the plans.

		// Walk through the from clause list
		// Create file scans from each element and hook them 
		// together with a join.
		// Create executable Expressions along the way.
		// At the end, add select-project iterator.

		Vector fromClause = query.getFromClause();
		Vector selectClause = query.getSelectClause();
		Expression whereClause = query.getWhereClause();

		TableAlias curTa = (TableAlias) fromClause.firstElement();
		DataSource curDs = curTa.getDataSource();
		StandardTable st = (StandardTable) curDs;
		String fileName = st.getFileName();
		Schema schema = st.getSchema();
		TypeDesc[] types = schema.getTypeDescs();
		topIter = new FileScanIterator(fileName, types);

		Vector fsOut = new Vector();
		String[] attrs = schema.getAttributeNames();
		for(int i = 0; i < attrs.length; i++){
			fsOut.addElement(new Selection(curTa.getAlias() , attrs[i]));
		}

		ExecExpr[] exProj = new ExecExpr[selectClause.size()];
		ExecExpr[] exWhere = new ExecExpr[(whereClause == null ? 0 : 1)];
		TypeDesc[] curTypes = new TypeDesc[selectClause.size()];

		for(int i = 0; i < selectClause.size(); i++){
			curTypes[i] = ((Expression) selectClause.elementAt(i)).getType();
			exProj[i] = new ExecSelect(0, 0);
		}

		topIter = new SelProj(topIter, exProj, exWhere, curTypes);
		for(int i = 1; i < fromClause.size(); i++){
			// make joins here
		}
	}

	public static void main(String[] args) throws IOException {
		makeQuery();	
		optimize();
		for(Tuple t = topIter.getFirst(); t != null; t = topIter.getNext()){
			t.print(System.out);
		}
	}
}
