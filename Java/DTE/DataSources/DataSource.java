package DataSources;

import Types.*;

/** DataSource interface is used to retrieve all the relevant 
    information about a particular relation. 
*/

public interface DataSource {

  /** function getSchema() returns the schema of this DataSource.
  */
	Schema getSchema();

  /** function getString() returns the string representation of this 
      DataSource.
  */
        String getString();

  //	AccessMethArray getAccessMethods();

  //	Statistics getStatistics();
        
}

