// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1992-1998
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================
//
// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.
//

//
// Description of module.
//

//
// $Id$
//
// $Log$
// Revision 1.14  1998/08/14 17:48:04  hongyu
// *** empty log message ***
//
// Revision 1.10  1998/06/11 15:07:44  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

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
    private int imageCount = 0, frameNum = 1;
    private int imageWidth = 47, imageHeight = 50;
    private Thread animator = null;
    private boolean isAnimated = false;
    private int waitInterval;

    public DEViseAnimPanel(jsdevisec what, Vector array, int time)
    {
        jsc = what;

        if (array == null) {
            isAnimated = false;
            minSize = new Dimension(imageWidth, imageHeight);
            return;
        } else {
            isAnimated = true;
            images = array;
        }

        if (time < 10)
            waitInterval = 10;
        else
            waitInterval = time;

        minSize = new Dimension(imageWidth, imageHeight);

        imageCount = images.size();
        if (imageCount < 4) {
            isAnimated = false;
            YGlobals.Ydebugpn("Empty image list received in DEViseAnimPanel constructor!");
            return;
        }

        imageCount = 4;

        // all image in the list must has same size
        currentImg = (Image)images.elementAt(0);
        if (currentImg == null) {
            isAnimated = false;
            YGlobals.Ydebugpn("NULL image received in DEViseAnimPanel constructor!");
            return;
        }

        imageWidth = currentImg.getWidth(this);
        imageHeight = currentImg.getHeight(this);
        if (imageWidth <= 0 || imageHeight <= 0) {
            isAnimated = false;
            YGlobals.Ydebugpn("Empty image received in DEViseAnimPanel constructor!");
            return;
        }

        for (int i = 1; i < imageCount; i++)  {
            Image image = (Image)images.elementAt(i);
            if (image == null) {
                isAnimated = false;
                YGlobals.Ydebugpn("Empty image received in DEViseAnimPanel constructor!");
                return;
            }

            int width = image.getWidth(this);
            int height = image.getHeight(this);
            if (width != imageWidth || height != imageHeight) {
                isAnimated = false;
                YGlobals.Ydebugpn("Different image size encounter in DEViseAnimPanel constructor!");
                return;
            }
        }

        minSize = new Dimension(imageWidth, imageHeight);
    }

    public Dimension getPreferredSize()
    {
        return minSize;
    }

    public Dimension getMinimumSize()
    {
        return minSize;
    }

    // implement double-buffering, set offScrImg = null if you want to restart double-buffering
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
            g.setColor(DEViseGlobals.uibgcolor);
            g.fillRect(0, 0, imageWidth, imageHeight);
        }
    }

    public void run()
    {
        Thread me = Thread.currentThread();
        me.setPriority(Thread.MIN_PRIORITY);

        frameNum = 1;
        while (animator == me)  {
            currentImg = (Image)images.elementAt(frameNum);

            if (frameNum == imageCount - 1)
                frameNum = 1;
            else
                frameNum++;

            repaint();

            try  {
                Thread.sleep(waitInterval);
            }  catch (InterruptedException e)  {
            }
        }
    }

    public void start()
    {
        if (!isAnimated)
            return;

        if (animator == null) {
            animator = new Thread(this);
            animator.start();

            jsc.stopButton.setBackground(Color.red);
            jsc.stopButton.setEnabled(true);
        }
    }

    public void stop()
    {
        if (!isAnimated)
            return;

        if (animator == null)
            return;

        if (animator.isAlive()) {
            animator.stop();
        }

        jsc.stopButton.setBackground(DEViseGlobals.buttonbgcolor);
        jsc.stopButton.setEnabled(false);

        // animator and currentImg will not be accessed by two thread at the same time,
        // since at this point, the thread animator is already stopped
        animator = null;
        currentImg = (Image)images.elementAt(0);
        repaint();
    }
}
