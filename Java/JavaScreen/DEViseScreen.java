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
    public Image offScrImg = null;
    Dimension screenDim = new Dimension(600, 400);
    Dimension screenEdge = new Dimension(2, 2);
    Dimension screenSize = null;
    Color screenColor = DEViseGlobals.uibgcolor;
    Font screenFont = DEViseGlobals.uifont;
    boolean isDimChanged = false;
    public boolean isUpdate = false;
    public boolean isGrid = false;
    Vector allWindows = new Vector();
    Vector allGDataItems = new Vector();
    DEViseWindow currentWindow = null;
    DEViseWindow clickWindow = null;
    Rectangle newWinLoc = new Rectangle(0, 0, 0, 0);
    boolean isDrawPos = false;

    public DEViseScreen(jsdevisec what)
    {
        this(what, null, null, null);
    }

    public DEViseScreen(jsdevisec what, Dimension dim)
    {
        this(what, dim, null, null);
    }

    public DEViseScreen(jsdevisec what, Dimension dim, Color color, Font font)
    {
        jsc = what;

        if (dim != null) {
            screenDim = dim;
        } else {
            screenDim.width = (int)(DEViseGlobals.SCREENSIZE.width - 140);
            screenDim.height = (int)(DEViseGlobals.SCREENSIZE.height - 140);
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

        addMouseMotionListener(new MouseMotionAdapter()
            {
                public void mouseMoved(MouseEvent event)
                {
                    Point p = event.getPoint();
                    if (jsc.dispatcher.getStatus() == 0) {
                        setCursor(DEViseGlobals.waitcursor);
                    } else {
                        setCursor(DEViseGlobals.handcursor);
                    }
                    //jsc.viewInfo.updateInfo(null, null, p.x, p.y);
                    jsc.viewInfo.updateInfo();
                    setCurrentWindow(null);
                    //requestFocus();
                }
            });
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

    public synchronized DEViseWindow getClickWindow()
    {
        return clickWindow;
    }

    public synchronized void setClickWindow(DEViseWindow win)
    {
        clickWindow = win;
    }

    public void updateGData(String name, Vector rect)
    {
        DEViseView view = getView(name);
        DEViseWindow win = getViewWindow(name);
        if (view == null || win == null) {
            if (name != null) {
                YGlobals.Ydebugpn("Can not find the view or window for view name " + name + "!");
            } else {
                YGlobals.Ydebugpn("Null name received for finding view!");
            }
            return;
        }

        view.updateGData(rect);
    }

    public void addGDataItem(Button c)
    {
        if (c == null)
            return;

        allGDataItems.addElement(c);

        if (getComponentCount() != 0) {
            add(c);
        }
    }

    public void removeGDataItem(Button c)
    {
        if (c == null)
            return;

        remove(c);
        allGDataItems.removeElement(c);
    }

    private void addAll()
    {
        if (allWindows != null) {
            int number = allWindows.size();
            for (int i = 0; i < number; i++)
                add((DEViseWindow)allWindows.elementAt(i));
        }

        if (allGDataItems != null) {
            int number = allGDataItems.size();
            for (int i = 0; i < number; i++)
                add((Button)allGDataItems.elementAt(i));
        }
    }

    public void updateCursor(String name, DEViseCursor rect)
    {
        DEViseView view = getView(name);
        DEViseWindow win = getViewWindow(name);

        if (win == null || view == null) {
            if (name != null) {
                YGlobals.Ydebugpn("Can not find the window or view for view name " + name + "!");
            } else {
                YGlobals.Ydebugpn("Null name received for finding view!");
            }
            return;
        }

        view.setCursor(rect);
        win.updateCursor();
    }

    public synchronized void setCurrentWindow(DEViseWindow win)
    {
        if (currentWindow != null)
            currentWindow.setCurrent(false);

        currentWindow = win;

        if (win == null) {
            return;
        } else {
            //remove(currentWindow);
            add(currentWindow, 0);
            currentWindow.setCurrent(true);
        }
    }

    public void setFocusToCurrentWindow()
    {
        if (!DEViseGlobals.isShowingProgramInfo) {
            requestFocus();
            if (currentWindow != null)
                currentWindow.requestFocus();
        }
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
            if (view != null) {
                break;
            }
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
            if (view != null) {
                break;
            }
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
            addAll();

            isUpdate = true;
            jsc.isSessionOpened = true;
            jsc.viewInfo.updateInfo();
            jsc.viewControl.updateControl(false);
            offScrImg = null;
            repaint();
        } else {
            allWindows.removeAllElements();
            allGDataItems.removeAllElements();

            currentWindow = null;
            jsc.isSessionOpened = false;
            jsc.viewInfo.updateInfo();
            jsc.viewControl.updateControl(true);
            offScrImg = null;
            repaint();
        }
    }

    public void updateWindowPos(DEViseWindow win, int x, int y, boolean isFinal)
    {
        if (win == null)
            return;

        if (!isFinal) {
            Rectangle loc = win.getLoc();
            newWinLoc.x = loc.x + x;
            newWinLoc.y = loc.y + y;
            newWinLoc.width = loc.width;
            newWinLoc.height = loc.height;
            isDrawPos = true;
            offScrImg = null;
            repaint();
        } else {
            win.windowLoc = new Rectangle(newWinLoc.x, newWinLoc.y, newWinLoc.width, newWinLoc.height);
            win.setBounds(newWinLoc);
            offScrImg = null;
            Vector views = win.getAllViews();
            if (views != null) {
                for (int i = 0; i < views.size(); i++) {
                    DEViseView view = (DEViseView)views.elementAt(i);
                    view.updateGData();
                }
            }

            repaint();
        }
    }

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (offScrImg == null)
            offScrImg = createImage(screenSize.width, screenSize.height);

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (isDimChanged) {
            isDimChanged = false;
            setSize(screenSize);
            //jsc.doLayout();
            if (jsc.inBrowser) {
                jsc.validate();
            } else {
                jsc.parentFrame.pack();
            }
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
                //YGlobals.Ydebugpn(win.getName() + " x " + bound.x + " y " + bound.y);
                if (bound != null)
                    win.setBounds(bound);
            }

            isUpdate = false;
        }

        super.paint(g);

        if (currentWindow != null)
            currentWindow.repaint();

        if (isDrawPos) {
            Color oldColor = g.getColor();
            g.setColor(Color.white);
            g.drawRect(newWinLoc.x, newWinLoc.y, newWinLoc.width, newWinLoc.height);
            g.setColor(oldColor);
            isDrawPos = false;
        }
        /*
        if (buttonLabel != null && buttonLabelLoc != null) {
            buttonLabel.setBounds(buttonLabelLoc);
        }
        */
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
