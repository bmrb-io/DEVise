// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen timer.  This class starts a thread that destroy
// an invisible applet after 60mins.

// ------------------------------------------------------------------------

// ========================================================================

package JavaScreen;

public class DEViseJSTimer implements Runnable
{
    private static final int DEBUG = 0;
    private int _timeout;
    private DEViseJSApplet applet = null;
    private Thread thread = null;

    // Timeout is in minutes.
    public DEViseJSTimer(DEViseJSApplet a, int visTimeout) {
        applet = a;

        // Convert timeout to milliseconds.
        _timeout = visTimeout * 60 * 1000;
    }

    public void start()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSTimer.start() (instance " +
                               applet.getInstanceNum() + ")");
        }

        if (thread != null) {
            if (DEViseGlobals.DEBUG_THREADS >= 1) {
                DEViseUtils.printAllThreads("Stopping thread " + thread);
            }
            thread.stop();
        }

        thread = new Thread(this);
        thread.setName("Visibility timer");
        thread.start();
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Starting thread " + thread);
        }
    }

    public void run()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSTimer.run() (instance " +
                               applet.getInstanceNum() + ")");
        }

        try {
            if (_timeout > 0) {
                Thread.sleep(_timeout);
            }

            System.out.println("Destroying applet " +
                               applet.getInstanceNum() + " because of visibility timeout");
            applet.destroyFromTimer();
        } catch (InterruptedException e) {
            if (DEBUG >= 1) {
                System.err.println("Visibility timer sleep interrupted, " +
                                   "instance " + applet.getInstanceNum());
            }
        } catch (Exception ex) {
            System.err.println("Exception destroying applet instance: " +
                               ex);
        }

        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Thread " + thread + " ending");
        }
    }

    public void stop()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseJSTimer.stop() (instance " +
                               applet.getInstanceNum() + ")");
        }
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Stopping thread " + thread);
        }
        thread.interrupt();
        try {
            thread.stop();
        } catch (Exception e) {
            if (DEBUG >= 1) {
                e.printStackTrace();
            }
        }
    }
}
