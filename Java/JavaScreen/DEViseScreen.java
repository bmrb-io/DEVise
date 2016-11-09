// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2012
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class provides an interface between the DEViseCmdDispatcher
// class and the DEViseCanvas and DEViseView classes.  It also handles
// events that don't fall into a canvas.

// One of the most important functions of this class is to maintain
// consistency of the DEViseCanvas and DEViseView objects, because
// they have a number of cross-references.

// This class also has a number of features dealing with the fact
// that the DEViseCmdDispatcher and the drawing are done in different
// threads.  It could be made safer by making more members private
// and more methods syncronized (according to Hongyu).

// There is one instance of this class for the entire JavaScreen.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseScreen.java

package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.*;
import javax.swing.*;

public class DEViseScreen extends JPanel
{
    jsdevisec jsc = null;

    Dimension screenDim = new Dimension(600, 400);

    // Hongyu says that having these three vectors for canvases is related
    // to making things thread-safe.  I guess it's so we only actually
    // add and remove objects to/from the screen in the paint() method,
    // which should be called in the GUI thread.
    Vector allCanvas = new Vector();
    Vector newCanvas = new Vector();
    Vector obsoleteCanvas = new Vector();

    // A list of all of the DEViseView objects we have, indexed by the
    // view name.
    Hashtable viewTable = new Hashtable();

    // This is for dealing with GData symbols that are rendered as
    // actual Java GUI objects (e.g., buttons).
    Vector newGData = new Vector();
    Vector obsoleteGData = new Vector();

    // Note: this must *not* get reset when we open or close a session,
    // because we're preserving Jmol state across some session opens/closes.
    Hashtable jmolStates = null;

    private DEViseView currentView = null; // the view the mouse is in
    public DEViseView lastActionView = null;
    public boolean guiAction = false;

    private static final int DEBUG = 0; // 0 - 3

    boolean isDimChanged = false;

    public Image offScrImg = null;

    public Point finalMousePosition = new Point(-1, -1);

