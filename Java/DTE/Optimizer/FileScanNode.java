package Optimizer;

import Iterators.*;
import Expressions.*;
import Types.*;
import DataSources.*;
import java.util.*;

public class FileScanNode implements PlanNode {
	TableAlias ta;
	Query query;

	// these members are initialized only after iterator is created
 
	TypeDesc[] outputTypes;
	Vector projList;

	public TypeDesc[] getOutputTypes(){
		return outputTypes;
	}
	public Vector getProjList(){
		return projList;
	}
	public FileScanNode(TableAlias ta, Query query){
		this.ta = ta;
		this.query = query;
	}
	public Iterator createIterator() throws java.io.IOException {
		DataSource curDs = ta.getDataSource();
		StandardTable st = (StandardTable) curDs;
		String fileName = st.getFileName();
		Schema schema = st.getSchema();
		TypeDesc[] types = schema.getTypeDescs();
		Iterator it = new FileScanIterator(fileName, types);

		Vector[] fsOut = new Vector[1];
		fsOut[0] = new Vector();
		String[] attrs = schema.getAttributeNames();
		for(int i = 0; i < attrs.length; i++){
			Selection s = new Selection(ta.getAlias() , attrs[i]);
			fsOut[0].addElement(s);
		}

		Vector aliases = new Vector();
		aliases.addElement(ta.getAlias());

		// must call createPredList before createProjList
		// because it may change the WHERE clause

		Vector predList = query.createPredList(aliases);
		ExecExpr[] exPred = new ExecExpr[predList.size()];
		for(int i = 0; i < predList.size(); i++){
			Expression e = (Expression) predList.elementAt(i);
			System.out.println("_____ " + e);
			exPred[i] = e.createExec(fsOut);
		}
		projList = query.createProjList(aliases);
		ExecExpr[] exProj = new ExecExpr[projList.size()];
		outputTypes = new TypeDesc[projList.size()];
		for(int i = 0; i < projList.size(); i++){
			Expression e = (Expression) projList.elementAt(i);
			exProj[i] = e.createExec(fsOut);
			outputTypes[i] = e.getType();
		}
		return new SelProj(it, exProj, exPred, outputTypes);
	}
}
