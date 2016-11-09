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

// ========================================================================

package JavaScreen;

import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;

public class DEVisePlayback implements Runnable
{
    private static final int DEBUG = 0;

    private DEViseCmdDispatcher _dispatcher = null;
    private Thread _thread = null;
    private String _filename = null;
    private jsdevisec _jsc = null;
    private BufferedReader _logFile = null;

    class JSCommand {
        public long timestamp;
        public String command;
    }

    public DEVisePlayback(jsdevisec jsc, DEViseCmdDispatcher dispatcher,
                          String filename) {
        _dispatcher = dispatcher;
        _filename = filename;
        _jsc = jsc;

        _thread = new Thread(this);
        _thread.setName("Playback");
        _thread.start();
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Starting thread " + _thread);
        }
    }

    public void run()
    {
        long pretime = 0;
        long t1 = 0, t2 = 0;
        Date d = null;
        DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
                         DateFormat.MEDIUM);
        try {
            _jsc.isPlayback = true;
            _jsc.playbackMode();

            if (_filename == null) {
                _jsc.showMsg("No log file name specified. Stop playback.");
                stop();
                return;
            }

            if ( _filename.startsWith("http") ) {
                URL myURL = new URL(_filename);
                _logFile = new BufferedReader(new InputStreamReader(
                                                  myURL.openStream()));
            } else {
                _logFile = new BufferedReader(new FileReader(_filename));
            }

            d = new Date();
            t1 = d.getTime();

            System.out.println("Starting playback of command log " +
                               _filename + "\n @ " + dtf.format(d));

            _jsc.pn("Starting playback of command log " + _filename);

            JSCommand cmd = null;
            while ((cmd = readCommand()) != null) {

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
                    gap = cmd.timestamp - pretime;
                    gap = Math.max(gap, 1);
                } else {
                    gap = 1;
                }

                _jsc.pn("  We are going to sleep " + gap + " ms" +
                        " and going to execute command " + cmd.command);

                if (DEBUG >= 1) {
                    System.out.println("  We are going to sleep " + gap +
                                       " ms");
                }

                Thread.sleep(gap);
                pretime = cmd.timestamp;

                if ( // cmd.command.startsWith(DEViseCommands.CONNECT) ||
                    cmd.command.startsWith(DEViseCommands.GET_SESSION_LIST) ||
                    cmd.command.startsWith(DEViseCommands.EXIT) ) {

                    //
                    // Special case -- don't execute these commands.
                    //
                    continue;

                } else if ( cmd.command.startsWith(DEViseCommands.STOP_COLLAB) ) {
                    //
                    // Special case -- clear the JS for reopening at
                    // client side.
                    //
                    _jsc.socketMode();
                    _jsc.specialID = -1;
                    _jsc.collabinterrupted = true;
                    _jsc.dispatcher.dispatcherThread.interrupt();

                    _jsc.animPanel.stop();
                    _jsc.stopButton.setBackground(_jsc.jsValues.uiglobals.bg);
                    _jsc.jscreen.updateScreen(false);
                    _jsc.dispatcher.clearStatus();

                    if (_jsc.sessionSaved) {
                        _jsc.isSessionOpened = true;
                        _jsc.sessionSaved = false;
                    }

                    try {
                        _jsc.pn("Sending: \"" + DEViseCommands.STOP_COLLAB +"\"");
                        _dispatcher.sockSendCmd(DEViseCommands.STOP_COLLAB);
                        _jsc.restoreDisplaySize();
                    } catch (YException e) {
                        _jsc.showMsg(e.getMsg());
                    }

                } else if ( cmd.command.startsWith(DEViseCommands.SET_3D_CONFIG) ) {

                    //
                    // Special case -- execute this command on the
                    // client side.
                    //
                    String[] args = DEViseGlobals.parseString(cmd.command);
                    _jsc.jscreen.collab3DView(args);
                    continue;

                } else {
                    if (cmd.command.startsWith(DEViseCommands.SET_DISPLAY_SIZE)) {
                        //
                        // Set display size to *our* display size, not
                        // the one used when the log was generated.
                        //
                        cmd.command = DEViseCommands.SET_DISPLAY_SIZE + " " +
                                      _jsc.jsValues.uiglobals.screenSize.width + " " +
                                      _jsc.jsValues.uiglobals.screenSize.height + " " +
                                      _jsc.jsValues.uiglobals.screenRes + " " +
                                      _jsc.jsValues.uiglobals.screenRes;
                    }
                    else if ( cmd.command.startsWith(DEViseCommands.COLLABORATE) ) {
                        //
                        // Special case -- clear the JS for collaboration
                        // set temp specialID
                        //
                        _jsc.specialID = 0;

                        if (_jsc.isSessionOpened ) {
                            _jsc.jscreen.updateScreen(false);
                            _jsc.sessionSaved = true;
                        }
                    }

                    //
                    // Check whether dispatcher is still running.
                    // If so, sleep one more ms.
                    //
                    while (_dispatcher.getStatus() ==
                            DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                        Thread.sleep(1);
                    }

                    //
                    // Send the command to the JSPoP.
                    //

                    _dispatcher.start(cmd.command);
                }
            }

            _logFile.close();

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
            d = new Date();
            t2 = d.getTime();

            String elapse = new Long(t2-t1).toString();
            System.out.println("Done playing back command log" +
                               "\n @ " + dtf.format(d));
            System.out.println("Elapse time: " + elapse + " ms.");

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
            _jsc.socketMode();

            // TEMP: destroy JS on exit.
            _jsc.destroy();

            stop();
        }

        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Thread " + _thread + " ending");
        }
    }

    public void stop()
    {
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Stopping thread " + _thread);
        }
        _thread.stop();
    }

    private JSCommand readCommand() throws IOException
    {
        JSCommand cmd = null;

        String timeLine = readLine();

        if (timeLine != null) {
            String cmdLine = readLine();
            if (cmdLine != null) {
                cmd = new JSCommand();
                cmd.timestamp = Long.valueOf(timeLine.trim()).longValue();
                cmd.command = cmdLine;
            }
        }

        if (DEBUG >= 1) {
            System.out.println("Read command (timestamp " + cmd.timestamp +
                               "): " + cmd.command);
        }

        return cmd;
    }

    // Read a non-blank, non-comment line.
    private String readLine() throws IOException
    {
        String origLine = null;
        String procLine = null;

        while ((procLine = stripComment(origLine)) == null) {
            origLine = _logFile.readLine();
            if (origLine == null) break;
            _jsc.pn("read line: " + origLine);
            if (DEBUG >= 1) {
                System.out.println("read line: " + origLine);
            }
        }

        return procLine;
    }

    // Returns null if line is blank or comment; returns non-comment part
    // of line otherwise.
    private String stripComment(String line)
    {
        String result = null;

        // Note: we may want to not consider a # a comment if it's inside
        // braces.
        if (line != null) {
            boolean blank = true;
            int last = -1;

            for (int index = 0; index < line.length(); index++) {
                char tmpC = line.charAt(index);
                if (!Character.isWhitespace(tmpC)) {
                    // # is comment character.
                    if (tmpC == '#') {
                        last = index - 1;
                        break;
                    } else {
                        blank = false;
                    }
                }
            }

            if (!blank) {
                if (last > -1) {
                    result = line.substring(0, last + 1);
                } else {
                    result = line;
                }
            }
        }

        return result;
    }
}
