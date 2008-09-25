// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// 2D specific aspects of a DEViseCanvas.

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6  2008/03/05 20:02:56  wenger
// Fixed bug 965 (disabled alt and shift modifiers in 2D views).
//
// Revision 1.5  2007/08/27 19:16:39  wenger
// Merged andyd_gui_br_7 thru andyd_gui_br_8 to trunk.
//
// Revision 1.4  2007/08/03 20:17:27  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.3  2007/06/27 17:47:58  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.2.2.8  2007/08/27 18:35:31  wenger
// Implemented "click to zoom" feature in toolbar zoom modes (zooms in or
// out 50% and re-centers).
//
// Revision 1.2.2.7  2007/08/10 15:27:27  wenger
// Toolbar now stays in the same mode after an action as opposed to
// resetting to the default mode.
//
// Revision 1.2.2.6  2007/08/03 19:21:15  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.2.2.5  2007/06/21 15:07:06  wenger
// Toolbar help mode now resets to normal after one click.
//
// Revision 1.2.2.4  2007/06/19 20:49:48  wenger
// Toolbar now works for the various zoom modes and for enlarging/reducing
// symbol size; removed buttons for Y-only zoom modes (not supported right
// now).
//
// Revision 1.2.2.3  2007/06/19 19:32:34  wenger
// Toolbar now works for help, home, cursor fill, and toggling visual
// filters; increased the spacing between the "sections" of icons.
//
// Revision 1.2.2.2  2007/06/18 19:57:20  wenger
// Toolbar works for drill-down (including Jmol); we go back to "normal"
// mode after drilling down; drill-down in Jmol is now disabled by
// default; removed Jmol menu options to enable/disable drill-down;
// removed unneeded utils stuff from 'make clean' target.
//
// Revision 1.2.2.1  2007/06/15 20:46:08  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.2  2006/05/26 16:22:15  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.2  2006/03/31 22:41:14  wenger
// Finished splitting up DEViseCanvas class.
//
// Revision 1.1.2.1  2006/03/30 20:51:25  wenger
// Partially done splitting up the DEViseCanvas class.
//

// ========================================================================

// DEViseCanvas2D.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.awt.event.*;

public class DEViseCanvas2D extends DEViseCanvas
{
    private static final int DEBUG = 0; // 0 - 3

    private static boolean allowAltModifier = false;
    private static boolean allowShiftModifier = false;

    //===================================================================
    // PUBLIC METHODS

    // v is base view if there is a pile in this canvas.
    public DEViseCanvas2D (DEViseView v, Image img)
    {
	super(v, img);
    }

    //===================================================================
    // PROTECTED METHODS

    protected void doMousePressed()
    {
        checkMousePos(sp, true);

	// Note:  some of the checks of the toolbar state here don't seem
	// to be totally necessary, but I'm still doing them, partly for
	// clarity and partly in case other things change and make them
	// necessary.  wenger 2007-08-03.

	if (activeView.isRubberBand) {
            // Control-drag is X-only zoom.
            if ((jsc.toolBar.doNormal() &&
	      jsc.jsValues.canvas.lastKey == KeyEvent.VK_CONTROL)
	      || jsc.toolBar.doZoomXOnly()) {
                _zoomMode = ZOOM_MODE_X;
            } else if (jsc.toolBar.doNormal() || jsc.toolBar.doZoomXY()) {
                _zoomMode = ZOOM_MODE_XY;
            } else {
                _zoomMode = ZOOM_MODE_NONE;
            }
	}

        if (isInViewDataArea && selectedCursor == null &&
          activeView.isRubberBand && jsc.toolBar.doRubberband()) {
            // We can only draw just the rubberband line if we're
            // drawing it in XOR mode.
            _paintRubberbandOnly = DRAW_XOR_RUBBER_BAND;
        }
    }

