// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2011
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

// ========================================================================

// YLogGUI.java

package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class YLogGUI extends JFrame
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
            if (SwingUtilities.isEventDispatchThread()) {
                dop(msg);
            } else {
                Runnable rundop = new RunDop(msg);
                SwingUtilities.invokeLater(rundop);
            }
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void pn(String msg, int level)
    {
        p(msg + "\n");
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    //TEMP -- what does this do?
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

    private class RunDop implements Runnable {
        private String _msg;

        RunDop(String msg) {
            _msg = msg;
        }

        public void run() {
            dop(_msg);
        }
    }

    private void dop(String msg)
    {
        addNumber(msg.length());
        textarea.append(msg);
    }
}

