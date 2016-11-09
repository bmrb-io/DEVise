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

// This class checks whether other threads may be hung, and reports
// a warning if so.

// There is one instance of this class for a JSPoP.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseThreadChecker.java

package JavaScreen;

import  java.util.*;
import  java.text.*;

public class DEViseThreadChecker implements Runnable
{
    private static int _debugLvl = 0;
    private static final int CHECK_INTERVAL = 10 * 1000; // millisec
    private static final int HUNG_INTERVAL = 600 * 1000; // millisec

    private Thread _thread = null;

    private Vector _threadList = null;

    private static DEViseThreadChecker _instance = null;

    public static DEViseThreadChecker getInstance()
    {
        return _instance;
    }

    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }

    public DEViseThreadChecker()
    {
        _instance = this;

        _threadList = new Vector();

        _thread = new Thread(this);
        _thread.setName("Thread checker");
        _thread.start();
    }

    public void register(DEViseCheckableThread thread)
    {
        if (_debugLvl >= 2) {
            System.out.println("DEViseThreadChecker.register(" +
                               thread.thread2Str() + ")");
        }
        _threadList.addElement(thread);
    }

    public void unregister(DEViseCheckableThread thread)
    {
        if (_debugLvl >= 2) {
            System.out.println("DEViseThreadChecker.unregister(" +
                               thread.thread2Str() + ")");
        }
        _threadList.removeElement(thread);
    }

    public void run()
    {
        while (true) {
            try {
                Thread.sleep(CHECK_INTERVAL);
            } catch (InterruptedException ex) {
            }

            if (_debugLvl >= 4) {
                System.out.println("Checking threads");
            }

            Date date = new Date();
            long currentTime = date.getTime();

            for (int index = 0; index < _threadList.size(); index++) {
                DEViseCheckableThread thread = (DEViseCheckableThread)
                                               _threadList.elementAt(index);
                long lastRun = thread.lastRunTime();
                if (currentTime - lastRun > HUNG_INTERVAL) {
                    DateFormat dtf = DateFormat.getDateTimeInstance(
                                         DateFormat.MEDIUM, DateFormat.MEDIUM);
                    System.err.println(thread.thread2Str() +
                                       " may be hung at " + dtf.format(date));
                    System.err.println("  " + thread.thread2Str() +
                                       " is alive?: " + thread.isAlive());
                    //TEMP -- force a stack track of the thread if possible
                    thread.intThread();
                }
            }
        }
    }
}
