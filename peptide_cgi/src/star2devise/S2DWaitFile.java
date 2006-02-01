// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements waiting for a "done file".  (This is used as
// a substitute for the Process.waitFor() method, which seems not to
// work on some platforms.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.3  2005/01/31 21:35:44  wenger
// Slight mods to atomic coordinates template for better highlight
// selection; s2d chem shift ref timeout increased to 120 sec., added
// try-again message if timed out; cleaned up warning/error messages
// somewhat.
//
// Revision 1.1.2.2  2003/10/30 16:57:13  wenger
// Added timeout on chemical shift reference processing.
//
// Revision 1.1.2.1  2003/10/29 18:52:26  wenger
// Chem shift reference no longer uses Process.waitFor(), since that
// seems not to work on some platforms.
//

// ========================================================================

package star2devise;

import java.io.File;

public class S2DWaitFile implements Runnable
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    private File _file;
    private int _delay;
    private Thread _newThread;
    private Thread _mainThread;
    private boolean _found = false;
    private boolean _done = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    // @param The name of the "done file" to wait on.
    public S2DWaitFile(String filename) throws S2DException
    {
	if (DEBUG >= 1) {
	    System.out.println("S2DWaitFile.S2DWaitFile(" + filename + ")");
	}

        _file = new File(filename);
	if (_file.exists()) {
	    if (!_file.delete()) {
	        throw new S2DError("Unable to delete existing wait file " +
		  filename);
	    }
	}
    }

    //-------------------------------------------------------------------
    // Wait for the file.
    // @param The number of milliseconds to wait between checks for file
    // existance.
    // @param The maximum number of milliseconds to wait before returning,
    // even if the "done file" doesn't exist.
    public void wait(int delay, int timeout) throws InterruptedException
    {
	if (DEBUG >= 1) {
            System.out.println("S2DWaitFile(" + _file.getName() +
	      ").wait(" + delay + ", " + timeout + ")");
        }

	_delay = delay;

	_mainThread = Thread.currentThread();
	_newThread = new Thread(this);
	_newThread.start();

	try {
	    Thread.sleep(timeout);
	} catch (InterruptedException ex) {
	    // No op -- we want to be interrupted here.
	}
	_done = true;

	if (!_found) {
	    throw new InterruptedException(
	      "Timed out waiting for done file " + _file.getName() +
	      " (you may want to try again)");
	}
    }

    public void run()
    {
	if (DEBUG >= 1) {
            System.out.println("S2DWaitFile(" + _file.getName() + ").run()");
        }

	while (!_found && !_done) {
	    try {
	        Thread.sleep(_delay);
	    } catch (InterruptedException ex) {
	    	// No op.
	    }
            if (DEBUG >= 2) {
	        System.out.println("Checking for done file");
	    }
	    _found = _file.exists();
	}

        if (DEBUG >= 2) {
	    if (_found) {
	        System.out.println("Done file " + _file.getName() +
		  " was found");
	    } else {
	        System.out.println("Done file " + _file.getName() +
		  " was NOT found");
	    }
	}

	if (_found) {
	    if (!_file.delete()) {
	        System.err.println("Warning: file " + _file.getName() +
	          " not deleted");
	    }
	}

	_mainThread.interrupt();
    }
}

// ========================================================================
