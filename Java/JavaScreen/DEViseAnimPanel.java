/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
 */
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
    private int imageCount = 0, frameNum = 1, imageWidth = 0, imageHeight = 0;
    private Thread animator = null;
    private int waitInterval = 0;
    private boolean isAnimated = false;
    
    public DEViseAnimPanel(jsdevisec what, Vector array, int time) throws DEViseException
    {
        if (what == null || array == null)
            throw new DEViseException("NULL argument in DEViseAnimPanel constructor!");
       
        jsc = what;
        images = array;
        waitInterval = time;
        
        imageCount = images.size();        
        if (imageCount == 0)  
            throw new DEViseException("Empty image list received in DEViseAnimPanel constructor!");
        
        // assume all image in the list has same size
        currentImg = (Image)images.elementAt(0);
        if (currentImg == null)  
            throw new DEViseException("NULL image received in DEViseAnimPanel constructor!");            
        imageWidth = currentImg.getWidth(this);
        imageHeight = currentImg.getHeight(this);
        if (imageWidth <= 0 || imageHeight <= 0) 
            throw new DEViseException("Empty image received in DEViseAnimPanel constructor!");            
                
        for (int i = 1; i < imageCount; i++)  {
            Image image = (Image)images.elementAt(i);
            if (image == null)
                throw new DEViseException("Empty image received in DEViseAnimPanel constructor!");                
            int width = image.getWidth(this);
            int height = image.getHeight(this);
            if (width != imageWidth || height != imageHeight)
                throw new DEViseException("Different image size encounter in DEViseAnimPanel constructor!");
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
        if (currentImg != null)  {
            if (isAnimated)  {
                g.drawImage(currentImg, 0, 0, this);
                if (frameNum < (imageCount - 1))
                    frameNum++; 
                else
                    frameNum = 1;
            
                currentImg = (Image)images.elementAt(frameNum);    
            }  else  {
                g.drawImage(currentImg, 0, 0, this);
            }
        }  else  {
            g.setColor(DEViseGlobals.uibgcolor);
            g.fillRect(0, 0, imageWidth, imageHeight);
        }
    }
    
    public void run()  
    {   
        Thread me = Thread.currentThread();
        me.setPriority(Thread.MIN_PRIORITY);
        
        while (animator == me && isAnimated)  {
            repaint();
            
            try  {
                Thread.sleep(waitInterval);
            }  catch (InterruptedException e)  {
            }
        }
    }
    
    public void start()
    {        
        isAnimated = true;
        currentImg = (Image)images.elementAt(1);
        frameNum = 1;
        
        if (animator == null)  {
            animator = new Thread(this);
            animator.start();
        }
    }
    
    public void stop()
    {
        if (animator != null && animator.isAlive())  {
            animator.stop();
        }
        
        isAnimated = false;
        animator = null;
        currentImg = (Image)images.elementAt(0);
        repaint();
    }                                                     
}