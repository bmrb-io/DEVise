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

// ========================================================================

// YLogGUI.java

import java.awt.*;


public class YLogGUI extends Frame
{
    protected TextArea textarea = null;
    protected int loglevel;

    public YLogGUI(int level, int width, int height, String title, Font font, Color bg, Color fg)
    {
        loglevel = level;

        if (width <= 0)
            width = 50;

        if (height <= 0)
            height = 10;

        textarea = new TextArea(height, width);

        if (font == null)
            font = new Font("Monospaced", Font.PLAIN, 14);
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
    }

    public YLogGUI(int level, int width, int height, String title, Font font)
    {
        this(level, width, height, title, font, null, null);
    }

    public YLogGUI(int width, int height, String title, Font font)
    {
        this(1, width, height, title, font, null, null);
    }

    public YLogGUI(int level, int width, int height, String title)
    {
        this(level, width, height, title, null, null, null);
    }

    public YLogGUI(int width, int height, String title)
    {
        this(1, width, height, title, null, null, null);
    }

    public YLogGUI(int level, int width, int height)
    {
        this(level, width, height, null, null, null, null);
    }

    public YLogGUI(int width, int height)
    {
        this(1, width, height, null, null, null, null);
    }

    public YLogGUI(int level)
    {
        this(level, 50, 10, null, null, null, null);
    }

    public YLogGUI()
    {
        this(1, 50, 10, null, null, null, null);
    }

    // Since method 'append' and 'setText' of 'TextArea' are synchronized
    // so the following method are not set as synchronized
    public void p(String msg, int level)
    {
        if (level > loglevel)
            return;
        else
            textarea.append(msg);
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void pn(String msg, int level)
    {
        if (level > loglevel)
            return;
        else
            textarea.append(msg + "\n");
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void clear()
    {
        textarea.setText("");
    }

    public void close()
    {
        dispose();
    }
}

