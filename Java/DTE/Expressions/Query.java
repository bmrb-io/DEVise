package Expressions;

import java.io.*;
import java.util.*;
import DataSources.*;
import Types.*;
import Optimizer.*;
import Iterators.*;

/**  */

public class Query {
	Vector selectClause;
	Vector fromClause;
	private Expression whereClause;

	// this member is created;

	Vector predList;	// Conj normal form of where clause

	public Query(Vector selCl, Vector fromCl, Expression whereCl){
		selectClause = selCl;
		fromClause = fromCl;
		whereClause = whereCl;

		predList = new Vector();
		if(whereClause != null){
			predList.addElement(whereClause);  // for now not in cnf
		}
	}

	public Vector getFromClause(){
		return fromClause;
	}
	public Vector getSelectClause(){
		return selectClause;
	}
	/*
	public Expression getWhereClause(){
		return whereClause;
	}
	*/
	public Vector createProjList(Vector aliases){
		Vector retVal = new Vector();
		for(int i = 0; i < selectClause.size(); i++){
			Expression e = (Expression) selectClause.elementAt(i);
			e.collect(aliases, retVal);
		}
		for(int i = 0; i < predList.size(); i++){
			Expression e = (Expression) predList.elementAt(i);
			e.collect(aliases, retVal);
		}
		return retVal;
	}

	public Vector createPredList(Vector aliases){

		// WARNING: this function changes the predicate list

		Vector retVal = new Vector();
		Vector newPredList = new Vector();
		for(int i = 0; i < predList.size(); i++){
			Expression e = (Expression) predList.elementAt(i);
			if(e.exclusive(aliases)){
				retVal.addElement(e);
			}
			else{
				newPredList.addElement(e);
			}
		}
		predList = newPredList;
		return retVal;
	}

        public String toString(){
               String s = "SELECT ";
               Expression e = null;
               TableAlias t = null;

               for ( int i = 0; i < selectClause.size(); i++ ) 
               {
                  e = ( Expression )( selectClause.elementAt(i) );
                  if ( i != (selectClause.size() -1) )
                     s = s + e.toString() + ", ";
                  else
                     s = s + e.toString() ;
               }

               s = s +  " FROM ";

               for ( int i = 0; i < fromClause.size(); i++ )
               {
                  t = ( TableAlias )( fromClause.elementAt(i) );
                  if ( i != (fromClause.size() -1))
                     s = s + t.toString() + " ";
                  else
                     s = s + t.toString();
               }


               if(whereClause != null){			
                  s = s + " WHERE " + whereClause.toString();
               }

               return s; 


        }

	public void typeCheck(RelationManager relMngr) throws IOException {
		SymbolTable symbolTable = new SymbolTable( );
		for(int i = 0; i < fromClause.size(); i++){
			TableAlias ta = (TableAlias) fromClause.elementAt(i);
			RelationId relId = ta.getRelationId();
			String alias = ta.getAlias();
			DataSource dataSrc = relMngr.createDataSource(relId);
			Schema schema = dataSrc.getSchema();
			TypeDesc[] types = schema.getTypeDescs();
			String[] attrs = schema.getAttributeNames();
			for(int j = 0; j < attrs.length; j++){
				symbolTable.put(new Selection(alias, attrs[j], types[j]));
			}
			ta.setDataSource(dataSrc);
		}
		for(int i = 0; i < selectClause.size(); i++){
			Expression e = (Expression) selectClause.elementAt(i);
			e = e.typeCheck(symbolTable);
			selectClause.setElementAt(e, i);
		}
		for(int i = 0; i < predList.size(); i++){
			Expression e = (Expression) predList.elementAt(i);
			e = e.typeCheck(symbolTable);
			predList.setElementAt(e, i);
		}
	}
	public String printTypes(){

		// to avoid writing new code, maybe we should parametrize
		// print function (0 for no types, 1 for print with types).

		return toString();
	}
	public PlanNode optimize() throws IOException {
		TableAlias curTa = (TableAlias) fromClause.elementAt(0);
		PlanNode leftNode = new FileScanNode(curTa, this);
          for(int j = 1; j < fromClause.size(); j++){
               curTa = (TableAlias) fromClause.elementAt(j);
               PlanNode rightNode = new FileScanNode(curTa, this);
			leftNode = new NLJoinNode(leftNode, rightNode, this);
          }
		return leftNode;
	}
}
