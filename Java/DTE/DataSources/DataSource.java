package DataSources;

/** DataSource interface is used to retreive all the relevant 
    information about a particular relation.
*/

public interface DataSource {
	Schema getSchema();
	AccessMethod[] getAccessMethods();
	Statistics getStatistics();
}

/** StandardTable is the simplest example of a DataSource. It is an
    ASCII file with white-spaces as field separators and is
    stored on a local file system */

/* This is called StandardInterface in C++ source file Interface.h */

public class StandardTable implements DataSource {
}

/** This data source is used to represent view definitions */

public class SQLView implements DataSource {
}
