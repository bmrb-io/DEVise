// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen command log playback.  This class starts a thread that sends 
// playback request.

// ------------------------------------------------------------------------
// $Log$
// Revision 1.7  2001/10/05 20:19:08  xuk
// Fixed bug 700: show error message for "null" log file name.
//
// Revision 1.6  2001/10/05 20:13:31  xuk
// *** empty log message ***
//
// Revision 1.5  2001/10/05 20:00:25  xuk
// Fixed bug 701: command log playwork can work for URL and simple file path.
//
// Revision 1.4  2001/10/05 19:26:32  xuk
// Fixed bug 698: manipulating 3D command playback.
//
// Revision 1.3  2001/10/02 22:05:51  xuk
// *** empty log message ***
//

// ========================================================================

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.io.*;
import java.net.*;

public class DEVisePlayback implements Runnable
{
    private static final int DEBUG = 0;

    private DEViseCmdDispatcher _dispatcher = null;
    private Thread _thread = null;
    private String _filename = null;
    private jsdevisec _jsc = null;

    public DEVisePlayback(jsdevisec jsc, DEViseCmdDispatcher dispatcher,
      String filename) {
        _dispatcher = dispatcher;
	_filename = filename;
	_jsc = jsc;

	_thread = new Thread(this);
	_thread.start();
    }

    public void run()
    {
	long pretime = 0, time = 0;

	try {
	    _jsc.isPlayback = true;

	    BufferedReader file = null;

	    if (_filename == null) {
		_jsc.showMsg("No log file name specified. Stop playback.");
		stop();
		return;
	    }

	    if ( _filename.startsWith("http") ) { 
		URL myURL = new URL(_filename);
		file = new BufferedReader(new InputStreamReader(
		  myURL.openStream()));
	    } else {
		file = new BufferedReader(new FileReader(_filename));
	    }

	    System.out.println("Starting playback of command log " + _filename);
	    _jsc.pn("Starting playback of command log " + _filename);

	    String line = null;
	     
	    while ((line = file.readLine()) != null) {
	        _jsc.pn("read line: " + line);
		if (DEBUG >= 1) {
	            System.out.println("read line: " + line);
		}
	        time = Long.valueOf(line).longValue();
		    
	        //
	        // If we are attempting to play back at the original
	        // rate, sleep to correspond with that rate.
	        //
	        // Note: we *must* sleep here, no matter what.  This
	        // seems to allow the dispatcher to start actually
	        // running one command, before we try to start the next
	        // one, which keeps things from getting goofed up.
	        // I'm not totally comfortable with this, but I am
	        // leaving it this way for now.  RKW 2001-10-16.
	        long gap;
	        if (_jsc.isOriginal && pretime != 0) {
		    gap = time - pretime;
		    gap = Math.max(gap, 1);
	        } else {
	            gap = 1;
	        }
	        _jsc.pn("  We are going to sleep " + gap + " ms");
		if (DEBUG >= 1) {
	            System.out.println("  We are going to sleep " + gap +
		      " ms");
		}
	        Thread.sleep(gap);
	        pretime = time;		
		    
	        line = file.readLine();
		if (DEBUG >= 1) {
	            System.out.println("read line: " + line);
		}
	        _jsc.pn("read line: " + line);

	        if ( line.startsWith(DEViseCommands.CONNECT) ||
	          line.startsWith(DEViseCommands.GET_SESSION_LIST) ||
	          line.startsWith(DEViseCommands.EXIT) ) {

		    //
		    // Special case -- don't execute these commands.
		    //
		    continue;

	        } else if ( line.startsWith(DEViseCommands.SET_3D_CONFIG) ) {

		    //
		    // Special case -- execute this command on the
		    // client side.
		    //
		    String[] args = DEViseGlobals.parseString(line);
		    _jsc.jscreen.collab3DView(args);
		    continue;

	        } else {
		    if (line.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
		        //
		        // Set display size to *our* display size, not
		        // the one used when the log was generated.
		        //
		        line = DEViseCommands.SET_DISPLAY_SIZE + " " + 
			    _jsc.jsValues.uiglobals.screenSize.width + " " + 
			    _jsc.jsValues.uiglobals.screenSize.height + " " + 
			    _jsc.jsValues.uiglobals.screenRes + " " + 
			    _jsc.jsValues.uiglobals.screenRes;
		    }

		    //
		    // Send the command to the JSPoP.
		    //
		    _dispatcher.start(line);
	        }
	    }

	    file.close();

	} catch (InterruptedException e) {
	    System.err.println("Wrong long number format: " + e.getMessage());
	} catch (NumberFormatException e) {
	    System.err.println("Wrong long number format: " + e.getMessage());
	} catch (FileNotFoundException e) {
	    System.err.println("File not found (" + e.getMessage() + "): " +
	      _filename);
	} catch (IOException e) {
	    System.err.println("File read error (" + e.getMessage() + "): " +
	      _filename);
	} finally {
	    System.out.println("Done playing back command log");
	    _jsc.pn("Done playing back command log");
	    if (!_jsc.isDisplay) {
		//
		// Close the session so that the JS isn't left in a confusing
		// state (session open but nothing showing).
		//
	        _dispatcher.start(DEViseCommands.CLOSE_SESSION);
	    }
	    _jsc.isDisplay = true;
	    _jsc.isPlayback = false;
	    stop();
	}
    }

    public void stop()
    {
        _thread.stop();
    }
}


