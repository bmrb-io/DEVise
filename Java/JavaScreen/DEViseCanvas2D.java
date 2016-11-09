// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
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

// ========================================================================

// DEViseCanvas2D.java

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
                    //drawDrillDownMark(p.x, p.y);
                    jsc.setDrillDownMarkerCoordinates(p.x, p.y, this);
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

            } else if (jsc.toolBar.doDataDownload()) {
                if (activeView.isDrillDown) {
                    cmd = DEViseCommands.GET_VIEW_DATA + " " +
                          activeView.getCurlyName() + " 1";
                }

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
            } else if (activeView.isRubberBand && jsc.toolBar.doZoomOnly()) {
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

    protected void doMouseWheelMoved(MouseWheelEvent e) {
        try {
            Point p = e.getPoint();
            if(p != null) {
                int x1 = p.x;
                int y1 = p.y;
                int direction  = e.getWheelRotation();
                if (DEBUG >= 1) {
                    System.out.println("doMouseWheelMoved " + x1 + "," + y1);
                }

                final double ZOOM_FACTOR = 0.5;
                int xMargin = (int)(activeView.viewLoc.width *  ZOOM_FACTOR / 2);
                int yMargin = (int)(activeView.viewLoc.height *  ZOOM_FACTOR / 2);

                String cmd = DEViseCommands.MOUSE_RUBBERBAND + " "
                             + activeView.getCurlyName() + " "
                             + activeView.translateX(x1 - xMargin, 2) + " "
                             + activeView.translateY(y1 - yMargin, 2) + " "
                             + activeView.translateX(x1 + xMargin, 2) + " "
                             + activeView.translateY(y1 + yMargin, 2);
                if (direction >= 0) {
                    cmd = cmd + " 0 0";
                } else {
                    cmd = cmd + " 1 0";
                }

                jscreen.guiAction = true;
                dispatcher.start(cmd);
            }
        } catch (YError err) {
            System.err.println(err.getMessage());
        }
    }
}
