package Optimizer;

import Iterators.*;
import Expressions.*;
import Types.*;
import DataSources.*;
import java.util.*;

public class FileScanNode extends PlanNode {
	TableAlias ta;

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

		inpProjLists = new Vector[1];
		inpProjLists[0] = new Vector();
		String[] attrs = schema.getAttributeNames();
		for(int i = 0; i < attrs.length; i++){
			Selection s = new Selection(ta.getAlias() , attrs[i]);
			inpProjLists[0].addElement(s);
		}

		aliases = new Vector();
		aliases.addElement(ta.getAlias());

		calcParameters();

		return new SelProj(it, exProj, exPred, outputTypes);
	}
}
