// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class forks a thread to use to animate the gears in the corner
// of the JavaScreen.  It does the animation by putting a new GIF onto
// the screen several times a second.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.46  2006/06/23 19:52:39  wenger
// Merged devise_jmol_br_1 thru devise_jmol_br_2 to the trunk.
//
// Revision 1.45.4.2  2006/06/07 15:15:37  wenger
// Removed ThreadDeath fixes from 26 May 2006 because for some reason
// they prevent the JavaScreen from being able to be run more than
// once in the same browser session (at least in Mozilla and Firefox).
//
// Revision 1.45.4.1  2006/05/26 19:18:09  wenger
// Catching a few ThreadDeath exceptions in places where we need
// to; cleaned up js script.
//
// Revision 1.45  2005/12/06 20:00:15  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.44  2003/01/13 19:23:41  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.43  2002/07/19 17:06:46  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.42.8.4  2003/05/02 17:16:12  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.42.8.3  2002/12/17 23:15:00  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.42.8.2  2002/11/25 21:29:28  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
// Revision 1.42.8.1  2002/07/19 16:05:17  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.42  2001/05/11 20:36:03  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.41  2001/01/08 20:31:50  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.40.4.1  2000/11/21 01:51:29  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.40  2000/04/24 20:21:58  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.39  2000/04/07 22:43:12  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.38  2000/03/23 16:26:11  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.37  1999/06/23 20:59:14  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseAnimPanel.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;

// This class represent an animated panel in DEVise
public class DEViseAnimPanel extends Canvas implements Runnable
{
    private jsdevisec jsc = null;

    private Image currentImg = null, offScrImg = null;
    private Vector images = null;
    private Dimension minSize = null;
    private int imageCount = 0, frameNum = 0, activeImageNumber = 1;
    private int imageWidth = 24, imageHeight = 25;
    private Thread animator = null;
    private boolean isAnimated = false;
    private int waitInterval;

    private Date startTime;
    private static final long MIN_WAIT_TIME = 1500; // milliseconds

    public DEViseAnimPanel(jsdevisec what, Vector array, int time)
    {
        jsc = what;

        if (time < 10)
            waitInterval = 10;
        else
            waitInterval = time;

        minSize = new Dimension(imageWidth, imageHeight);

        if (array == null || array.size() < 9) {
            return;
        } else {
            images = array;
            imageCount = 9;
        }

        // all animation symbols must has same size
        currentImg = (Image)images.elementAt(0);
        if (currentImg == null) {
            return;
        }

        imageWidth = currentImg.getWidth(this);
        imageHeight = currentImg.getHeight(this);
        if (imageWidth <= 0 || imageHeight <= 0) {
            return;
        }

        for (int i = 1; i < imageCount; i++)  {
            Image image = (Image)images.elementAt(i);
            if (image == null) {
                return;
            }

            int width = image.getWidth(this);
            int height = image.getHeight(this);
            if (width != imageWidth || height != imageHeight) {
                return;
            }
        }

        minSize = new Dimension(imageWidth, imageHeight);
        isAnimated = true;
    }

    public Dimension getPreferredSize()
    {
        return minSize;
    }

    public Dimension getMinimumSize()
    {
        return minSize;
    }

    // implement double-buffering, set offScrImg = null if you want to
    // restart double-buffering
    public void update(Graphics g)
    {
        if (offScrImg == null)
            offScrImg = createImage(imageWidth, imageHeight);

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (currentImg != null) {
            g.drawImage(currentImg, 0, 0, this);
        } else {
            g.setColor(jsc.jsValues.uiglobals.bg);
            g.fillRect(0, 0, imageWidth, imageHeight);
        }
    }

    public void run()
    {
        Thread me = Thread.currentThread();
        me.setPriority(Thread.MIN_PRIORITY);

        setActiveImageNumber(1);
        frameNum = 1;

        while (animator == me)  {
            currentImg = (Image)images.elementAt(nextFrame());

            repaint();

            try  {
                Thread.sleep(waitInterval);
            }  catch (InterruptedException e)  {
            }
        }

	if (DEViseGlobals.DEBUG_THREADS >= 1) {
	    DEViseUtils.printAllThreads("Thread " + animator + " ending");
	}
    }

    public synchronized void setActiveImageNumber(int number)
    {
        if (number != 1 && number != 5)
            return;

        activeImageNumber = number;
    }

    public synchronized int nextFrame()
    {
        if (activeImageNumber == 1) {
            if (frameNum == 5)
                return (frameNum = 1);
            else
                return frameNum++;
        } else {
            if (frameNum == 9)
                return (frameNum = 5);
            else
                return frameNum++;
        }
    }

    public void start()
    {
	startTime = new Date();

        if (!isAnimated) {
            return;
        }

        if (animator == null) {
            animator = new Thread(this);
            animator.setName("Animator panel");
            animator.start();
            
	    if (DEViseGlobals.DEBUG_THREADS >= 1) {
		DEViseUtils.printAllThreads("Starting thread " + animator);
	    }
        }
    }

    public void stop()
    {
	if (startTime != null) {
	    Date now = new Date();
	    long interval = now.getTime() - startTime.getTime();

	    if (interval < jsc.jsValues.uiglobals.minWaitTime) {
	    	try {
		    Thread.sleep(jsc.jsValues.uiglobals.minWaitTime - interval);
		} catch (InterruptedException ex) {
		    System.err.println("Sleep interrupted: " + ex.toString());
		}
	    }

	    startTime = null;
	}

        if (!isAnimated) {
            return;
        }

        if (animator == null) {
            return;
        }

        if (animator.isAlive()) {
	    if (DEViseGlobals.DEBUG_THREADS >= 1) {
		DEViseUtils.printAllThreads("Stopping thread " + animator);
	    }
            animator.stop();
        }

        // animator and currentImg will not be accessed by two thread at the same time,
        // since at this point, the thread animator is already stopped
        animator = null;
        currentImg = (Image)images.elementAt(0);

        repaint();
    }
}
