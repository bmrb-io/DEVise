// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$
// Revision 1.10  1999/08/03 07:37:46  hongyu
// add support for read animation symbol from JAR file           by Hongyu Yao
//
// Revision 1.9  1999/08/03 05:56:50  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.8  1999/06/23 20:59:19  wenger
// Added standard DEVise header.
//

// ========================================================================

// YImageCanvas.java

import java.awt.*;
import java.awt.event.*;
import java.util.*;


public class YImageCanvas extends Canvas
{
    //Image image = null;
    Vector image = new Vector();
    Image offScrImg = null, lastImage = null;
    //String string = null;
    Vector string = new Vector();
    String lastString = null;
    Font font = new Font("Monospaced", Font.BOLD, 14);
    Color fgcolor = new Color(255, 0, 0);
    Color bgcolor = new Color(255, 255, 255);
    int mx = 10, my = 10, maxLength = 2, xshift = 0;
    int imageWidth, imageHeight;
    Dimension size = null;
    boolean isImage = true;

    public YImageCanvas(Image img) throws YException
    {
        if (img == null)
            throw new YException("Null Image!");

        imageWidth = img.getWidth(this);
        imageHeight = img.getHeight(this);
        if (imageWidth <= 0 || imageHeight <= 0)
            throw new YException("Invalid Image!");

        //image = img;
        image.addElement(img);

        size = new Dimension(imageWidth, imageHeight);
        isImage = true;
    }

    public YImageCanvas(String s) throws YException
    {
        if (s == null) {
            throw new YException("Null String!");
        }

        //string = s;
        string.addElement(s);

        imageWidth = maxLength * mx + 6;
        imageHeight = my + 6;
        size = new Dimension(imageWidth, imageHeight);
        isImage = false;
    }

    public YImageCanvas(String s, Font f, Color bg, Color fg, int x, int y, int max, int shift) throws YException
    {
        if (s == null) {
            throw new YException("Null String!");
        }

        if (f != null)
            font = f;
        if (bg != null)
            bgcolor = bg;
        if (fg != null)
            fgcolor = fg;
        if (x > 0)
            mx = x;
        if (y > 0)
            my = y;
        if (max > 0)
            maxLength = max;

        xshift = shift;

        //string = s;
        string.addElement(s);

        imageWidth = maxLength * mx + 6;
        imageHeight = my + 6;
        size = new Dimension(imageWidth, imageHeight);
        isImage = false;
    }

    public Dimension getPreferredSize()
    {
        return size;
    }

    public Dimension getMinimumSize()
    {
        return size;
    }

    public synchronized boolean setString(String s)
    {
        if (isImage || s == null)
            return false;

        if (s.length() > maxLength) {
            return false;
        }

        //string = s;
        string.addElement(s);

        //offScrImg = null;

        repaint();

        return true;
    }

    public synchronized boolean setImage(Image img)
    {
        if (!isImage)
            return false;

        if (img == null)
            return false;

        if (img.getWidth(this) != imageWidth || img.getHeight(this) != imageHeight)
            return false;

        //image = img;
        image.addElement(img);

        //offScrImg = null;

        repaint();

        return true;
    }

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (g == null)
            return;

        if (offScrImg == null) {
            offScrImg = createImage(imageWidth, imageHeight);
        }

        if (offScrImg == null) {
            paint(g);
            return;
        } else {
            /*
            int waittime = 0;
            while (offScrImg.getWidth(this) < 0 || offScrImg.getHeight(this) < 0) {
                try {
                    Thread.sleep(50);
                } catch (InterruptedException e) {
                    waittime += 50;
                    if (waittime > 1000) {
                        return;
                    }
                }
            }
            */
            Graphics og = offScrImg.getGraphics();
            
            paint(og);
            g.drawImage(offScrImg, 0, 0, this);
            
            while (string.size() > 0 || image.size() > 0) {
                paint(og);
                g.drawImage(offScrImg, 0, 0, this);
            } 
                
            og.dispose();
        }
    }

    public void paint(Graphics g)
    {
        if (isImage) {
            //if (image != null) {
            //    g.drawImage(image, 0, 0, this);
            //}
            
            if (image.size() > 0) {
                lastImage = (Image)image.firstElement();
                image.removeElement(lastImage);
            }

            if (lastImage != null) {
                g.drawImage(lastImage, 0, 0, this);
            }
        } else {
            //if (string != null) {
            //    g.setColor(bgcolor);
            //    g.fillRect(0, 0, imageWidth, imageHeight);
            //    g.setColor(fgcolor);
            //    g.setFont(font);
            //    int xstart = (imageWidth - string.length() * mx) / 2 + xshift;
            //    g.drawString(string, xstart, imageHeight - 3);
            //}

            if (string.size() > 0) {
                lastString = (String)string.firstElement();
                string.removeElement(lastString);
            }

            if (lastString != null) {
                g.setColor(bgcolor);
                g.fillRect(0, 0, imageWidth, imageHeight);
                g.setColor(fgcolor);
                g.setFont(font);
                int xstart = (imageWidth - lastString.length() * mx) / 2 + xshift;
                g.drawString(lastString, xstart, imageHeight - 3);
            }
        }
    }
}

