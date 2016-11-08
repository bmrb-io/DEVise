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

// $Id$

// $Log$
// Revision 1.5.50.2  2015/07/09 19:00:13  wenger
// Final cleanup of Aditya's changes.
//
// Revision 1.5.50.1  2015/07/08 20:57:11  wenger
// Merged aditya1_br_7 thru aditya1_br_8 to aditya_merge_br.
//
// Revision 1.5.48.1  2015/02/04 20:05:32  kancherla
// Handling modifyThread exception
//
// Revision 1.5  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.4  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.3  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.2.2.7  2003/05/02 17:16:16  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.2.2.6  2003/03/28 17:21:29  wenger
// Made JS invisibility destruction timeout configurable; fixed some other
// problems with loading and re-loading the applet.
//
// Revision 1.2.2.5  2002/12/17 23:15:01  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.2.2.4  2002/11/25 21:29:35  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
// Revision 1.2.2.3  2002/08/19 16:51:37  wenger
// Applet instances are now re-used according to URL if the "reloadapplet"
// flag is false.
//
// Revision 1.2.2.2  2002/08/16 21:56:56  wenger
// Fixed bug 807 (reload twice in Netscape goofs up JS applets); fixed
// various other problems with destroying hidden applets.
//
// Revision 1.2.2.1  2002/07/19 16:05:21  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.2  2002/03/12 19:59:21  wenger
// Added RCS Id and Log.
//

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
        try{
        thread.stop();
        } catch (Exception e){
            if (DEBUG >= 1) {
                e.printStackTrace();
            }
        }
    }
}