    protected void doMouseReleased(MouseEvent event)
    {
        if (isMouseDragged && isInViewDataArea) {
            Point p = event.getPoint();
            String cmd = "";

	    // Note:  some of the checks of the toolbar state here don't seem
	    // to be totally necessary, but I'm still doing them, partly for
	    // clarity and partly in case other things change and make them
	    // necessary.  wenger 2007-08-03.

            if (selectedCursor != null && jsc.toolBar.doCursorOps()) {
	        // move or resize cursor
                ep.x = activeView.translateX(p.x, 1);
                ep.y = activeView.translateY(p.y, 1);

                jsc.viewInfo.updateInfo(activeView.getX(ep.x),
                activeView.getY(ep.y));

                int dx = ep.x - sp.x, dy = ep.y - sp.y;
                DEViseCursor cursor = selectedCursor;

                cursor.updateCursorLoc(dx, dy, whichCursorSide, true);

                cursor.image = null;

                cmd = cmd + DEViseCommands.CURSOR_CHANGED + " {" +
                  cursor.name + "} " + cursor.x + " " + cursor.y +
		  " " + cursor.width + " " + cursor.height;

                jscreen.guiAction = true;
                dispatcher.start(cmd);

            } else if (activeView.isRubberBand && jsc.toolBar.doRubberband()) {
	        // rubber band (zoom)
		// Move point within data area if it's outside.
                ep.x = activeView.translateX(p.x, 1);
                ep.y = activeView.translateY(p.y, 1);

                jsc.viewInfo.updateInfo(activeView.getX(ep.x),
	          activeView.getY(ep.y));

                int w = ep.x - sp.x, h = ep.y - sp.y;
                if (w < 0) w = -w;
                if (h < 0) h = -h;

                if (w > jsc.jsValues.uiglobals.rubberBandLimit.width ||
		  h > jsc.jsValues.uiglobals.rubberBandLimit.height) {
		    doRubberbandZoom(sp.x, sp.y, ep.x, ep.y);
		    _zoomMode = ZOOM_MODE_NONE;
		    // jsc.toolBar.setNormal();
                }
            }

            isInViewDataArea = false;
            selectedCursor = null;
            whichCursorSide = DEViseCursor.sideNone;
            isMouseDragged = false;

            repaint();

	    // Workaround for bug 607.  Note that this means that things
	    // will *not* work correctly if you do two consecutive drags
	    // while holding down the control key the whole time.
	    // RKW 2000-08-07.
	    jsc.jsValues.canvas.lastKey = KeyEvent.VK_UNDEFINED;
        }
    }

    protected void doMouseClicked(MouseEvent event)
    {
        if (!isMouseDragged && isInViewDataArea) {
            String cmd = null;
            Point p = event.getPoint();

            if ( ( allowShiftModifier && jsc.toolBar.doNormal() &&
	      jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) ||
	      jsc.toolBar.doDrillDown()) {
                if (activeView.isDrillDown) {
                    cmd = DEViseCommands.SHOW_RECORDS + " " +
                      activeView.getCurlyName() + " " +
		      activeView.translateX(p.x, 2) + " " +
		      activeView.translateY(p.y, 2);
	        }

	    } else if (jsc.toolBar.doCursorFill()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " + DEVISE_KEY_CURSOR_FILL;
		// jsc.toolBar.setNormal();

//TEMPTOOLBAR -- need to get view help to work on Jmol and "plain" 3D views
	    } else if (jsc.toolBar.doViewHelp()) {
		// jsc.toolBar.setNormal();
		showHideHelp();

	    } else if (jsc.toolBar.doHome()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " + DEVISE_KEY_HOME;
		// jsc.toolBar.setNormal();

	    } else if (jsc.toolBar.doToggleFilter()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " + DEVISE_KEY_TOGGLE_FILTER;
		// jsc.toolBar.setNormal();

	    } else if (jsc.toolBar.doIncreaseSymSize()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " +
		  DEVISE_KEY_INCREASE_SYM_SIZE;
		// jsc.toolBar.setNormal();

	    } else if (jsc.toolBar.doDecreaseSymSize()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " +
		  DEVISE_KEY_DECREASE_SYM_SIZE;
		// jsc.toolBar.setNormal();

            } else if (activeView.isCursorMove && jsc.toolBar.doCursorOps()) {
                DEViseCursor cursor = activeView.getFirstCursor();

                if (cursor != null && (cursor.isXMovable ||
	          cursor.isYMovable)) {
                    Rectangle loc = cursor.cursorLocInCanvas;

                    int dx = p.x - loc.x - loc.width / 2;
                    int dy = p.y - loc.y - loc.height / 2;

                    cursor.updateCursorLoc(dx, dy, whichCursorSide,
	              true);

                    cmd = DEViseCommands.CURSOR_CHANGED + " {" +
		      cursor.name + "} " + cursor.x + " " + cursor.y + " "
		      + cursor.width + " " + cursor.height;

                    cursor.image = null;
                }
            } else if (activeView.isRubberBand && jsc.toolBar.doZoomMode()) {
		doClickZoom(p.x, p.y);
	    }

            if (cmd != null) {
                jscreen.guiAction = true;
                dispatcher.start(cmd);
            }

            isInViewDataArea = false;
            selectedCursor = null;
            whichCursorSide = DEViseCursor.sideNone;
            isMouseDragged = false;

            repaint();
        }
    }

