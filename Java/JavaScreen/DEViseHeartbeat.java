// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2002
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen client heartbeat.  This class starts a thread that sends a
// heartbeat every 10 minutes.

// ------------------------------------------------------------------------

// ========================================================================

package JavaScreen;

public class DEViseHeartbeat implements Runnable
{
    private static final int DEBUG = 0;

    // ten minutes in millisec.
    private static final int HB_INTERVAL = 10 * 60 * 1000;

    private DEViseCmdDispatcher _dispatcher = null;

    private Thread _hbThread = null;

    private int _hbCount = 0;

    public DEViseHeartbeat(DEViseCmdDispatcher dispatcher) {
        _dispatcher = dispatcher;

        _hbThread = new Thread(this);
        _hbThread.setName("Heartbeat");
        _hbThread.start();
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Starting thread " + _hbThread);
        }
    }

    public void run()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseHeartBeat.run()");
        }

        while (true) {
            try {
                Thread.sleep(HB_INTERVAL);
            } catch (InterruptedException ex) {
            }

            if (DEBUG >= 1) {
                System.out.println("Sending heartbeat");
            }

            if (_dispatcher.jsc.specialID == -1) { // normal JS
                _dispatcher.start(DEViseCommands.HEART_BEAT + " " +
                                  " " + _hbCount);
            } else {
                //TEMP -- I don't like that we're bypassing the whole
                // command dispatcher here.  RKW 2002-07-18.
                try {
                    _dispatcher.commSocket.sendCmd(new String[]
                                                   {DEViseCommands.HEART_BEAT,
                                                    Integer.toString(_hbCount)
                                                   },
                                                   DEViseGlobals.API_JAVA,
                                                   _dispatcher.jsc.jsValues.connection.connectionID);
                    _dispatcher.jsc.pn("Sent Heartbeat to collaboration JS.");
                } catch (YException e) {
                    String errMsg = e.getMessage();
                    _dispatcher.jsc.pn("Error in sending heartbeat: " +
                                       errMsg);
                }
            }

            _hbCount++;
        }
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseHeartBeat.stop()");
        }
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Stopping thread " + _hbThread);
        }
        _hbThread.stop();
    }
}
