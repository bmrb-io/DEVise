package Expressions;

import java.io.*;
import java.util.*;

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
}
