package Optimizer;

import Iterators.*;
import Expressions.*;
import Types.*;
import DataSources.*;
import java.util.*;

public class NLJoinNode extends PlanNode {

	static final int bucketSize = 1000;

	PlanNode[] inputNodes;

	public NLJoinNode(PlanNode leftNode, PlanNode rightNode, Query query){
		inputNodes = new PlanNode[2];
		inputNodes[0] = leftNode;
		inputNodes[1] = rightNode;
		this.query = query;
	}
	public Iterator createIterator() throws java.io.IOException {

		inpProjLists = new Vector[2];
		aliases = new Vector();
		Iterator[] inpIters = new Iterator[2];
		TypeDesc[][] inpTypes = new TypeDesc[2][];

		for(int j = 0; j < 2; j++){
			inpIters[j] = inputNodes[j].createIterator();
			inpProjLists[j] = inputNodes[j].getProjList();

			Vector als = inputNodes[j].getAliases();
			for(int i = 0; i < als.size(); i++){
				aliases.addElement(als.elementAt(i));
			}
			inpTypes[j] = inputNodes[j].getOutputTypes();
		}

		calcParameters();

		return new NLjoinIterator(inpIters[0], inpIters[1], exProj, exPred, 
			outputTypes, inpTypes[0], inpTypes[1], bucketSize);
	}
}
