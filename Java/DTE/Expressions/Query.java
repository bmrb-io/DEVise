package Expressions;

import java.io.*;
import java.util.*;
import DataSources.*;
/**  */

public class Query {
	Vector selectClause;
	Vector fromClause;
	Expression whereClause;

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
	public Expression getWhereClause(){
		return whereClause;
	}
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

	// Jie, add toString function that will return string like:
	// "select t.x + 1 from 1.1 as t"

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
}

