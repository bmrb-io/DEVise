package Expressions;

import java.io.*;
import java.util.*;
import DataSources.*;
/**  */

public class Query {
	Vector selectClause;
	Vector fromClause;
	Expression whereClause;

	public Query(Vector selCl, Vector fromCl, Expression whereCl){
		selectClause = selCl;
		fromClause = fromCl;
		whereClause = whereCl;
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


               s = s + " WHERE " + whereClause.toString();

               return s; 


        }

}

