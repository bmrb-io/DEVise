import java.io.*;
import Iterators.*;
import Types.*;
import Expressions.*;
import Parser.*;


/** This is a main DTE class */

public class DTE {
	public static void main(String[] args) throws IOException {
		SQLParser parser = new SQLParser(System.in);
		Query query = parser.query();
	}
}
