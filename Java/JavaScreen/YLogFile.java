// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.10  1999/12/10 15:37:02  wenger
// Added standard headers to source files.
//
// ========================================================================


import java.io.*;

public class YLogFile
{
    protected BufferedWriter logfile = null;
    protected String filename = null;

    // loglevel <= 0 means no log information is written to file, otherwise,
    // log information will be written to file if loglevel is larger than or
    // equal to the specified level in various 'print' function
    protected int loglevel;

    // status > 0 means everything is fine
    // status = 0 means loglevel <= 0 so no log information is written
    // status = -1 means can not open the log file
    // status = -2 means something is wrong while try to write to log file
    protected int status;

    public YLogFile(String file, int level, boolean isAppend)
    {
        if (file == null)
            filename = new String("defaults.log");
        else
            filename = file;

        loglevel = level;

        if (loglevel > 0) {
            open(isAppend);
        } else {
            status = 0;
        }
    }

    public YLogFile(String file, int level)
    {
        this(file, level, false);
    }

    public YLogFile(String file)
    {
        this(file, 1, false);
    }

    public YLogFile(int level)
    {
        this("defaults.log", level, false);
    }

    public YLogFile()
    {
        this("defaults.log", 1, false);
    }

    private void open(boolean isAppend)
    {
        try {
            logfile = new BufferedWriter(new FileWriter(filename, isAppend));
            status = 1;
        } catch (IOException e) {
            logfile = null;
            status = -1;
        }
    }

    public synchronized void close()
    {
        if (logfile != null) {
            try {
                logfile.close();
            } catch (IOException e) {
                status = -2;
            }

            logfile = null;
        }
    }

    public synchronized void print(String msg, int level, boolean isAddNewline, boolean isCloseFile)
    {
        if (level > loglevel || status <= 0) {
            return;
        } else {
            try {
                if (logfile == null) {
                    open(true);
                }

                logfile.write(msg, 0, msg.length());
                if (isAddNewline) {
                    logfile.newLine();
                }

                if (isCloseFile) {
                    close();
                }
            } catch (IOException e) {
                status = -2;
                close();
            }
        }
    }

    public void pn(String msg, int level, boolean isCloseFile)
    {
        print(msg, level, true, isCloseFile);
    }

    public void pn(String msg, int level)
    {
        print(msg, level, true, false);
    }

    public void pn(String msg)
    {
        print(msg, 1, true, false);
    }

    public void p(String msg, int level, boolean isCloseFile)
    {
        print(msg, level, false, isCloseFile);
    }

    public void p(String msg, int level)
    {
        print(msg, level, false, false);
    }

    public void p(String msg)
    {
        print(msg, 1, false, false);
    }

    public synchronized int getStatus()
    {
        return status;
    }

    public int getLoglevel()
    {
        return loglevel;
    }

    public String getFilename()
    {
        return filename;
    }
}