    //-------------------------------------------------------------------
    // Constructor.
    public DEViseScreen(jsdevisec what)
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.DEViseScreen()");
        }

        jsc = what;

        screenDim = new Dimension(jsc.jsValues.uiglobals.screenSize.width,
                                  jsc.jsValues.uiglobals.screenSize.height);

        setLayout(null);
        setBackground(jsc.jsValues.uiglobals.screenBg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        this.enableEvents(AWTEvent.MOUSE_MOTION_EVENT_MASK);

        addMouseMotionListener(new MouseMotionAdapter()
        {
            public void mouseMoved(MouseEvent event)
            {
                Point p = event.getPoint();

                if (jsc.dispatcher.getStatus() ==
                        DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                    setTemporaryCursor(jsc.mouseCursor.waitCursor);
                } else {
                    setTemporaryCursor(jsc.mouseCursor.defaultCursor);
                }

                if (currentView != null) {
                    jsc.viewInfo.updateInfo();

                    setCurrentView(null);
                }
            }
        });
        addFocusListener(new DSFocusListener());
        addKeyListener(new DSKeyListener());
    }

    //-------------------------------------------------------------------
    protected void setToPermanentCursor()
    {
        jsc.mouseCursor.setToPermanentCursor(this);
    }

    //-------------------------------------------------------------------
    protected void setTemporaryCursor(Cursor cursor)
    {
        jsc.mouseCursor.setTemporaryCursor(cursor, this);
    }

    //-------------------------------------------------------------------
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

    //-------------------------------------------------------------------
    // Hongyu says that we have both this method and a mouse motion
    // listener because this method gets both move and drag events,
    // which we want in order to update the mouse location display.
    protected void processMouseMotionEvent(MouseEvent event)
    {
        finalMousePosition.x = event.getX();
        finalMousePosition.y = event.getY();

        super.processMouseMotionEvent(event);
    }

    //-------------------------------------------------------------------
    //TEMP -- what does this actually do?  Disabling it doesn't seem
    // to make any obvious difference...  wenger 2007-06-25
    public void reEvaluateMousePosition()
    {
        Runnable doReEvaluateMosePosition = new DoReEvaluateMousePosition();
        SwingUtilities.invokeLater(doReEvaluateMosePosition);
    }

    //-------------------------------------------------------------------
    // Hongyu says that this code has something to do with fixing a
    // case where Java does things wrong.
    private class DoReEvaluateMousePosition implements Runnable {
        public void run() {
            if (finalMousePosition.x < 0) {
                if (currentView != null) {
                    jsc.viewInfo.updateInfo();
                    setCurrentView(null);
                }

                return;
            }

            //if (!DEViseUIGlobals.isApplet) {
            //    MouseEvent finishEvent = new MouseEvent(this, MouseEvent.MOUSE_MOVED, DEViseGlobals.getCurrentTime(), 0, finalMousePosition.x, finalMousePosition.y, 0, false);
            //    Toolkit tk = getToolkit();
            //    //EventQueue queue = new EventQueue();
            //    EventQueue queue = tk.getSystemEventQueue();
            //    queue.postEvent(finishEvent);
            //} else {
            for (int i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(i);
                Rectangle loc = canvas.getLocInScreen();
                Point p = new Point(finalMousePosition.x - loc.x,
                                    finalMousePosition.y - loc.y);
                if (p.x >= 0 && p.x <= loc.width && p.y >= 0 &&
                        p.y <= loc.height) {
                    canvas.checkMousePos(p, false);
                    repaint();
                    return;
                }
            }

            if (currentView != null) {
                jsc.viewInfo.updateInfo();
                setCurrentView(null);
            }

            setToPermanentCursor();
            //}
        }
    }

    //-------------------------------------------------------------------
    public Dimension getPreferredSize()
    {
        return screenDim;
    }

    //-------------------------------------------------------------------
    public Dimension getMinimumSize()
    {
        // Give the layout manager some room to work!!
        int width = Math.max(100, screenDim.width - 200);
        int height = Math.max(100, screenDim.height - 200);
        Dimension minSize = new Dimension (width, height);
        return minSize;
    }

    //-------------------------------------------------------------------
    // At this time, you can not change the size of the screen after a session
    // is opened. If you really want to change the screen size, you will need
    // to close the session, re-set the size of the screen and reopen the
    // session
    public void setScreenDim(int width, int height)
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.setScreenDim(" + width +
                               ", " + height + ")");
        }

        if (width < 1 || height < 1) {
            return;
        }

        if (screenDim.width == width && screenDim.height == height) {
            return;
        }

        screenDim.width = width;
        screenDim.height = height;

        jsc.jsValues.uiglobals.screenSize.width = screenDim.width;
        jsc.jsValues.uiglobals.screenSize.height = screenDim.height;

        isDimChanged = true;

        if (jsc.isSessionOpened) {
            // We do this stuff in a new thread so that the stop button
            // shows up red immediately, and the GUI is responsive.
            Thread thread = new Thread(new SessionReopen());
            thread.start();
        }

        repaint();
    }

    //-------------------------------------------------------------------
    private class SessionReopen implements Runnable
    {
        public void run()
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseScreen.SessionReopen.run()");
            }

            saveJmolStates();

            // Save and close the current session.
            String command = new String();
            command = DEViseCommands.SAVE_CUR_SESSION + "\n";
            jsc.dispatcher.start(command);
            jsc.dispatcher.waitForCmds();
            jsc.closeSession();

            // Send updated size, re-open the session.
            jsc.dispatcher.waitForCmds();
            command = DEViseCommands.SET_DISPLAY_SIZE + " " +
                      jsc.jsValues.uiglobals.screenSize.width + " " +
                      jsc.jsValues.uiglobals.screenSize.height + " " +
                      jsc.jsValues.uiglobals.screenRes + " " +
                      jsc.jsValues.uiglobals.screenRes + "\n";
            command += DEViseCommands.REOPEN_SESSION + "\n";
            jsc.dispatcher.start(command);
            jsc.dispatcher.waitForCmds();

            restoreJmolStates();
        }
    }

    //-------------------------------------------------------------------
    // Show help messages in all views.
    public synchronized void showAllHelp()
    {
        if (DEBUG >= 2) {
            System.out.println("DEViseScreen.showAllHelp()");
        }

        String cmd = "";
        for (int i = 0; i < allCanvas.size(); i++) {
            DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
            cmd = cmd + DEViseCommands.GET_VIEW_HELP + " " +
                  c.view.getCurlyName() + " " + 0 + " " + 0 + "\n";
            // If this view has children, send a command to show their help as well.
            for(int j = 0; j < c.view.viewChilds.size(); j++) {
                DEViseView v = (DEViseView)c.view.viewChilds.elementAt(j);
                cmd = cmd + DEViseCommands.GET_VIEW_HELP + " " +
                      v.getCurlyName() + " " + 0 + " " + 0 + "\n";
            }
        }

        jsc.dispatcher.start(cmd);
    }

    //-------------------------------------------------------------------
    // Hide help messages in all views.
    public synchronized void hideAllHelp()
    {
        if (DEBUG >= 2) {
            System.out.println("DEViseScreen.hideAllHelp()");
        }

        for (int i = 0; i < allCanvas.size(); i++) {
            DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
            c.helpMsg = null;

            for(int j = 0; j < c.childViewHelpMsgs.size(); j++) {
                c.childViewHelpMsgs.setElementAt(null, j);
            }
        }

        repaint();
    }

    //-------------------------------------------------------------------
    public void addView(DEViseView view)
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.addView(" +
                               view.viewName + ")");
        }

        if (view == null) {
            return;
        }

        // view already exists
        if (viewTable.get(view.viewName) != null) {
            return;
        }

        // parent view always created before view symbol
        if (view.parentView != null) {
            view.parentView.addChild(view);
        }

        // top view always created before bottom view
        if (view.pileBaseView != null) {
            view.pileBaseView.addPiledView(view);
        }

        viewTable.put(view.viewName, view);
    }

    //-------------------------------------------------------------------
    // Put an image into a view, creating the appropriate DEViseCanvas
    // object if necessary.
    // Only the base view (not view symbols, not non-base views in piles)
    // will have an image and only the base view will associated with a
    // canvas; a canvas will only be assiciated one view and one view only.
    public void updateViewImage(String name, Image image)
    {
        if (name == null || image == null) {
            return;
        }

        if (DEBUG >= 2) {
            System.out.println("DEViseScreen.updateViewImage(" +
                               name + ")");
        }

        DEViseView view = getView(name);

        if (view == null) {
            return;
        }

        if (view.canvas == null) {
            DEViseCanvas canvas = null;
            if (view.viewDimension == 2) {
                canvas = new DEViseCanvas2D(view, image);
            } else if (view.viewDimension == 3) {
                if (view.getUseJmol()) {
                    canvas = new DEViseCanvas3DJmol(view, image);
                } else {
                    canvas = new DEViseCanvas3DPlain(view, image);
                }
            } else {
                System.err.println("Illegal number of dimensions: " +
                                   view.viewDimension);
            }
            view.canvas = canvas;

            int i;
            for (i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
                if (c.view.viewZ > canvas.view.viewZ) {
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

        updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // Remove the given view by name.
    //
    // When we remove a view, everything assiciated with this view is removed,
    // such as child views, cursors, gdata, and even the canvas and piled
    // views (if the view to be removed is a base view).
    public void removeView(String name)
    {
        if (name == null) {
            return;
        }

        removeView(getView(name));
    }

    //-------------------------------------------------------------------
    // Remove the given view by object reference.
    public synchronized void removeView(DEViseView view)
    {
        if (view == null) {
            return;
        }

        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.removeView(" +
                               view.viewName + ")");
        }

        Vector vector = null;

        // remove itself from its parent or piled view, if any
        if (view.pileBaseView != null) {
            view.pileBaseView.removePiledView(view);
        }

        if (view.parentView != null) {
            view.parentView.removeChild(view);
        }

        // remove GDatas associated with it
        vector = view.viewGDatas;
        if (vector.size() > 0) {
            for (int i = 0; i < vector.size(); i++) {
                DEViseGData data = (DEViseGData)vector.elementAt(i);
                if (data.symbol != null) {
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
        viewTable.remove(view.viewName);

        updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // remove the child views only
    public void removeChildViews(String viewName)
    {
        if (viewName == null) {
            return;
        }

        removeChildViews(getView(viewName));
    }

    //-------------------------------------------------------------------
    public void removeChildViews(DEViseView view)
    {
        if (view == null) {
            return;
        }

        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.removeChildViews(" +
                               view.viewName + ")");
        }

        Vector vector = view.viewChilds;
        while (vector.size() > 0) {
            removeView((DEViseView)vector.elementAt(0));
        }

        // In this case, also remove any help messages that might currently be displayed by the
        // children by setting them to null
        if(view.canvas != null) {
            for(int i = 0; i < view.canvas.childViewHelpMsgs.size(); i++) {
                view.canvas.childViewHelpMsgs.setElementAt(null, i);
            }
        }
        // Can not use the for iteration, because removeView also remove that view
        // from its parents, the the size of the viewChilds of the parentView is
        // actually changing on the fly
        //for (int i = 0; i < child.size(); i++) {
        //    removeView(view.getChild(i));
        //}

        updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // Translate view name into view object.
    public DEViseView getView(String name)
    {
        if (name == null) {
            return null;
        }

        return (DEViseView)viewTable.get(name);
    }

    //-------------------------------------------------------------------
    public DEViseView getCurrentView()
    {
        return currentView;
    }

    //-------------------------------------------------------------------
    public void setCurrentView(DEViseView view)
    {
        if (DEBUG >= 3) {
            System.out.println("DEViseScreen.setCurrentView(" +
                               view != null ? view.viewName : "null" + ")");
        }

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
            if (currentView.getCanvas() != null) {
                currentView.getCanvas().requestFocus();
                currentView.getCanvas().repaint();
            }
        }
    }

    //-------------------------------------------------------------------
    // Remove the given view's current GData and add the new GData;
    // force a repaint.
    public synchronized void updateViewGData(String viewName, Vector gdList)
    {
        if (DEBUG >= 2) {
            System.out.println("DEViseScreen.updateViewGData(" +
                               viewName + ")");
        }

        DEViseView view = getView(viewName);

        if (view == null) {
            return;
        }

        Vector viewGD = view.viewGDatas;
        if (viewGD.size() > 0) {
            for (int i = 0; i < viewGD.size(); i++) {
                DEViseGData data = (DEViseGData)viewGD.elementAt(i);
                if (data.symbol != null) {
                    obsoleteGData.addElement(data);
                }
            }

            view.removeAllGData();
        }

        if (gdList != null && gdList.size() > 0) {
            for (int i = 0; i < gdList.size(); i++) {
                DEViseGData g = (DEViseGData)gdList.elementAt(i);
                if (g.isJavaSymbol) {
                    newGData.addElement(g);
                }

                view.addGData(g);
            }
        }

        //TEMP -- this could really stand to get cleaned up
        if (view.viewDimension == 3 && view.canvas != null) {
            DEVise3DLCS oldLcs = null;
            int oldSX = 0, oldSY = 0, oldTSX = 0, oldTSY = 0;
            if (view.canvas.crystal != null) {
                oldLcs = view.canvas.crystal.lcs;
                oldSX = view.canvas.crystal.shiftedX;
                oldSY = view.canvas.crystal.shiftedY;
                oldTSX = view.canvas.crystal.totalShiftedX;
                oldTSY = view.canvas.crystal.totalShiftedY;
            }
            view.canvas.clear3DViewer();
            view.canvas.create3DViewer();
            if (oldLcs != null) {
                view.canvas.crystal.lcs = oldLcs;
                view.canvas.crystal.shiftedX = oldSX;
                view.canvas.crystal.shiftedY = oldSY;
                view.canvas.crystal.totalShiftedX = oldTSX;
                view.canvas.crystal.totalShiftedY = oldTSY;
            }
        } else if (view.pileBaseView != null &&
                   view.pileBaseView.viewDimension == 3 &&
                   view.pileBaseView.canvas != null) {
            DEVise3DLCS oldLcs = null;
            int oldSX = 0, oldSY = 0, oldTSX = 0, oldTSY = 0;
            if (view.pileBaseView.canvas.crystal != null) {
                oldLcs = view.pileBaseView.canvas.crystal.lcs;
                oldSX = view.pileBaseView.canvas.crystal.shiftedX;
                oldSY = view.pileBaseView.canvas.crystal.shiftedY;
                oldTSX = view.pileBaseView.canvas.crystal.totalShiftedX;
                oldTSY = view.pileBaseView.canvas.crystal.totalShiftedY;
            }
            view.pileBaseView.canvas.create3DViewer();
            if (oldLcs != null) {
                view.pileBaseView.canvas.crystal.lcs = oldLcs;
                view.pileBaseView.canvas.crystal.shiftedX = oldSX;
                view.pileBaseView.canvas.crystal.shiftedY = oldSY;
                view.pileBaseView.canvas.crystal.totalShiftedX = oldTSX;
                view.pileBaseView.canvas.crystal.totalShiftedY = oldTSY;
            }
        }

        updateObjects();
        repaint();
    }

    //-------------------------------------------------------------------
    // Add the given cursor to the named view.
    public void updateCursor(String viewName, DEViseCursor cursor)
    {
        if (DEBUG >= 1) {
            System.out.println("DIAG DEViseScreen.updateCursor(" +
                               viewName + ", " + cursor.name + ")");
        }

        DEViseView view = getView(viewName);

        if (view == null) {
            return;
        }

        if (cursor != null) {
            if (view.addCursor(cursor)) {
                updateObjects();
                repaint();
            }
        }
    }

    //-------------------------------------------------------------------
    // Remove the named cursor from the named view.
    public void hideCursor(String cursorName, String viewname)
    {
        DEViseView view = getView(viewname);

        if (view != null) {
            if (view.hideCursor(cursorName)) {
                updateObjects();
                repaint();
            }
        }
    }

    //-------------------------------------------------------------------
    public void updateViewDataRange(String viewName, String axis,
                                    float min, float max, String format, float factor,
                                    int label, int type, int size, int bold, int italic)
    {
        DEViseView view = getView(viewName);

        if (view != null && axis != null) {
            view.updateDataRange(axis, min, max, format, factor,
                                 label, type, size, bold, italic);
        }
    }

    //-------------------------------------------------------------------
    public synchronized void setLastAction(String cmd)
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

    //-------------------------------------------------------------------
    // Update the JS when a session is opened or closed.
    public synchronized void updateScreen(boolean opening)
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.updateScreen(" + opening + ")");
        }

        if (opening) {
            jsc.isSessionOpened = true;

            if (jsc.parentFrame != null && jsc._currentVisType != null) {
                jsc.parentFrame.setTitle(jsc._currentVisType + " (" +
                                         DEViseUIGlobals.javaScreenTitle + ")");
            }
        } else {
            // Note: this section of code does the work of cleaning
            // up when we close a session.  RKW 2000-03-31.

            if (jsc.parentFrame != null) {
                jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle);
            }

            // Remove the view's references to all of its GData records,
            // so they hopefully get garbage collected. RKW 2000-03-31.
            Enumeration views = viewTable.elements();
            while (views.hasMoreElements()) {
                DEViseView tmpView = (DEViseView)views.nextElement();
                tmpView.removeAllGData();
            }

            // Call close() on all DEViseCanvases (needed to get rid of tree windows
            // in Jmol canvases, at least for now).
            for (int index = 0; index < allCanvas.size(); index++) {
                DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(index);
                canvas.close();
            }

            // Get rid of any session-specific menu items we have.
            jsc.hideSessionMenu();

            // Note: I'm wondering if some of this stuff should only happen
            // in the event thread, and the fact that it's happening in
            // a command thread here might be causing us some problems.
            // But when I moved it to the event thread using
            // SwingUtilities.invokeLater, things didn't work -- it seemed
            // like some stuff here was not happening in the correct order
            // relative to other parts of the command.  wenger 2011-10-10
            removeAll();

            allCanvas = new Vector();
            obsoleteGData = new Vector();
            newGData = new Vector();
            newCanvas = new Vector();
            obsoleteCanvas = new Vector();

            viewTable = new Hashtable();

            currentView = null;
            lastActionView = null;

            jsc.isSessionOpened = false;

            jsc.viewInfo.updateInfo();
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_IDLE, false);
            jsc.viewInfo.updateCount(0);

            setToPermanentCursor();

            offScrImg = null;

            // Get rid of our current fonts, so the list of fonts doesn't
            // keep increasing as we perhaps draw different GDatas.
            DEViseFonts.ClearFonts();

            // We've just set a whole bunch of references to null, so
            // this should be a good time to garbage collect.
            System.gc();

            updateObjects();
            repaint();
        }
    }

    //-------------------------------------------------------------------
    // Show the given help message in the named view.
    public synchronized void showHelpMsg(String viewName, String msg)
    {
        DEViseView view = getView(viewName);
        if (view == null) {
            return;
        }

        if (msg == null) {
            return;
        }

        if (view.canvas != null && view.parentView == null) {
            view.canvas.helpMsg = msg;
            view.canvas.repaint();
        }

        // We are a child view, figure out where to add childViewHelpMsgs vector
        else if (view.parentView != null) {
            int element = view.parentView.viewChilds.indexOf(view);
            if(view.parentView.canvas.childViewHelpMsgs.size() <= element) {
                view.parentView.canvas.childViewHelpMsgs.addElement(msg);
            }
            else {
                view.parentView.canvas.childViewHelpMsgs.setElementAt(msg, element);
            }

            view.parentView.canvas.repaint();
        }
    }

    //-------------------------------------------------------------------
    // Enable double-buffering
    //TEMP -- hmm -- does this goof up Jmol?
    public synchronized void update(Graphics gc)
    {
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

    //-------------------------------------------------------------------
    // Update objects (canvases, buttons, etc.) -- remove obsolete ones
    // and actually add new ones to the GUI.
    private void updateObjects()
    {
        Runnable doUpdateObjects = new DoUpdateObjects();
        SwingUtilities.invokeLater(doUpdateObjects);
    }

    //-------------------------------------------------------------------
    // This class allows us to make sure that we actually do the object
    // updating in the GUI thread.
    private class DoUpdateObjects implements Runnable {
        public void run() {
            if (isDimChanged) {
                isDimChanged = false;
                setSize(screenDim);

                if (jsc.jsValues.uiglobals.inBrowser) {
                    //TEMP  AWT uses validate to cause a container to lay out its subcomponents again after the components it contains have been added to or modified
                    //TEMP -- do we really need to call validate here?
                    jsc.validate();
                } else {
                    jsc.parentFrame.pack();
                }

                jsc.repaint();
            }

            // Actually remove from the GUI any canvases that are no longer
            // needed (doing this here so it's done in the GUI thread).
            while (obsoleteCanvas.size() > 0) {
                DEViseCanvas c = (DEViseCanvas)newCanvas.firstElement();
                remove(c);
                obsoleteCanvas.removeElement(c);
            }

            // Actually add to the GUI any new canvases (doing this here
            // so it's done in the GUI thread).
            while (newCanvas.size() > 0) {
                DEViseCanvas c = (DEViseCanvas)newCanvas.firstElement();
                add(c, c.posZ);
                c.setBounds(c.getLocInScreen());
                newCanvas.removeElement(c);
            }

            // Actually remove from the GUI any GUI objects (e.g., buttons)
            // corresponding to GData that's no longer valid (doing this
            // here so it's done in the GUI thread).  ("Normal" GDatas never
            // get into the obsoleteGData list.)
            while (obsoleteGData.size() > 0) {
                DEViseGData gdata = (DEViseGData)obsoleteGData.firstElement();
                gdata.parentView.canvas.remove(gdata.symbol);
                obsoleteGData.removeElement(gdata);
                gdata.parentView.canvas._hasJavaSym = false;
            }

            // Actually add to the GUI any GUI objects (e.g., buttons)
            // corresponding to new GData records (doing this here so
            // it's done in the GUI thread).  ("Normal" GDatas never
            // get into the newGData list.)
            while (newGData.size() > 0) {
                DEViseGData gdata = (DEViseGData)newGData.firstElement();
                newGData.removeElement(gdata);
                if (gdata._menuType.equals("")) {
                    if (gdata.symbol == null) {
                        JButton button = new DEViseButton(gdata._buttonLabel,
                                                          jsc.jsValues);
                        button.setActionCommand(gdata._buttonCmd);
                        button.setFont(DEViseFonts.getFont(10,
                                                           DEViseFonts.MONOSPACED, 0, 0));
                        button.addActionListener(new ActionListener()
                        {
                            public void actionPerformed(ActionEvent event)
                            {
                                // Note: this is very specific to BMRB
                                // dynamics movies.
                                if (event.getActionCommand().contains(
                                            "&residues=")) {
                                    if (!jsc.showDynamicsMovieDlg()) {
                                        // User clicked "Cancel".
                                        return;
                                    }
                                }
                                jsc.showDocument(event.getActionCommand(),
                                                 true);
                            }
                        });

                        gdata.symbol = button;
                    }
                    Component gs = gdata.symbol;
                    gdata.parentView.canvas.add(gs);
                    gs.setBounds(gdata.GDataLoc);
                    gdata.parentView.canvas._hasJavaSym = true;
                } else {
                    jsc.createSessionMenuItem(gdata._menuType,
                                              gdata._menuName, gdata._buttonLabel, gdata._buttonCmd);
                }
            }
        }
    }

    //-------------------------------------------------------------------
    // Find the DEViseCanvas (if any) that the mouse pointer is currently
    // within.
    private DEViseCanvas findMouseCanvas()
    {
        for (int index = 0; index < allCanvas.size(); index++) {
            DEViseCanvas canvas = (DEViseCanvas)allCanvas.elementAt(index);
            if (canvas.mouseIsWithin()) {
                return canvas;
            }
        }

        return null;
    }

    //-------------------------------------------------------------------
    // Save the Jmol states for all Jmol canvases.  Note: this hasn't
    // really been tested with multiple Jmol canvases in a single
    // session (part of bug 1005 fix).
    private void saveJmolStates()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.saveJmolStates()");
        }

        jmolStates = new Hashtable();

        Enumeration views = viewTable.elements();
        while (views.hasMoreElements()) {
            DEViseView tmpView = (DEViseView)views.nextElement();
            DEViseCanvas canvas = tmpView.canvas;
            if (canvas != null) {
                Object jmState = canvas.saveJmolState();
                if (jmState != null) {
                    if (DEBUG >= 1) {
                        System.out.println("Saving Jmol state for view " +
                                           tmpView.viewName);
                    }
                    jmolStates.put(tmpView.viewName, jmState);
                }
            }
        }
    }

    //-------------------------------------------------------------------
    // Restore the Jmol states for all Jmol canvases.  Note: this hasn't
    // really been tested with multiple Jmol canvases in a single
    // session (part of bug 1005 fix).
    private void restoreJmolStates()
    {
        if (DEBUG >= 1) {
            System.out.println("DEViseScreen.restoreJmolStates()");
        }

        Enumeration views = viewTable.elements();
        while (views.hasMoreElements()) {
            DEViseView tmpView = (DEViseView)views.nextElement();
            DEViseCanvas canvas = tmpView.canvas;
            Object jmState = jmolStates.get(tmpView.viewName);
            if (jmState != null) {
                if (DEBUG >= 1) {
                    System.out.println("Restoring Jmol state for view " +
                                       tmpView.viewName);
                }
                canvas.restoreJmolState(jmState);
                // Make sure we don't restore more than once from one
                // save.
                jmolStates.remove(tmpView.viewName);
            }
        }
    }

    //-------------------------------------------------------------------
    //
    // Note: we don't absolutely need this class; it's here just to help
    // debug the problem with input focus sometimes improperly getting
    // taken away from a DEViseCanvas.
    //
    class DSFocusListener extends FocusAdapter
    {
        //---------------------------------------------------------------
        public void focusGained(FocusEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseScreen gained focus");
            }
        }

        //---------------------------------------------------------------
        public void focusLost(FocusEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseScreen lost focus");
            }
        }
    }

    //-------------------------------------------------------------------
    //
    // Note: the methods here should not generally get called; however,
    // the JVM sometimes gets confused, and transfers input focus
    // to the DEViseScreen when the mouse is in a view (DEViseCanvas).
    // In that case, the event comes here and we have to pass it
    // along to the appropriate canvas.
    //
    class DSKeyListener extends KeyAdapter
    {
        //---------------------------------------------------------------
        public void keyPressed(KeyEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("DEViseScreen.keyPressed(" +
                                   event.getKeyChar() + ")");
            }

            DEViseCanvas canvas = findMouseCanvas();
            if (canvas != null) {
                canvas.dispatchEvent(event);
            }
        }

        //---------------------------------------------------------------
        public void keyReleased(KeyEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("DEViseScreen.keyReleased(" +
                                   event.getKeyChar() + ")");
            }

            DEViseCanvas canvas = findMouseCanvas();
            if (canvas != null) {
                canvas.dispatchEvent(event);
            }
        }
    }

    //-------------------------------------------------------------------
    // Following methods are for collaboration JavaScreens to
    // process 3D view actions.
    public void collab3DView(String[] args)
    {
        DEViseView view = getView(args[1]);
        if (view == null) {
            jsc.pn("No view found!");
            return;
        }

        DEViseCanvas canvas = view.getCanvas();
        if (canvas == null) {
            jsc.pn("No canvas found!");
            return;
        }

        DEViseCrystal crystal = canvas.crystal;
        if (crystal == null) {
            jsc.pn("No crystal found!");
            return;
        }

        jsc.pn("Crystal found!");

        float[][] data = new float[3][3];
        float[] origin = new float[3];
        Float f = null;

        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                f = new Float(args[3*i+j+2]);
                data[i][j] = f.floatValue();
            }
        }

        for (int i=0; i<3; i++) {
            f = new Float(args[11+i]);
            origin[i] = f.floatValue();
        }

        crystal.lcs.assign(data, origin);

        crystal.shiftedX = Integer.parseInt(args[14]);
        crystal.shiftedY = Integer.parseInt(args[15]);

        canvas.isMouseDragged = true;
        crystal.isTransformed = true;
        canvas.repaint();
        repaint();
    }
}
