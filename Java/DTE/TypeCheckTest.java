import java.io.*;
import java.util.*;
import Iterators.*;
import DataSources.*;
import Types.*;
import Expressions.*;
import Parser.*;

/** This is a main DTE class */

public class TypeCheckTest {
    private RelationId relId;
    private Query query;
    private SymbolTable symbolTable;

    public TypeCheckTest( ) {
    }

    void parse( ) {
        Vector selectClause = new Vector();
        Vector fromClause = new Vector();
        Expression whereClause = null;

        // Hongju, create some fictitious relId here (like 1.1)
        // Then, when you typecheck, don't ask relationManager for
        // the schema but assume it is "int i, double d" or 
        // something

        relId = new RelationId(1,1);
        fromClause.addElement( new TableAlias(relId, "t") );

        TypeDesc type = new IntDesc( );
        Selection s = new Selection( "t", "a", type );
        Constant c = new Constant( new DTE_Int( 3 ) );
        Operator op = new Operator( "+", s, c );
        selectClause.addElement( op );
        query = new Query(selectClause, fromClause, whereClause);
        System.out.println("Query was: " + query);
    }
 
    void typeCheck( ) {
        symbolTable = new SymbolTable( );
	symbolTable.put( new Selection("t", "i", new IntDesc()) );
	symbolTable.put( new Selection("t", "d") );

	Operator op = (Operator)(query.getSelectClause( ).elementAt(0));
	op =(Operator)op.typeCheck( symbolTable );

    // Hongju has to implement this part and related stuff.
    // To demonstrate that it works, build manually a bunch of queries,
    // typecheck them, and display them annotated with type information.
    // Example: select [int: [int: i.x] + [int: i.y]] ...

    // Also, use the setDataSource method on each TableAlias
    // in from clause to set the dataSource returned by relationMngr
    }

    public static void main(String[] args) throws IOException {
        TypeCheckTest test = new TypeCheckTest( );
        test.parse( );
        test.typeCheck( );
    }
}
