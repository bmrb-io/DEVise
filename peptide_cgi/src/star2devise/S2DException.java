// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2004
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Exception for Star2Devise.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.4  2004/04/09 18:15:15  wenger
// Added -debug flag to control output of stace traces.
//
// Revision 1.1.2.3  2004/03/04 18:26:42  wenger
// Fixed bug 030 (static variable problems in Jafar); removed
// S2DTmpFile (no longer used).
//
// Revision 1.1.2.2  2004/01/30 18:20:41  wenger
// S2D errors and warnings are now saved to files (the files are
// named according to the data name).
//
// Revision 1.1.2.1  2003/04/22 21:58:14  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.2  2001/01/17 19:55:46  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//
// Revision 1.1  2000/08/03 19:11:52  wenger
// Added S2DException class; better tolerance for certain missing data.
//

// ========================================================================

package star2devise;

import java.io.*;

public abstract class S2DException extends Exception
{
    private static int _debug = 0;

    protected static String _name = "noname";

    public static void setDebugLvl(int debugLvl) { _debug = debugLvl; }

    public static int getDebugLvl() { return _debug; }

    public S2DException(String msg)
    {
        super(msg);

	if (getDebugLvl() >= 3) {
	    printStackTrace();
	}
    }

    public static void setName(String name)
    {
        _name = name;
	S2DError.removeLogFile();
	S2DWarning.removeLogFile();
    }

    public static void clear()
    {
    	_name = "noname";
	S2DError.clear();
	S2DWarning.clear();
    }
}

// ========================================================================

