package DataSources;

import Types.*;

/** DataSource interface is used to retreive all the relevant 
    information about a particular relation.
*/

public interface DataSource {

	Schema getSchema();
	
  //	AccessMethArray getAccessMethods();

  //	Statistics getStatistics();
        
        String getString();
}

