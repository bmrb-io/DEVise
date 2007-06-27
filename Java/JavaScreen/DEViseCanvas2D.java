// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2006
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

        // Control-drag is X-only zoom.
        if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_CONTROL ||
	  jsc.toolBar.doZoomX()) {
            _zoomMode = ZOOM_MODE_X;
        } else {
            _zoomMode = ZOOM_MODE_XY;
        }

        if (isInViewDataArea && selectedCursor == null &&
          activeView.isRubberBand) {
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

            if (selectedCursor != null) { // move or resize cursor
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
            } else if (activeView.isRubberBand) { // rubber band (zoom)
                ep.x = activeView.translateX(p.x, 1);
                ep.y = activeView.translateY(p.y, 1);

                jsc.viewInfo.updateInfo(activeView.getX(ep.x),
	          activeView.getY(ep.y));

                int w = ep.x - sp.x, h = ep.y - sp.y;
                if (w < 0)
                    w = -w;
                if (h < 0)
                    h = -h;

                if (w > jsc.jsValues.uiglobals.rubberBandLimit.width ||
		  h > jsc.jsValues.uiglobals.rubberBandLimit.height) {
		    try {
                        cmd = cmd + DEViseCommands.MOUSE_RUBBERBAND +
		          " " + activeView.getCurlyName() + " " +
			  activeView.translateX(sp.x, 2) + " " +
			  activeView.translateY(sp.y, 2) + " " +
			  activeView.translateX(ep.x, 2) + " " +
			  activeView.translateY(ep.y, 2);

			// Alt-drag zooms out.
                        if (jsc.jsValues.canvas.lastKey == 
			    KeyEvent.VK_ALT || 
			    jsc.jsValues.canvas.lastKey == 
			    KeyEvent.VK_SHIFT ||
			    jsc.toolBar.doZoomOut()) {
                            cmd = cmd + " 1";
                        } else {
                            cmd = cmd + " 0";
                        }

			if (_zoomMode == ZOOM_MODE_X) {
                            cmd = cmd + " 1";
                        } else if (_zoomMode == ZOOM_MODE_XY) {
                            cmd = cmd + " 0";
                        } else {
			    throw new YError("Illegal zoom mode (" +
			      _zoomMode + ")");
			}

                        jscreen.guiAction = true;
                        dispatcher.start(cmd);
                    } catch (YError err) {
		        System.err.println(err.getMessage());
		    }
		    _zoomMode = ZOOM_MODE_NONE;
		    jsc.toolBar.setNormal();
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

            if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT ||
	      jsc.toolBar.doDrillDown()) {
		//TEMPTOOLBAR -- should we turn off drill-down mode here??
                if (activeView.isDrillDown) {
                    cmd = DEViseCommands.SHOW_RECORDS + " " +
                      activeView.getCurlyName() + " " +
		      activeView.translateX(p.x, 2) + " " +
		      activeView.translateY(p.y, 2);
	        }

	    } else if (jsc.toolBar.doCursorFill()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " + DEVISE_KEY_CURSOR_FILL;
		jsc.toolBar.setNormal();

//TEMPTOOLBAR -- need to get view help to work on Jmol and "plain" 3D views
	    } else if (jsc.toolBar.doViewHelp()) {
		jsc.toolBar.setNormal();
		showHideHelp();

	    } else if (jsc.toolBar.doHome()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " + DEVISE_KEY_HOME;
		jsc.toolBar.setNormal();

	    } else if (jsc.toolBar.doToggleFilter()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " + DEVISE_KEY_TOGGLE_FILTER;
		jsc.toolBar.setNormal();

	    } else if (jsc.toolBar.doIncreaseSymSize()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " +
		  DEVISE_KEY_INCREASE_SYM_SIZE;
		jsc.toolBar.setNormal();

	    } else if (jsc.toolBar.doDecreaseSymSize()) {
		cmd = DEViseCommands.KEY_ACTION + " " +
		  activeView.getCurlyName() + " " +
		  DEVISE_KEY_DECREASE_SYM_SIZE;
		jsc.toolBar.setNormal();

            } else if (activeView.isCursorMove) {
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
}
