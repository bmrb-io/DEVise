// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used for displaying the "traffic lights".

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.16  2000/05/22 17:52:50  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.15  2000/03/23 16:26:17  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.14  1999/12/10 15:37:01  wenger
// Added standard headers to source files.
//
// ========================================================================


import java.awt.*;
import java.awt.event.*;

public class YImageCanvas extends Canvas
{
    Image image = null, offScrImg = null;
    String string = null;
    Font font = DEViseFonts.getFont(14, DEViseFonts.MONOSPACED, 1, 0);
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

        image = img;
        imageWidth = image.getWidth(this);
        imageHeight = image.getHeight(this);
        if (imageWidth <= 0 || imageHeight <= 0)
            throw new YException("Invalid Image!");

        size = new Dimension(imageWidth, imageHeight);
        isImage = true;
    }

    public YImageCanvas(String s) throws YException
    {
        if (s == null) {
            throw new YException("Null String!");
        }

        string = s;
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

        string = s;
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

        string = s;

        offScrImg = null;

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

        image = img;
        offScrImg = null;

        repaint();

        return true;
    }

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (offScrImg == null) {
            offScrImg = createImage(imageWidth, imageHeight);
        }

        Graphics og = offScrImg.getGraphics();
        paint(og);

	if (offScrImg != null) {
            g.drawImage(offScrImg, 0, 0, this);
	} else {
	    paint(g);
        }

        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (isImage) {
            if (image != null) {
                g.drawImage(image, 0, 0, this);
            }
        } else {
            if (string != null) {
                g.setColor(bgcolor);
                g.fillRect(0, 0, imageWidth, imageHeight);
                g.setColor(fgcolor);
                g.setFont(font);
                int xstart = (imageWidth - string.length() * mx) / 2 + xshift;
                g.drawString(string, xstart, imageHeight - 3);
            }
        }
    }
}
        
