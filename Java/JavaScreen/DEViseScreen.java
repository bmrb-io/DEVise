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
// Revision 1.37  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.36  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.35  1999/08/04 06:05:54  hongyu
// *** empty log message ***
//
// Revision 1.34  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.33  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.31  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseScreen.java

import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class DEViseScreen extends Panel
{
    jsdevisec jsc = null;

    Dimension screenDim = new Dimension(600, 400);

    Vector allCanvas = new Vector();
    Vector newCanvas = new Vector();
    Vector obsoleteCanvas = new Vector();

    Vector allViews = new Vector();
    Hashtable viewTable = new Hashtable();

    Vector javaGDatas = new Vector();
    Vector newGData = new Vector();
    Vector obsoleteGData = new Vector();

    DEViseView currentView = null;
    public DEViseView lastActionView = null;
    public boolean guiAction = false;

    boolean isDimChanged = false;

    public Image offScrImg = null;

    public Point finalMousePosition = new Point(-1, -1);

    public DEViseScreen(jsdevisec what)
    {
        jsc = what;

        screenDim = new Dimension(DEViseGlobals.screenSize.width, DEViseGlobals.screenSize.height);

        setLayout(null);
        setBackground(DEViseGlobals.bg);
        setForeground(DEViseGlobals.fg);
        setFont(DEViseGlobals.font);

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        this.enableEvents(AWTEvent.MOUSE_MOTION_EVENT_MASK);

        addMouseMotionListener(new MouseMotionAdapter()
            {
                public void mouseMoved(MouseEvent event)
                {
                    Point p = event.getPoint();

                    if (jsc.dispatcher.getStatus() != 0) {
                        setCursor(DEViseGlobals.waitCursor);
                    } else {
                        setCursor(DEViseGlobals.defaultCursor);
                    }

                    if (currentView != null) {
                        jsc.viewInfo.updateInfo();

                        setCurrentView(null);
                    }
                }
            });
    }

    protected void processMouseEvent(MouseEvent event)
    {
        if (event.getID() == MouseEvent.MOUSE_EXITED) {
            finalMousePosition.x = -1;
            finalMousePosition.y = -1;
        } else {
            finalMousePosition.x = event.getX();
            finalMousePosition.y = event.getY();
        }

        super.processMouseEvent(event);
    }

    protected void processMouseMotionEvent(MouseEvent event)
    {
        finalMousePosition.x = event.getX();
        finalMousePosition.y = event.getY();

        super.processMouseMotionEvent(event);
    }

    public void reEvaluateMousePosition()
    {
        if (finalMousePosition.x < 0) {
            if (currentView != null) {
                jsc.viewInfo.updateInfo();
                setCurrentView(null);
            }

            return;
        }

        //if (!DEViseGlobals.isApplet) {
        //    MouseEvent finishEvent = new MouseEvent(this, MouseEvent.MOUSE_MOVED, DEViseGlobals.getCurrentTime(), 0, finalMousePosition.x, finalMousePosition.y, 0, false);
        //    Toolkit tk = getToolkit();
        //    //EventQueue queue = new EventQueue();
        //    EventQueue queue = tk.getSystemEventQueue();
        //    queue.postEvent(finishEvent);
        //} else {
            for (int i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(i);
                Rectangle loc = canvas.getLocInScreen();
                Point p = new Point(finalMousePosition.x - loc.x, finalMousePosition.y - loc.y);
                if (p.x >= 0 && p.x < loc.width && p.y >= 0 && p.y < loc.height) {
                    canvas.checkMousePos(p, false, false);
                    return;
                }
            }

            if (currentView != null) {
                jsc.viewInfo.updateInfo();
                setCurrentView(null);
            }

            setCursor(DEViseGlobals.defaultCursor);
        //}
    }

    public Dimension getPreferredSize()
    {
        return screenDim;
    }

    public Dimension getMinimumSize()
    {
        return screenDim;
    }

    // At this time, you can not change the size of the screen after a session
    // is opened. If you really want to change the screen size, you will need
    // to close the session, re-set the size of the screen and reopen the
    // session
    public void setScreenDim(int width, int height)
    {
        if (width < 1 || height < 1)
            return;

        screenDim.width = width;
        screenDim.height = height;

        DEViseGlobals.screenSize.width = screenDim.width;
        DEViseGlobals.screenSize.height = screenDim.height;

        isDimChanged = true;

        repaint();
    }

    public void addView(DEViseView view)
    {
        if (view == null)
            return;

        // view already exists
        if (viewTable.get(view.viewName) != null)
            return;

        // parent view always created before view symbol
        if (view.parentView != null) {
            view.parentView.addChild(view);
        }

        // top view always created before bottom view
        if (view.piledView != null) {
            view.piledView.addPile(view);
        }

        allViews.addElement(view);

        viewTable.put(view.viewName, view);
    }

    // Only top view (not view symbol, not bottom view in piled views) will have an image
    // and only top view will associated with a canvas, a canvas will only be assiciated
    // one view and one view only
    public void updateViewImage(String name, Image image)
    {
        if (name == null || image == null)
            return;

        DEViseView view = getView(name);

        if (view == null)
            return;

        if (view.canvas == null) {
            DEViseCanvas canvas = new DEViseCanvas(view, image);
            view.canvas = canvas;

            int i;
            for (i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
                if (c.view.viewZ < canvas.view.viewZ) {
                    canvas.posZ = i;
                    break;
                }
            }
            if (i > 0 && canvas.posZ == 0) {
                canvas.posZ = allCanvas.size();
            }

            allCanvas.insertElementAt(canvas, canvas.posZ);

            newCanvas.addElement(canvas);
        } else {
            view.canvas.updateImage(image);
        }

        repaint();
    }

    // while remove a view, everything assiciate with this view is gone, such as view symbol,
    // cursors, gdatas, even canvas and piled views (if the view to be remove is a top view)
    public void removeView(String name)
    {
        if (name == null)
            return;

        removeView(getView(name));
    }

    public synchronized void removeView(DEViseView view)
    {
        if (view == null) {
            return;
        }

        Vector vector = null;

        // remove itself from its parent or piled view, if any
        if (view.piledView != null) {
            view.piledView.removePile(view);
        }

        if (view.parentView != null) {
            view.parentView.removeChild(view);
        }

        // remove GDatas associated with it
        vector = view.viewGDatas;
        if (vector.size() > 0) {
            for (int i = 0; i < vector.size(); i++) {
                DEViseGData data = (DEViseGData)vector.elementAt(i);
                if (data.isJavaSymbol) {
                    javaGDatas.removeElement(data);
                    obsoleteGData.addElement(data);
                }
            }

            view.removeAllGData();
        }

        // remove cursors associated with this view
        view.removeAllCursor();

        // remove any piled view might associated with this view
        vector = view.viewPiledViews;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0)); // recursive call to removeView
        }

        // remove any child view might associated with this view
        vector = view.viewChilds;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0)); // recursive call to removeView
        }

        // if this view is current view, set current view to be null
        if (currentView == view) {
            currentView = null;
        }

        // remove canvas associated with this view, if any
        if (view.canvas != null) {
            allCanvas.removeElement(view.canvas);
            obsoleteCanvas.addElement(view.canvas);
        }

        // remove this view from view table
        allViews.removeElement(view);
        viewTable.remove(view.viewName);

        repaint();
    }

    // remove the child views only
    public void removeChildViews(String name)
    {
        if (name == null)
            return;

        removeChildViews(getView(name));
    }

    public void removeChildViews(DEViseView view)
    {
        if (view == null)
            return;

        Vector vector = view.viewChilds;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0));
        }
        // Can not use the for iteration, because removeView also remove that view
        // from its parents, the the size of the viewChilds of the parentView is
        // actually changing on the fly
        //for (int i = 0; i < child.size(); i++) {
        //    removeView(view.getChild(i));
        //}

        repaint();
    }

    public DEViseView getView(String name)
    {
        if (name == null)
            return null;

        return (DEViseView)viewTable.get(name);
    }

    public DEViseView getCurrentView()
    {
        return currentView;
    }

    public void setCurrentView(DEViseView view)
    {
        if (currentView != null) {
            if (currentView.getCanvas() != null) {
                currentView.getCanvas().repaint();
                if (currentView != view && currentView.getCanvas().activeView == currentView) {
                    currentView.getCanvas().activeView = null;
                }
            }
        }

        currentView = view;

        if (!jsc.isShowingMsg() && currentView != null) {
            requestFocus();
            if (currentView.getCanvas() != null)
                currentView.getCanvas().requestFocus();
        }
    }

    public synchronized void updateGData(String name, Vector vect)
    {
        DEViseView view = getView(name);

        if (view == null) {
            return;
        }

        Vector vector = view.viewGDatas;
        if (vector.size() > 0) {
            for (int i = 0; i < vector.size(); i++) {
                DEViseGData data = (DEViseGData)vector.elementAt(i);
                if (data.isJavaSymbol) {
                    javaGDatas.removeElement(data);
                    obsoleteGData.addElement(data);
                }
            }

            view.removeAllGData();
        }

        if (vect != null && vect.size() > 0) {
            for (int i = 0; i < vect.size(); i++) {
                DEViseGData g = (DEViseGData)vect.elementAt(i);
                if (g.isJavaSymbol) {
                    javaGDatas.addElement(g);
                    newGData.addElement(g);
                }

                view.addGData(g);
            }
        }

        if (view.viewDimension == 3 && view.canvas != null) {
            view.canvas.createCrystal();
        }

        repaint();
    }

    public void updateCursor(String name, DEViseCursor cursor)
    {
        DEViseView view = getView(name);

        if (view == null) {
            return;
        }

        if (cursor != null) {
            if (view.addCursor(cursor)) {
                repaint();
            }
        }
    }

    public void removeCursor(String name, String viewname)
    {
        DEViseView view = getView(viewname);

        if (view != null) {
            if (view.removeCursor(name)) {
                repaint();
            }
        }
    }

    public void updateViewDataRange(String name, String axis, double min, double max)
    {
        DEViseView view = getView(name);

        if (view != null && axis != null) {
            view.updateDataRange(axis, min, max);
        }
    }

    public void setLastAction()
    {
        if (guiAction) {
            guiAction = false;
            if (currentView != null) {
                if (lastActionView != currentView) {
                    if (lastActionView != null) {
                        lastActionView.isFirstTime = true;
                    }

                    lastActionView = currentView;
                    if (currentView.isFirstTime) {
                        currentView.isFirstTime = false;
                    }
                }
            }
        }
    }

    public synchronized void updateScreen(boolean flag)
    {
        if (flag) {
            jsc.isSessionOpened = true;
        } else {
            removeAll();

            allCanvas.removeAllElements();
            allViews.removeAllElements();
            javaGDatas.removeAllElements();

            newCanvas.removeAllElements();
            obsoleteCanvas.removeAllElements();

            viewTable = new Hashtable();

            currentView = null;
            lastActionView = null;

            jsc.isSessionOpened = false;

            jsc.viewInfo.updateInfo();
            jsc.viewInfo.updateImage(0, 0);
            jsc.viewInfo.updateCount(0);

            jsc.lastCursor = DEViseGlobals.defaultCursor;
            setCursor(jsc.lastCursor);

            offScrImg = null;

            repaint();
        }
    }

    // Enable double-buffering
    public void update(Graphics gc)
    {
        if (gc == null) {
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(screenDim.width, screenDim.height);
        }

        if (offScrImg == null) {
            paint(gc);
        } else {
            Graphics og = offScrImg.getGraphics();
            paint(og);
            gc.drawImage(offScrImg, 0, 0, this);
            og.dispose();
        }
    }

    public void paint(Graphics g)
    {
        if (isDimChanged) {
            isDimChanged = false;
            setSize(screenDim);

            if (DEViseGlobals.inBrowser) {
                jsc.validate();
            } else {
                jsc.parentFrame.pack();
            }

            jsc.repaint();
        }

        while (obsoleteCanvas.size() > 0) {
            DEViseCanvas c = (DEViseCanvas)newCanvas.firstElement();
            remove(c);
            obsoleteCanvas.removeElement(c);
        }

        while (newCanvas.size() > 0) {
            DEViseCanvas c = (DEViseCanvas)newCanvas.firstElement();
            add(c, c.posZ);
            c.setBounds(c.getLocInScreen());
            newCanvas.removeElement(c);
        }

        while (obsoleteGData.size() > 0) {
            DEViseGData gdata = (DEViseGData)obsoleteGData.firstElement();
            remove(gdata.symbol);
            obsoleteGData.removeElement(gdata);
        }

        while (newGData.size() > 0) {
            DEViseGData gdata = (DEViseGData)newGData.firstElement();
            Component gs = gdata.symbol;
            add(gs);
            gs.setBounds(gdata.GDataLocInScreen);
            newGData.removeElement(gdata);
        }

        super.paint(g);
    }
}
