package Expressions;

import java.io.*;

/**  */

public class Query {
	Vector selectClause;
	Vector fromClause;
	Expression whereClause;
	public Query(Vector selectClause, Vector fromClause, 
		Expression whereClause){
		
	}
	// Jie, add toString function that will return string like:
	// "select t.x + 1 from 1.1 as t"
}
