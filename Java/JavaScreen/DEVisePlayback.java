// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
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
// ------------------------------------------------------------------------
// $Log$
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
    private DEViseCmdDispatcher _dispatcher = null;
    private Thread _thread = null;
    private String _filename = null;
    private jsdevisec _jsc = null;

    public DEVisePlayback(jsdevisec jsc, DEViseCmdDispatcher dispatcher, String filename) {
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
	    BufferedReader file = null;

	    if ( _filename.startsWith("http") ) { 
		URL myURL = new URL(_filename);
		file = new BufferedReader(new InputStreamReader(myURL.openStream()));
	    } else {
		file = new BufferedReader(new FileReader(_filename));
	    }

	    String line = null;
	     
	    if (_jsc.isDisplay && _jsc.isOriginal) {
		while ((line = file.readLine()) != null) {
		    _jsc.pn("read line: " + line);
		    time = Long.valueOf(line).longValue();
		    
		    if (pretime != 0) {
			long gap = time-pretime;
			_jsc.pn("we are going to sleep " + gap + "ms.");
			Thread.sleep(gap);
		    }
		    pretime = time;		
		    
		    line = file.readLine();
		    _jsc.pn("read line: " + line);
		    if ( line.startsWith(DEViseCommands.CONNECT) || 
			 line.startsWith(DEViseCommands.GET_SESSION_LIST) ||
			 line.startsWith(DEViseCommands.EXIT) ) {
			continue;
		    } else if ( line.startsWith(DEViseCommands.SET_3D_CONFIG) ) {
			String[] args = DEViseGlobals.parseString(line);
			_jsc.jscreen.collab3DView(args);
			continue;
		    } else {
			if (line.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
			    line = DEViseCommands.SET_DISPLAY_SIZE + " " + 
				_jsc.jsValues.uiglobals.screenSize.width + " " + 
				_jsc.jsValues.uiglobals.screenSize.height + " " + 
				_jsc.jsValues.uiglobals.screenRes + " " + 
				_jsc.jsValues.uiglobals.screenRes;
			}
			_dispatcher.start(line);
		    }
		}
	    } else { // not original playback
		String cmd = new String();
		while ((line = file.readLine()) != null) {
		    _jsc.pn("read line: " + line);

		    line = file.readLine();
		    _jsc.pn("read line: " + line);

		    if ( line.startsWith(DEViseCommands.CONNECT) || 
			 line.startsWith(DEViseCommands.GET_SESSION_LIST) ||
			 line.startsWith(DEViseCommands.EXIT) ) {
			continue;
		    } else {
			if (line.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
			    line = DEViseCommands.SET_DISPLAY_SIZE + " " + 
				_jsc.jsValues.uiglobals.screenSize.width + " " + 
				_jsc.jsValues.uiglobals.screenSize.height + " " + 
				_jsc.jsValues.uiglobals.screenRes + " " + 
				_jsc.jsValues.uiglobals.screenRes;
			}
			cmd = cmd + line + "\n"; 
		    } 
		}
		_dispatcher.start(cmd);
	    }

	    file.close();
	} catch (InterruptedException e) {
	} catch (NumberFormatException e) {
	    _jsc.showMsg("Wrong long number format.");
	    stop();
	} catch (FileNotFoundException e) {
	    _jsc.showMsg("File not found: " + _filename);
	    stop();
	} catch (IOException e) {
	    _jsc.showMsg("File read error: " + _filename);
	    stop();
	}
    }

    public void stop()
    {
        _thread.stop();
    }
}


