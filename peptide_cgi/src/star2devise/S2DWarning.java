// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2004
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Warning for Star2Devise.

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
// Revision 1.1.2.2  2004/01/30 18:20:42  wenger
// S2D errors and warnings are now saved to files (the files are
// named according to the data name).
//
// Revision 1.1.2.1  2003/04/22 21:58:16  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1  2001/01/17 20:00:08  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

package star2devise;

import java.io.*;

public class S2DWarning extends S2DException
{
    private static FileWriter _writer = null;

    public S2DWarning(String msg)
    {
        super(msg);

        if (getDebugLvl() >= 0) {
            if (_writer == null) {
                createWriter();
            }
            if (_writer != null) {
                try {
                    _writer.write(msg + "\n");
                    _writer.flush();
                } catch(IOException ex) {
                    System.err.println("Error writing error log: " + ex);
                }
            }
        }

        if (getDebugLvl() >= 2) {
            printStackTrace(new PrintWriter(_writer));
        }
    }

    public static void removeLogFile()
    {
        _writer = null;
        try {
            S2DUtils.deleteFile(getFileName());
        } catch(IOException ex) {
            System.err.println(
                "Error trying to delete warning log file: " + ex);
        }
    }

    public static void clear()
    {
        _writer = null;
    }

    private static String getFileName()
    {
        return "tmp/s2d." + _name + ".warn";
    }

    private static void createWriter()
    {
        try {
            _writer = S2DFileWriter.create(getFileName());
        } catch(IOException ex) {
            System.err.println("Can't create S2DError file writer: " + ex);
        }
    }
}

// ========================================================================

