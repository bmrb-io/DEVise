// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.10.8.1  2002/11/22 21:55:10  wenger
// More fixes for stupid scrollbar warnings.
//
// Revision 1.10  2001/05/11 20:36:11  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.9  2001/01/08 20:31:54  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.8.4.2  2000/11/22 17:44:00  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.8.4.1  2000/11/21 01:51:34  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.8  2000/05/22 17:52:50  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.7  2000/03/23 16:26:17  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.6  1999/10/12 21:58:14  hongyu
// *** empty log message ***
//
// Revision 1.5  1999/10/10 19:59:26  hongyu
// *** empty log message ***
//
// Revision 1.4  1999/10/10 08:49:54  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.3  1999/06/23 20:59:20  wenger
// Added standard DEVise header.
//

// ========================================================================

// YLogGUI.java
// last updated on 09/27/99

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;

public class YLogGUI extends Frame
{
    protected TextArea textarea = null;

    // In JDK, there is a certain number of characters can be display in
    // TextArea (because class String has a limit of how many characters can
    // be stored in it), after that, any text appended will not shown. So
    // this class will clear up the TextArea 'max' number of characters
    // has been added into its textarea
    public static final int max = 35000;
    protected int number = 0;

    // loglevel <= 0 means no log information is written
    // otherwise, log information will be written if loglevel is larger
    // than or equal to the specified level in various 'print' function
    protected int loglevel;

    protected boolean isValid = false;

    public YLogGUI(int width, int height, String title, int level, boolean isClosable, Font font, Color bg, Color fg)
    {
        loglevel = level;

        if (width < 25 || width > 250)
            width = 50;

        if (height < 5 || height > 50)
            height = 10;

        textarea = new TextArea("\n", height, width,
	  TextArea.SCROLLBARS_VERTICAL_ONLY);

        if (font == null)
            font = DEViseFonts.getFont(14, DEViseFonts.MONOSPACED, 0, 0);
        if (title == null)
            title = new String("Program Information");
        if (bg == null)
            bg = Color.white;
        if (fg == null)
            fg = Color.black;

        textarea.setFont(font);
        textarea.setBackground(bg);
        textarea.setForeground(fg);
        textarea.setEditable(false);

        add("Center", textarea);

        setTitle(title);

        pack();

        setVisible(true);

        isValid = true;

        if (isClosable) {
            this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);
        }
    }

    public YLogGUI(int width, int height, String title, int level, boolean isClosable, Font font)
    {
        this(width, height, title, level, isClosable, font, null, null);
    }

    public YLogGUI(int width, int height, String title, int level, boolean isClosable)
    {
        this(width, height, title, level, isClosable, null, null, null);
    }

    public YLogGUI(int width, int height, String title, int level)
    {
        this(width, height, title, level, true, null, null, null);
    }

    public YLogGUI(int width, int height, String title)
    {
        this(width, height, title, 1, true, null, null, null);
    }

    public YLogGUI(int width, int height)
    {
        this(width, height, null, 1, true, null, null, null);
    }
    
    public YLogGUI(int level)
    {
        this(50, 10, null, level, true, null, null, null);
    }
    
    public YLogGUI()
    {
        this(50, 10, null, 1, true, null, null, null);
    }

    // event processing
    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            close();
            return;
        }

        super.processEvent(event);
    }

    // Since method 'append' and 'setText' of 'TextArea' are synchronized
    // so the following method are not set as synchronized
    public void p(String msg, int level)
    {
        if (level > loglevel || !isWindowValid()) {
            return;
        } else {
            addNumber(msg.length());
            textarea.append(msg);
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void pn(String msg, int level)
    {
        if (level > loglevel || !isWindowValid()) {
            return;
        } else {
            addNumber(msg.length() + 1);
            textarea.append(msg + "\n");
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public synchronized void addNumber(int num)
    {
        number += num;
        if (number > max) {
            String t = textarea.getText();
            number = t.length() / 2;
            t = t.substring(number);
            textarea.setText(t);
            number = number + 1 + num;
        }
    }

    public void clear()
    {
        textarea.setText("");
    }

    public synchronized boolean isWindowValid()
    {
        return isValid;
    }

    public synchronized void close()
    {
        isValid = false;
        dispose();
    }
}

