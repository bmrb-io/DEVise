package Optimizer;

import Iterators.*;
import Expressions.*;
import Types.*;
import java.util.*;

public abstract class PlanNode {
	Query query;

	// these members are initialized only after iterator is created

	ExecExpr[] exPred;
	ExecExpr[] exProj;
	TypeDesc[] outputTypes;
	Vector projList;
	Vector aliases;
	Vector[] inpProjLists;

	Vector getAliases(){
		return aliases;
	}
     TypeDesc[] getOutputTypes(){
		return outputTypes;
	}
     Vector getProjList(){
		return projList;
	}
	void calcParameters(){

          // must call createPredList before createProjList
          // because it may change the WHERE clause

          Vector predList = query.createPredList(aliases);
          exPred = new ExecExpr[predList.size()];
          for(int i = 0; i < predList.size(); i++){
               Expression e = (Expression) predList.elementAt(i);
               exPred[i] = e.createExec(inpProjLists);
          }
          projList = query.createProjList(aliases);
          exProj = new ExecExpr[projList.size()];
          outputTypes = new TypeDesc[projList.size()];
          for(int i = 0; i < projList.size(); i++){
               Expression e = (Expression) projList.elementAt(i);
               exProj[i] = e.createExec(inpProjLists);
               outputTypes[i] = e.getType();
          }
	}
	public abstract Iterator createIterator() throws java.io.IOException;
}
