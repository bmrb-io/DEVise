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

// $Id$

// $Log$
// Revision 1.69  2001/05/03 16:25:56  xuk
// Added multiply factor for displaying mouse postion.
// Changed updateViewDataRange() function;
// Added factor argument when calling updateInfo().
//
// Revision 1.68  2001/04/01 03:51:17  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.67  2001/03/25 20:39:29  xuk
// Added collab3DView() method to process 3D view collaboration commands.
//
// Revision 1.66  2001/03/20 17:49:46  xuk
// Added collaboration for 3D Views.
//
// Revision 1.65  2001/01/08 20:31:52  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.63.4.5  2000/11/21 01:51:33  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.63.4.4  2000/10/18 20:28:11  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.64  2000/10/03 19:23:27  wenger
// Fixed bugs 592 and 594 (JavaScreen sometimes missing keystrokes in views).
//
// Revision 1.63.4.3  2000/10/18 18:29:23  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//
// Revision 1.63.4.2  2000/10/11 02:59:48  xuk
// Add notifyall() to wake up dispatcher thread.
//
// Revision 1.63.4.1  2000/10/09 16:32:22  xuk
// Change dispathcer.start() to dispatcher.command = cmd, since DEViseCmdDispatcher thread starts only once now.
//
// Revision 1.63  2000/07/20 16:26:07  venkatan
// Mouse Location Display format - is now controlled by printf type
// format strings specified by the VIEW_DATA_AREA command
//
// Revision 1.62  2000/06/21 18:37:30  wenger
// Removed a bunch of unused code (previously just commented out).
//
// Revision 1.61  2000/06/21 18:10:15  wenger
// Changes to 3D requested by BMRB: removed axes; up/down mouse movement
// does zooming; molecule doesn't move when changing atoms; 'r' resets
// location and zoom as well as rotation.
//
// Revision 1.60  2000/06/12 22:13:56  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.59  2000/05/25 14:47:39  wenger
// 3D coordinate system remains unchanged when new GData arrives; 'r' or 'R'
// in view resets to default coordinates.
//
// Revision 1.58  2000/05/22 17:52:49  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.57  2000/05/12 20:43:56  wenger
// Added more comments to the DEViseScreen, DEViseCanvas, and jsdevisec
// classes and cleaned up the code; commented out unused code; added
// named constants for location of mouse pointer on a DEVise cursor.
//
// Revision 1.56  2000/05/11 20:59:49  wenger
// More comments.
//
// Revision 1.55  2000/05/11 20:19:33  wenger
// Cleaned up jsdevisec.java and added comments; eliminated
// jsdevisec.lastCursor (not really needed).
//
// Revision 1.54  2000/05/04 15:53:31  wenger
// Added consistency checking, added comments, commented out unused code
// in DEViseScreen.java, DEViseCanvas.java, DEViseView.java,
// DEViseCmdDispatcher.java.
//
// Revision 1.53  2000/04/27 20:15:25  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.52  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.51  2000/04/07 22:43:14  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.50  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.49  2000/03/31 19:29:16  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.48  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.47  2000/02/23 21:12:13  hongyu
// *** empty log message ***
//
// Revision 1.46  2000/02/23 10:44:02  hongyu
// *** empty log message ***
//
// Revision 1.45  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.44  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.43  1999/12/07 23:18:21  hongyu
// *** empty log message ***
//
// Revision 1.42  1999/11/03 08:00:49  hongyu
// *** empty log message ***
//
// Revision 1.41  1999/11/01 17:40:16  hongyu
// *** empty log message ***
//
// Revision 1.40  1999/10/14 15:09:05  hongyu
// *** empty log message ***
//
// Revision 1.39  1999/10/12 21:58:14  hongyu
// *** empty log message ***
//
// Revision 1.38  1999/10/12 00:08:43  hongyu
// *** empty log message ***
//
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

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.lang.*;

public class DEViseScreen extends Panel
{
    jsdevisec jsc = null;

    Dimension screenDim = new Dimension(600, 400);

    // Hongyu says that having these three vectors for canvases is related
    // to making things thread-safe.
    Vector allCanvas = new Vector();
    Vector newCanvas = new Vector();
    Vector obsoleteCanvas = new Vector();

    // Hongyu says that we have both a Vector and a HashTable of views
    // for efficiency.  I'm a bit worried about the possibility of
    // inconsistencies...  RKW 2000-05-04.
    Vector allViews = new Vector();
    Hashtable viewTable = new Hashtable();

    Vector javaGDatas = new Vector();
    Vector newGData = new Vector();
    Vector obsoleteGData = new Vector();

    private DEViseView currentView = null; // the view the mouse is in
    public DEViseView lastActionView = null;
    public boolean guiAction = false;
    private String lastCommand = null;

    private static final int DEBUG = 0; // 0 - 3

    boolean isDimChanged = false;
    boolean helpclicked = false;

    public Image offScrImg = null;

    public Point finalMousePosition = new Point(-1, -1);

