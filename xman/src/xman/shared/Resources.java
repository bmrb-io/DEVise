//--------------------------------------------------------------------------
// 
// File Name            : resources.java
//
// Description          : Contains resources information used by experiment
//                        management.
//
// Author               : Hao Wang
//
// D.O.B.               : 2/7/2000
//
// $id:$
//--------------------------------------------------------------------------   
package xman.shared;

import java.awt.Font;
import java.text.SimpleDateFormat;
import java.text.DateFormat;

public interface Resources {

    final static String STR_APP_NAME              = "Experiment Management";
    final static String STR_APP_VERSION           = "0.1";
    final static String STR_OKAY                  = "Ok";
    final static String STR_CANCEL                = "Cancel";
    final static String STR_HELP                  = "Help";
    final static String STR_BROWSE_TABLET         = "Experiment Info";
    final static String STR_DATA_TABLET           = "Add new experiment";
    final static String STR_ATTRIBUTE_TABLET      = "Attribute Editor";
    final static String STR_QUERY_TABLET          = "Query";
    final static String STR_BROWSE_TABLET_HELP    = "Browing data in the system";
    final static String STR_DATA_TABLET_HELP      = "Adding new experiment data to the system";
    final static String STR_ATTRIBUTE_TABLET_HELP = "Adding/modifying experiemnt attributes";
    final static String STR_QUERY_TABLET_HELP     = "Querying";
    final static String STR_XMAN_META_FILE        = ".xmanMeta";

    final static String[] importFilter = { "Paradyn Trace Data",
					   "Sun HPC Trace Data", 
					   "HP Trace" };


    // More shared
    final static DateFormat formatter =
	DateFormat.getDateInstance(DateFormat.MEDIUM);
    final static Font defaultFont = new Font("Times", Font.PLAIN, 12);

    // Currently supported data
    final static short PARADYN = 0;

    final static short[] Supported = { PARADYN };
};