    protected void doMouseDragged(Point p)
    {
        if (isInViewDataArea) {
            ep.x = activeView.translateX(p.x, 1);
            ep.y = activeView.translateY(p.y, 1);

            jsc.viewInfo.updateInfo(activeView.getX(ep.x),
              activeView.getY(ep.y));

            if (selectedCursor != null) {

                DEViseCursor cursor = selectedCursor;

                int dx = ep.x - sp.x, dy = ep.y - sp.y;

                cursor.updateCursorLoc(dx, dy, whichCursorSide, false);
            }

            jsc.jsValues.canvas.isInteractive = true;
            jsc.jsValues.canvas.sourceCanvas = this;
            repaint();
        }
    }

    protected void doMouseMoved(Point p)
    {
        checkMousePos(p, true);
    }

    //===================================================================
    // PROTECTED METHODS

    // Do a zoom resulting from a single mouse click (in one of the zoom-
    // only toolbar modes).
    private void doClickZoom(int x1, int y1)
    {
	if (DEBUG >= 1) {
	    System.out.println("doClickZoom(" + x1 + ", " + y1 + ")");
	}

	final double ZOOM_FACTOR = 0.5;
	int xMargin = (int)(activeView.viewLoc.width * ZOOM_FACTOR / 2);
	int yMargin = (int)(activeView.viewLoc.height * ZOOM_FACTOR / 2);

        doRubberbandZoom(x1 - xMargin, y1 - yMargin, x1 + xMargin,
	  y1 + yMargin);
    }

    // Do a zoom resulting from dragging out a rubberband line.
    private void doRubberbandZoom(int x1, int y1, int x2, int y2)
    {
	if (DEBUG >= 1) {
	    System.out.println("doRubberbandZoom(" + x1 + ", " + y1 +
	      ", " + x2 + ", " + y2 + ")");
	}

        try {
            String cmd = DEViseCommands.MOUSE_RUBBERBAND + " " +
	      activeView.getCurlyName() + " " +
	      activeView.translateX(x1, 2) + " " +
	      activeView.translateY(y1, 2) + " " +
	      activeView.translateX(x2, 2) + " " +
	      activeView.translateY(y2, 2);

            // Alt-drag zooms out.
            if ( (allowShiftModifier &&
	      jsc.jsValues.canvas.lastKey == KeyEvent.VK_ALT) ||
              (allowAltModifier &&
	      jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) ||
              jsc.toolBar.doZoomOut()) {
                cmd = cmd + " 1";
            } else if (jsc.toolBar.doZoomIn()) {
                cmd = cmd + " 0";
            } else {
                throw new YError("Illegal zoom mode -- neither in or out!");
            }

            if (_zoomMode == ZOOM_MODE_X) {
                cmd = cmd + " 1";
            } else if (_zoomMode == ZOOM_MODE_XY) {
                cmd = cmd + " 0";
            } else {
                throw new YError("Illegal zoom mode (" + _zoomMode + ")");
            }

            jscreen.guiAction = true;
            dispatcher.start(cmd);
    
        } catch (YError err) {
            System.err.println(err.getMessage());
        }
    }
}