    public DEViseScreen(jsdevisec what)
    {
        jsc = what;

        screenDim = new Dimension(jsc.jsValues.uiglobals.screenSize.width, jsc.jsValues.uiglobals.screenSize.height);

        setLayout(null);
        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        this.enableEvents(AWTEvent.MOUSE_MOTION_EVENT_MASK);

        addMouseMotionListener(new MouseMotionAdapter()
            {
                public void mouseMoved(MouseEvent event)
                {
                    Point p = event.getPoint();

                    if (jsc.dispatcher.getStatus() != 0) {
                        setCursor(DEViseUIGlobals.waitCursor);
                    } else {
                        setCursor(DEViseUIGlobals.defaultCursor);
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

    // Hongyu says that we have both this method and a mouse motion
    // listener because this method gets both move and drag events,
    // which we want in order to update the mouse location display.
    protected void processMouseMotionEvent(MouseEvent event)
    {
        finalMousePosition.x = event.getX();
        finalMousePosition.y = event.getY();

        super.processMouseMotionEvent(event);
    }

    // Hongyu says that this code has something to do with fixing a
    // case where Java does things wrong.
    public void reEvaluateMousePosition()
    {
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
                Point p = new Point(finalMousePosition.x - loc.x, finalMousePosition.y - loc.y);
                if (p.x >= 0 && p.x <= loc.width && p.y >= 0 && p.y <= loc.height) {
                    canvas.checkMousePos(p, false);
                    repaint();
                    return;
                }
            }

            if (currentView != null) {
                jsc.viewInfo.updateInfo();
                setCurrentView(null);
            }

            setCursor(DEViseUIGlobals.defaultCursor);
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

        jsc.jsValues.uiglobals.screenSize.width = screenDim.width;
        jsc.jsValues.uiglobals.screenSize.height = screenDim.height;

        isDimChanged = true;

        repaint();
    }

    // Show help messages in all views.
    public synchronized void showAllHelp()
    {
        helpclicked = !helpclicked;

        if (helpclicked) {
            String cmd = "";
            for (int i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
                cmd = cmd + DEViseCommands.GET_VIEW_HELP + " " +
		  c.view.getCurlyName() + " " + 0 + " " + 0 + "\n";
            }

            jsc.dispatcher.start(cmd);
        } else {
            for (int i = 0; i < allCanvas.size(); i++) {
                DEViseCanvas c = (DEViseCanvas)allCanvas.elementAt(i);
                c.helpMsg = null;
            }

            repaint();
        }
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
        if (view.pileBaseView != null) {
            view.pileBaseView.addPiledView(view);
        }

        allViews.addElement(view);

        viewTable.put(view.viewName, view);
    }

    // Put an image into a view, creating the appropriate DEViseCanvas
    // object if necessary.
    // Only the base view (not view symbols, not non-base views in piles)
    // will have an image and only the base view will associated with a
    // canvas; a canvas will only be assiciated one view and one view only.
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

        repaint();
    }

    // Remove the given view by name.
    //
    // When we remove a view, everything assiciated with this view is removed,
    // such as child views, cursors, gdata, and even the canvas and piled
    // views (if the view to be removed is a base view).
    public void removeView(String name)
    {
        if (name == null)
            return;

        removeView(getView(name));
    }

    // Remove the given view by object reference.
    public synchronized void removeView(DEViseView view)
    {
        if (view == null) {
            return;
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
    public void removeChildViews(String viewName)
    {
        if (viewName == null)
            return;

        removeChildViews(getView(viewName));
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

    // Translate view name into view object.
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
            if (currentView.getCanvas() != null) {
                currentView.getCanvas().requestFocus();
                currentView.getCanvas().repaint();
            }
        }
    }

    // Remove the given view's current GData and add the new GData;
    // force a repaint.
    public synchronized void updateGData(String viewName, Vector gdList)
    {
        DEViseView view = getView(viewName);

        if (view == null) {
            return;
        }

        Vector viewGD = view.viewGDatas;
        if (viewGD.size() > 0) {
            for (int i = 0; i < viewGD.size(); i++) {
                DEViseGData data = (DEViseGData)viewGD.elementAt(i);
                if (data.isJavaSymbol) {
                    javaGDatas.removeElement(data);
                    obsoleteGData.addElement(data);
                }
            }

            view.removeAllGData();
        }

        if (gdList != null && gdList.size() > 0) {
            for (int i = 0; i < gdList.size(); i++) {
                DEViseGData g = (DEViseGData)gdList.elementAt(i);
                if (g.isJavaSymbol) {
                    javaGDatas.addElement(g);
                    newGData.addElement(g);
                }

                view.addGData(g);
            }
        }

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
	    view.canvas.crystal = null;
	    view.canvas.createCrystal();
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
            view.pileBaseView.canvas.createCrystal();
	    if (oldLcs != null) {
	        view.pileBaseView.canvas.crystal.lcs = oldLcs;
		view.pileBaseView.canvas.crystal.shiftedX = oldSX;
		view.pileBaseView.canvas.crystal.shiftedY = oldSY;
		view.pileBaseView.canvas.crystal.totalShiftedX = oldTSX;
		view.pileBaseView.canvas.crystal.totalShiftedY = oldTSY;
	    }
        }

        repaint();
    }

    // Add the given cursor to the named view.
    public void updateCursor(String viewName, DEViseCursor cursor)
    {
        DEViseView view = getView(viewName);

        if (view == null) {
            return;
        }

        if (cursor != null) {
            if (view.addCursor(cursor)) {
                repaint();
            }
        }
    }

    // Remove the named cursor from the named view.
    public void removeCursor(String cursorName, String viewname)
    {
        DEViseView view = getView(viewname);

        if (view != null) {
            if (view.removeCursor(cursorName)) {
                repaint();
            }
        }
    }

    public void updateViewDataRange(String viewName, String axis,
      float min, float max, String format, float factor)
    {
        DEViseView view = getView(viewName);

        if (view != null && axis != null) {
            view.updateDataRange(axis, min, max, format, factor);
        }
    }

    public synchronized void setLastAction(String cmd)
    {
        lastCommand = cmd;

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

    // Update the JS when a session is opened or closed.
    public synchronized void updateScreen(boolean flag)
    {
        if (flag) {
            jsc.isSessionOpened = true;
        } else {
	    // Note: this section of code does the work of cleaning
	    // up when we close a session.  RKW 2000-03-31.

	    // Remove the view's references to all of its GData records,
	    // so they hopefully get garbage collected. RKW 2000-03-31.
            for (int viewNum = 0; viewNum < allViews.size(); viewNum++) {
	        DEViseView tmpView = (DEViseView)allViews.elementAt(viewNum);
	        tmpView.removeAllGData();
	    }

            removeAll();

            allCanvas = new Vector();
            allViews = new Vector();
            javaGDatas = new Vector();
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

            setCursor(DEViseUIGlobals.defaultCursor);

            offScrImg = null;

	    // Get rid of our current fonts, so the list of fonts doesn't
	    // keep increasing as we perhaps draw different GDatas.
	    DEViseFonts.ClearFonts();

	    // We've just set a whole bunch of references to null, so
	    // this should be a good time to garbage collect.
	        System.gc();

            repaint();
        }
    }

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

        if (view.canvas != null) {
            view.canvas.helpMsg = msg;
            view.canvas.repaint();
        }
    }

    // Enable double-buffering
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

    public void paint(Graphics g)
    {
        if (isDimChanged) {
            isDimChanged = false;
            setSize(screenDim);

            if (jsc.jsValues.uiglobals.inBrowser) {
                jsc.validate();
            } else {
                jsc.parentFrame.pack();
            }

            jsc.repaint();
        }

	// ADD COMMENT -- what is this stuff for??
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
        //obsoleteGData = new Vector();

        while (newGData.size() > 0) {
            DEViseGData gdata = (DEViseGData)newGData.firstElement();
            Component gs = gdata.symbol;
            add(gs);
            gs.setBounds(gdata.GDataLocInScreen);
            newGData.removeElement(gdata);
        }
        //newGData = new Vector();

        super.paint(g);
    }

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

    //
    // Note: we don't absolutely need this class; it's here just to help
    // debug the problem with input focus sometimes improperly getting
    // taken away from a DEViseCanvas.
    //
    class DSFocusListener extends FocusAdapter
    {
        public void focusGained(FocusEvent event)
	{
	    if (DEBUG >= 1) {
	        System.out.println("DEViseScreen gained focus");
	    }
	}

	public void focusLost(FocusEvent event)
	{
	    if (DEBUG >= 1) {
	        System.out.println("DEViseScreen lost focus");
	    }
	}
    }

    //
    // Note: the methods here should not generally get called; however,
    // the JVM sometimes gets confused, and transfers input focus
    // to the DEViseScreen when the mouse is in a view (DEViseCanvas).
    // In that case, the event comes here and we have to pass it
    // along to the appropriate canvas.
    //
    class DSKeyListener extends KeyAdapter
    {
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

    // Following methods are for collaboration JavaScreens to 
    // process 3D view actions.
    
    public void collab3DView(String[] args)
    {
	DEViseView view = null;
	DEViseCanvas canvas = null;
	DEViseCrystal crystal = null;
	int i = 0, j = 0;

	for (i = 0; i < allViews.size(); i++) {
	    view = (DEViseView)allViews.elementAt(i);
	    if (view.viewName.equals(args[1]))
		break;
	}

	if ((i == allViews.size()) || (view == null)) {
	    jsc.pn("No view found!");
	    return;
	}

	canvas = view.getCanvas();
	if (canvas == null) {
	    jsc.pn("No canvas found!");
	    return;
	}

	crystal = canvas.crystal;
	if (crystal == null) {
	    jsc.pn("No crystal found!");
	    return;
	}	    

	jsc.pn("Crystal found!");

	float[][] data = new float[3][3];
	float[] origin = new float[3];
	Float f = null;

	for (i=0; i<3; i++) 
	    for (j=0; j<3; j++) {
		f = new Float(args[3*i+j+2]);
		data[i][j] = f.floatValue();
	    }

	for (i=0; i<3; i++) {
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


