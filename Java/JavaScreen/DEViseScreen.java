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
// Revision 1.6  1998/08/14 17:48:06  hongyu
// *** empty log message ***
//
// Revision 1.2  1998/06/11 15:07:47  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class DEViseScreen extends Panel
{
    jsdevisec jsc = null;
    Dimension screenDim = null;
    Dimension screenEdge = new Dimension(2, 2);
    Dimension screenSize = null;
    Color screenColor = DEViseGlobals.uibgcolor;
    Font screenFont = DEViseGlobals.uifont;
    boolean isDimChanged = false;
    boolean isUpdate = false;
    public boolean isGrid = false;
    Vector allWindows = new Vector();
    DEViseWindow currentWindow = null;

    public DEViseScreen(jsdevisec what)
    {
        this(what, null, null, null);
    }

    public DEViseScreen(jsdevisec what, Dimension dim, Color color, Font font)
    {
        jsc = what;

        if (dim != null) {
            screenDim = dim;
        } else {
            screenDim = new Dimension(640, 480);
            int width = (int)(DEViseGlobals.SCREENSIZE.width * 0.75);
            int height = (int)(DEViseGlobals.SCREENSIZE.height * 0.75);
            screenDim.width = width;
            screenDim.height = height;
        }

        if (color != null)
            screenColor = color;
        if (font != null)
            screenFont = font;

        setLayout(null);
        setBackground(screenColor);
        setForeground(screenColor);
        setFont(screenFont);
        screenSize = new Dimension(screenDim.width + 2 * screenEdge.width, screenDim.height + 2 *screenEdge.height);
    }

    public Dimension getPreferredSize()
    {
        return screenSize;
    }

    public Dimension getMinimumSize()
    {
        return screenSize;
    }

    public synchronized Dimension getScreenDim()
    {
        return screenDim;
    }

    // At this time, you can not change the size of the screen after a session
    // is opened. If you really want to change the screen size, you will need
    // to close the session, re-set the size of the screen and reopen the
    // session
    public synchronized void setScreenDim(Dimension dim)
    {
        if (dim == null)
            return;

        screenDim = dim;
        screenSize = new Dimension(screenDim.width + 2 * screenEdge.width, screenDim.height + 2 * screenEdge.height);
        isDimChanged = true;
        repaint();
    }

    public synchronized void addWindow(DEViseWindow win)
    {
        if (win == null)
            return;

        allWindows.addElement(win);
    }

    public synchronized void removeWindow(String name)
    {
        if (name == null)
            return;

        DEViseWindow win = getWindow(name);
        if (win != null)
            allWindows.removeElement(win);
    }

    public synchronized void updateWindow(String name, Image img)
    {
        if (img == null || name == null)
            return;

        DEViseWindow win = getWindow(name);
        if (win == null)
            return;

        win.setImage(img);
    }

    public synchronized DEViseWindow getWindow(String name)
    {
        if (name == null)
            return null;

        int number = allWindows.size();
        DEViseWindow win = null;

        for (int i = 0; i < number; i++) {
            win = (DEViseWindow)allWindows.elementAt(i);
            if (win.getName().equals(name))
                break;
        }

        return win;
    }

    public synchronized DEViseWindow getCurrentWindow()
    {
        return currentWindow;
    }

    public void updateGData(String name, Vector rect)
    {
        DEViseWindow win = getViewWindow(name);
        if (win == null) {
            YGlobals.Ydebugpn("GData window is null!");
            return;
        }

        win.updateGData(rect);
    }

    public void drawCursor(String name, Rectangle rect)
    {
        DEViseWindow win = getViewWindow(name);
        if (win == null)
            return;

        win.drawCursor(rect);
    }

    public void eraseCursor(String name)
    {
        DEViseWindow win = getViewWindow(name);
        if (win == null)
            return;

        win.eraseCursor();
    }

    public synchronized void setCurrentWindow(DEViseWindow win)
    {
        if (win == null)
            return;

        if (currentWindow != null)
            currentWindow.setCurrent(false);

        currentWindow = win;
        remove(currentWindow);
        add(currentWindow, 0);
        currentWindow.setCurrent(true);
    }

    public synchronized Vector getAllWindows()
    {
        return allWindows;
    }

    public synchronized DEViseView getView(String name)
    {
        if (name == null)
            return null;

        int number = allWindows.size();
        DEViseWindow win = null;
        DEViseView view = null;

        for (int i = 0; i < number; i++) {
            win = (DEViseWindow)allWindows.elementAt(i);
            view = win.getView(name);
            if (view != null)
                break;
        }

        return view;
    }

    public synchronized DEViseWindow getViewWindow(String name)
    {
        if (name == null)
            return null;

        int number = allWindows.size();
        DEViseWindow win = null;
        DEViseView view = null;

        for (int i = 0; i < number; i++) {
            win = (DEViseWindow)allWindows.elementAt(i);
            view = win.getView(name);
            if (view != null)
                break;
        }

        if (view != null)
            return win;
        else
            return null;
    }

    public synchronized void updateScreen(boolean flag)
    {
        removeAll();

        if (flag) {
            int number = allWindows.size();
            for (int i = 0; i < number; i++)
                add((DEViseWindow)allWindows.elementAt(i));

            isUpdate = true;
            jsc.setUI(flag);
            repaint();
        } else {
            allWindows.removeAllElements();
            currentWindow = null;
            jsc.setUI(flag);
            repaint();
        }
    }

    public void paint(Graphics g)
    {
        if (isDimChanged) {
            isDimChanged = false;
            setSize(screenSize);
            //jsc.doLayout();
            //jsc.validate();
            jsc.pack();
            jsc.repaint();
        }

        if (isUpdate) {
            int number = allWindows.size();
            DEViseWindow win = null;
            Rectangle bound = null;
            for (int i = 0; i < number; i++) {
                win = (DEViseWindow)allWindows.elementAt(i);
                bound = win.getLoc();
                bound.x += screenEdge.width;
                bound.y += screenEdge.height;
                if (bound != null)
                    win.setBounds(bound);
            }

            isUpdate = false;
        }

        super.paint(g);

        if (currentWindow != null)
            currentWindow.repaint();

        Color oldColor = g.getColor();
        g.setColor(Color.white);
        for (int i = 0; i < screenEdge.width; i++) {
            g.drawLine(i, 0, i, screenSize.height - 1);
            g.drawLine(screenSize.width - i - 1, 0, screenSize.width - i - 1, screenSize.height - 1);
        }
        for (int i = 0; i < screenEdge.height; i++) {
            g.drawLine(0, i, screenSize.width - 1, i);
            g.drawLine(0, screenSize.height - i - 1, screenSize.width - 1, screenSize.height - i - 1);
        }
        g.setColor(oldColor);
    }
}
