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

//TEMP Look at the interfaces in org.jmol.api
//TEMP -- why is DEViseCanvas a Container instead of a Canvas???

// This class provides an interface to canvases in the JavaScreen.
// It puts images into canvases, and also does the drawing of cursors,
// GData, and so on.  It holds some information related to piles (pile
// information is divided between DEViseCanvas and DEViseView).  This
// class also handles events that occur in views.

// There is one instance of this class for each top-level view in
// the session (note that if top-level views are piled, there's only
// one DEViseCanvas for the entire pile).

// ------------------------------------------------------------------------

// ========================================================================

// DEViseCanvas.java

package JavaScreen;

import  java.awt.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;
import  javax.swing.*;

public class DEViseCanvas extends JComponent
{
    protected jsdevisec jsc = null;
    protected DEViseScreen jscreen = null;
    protected DEViseCmdDispatcher dispatcher = null;
    public int posZ = 0;

    public int oldHighlightAtomIndex = -1, highlightAtomIndex = -1;
    //TEMP -- what is point *for*?
    public Point point = new Point();
    public DEViseCrystal crystal = null;

    // The view associated with this canvas (base view if piled; parent
    // pile if there are view symbols).
    public DEViseView view = null;

    // ADD COMMENT -- we need clarification here as to why this is on a
    // per-canvas basis, since there can actually only be one active view
    // at a time in the entire JS; Hongyu says that this does not
    // necessarily get set to null when the active view becomes inactive.
    public DEViseView activeView = null;

    protected Dimension canvasDim = null;

    private Image image = null;
    private Image offScrImg = null;
    private boolean isImageUpdated = false;

    public boolean isMouseDragged = false, isInViewDataArea = false;
    protected DEViseCursor selectedCursor = null;

    // See DEViseCursor.side* for values.
    protected int whichCursorSide = DEViseCursor.sideNone;

    // Keep track of whether the mouse is actually in this canvas (part of
    // the fix for the "missing keystrokes" bugs.
    private boolean _mouseIsInCanvas = false;

    // These variables will hold the help messages to diplay when the 'Help' button is pressed
    // for both the main view and any child views.
    public String helpMsg = null;
    public Vector childViewHelpMsgs = new Vector();

    public boolean _hasJavaSym; // e.g., a button

    //TEMP -- what are these? op is old point???
    Point sp = new Point(), ep = new Point(), op = new Point();

    //
    // This is a fix to a problem that seems to be in the JVM itself:
    // when a mouse button press is closely followed in time by moving
    // the mouse, Java calls the mouseMoved() method of the
    // MouseMotionListener as opposed to the mouseDragged() method,
    // even though it has already called mousePressed() in the
    // MouseListener.  RKW 2000-05-24.
    //
    private boolean buttonIsDown = false;

    private static final int DEBUG = 0; // 0 - 3

    // Various zoom modes.  Note that _zoomMode is only valid while the
    // mouse button is down or has just been released.
    protected static final int ZOOM_MODE_NONE = 0;
    protected static final int ZOOM_MODE_X = 1;
    protected static final int ZOOM_MODE_Y = 2;
    protected static final int ZOOM_MODE_XY = 3;
    protected static int _zoomMode = ZOOM_MODE_NONE;

    // ASCII codes for keys that we have to send to DEVise (we have to
    // send the actual ASCII integer value as opposed to the character).
    protected static int DEVISE_KEY_TOGGLE_FILTER = 8; // (backspace)
    protected static int DEVISE_KEY_INCREASE_SYM_SIZE = 43; // '+'
    protected static int DEVISE_KEY_DECREASE_SYM_SIZE = 45; // '-'
    protected static int DEVISE_KEY_HOME = 53; // '5'
    protected static int DEVISE_KEY_CURSOR_FILL = 67; // 'C'

    // This is used to paint only rubberband lines (in XOR mode) while
    // we are dragging the rubberband line.  (In other words, we don't
    // actually repaint all of the other stuff on the canvas.)
    protected boolean _paintRubberbandOnly = false;

    // Set this to true to draw rubberband lines in XOR mode (so we can
    // just redraw the rubberband line itself on each mouse drag).
    protected static final boolean DRAW_XOR_RUBBER_BAND = true;

    // Whether to draw a drill-down marker at the point where the mouse
    // was clicked.
    protected static final boolean DRAW_DRILL_DOWN_MARKER = true;

    // Whether a drill-down marker should currently be drawn in this
    // canvas.
    private boolean drawDD = false;

    // The location of the drill-down marker (only valid if drawDD is true).
    private int ddXCoord, ddYCoord;

    // v is base view if there is a pile in this canvas.
    public DEViseCanvas(DEViseView v, Image img)
    {
        view = v;
        jsc = view.jsc;
        jscreen = jsc.jscreen;
        dispatcher = jsc.dispatcher;
        image = img;

        if (image != null) {
            canvasDim = new Dimension(image.getWidth(this), image.getHeight(this));
            view.viewLoc.width = canvasDim.width;
            view.viewLoc.height = canvasDim.height;
            view.viewLocInCanvas.width = canvasDim.width;
            view.viewLocInCanvas.height = canvasDim.height;
            isImageUpdated = true;
        }
        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        this.enableEvents(AWTEvent.MOUSE_MOTION_EVENT_MASK);
        this.enableEvents(AWTEvent.KEY_EVENT_MASK);
        addMouseListener(new ViewMouseListener());
        addMouseMotionListener(new ViewMouseMotionListener());
        addKeyListener(new ViewKeyListener());
        addFocusListener(new ViewFocusListener());
        addMouseWheelListener(new ViewMouseWheelListener());
    }

    // Called when a session is closed.
    public void close() {}

    public Dimension getPreferredSize()
    {
        return canvasDim;
    }

    public Dimension getMinimumSize()
    {
        return canvasDim;
    }

    public Rectangle getLocInScreen()
    {
        return view.viewLoc;
    }

    public boolean mouseIsWithin()
    {
        return _mouseIsInCanvas;
    }

    // Turn on a drill-down marker at the given location.
    public void drawDrillDownMark(int xCoord, int yCoord)
    {
        jsc.drillDownCanvas = this;
        if (DRAW_DRILL_DOWN_MARKER) {
            ddXCoord = xCoord;
            ddYCoord = yCoord;
            drawDD = true;
            repaint();
        }
    }

    // Turn off the drill-down marker.
    public void eraseDrillDownMark()
    {
        if (DRAW_DRILL_DOWN_MARKER) {
            drawDD = false;
            repaint();
        }
    }

    // This function is used to draw cursors because direct XOR drawing
    // (with Graphics.setXORMode()) doesn't seem to work in Netscape.
    // loc is relative to this canvas.
    public Image getCroppedXORImage(Color c, Rectangle loc)
    {
        if (image == null)
            return null;

        CropImageFilter croppedFilter = new CropImageFilter(loc.x, loc.y,
                loc.width, loc.height);
        FilteredImageSource source = new FilteredImageSource(
            image.getSource(), croppedFilter);
        Image img = createImage(source);
        if (img == null) {
            return null;
        }

        source = new FilteredImageSource(img.getSource(),
                                         new XORFilter(c, loc.width, loc.height, XORFilter.typeFrame));
        img = createImage(source);

        return img;
    }

    public synchronized void updateImage(Image img)
    {
        if (img != null) {
            image = img;
            canvasDim.width = image.getWidth(this);
            canvasDim.height = image.getHeight(this);
            view.viewLoc.width = canvasDim.width;
            view.viewLoc.height = canvasDim.height;
            view.viewLocInCanvas.width = canvasDim.width;
            view.viewLocInCanvas.height = canvasDim.height;

            isImageUpdated = true;
        }

        // Necessary to draw correct image because double-buffering is used
        //offScrImg = null;
    }

    // Enable double-buffering
    public void update(Graphics gc)
    {
        if (gc == null) {
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(canvasDim.width, canvasDim.height);
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

    // enable double-buffering w/o calling update
    public void paint(Graphics gc)
    {
        if (gc == null) {
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(canvasDim.width, canvasDim.height);
        }

        Graphics og = offScrImg.getGraphics();
        paintCanvas(og);
        gc.drawImage(offScrImg, 0, 0, this);
        og.dispose();

        // Needed for Jmol.
        super.paint(gc);
    }

    public void paintCanvas(Graphics gc)
    {
        // for command log playback
        if (jsc.isPlayback && !jsc.isDisplay) {
            return;
        }

        if (jsc.jsValues.canvas.isInteractive) {
            if (jsc.jsValues.canvas.sourceCanvas != this) {
                return;
            }
        }

//TEMP -- check whether help works in Jmol views -- no, it doesn't
        if (!_paintRubberbandOnly) {
            // draw 3D molecular view
            if (paintCrystal(gc)) {
                paintHighlight(gc);
                paintHelpMsg(gc);
                return;
            }

            // draw the background image
            paintBackground(gc);

            // draw titles for all views, including view symbols but not
            // piled views
            paintTitle(gc);

            // draw all the cursors
            paintCursor(gc);

            // draw all the GDatas
            paintGData(gc, view);

            // draw the drill-down mark, if any
            paintDrillDownMark(gc);
        }

        // draw rubber band
        paintRubberBand(gc);

        if (!_paintRubberbandOnly) {
            // draw highlight border
            paintHighlight(gc);

            // draw sxis labels
            paintAxisLabel(gc);

            paintHelpMsg(gc);
        }
    }

    protected synchronized void paintBackground(Graphics gc)
    {
        if (image != null) {
            gc.drawImage(image, 0, 0, canvasDim.width, canvasDim.height, this);
        } else {
            gc.setColor(new Color(view.viewBg));
            gc.fillRect(0, 0, canvasDim.width, canvasDim.height);
        }
    }

    private void paintHelpMsg(Graphics gc)
    {
        if (helpMsg == null) {
            return;
        }

        int helpMsgX = 0, helpMsgY = 0;
        String tempHelpMsg = "";
        int minX = 0, minY = 0;

        for(int j = 0; ; j++) {
            if(j == 0) {
                tempHelpMsg = helpMsg;
            }
            else {
                if(childViewHelpMsgs.size() >= j) {
                    tempHelpMsg = (String) childViewHelpMsgs.elementAt(j - 1);
                    if(tempHelpMsg == null) continue;
                    if(view.viewChilds.size() >= j) {
                        helpMsgX = ((DEViseView) view.viewChilds.elementAt(j - 1)).viewLoc.x;
                        helpMsgY = ((DEViseView) view.viewChilds.elementAt(j - 1)).viewLoc.y;
                        // This if will keep help msgs from overlapping
                        if(helpMsgY < minY && helpMsgX < minX) {
                            helpMsgY = minY;
                        }
                    }
                    else {
                        break;
                    }
                }
                else {
                    break;
                }
            }

            Toolkit tk = Toolkit.getDefaultToolkit();
            FontMetrics fm = tk.getFontMetrics(jsc.jsValues.uiglobals.textFont);
            int ac = fm.getAscent(), dc = fm.getDescent(), height = ac + dc + 12;
            int width = fm.stringWidth(tempHelpMsg) + 12;
            int minHeight = height + height/3;
            int minWidth = fm.stringWidth("THIS IS THE MINIMUM");

            if(canvasDim.width < minWidth || canvasDim.height < minHeight) {
                return;
            }

            StringTokenizer st = new StringTokenizer(tempHelpMsg, "\n");
            Vector sv = new Vector();
            String back = "";

            while(st.hasMoreTokens()|| !back.equals("")) {
                String tmp = back + " ";
                if(st.hasMoreTokens()) {
                    tmp  +=  st.nextToken();
                }

                int tokenWidth = fm.stringWidth(tmp) + 12;
                if(tokenWidth > canvasDim.width) {
                    String[] a = stringSplit(tmp, fm);
                    tmp = a[0];
                    back = a[1];
                    sv.addElement(tmp);

                }
                else {
                    sv.addElement(tmp);
                    back = "";
                }
            }

            if( back.equals(""));
            else {
                sv.addElement(back);
            }

            int totHeight = height * sv.size() ;
            if( sv.size () > 3 ) {
                totHeight += sv.size() * (-5);
            }

            int allowedHeight = 4* canvasDim.height/5;
            if(totHeight> allowedHeight ) {
                // may want to put some message;
                return;
            }


            //      int dispLength = canvasDim.width;
            //      computing display length

            int dispLength = 0;
            for(int i = 0; i < sv.size(); i++) {
                String s = (String) sv.elementAt(i);
                int w = fm.stringWidth(s);
                if(dispLength < w ) {
                    dispLength = w;
                }
            }
            dispLength += 12;
            if( dispLength < canvasDim.width) {
                int k = canvasDim.width - dispLength;
                if( k  > 6) {
                    dispLength +=6;
                }
                else {
                    dispLength += k;
                }
            }

            // setting up the color background and text color.
            gc.setColor(Color.black);
            gc.drawRect(helpMsgX, helpMsgY, dispLength -1, totHeight);
            // If this is the main help msg, make sure no other msg is displayed over this one
            if(j == 0) {
                minX = dispLength - 1 + 1;
                minY = totHeight + 1;
            }
            gc.setColor(new Color(255, 255, 192));
            gc.fillRect(helpMsgX + 1, helpMsgY + 1,dispLength - 2, totHeight- 1);
            gc.setColor(Color.black);
            gc.setFont(jsc.jsValues.uiglobals.textFont);

            // printing each of the line from the Vector of lines.
            for( int i = 0 ; i < sv.size(); i++) {
                gc.drawString(((String) sv.elementAt(i)), helpMsgX + 6, helpMsgY + height - dc - 6);
                helpMsgY += (height - dc -6);
            }
        }
    }

    private String[]  stringSplit(String a, FontMetrics fm) {
        StringTokenizer st = new StringTokenizer(a, " ");
        int defWidth = 0;
        String tmp ="";
        String[] res = new String[2];
        res[0] = "";
        res[1] = "";
        while( st.hasMoreTokens()) {
            tmp = st.nextToken();
            defWidth = fm.stringWidth(res[0]) + fm.stringWidth(tmp + " ") ;
            if((defWidth + 5) > canvasDim.width) {
                break;
            }
            else {
                res[0] = res[0] + " " + tmp;
            }
        }
        res[1] = tmp + " " ;
        while( st.hasMoreTokens()) {
            res[1] += (st.nextToken() + " ");
        }
        // System.out.println( a + " split as " + res[0] + " and " + res[1]);

        return res;
    }

    protected synchronized boolean paintCrystal(Graphics gc)
    {
        return false;
    }

    protected void resetCrystal()
    {
    }

    protected void doMousePressed()
    {
    }

    protected void doMouseReleased(MouseEvent event)
    {
    }

    protected void doMouseClicked(MouseEvent event)
    {
    }

    protected void doMouseDragged(Point p)
    {
    }

    protected void doMouseMoved(Point p)
    {
    }

    protected void doMouseWheelMoved(MouseWheelEvent e)
    {
    }

    private synchronized void paintHighlight(Graphics gc)
    {
        // Border for Active view is disabled. - no highlighting
        /* if (activeView != null && activeView == jscreen.getCurrentView()) {
                Rectangle loc = activeView.viewLocInCanvas;

                gc.setColor(Color.red);
                gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
                gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3, loc.height - 3);
        }
        */
    }

    private synchronized void paintAxisLabel(Graphics gc)
    {
        Rectangle loc;

        // handling child view
        for (int i = 0; i < view.viewChilds.size(); i++) {
            DEViseView childView = (DEViseView)view.viewChilds.elementAt(i);
            childView.paintAxisLabels(gc, true);
        }

        view.paintAxisLabels(gc, false);
    }


    // only top views get title drawn
    // TEMP -- child views of piled views should probably have titles drawn
    private synchronized void paintTitle(Graphics gc)
    {
        //TEMP -- should probably reverse the order here

        // handling child view
        for (int i = 0; i < view.viewChilds.size(); i++) {
            DEViseView v = (DEViseView)view.viewChilds.elementAt(i);

            if (v.viewDTFont != null && v.viewTitle != null) {
                gc.setColor(new Color(v.viewFg));
                gc.setFont(v.viewDTFont);

                // Fit title into available width of view.
                String title = fitStringToWidth(v.viewTitle,
                                                v.viewLoc.width, gc.getFontMetrics());

                // Re-center (possibly shortened) title.
                int titleX = (v.viewLoc.width -
                              gc.getFontMetrics().stringWidth(title)) / 2 +
                             v.viewLocInCanvas.x;
                gc.drawString(title, titleX, v.viewDTY);
            }
        }

        // handling itself
        if (view.viewDTFont != null && view.viewTitle != null) {
            gc.setColor(new Color(view.viewFg));
            gc.setFont(view.viewDTFont);

            // Fit title into available width of view.
            String title = fitStringToWidth(view.viewTitle,
                                            view.viewLoc.width, gc.getFontMetrics());

            // Re-center (possibly shortened) title.
            int titleX = (view.viewLoc.width -
                          gc.getFontMetrics().stringWidth(title)) / 2 +
                         view.viewLocInCanvas.x;
            gc.drawString(title, titleX, view.viewDTY);
        }
    }

    // every view will have its cursor properly drawn
    private synchronized void paintCursor(Graphics gc)
    {
        paintCursor(gc, view);

        isImageUpdated = false;
    }

    private synchronized void paintCursor(Graphics gc, DEViseView v)
    {
        //TEMP -- should probably reverse the order here

        // handling child views
        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);

            paintCursor(gc, vv);
        }

        // handling itself
        Rectangle loc = null;
        DEViseCursor cursor = null;
        for (int i = 0; i < v.viewCursors.size(); i++) {
            cursor = (DEViseCursor)v.viewCursors.elementAt(i);
            if (cursor.cursorIsVisible()) {
                loc = cursor.getLocInCanvas();

                if (selectedCursor == cursor && isMouseDragged) {
                    // draw cursor movement box

                    if (loc.width < 4) loc.width = 4;
                    if (loc.height < 4) loc.height = 4;

                    gc.setColor(Color.yellow);
                    gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
                    gc.setColor(Color.red);
                    gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3,
                                loc.height - 3);

                    // Hmm -- I guess this is here because we can only
                    // drag one cursor at a time.  wenger 2008-07-15
                    continue;
                }

                if (cursor.image == null || isImageUpdated) {
                    cursor.image = getCroppedXORImage(cursor.color, loc);
                }

                if (cursor.image != null) {
                    gc.drawImage(cursor.image, loc.x, loc.y, this);
                }
            }
        }

        // handling piled views
        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            paintCursor(gc, vv);
        }
    }

    private synchronized void paintGData(Graphics gc, DEViseView v)
    {
        //TEMP -- should probably reverse the order here

        // handling child views
        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);

            paintGData(gc, vv);
        }

        // handling itself
        DEViseGData gdata = null;
        Color tmpColor = null;
        Font tmpFont = null;
        for (int i = 0; i < v.viewGDatas.size(); i++) {
            gdata = (DEViseGData)v.viewGDatas.elementAt(i);

            if (gdata.string != null && (gdata.symbolType == gdata._symText ||
                                         gdata.symbolType == gdata._symFixedText)) {

                // Set the color and font only when necessary.
                if (!gdata.color.equals(tmpColor)) {
                    gc.setColor(gdata.color);
                    tmpColor = gdata.color;
                }
                if (!gdata.font.equals(tmpFont)) {
                    gc.setFont(gdata.font);
                    tmpFont = gdata.font;
                }

                gc.drawString(gdata.string, gdata.x, gdata.y);
            }
        }

        // handling piled views
        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            paintGData(gc, vv);
        }
    }

    static boolean _lastRBValid = false;
    static Rectangle _lastRB = new Rectangle();

    private synchronized void paintRubberBand(Graphics gc)
    {
        if (isInViewDataArea && isMouseDragged && selectedCursor == null &&
                activeView.isRubberBand && jsc.toolBar.doRubberband()) {
            int x0, y0, w, h;

            if (sp.x > ep.x)  {
                x0 = ep.x;
                w = sp.x - x0;
            }  else  {
                x0 = sp.x;
                w = ep.x - x0;
            }

            // If not zooming in the X direction, the zoom box should
            // fill the whole data area horizontally.
            if (_zoomMode != ZOOM_MODE_X && _zoomMode != ZOOM_MODE_XY) {
                x0 = activeView.viewDataLoc.x + activeView.viewLocInCanvas.x;
                w = activeView.viewDataLoc.width;
            }

            if (sp.y > ep.y)  {
                y0 = ep.y;
                h = sp.y - y0;
            }  else  {
                y0 = sp.y;
                h = ep.y - y0;
            }

            // If not zooming in the Y direction, the zoom box should
            // fill the whole data area vertically.
            if (_zoomMode != ZOOM_MODE_Y && _zoomMode != ZOOM_MODE_XY) {
                y0 = activeView.viewDataLoc.y + activeView.viewLocInCanvas.y;
                h = activeView.viewDataLoc.height;
            }

            if (w < 4) {
                w = 4;
            }
            if (h < 4) {
                h = 4;
            }

            // Outer rectangle.
            if (DRAW_XOR_RUBBER_BAND) {
                gc.setXORMode(Color.red);
                if (_lastRBValid) {
                    // Undraw the last rubberband line.
                    gc.drawRect(_lastRB.x, _lastRB.y, _lastRB.width,
                                _lastRB.height);
                }
            } else {
                gc.setColor(Color.red);
            }
            gc.drawRect(x0, y0, w, h);

            // Inner rectangle.
            if (DRAW_XOR_RUBBER_BAND) {
                gc.setXORMode(Color.yellow);
                gc.setXORMode(Color.black); // make this one not show up
                if (_lastRBValid) {
                    // Undraw the last rubberband line.
                    gc.drawRect(_lastRB.x + 1, _lastRB.y + 1,
                                _lastRB.width - 2, _lastRB.height - 2);
                }
            } else {
                gc.setColor(Color.yellow);
            }
            gc.drawRect(x0 + 1, y0 + 1, w - 2, h - 2);

            if (DRAW_XOR_RUBBER_BAND) {
                gc.setPaintMode();

                _lastRB.x = x0;
                _lastRB.y = y0;
                _lastRB.width = w;
                _lastRB.height = h;
                _lastRBValid = true;
            }
        }
    }

    // Paint the drill-down mark (if any).
    private synchronized void paintDrillDownMark(Graphics gc)
    {
        if (drawDD) {
            boolean DRAW_XOR = false;
            int SIZE = 7;

            if (DRAW_XOR) {
                gc.setXORMode(Color.white);
            } else {
                gc.setColor(Color.white);
            }
            gc.drawLine(ddXCoord-1, ddYCoord-SIZE, ddXCoord-1, ddYCoord+SIZE);
            gc.drawLine(ddXCoord+1, ddYCoord-SIZE, ddXCoord+1, ddYCoord+SIZE);
            gc.drawLine(ddXCoord-SIZE, ddYCoord-1, ddXCoord+SIZE, ddYCoord-1);
            gc.drawLine(ddXCoord-SIZE, ddYCoord+1, ddXCoord+SIZE, ddYCoord+1);

            if (DRAW_XOR) {
                gc.setXORMode(Color.red);
            } else {
                gc.setColor(Color.red);
            }
            gc.drawLine(ddXCoord, ddYCoord-SIZE, ddXCoord, ddYCoord+SIZE);
            gc.drawLine(ddXCoord-SIZE, ddYCoord, ddXCoord+SIZE, ddYCoord);

            if (DRAW_XOR) {
                gc.setPaintMode();
            }
        }
    }

    protected void processMouseEvent(MouseEvent event)
    {
        int id = event.getID();

        if (DEBUG >= 1) {
            System.out.println("DEViseCanvas(" + view.viewName +
                               ").processMouseEvent()");
            if (id == MouseEvent.MOUSE_PRESSED) System.out.println(
                    "  Mouse pressed");
            if (id == MouseEvent.MOUSE_CLICKED) System.out.println(
                    "  Mouse clicked");
            if (id == MouseEvent.MOUSE_RELEASED) System.out.println(
                    "  Mouse released");
        }

        jsc.jscreen.finalMousePosition.x = view.viewLoc.x + event.getX();
        jsc.jscreen.finalMousePosition.y = view.viewLoc.y + event.getY();
        //jsc.pn("DEViseCanvas.processMouseEvent()");
        if (id == MouseEvent.MOUSE_PRESSED || id == MouseEvent.MOUSE_RELEASED
                || id == MouseEvent.MOUSE_CLICKED) {
            if (dispatcher.getStatus() ==
                    DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                if (id == MouseEvent.MOUSE_PRESSED) {
                    jsc.showMsg("JavaScreen still talking to the Server!\nPlease try again later or press STOP button!");
                }

                return;
            }
        }

        super.processMouseEvent(event);
    }

    protected void processMouseMotionEvent(MouseEvent event)
    {
        if (DEBUG >= 2) {
            System.out.println("DEViseCanvas(" + view.viewName +
                               ").processMouseMotionEvent()");
        }

        int id = event.getID();

        jsc.jscreen.finalMousePosition.x = view.viewLoc.x + event.getX();
        jsc.jscreen.finalMousePosition.y = view.viewLoc.y + event.getY();

        if (id == MouseEvent.MOUSE_DRAGGED) {
            if (dispatcher.getStatus() ==
                    DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                return;
            }
        }

        super.processMouseMotionEvent(event);
    }

    //TEMP -- move to DEViseCanvas2D?
    protected void processKeyEvent(KeyEvent event)
    {
        if (view.viewDimension != 3) {
            if (dispatcher.getStatus() ==
                    DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                //if (event.getID() == KeyEvent.KEY_PRESSED) {
                //    jsc.showMsg("Java Screen still talking to the Server!\nPlease try again later or press STOP button!");
                //}

                return;
            }
        }

        super.processKeyEvent(event);
    }

    // This function will do one of two things.  If the help button has
    // been pushed, and a tan box is showing the help in this view, this
    // function will hide that help box.  If there is not currently a
    // help box displayed, this function will show the view's help in a
    // dialog box.
    protected void showHideHelp()
    {
        // Check to see if a help box is being displayed.
        if(helpMsg != null) {
            // Hide the help for this view and any of its children
            helpMsg = null;

            for(int i = 0; i < childViewHelpMsgs.size(); i++) {
                childViewHelpMsgs.setElementAt(null, i);
            }
            repaint();
            return;
        }

        //
        // Show this view's help within a dialog box.
        //
        jsc.jsValues.connection.helpBox = true;
        String cmd = DEViseCommands.GET_VIEW_HELP + " " +
                     activeView.getCurlyName() + " " + 0 + " " + 0;
        // jscreen.guiAction = true;
        dispatcher.start(cmd);
    }

    // start of class ViewKeyListener
    class ViewKeyListener extends KeyAdapter
    {
        // event sequence: 1. keypressed 2.keytyped 3.keyreleased
        public void keyPressed(KeyEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseCanvas(" + view.viewName +
                                   ").keyPressed(" + event.getKeyChar() + ")");
            }
            jsc.jsValues.canvas.lastKey = event.getKeyCode();
        }

        //
        // Note: should we move most of this to using a keyTyped handler
        // instead of a keyReleased handler?  That is supposed to be
        // higer-level and more hardware-independent.  RKW 2002-04-09.
        //

        // If this is not a 3D-view, send the appropriate key action
        // to the devised.
        public void keyReleased(KeyEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("DEViseCanvas(" + view.viewName +
                                   ").keyReleased(" + event.getKeyChar() + ", " +
                                   event.getKeyCode() + ")");
            }
            jsc.jsValues.canvas.lastKey = KeyEvent.VK_UNDEFINED;

            char keyChar = event.getKeyChar();
            int keyCode = event.getKeyCode();

            // F1 key shows or hides view help.
            if (keyCode == KeyEvent.VK_F1) {
                showHideHelp();
                return;
            }

            if (view.viewDimension == 3) {
                if (keyChar == 'r' || keyChar == 'R') {
                    resetCrystal();
                }
                return;
            }

            int actualKey = translateKey(keyChar, keyCode);

            if (actualKey != 0 && activeView != null) {
                if (activeView.isKey || isSpecialKey(actualKey)) {
                    String cmd = "";
                    cmd = cmd + DEViseCommands.KEY_ACTION + " " +
                          activeView.getCurlyName() + " " + actualKey;
                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                } else {
                    // For some reason, after we show the dialog we get
                    // an extra key event from the DEViseScreen.
                    // RKW 2002-04-09.
                    if (event.getComponent() instanceof DEViseCanvas) {
                        jsc.showMsg("Key actions are disabled in this view");
                    }
                }
            }
        }

        private int translateKey(char keyChar, int keyCode)
        {
            if (DEBUG >= 2) {
                System.out.println(
                    "DEViseCanvas.ViewKeyListener.translateKey(" + keyChar +
                    ", " + keyCode + ")");
            }

            int actualKey;

            // Note: check against 0xff here is to (hopefully) fix the
            // first part of bug 765.  RKW 2002-04-09.
            if (keyChar != KeyEvent.CHAR_UNDEFINED && keyChar <= 0xff) {
                if (DEBUG >= 2) {
                    System.out.println("key char is defined");
                }

                actualKey = (int)keyChar;

            } else {
                if (DEBUG >= 2) {
                    System.out.println("key char is NOT defined");
                }

                // TEMP -- document where these constants come from
                switch (keyCode) {
                case KeyEvent.VK_NUMPAD0:
                    actualKey = 0x40000 + 28;
                    break;
                case KeyEvent.VK_NUMPAD1:
                    actualKey = 0x40000 + 29;
                    break;
                case KeyEvent.VK_NUMPAD2:
                    actualKey = 0x40000 + 30;
                    break;
                case KeyEvent.VK_NUMPAD3:
                    actualKey = 0x40000 + 31;
                    break;
                case KeyEvent.VK_NUMPAD4:
                    actualKey = 0x40000 + 32;
                    break;
                case KeyEvent.VK_NUMPAD5:
                    actualKey = 0x40000 + 33;
                    break;
                case KeyEvent.VK_NUMPAD6:
                    actualKey = 0x40000 + 34;
                    break;
                case KeyEvent.VK_NUMPAD7:
                    actualKey = 0x40000 + 35;
                    break;
                case KeyEvent.VK_NUMPAD8:
                    actualKey = 0x40000 + 36;
                    break;
                case KeyEvent.VK_NUMPAD9:
                    actualKey = 0x40000 + 37;
                    break;
                //case KeyEvent.VK_F1:
                //   actualKey = 0x50000 + 1;
                //   break;
                //case KeyEvent.VK_F2:
                //   actualKey = 0x50000 + 2;
                //  break;
                case KeyEvent.VK_F3:
                    actualKey = 0x50000 + 3;
                    break;
                case KeyEvent.VK_F4:
                    actualKey = 0x50000 + 4;
                    break;
                case KeyEvent.VK_F5:
                    actualKey = 0x50000 + 5;
                    break;
                case KeyEvent.VK_F6:
                    actualKey = 0x50000 + 6;
                    break;
                case KeyEvent.VK_F7:
                    actualKey = 0x50000 + 7;
                    break;
                case KeyEvent.VK_F8:
                    actualKey = 0x50000 + 8;
                    break;
                case KeyEvent.VK_F9:
                    actualKey = 0x50000 + 9;
                    break;
                case KeyEvent.VK_F10:
                    actualKey = 0x50000 + 10;
                    break;
                case KeyEvent.VK_F11:
                    actualKey = 0x50000 + 11;
                    break;
                case KeyEvent.VK_F12:
                    actualKey = 0x50000 + 12;
                    break;
                case KeyEvent.VK_HOME:
                    actualKey = 0x20000 + 1;
                    break;
                case KeyEvent.VK_LEFT:
                    actualKey = 0x20000 + 2;
                    break;
                case KeyEvent.VK_UP:
                    actualKey = 0x20000 + 3;
                    break;
                case KeyEvent.VK_RIGHT:
                    actualKey = 0x20000 + 4;
                    break;
                case KeyEvent.VK_DOWN:
                    actualKey = 0x20000 + 5;
                    break;
                case KeyEvent.VK_PAGE_UP:
                    actualKey = 0x20000 + 7;
                    break;
                case KeyEvent.VK_PAGE_DOWN:
                    actualKey = 0x20000 + 9;
                    break;
                case KeyEvent.VK_END:
                    actualKey = 0x20000 + 10;
                    break;
                case KeyEvent.VK_BACK_SPACE:
                    actualKey = 8;
                    break;
                default:
                    actualKey = 0;
                    break;
                }
            }

            if (keyCode == KeyEvent.VK_ESCAPE) {
                jsc.toolBar.setNormal();
                actualKey = 0; // don't pass this to the server
            }

            if (DEBUG >= 2) {
                System.out.println("actualKey = " + actualKey);
            }

            return actualKey;
        }

        // Determine whether the key is a "special" key that should be
        // allowed even if other key actions are disabled.  (See
        // ViewGraph::HandleKey().)
        private boolean isSpecialKey(int actualKey)
        {
            boolean result = false;

            switch (actualKey) {
            case 'c':
            case 'C':
            case '\b':
                result = true;
                break;
            }

            return result;
        }
    }
    // end of class ViewKeyListener

    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter
    {
        // event sequence: 1. mousePressed 2. mouseReleased 3. mouseClicked
        public void mousePressed(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println(
                    "DEViseCanvas(" + view.viewName +
                    ").ViewMouseListener.mousePressed()");
            }

            // Bug fix -- see notes at variable declaration.
            buttonIsDown = true;

            // The starting point will be in this view, otherwise this event
            // will not be caught.

            // Each mouse click will be here once, so double click actually
            // will enter this twice.

            // The position of this event will be relative to this view, and
            // will not exceed the range of this view, ie, p.x >= 0 &&
            // p.x < view.width ...

            Point p = event.getPoint();
            ep.x = op.x = sp.x = p.x;
            ep.y = op.y = sp.y = p.y;

            doMousePressed();
        }

        public void mouseReleased(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println(
                    "DEViseCanvas(" + view.viewName +
                    ").ViewMouseListener.mouseReleased()");
            }

            // Bug fix -- see notes at variable declaration.
            buttonIsDown = false;

            _lastRBValid = false;

            _paintRubberbandOnly = false;

            // Each mouse click will be here once, so double click actually
            // will enter this twice. Also, this event will always reported
            // with each mouse click and before the mouseClick event is
            // reported.

            jsc.jsValues.canvas.isInteractive = false;

            doMouseReleased(event);
        }

        public void mouseClicked(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println(
                    "DEViseCanvas(" + view.viewName +
                    ").ViewMouseListener.mouseClicked()");
            }

            doMouseClicked(event);
        }

        public void mouseEntered(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("Mouse entered view " + view.curlyName);
            }
            _mouseIsInCanvas = true;
        }

        public void mouseExited(MouseEvent event)
        {
            if (DEBUG >= 1) {
                System.out.println("Mouse exited view " + view.curlyName);
            }
            _mouseIsInCanvas = false;
        }

    }
    // end of class ViewMouseListener

    // start of class ViewMouseWheelListener
    class ViewMouseWheelListener implements MouseWheelListener
    {
        private long time = Calendar.getInstance().getTimeInMillis();
        @Override
        public void mouseWheelMoved(MouseWheelEvent e) {
            if (DEBUG >= 1) {
                System.out.println("Mouse wheelmoved in ViewMouseListener.mouseWheelMoved()");
            }
            long eventTime = e.getWhen();
            if(eventTime - time > 1000) {
                doMouseWheelMoved(e);
                time = eventTime;
            }
        }
    }
// end of class ViewMouseWheelListener


    // start of class ViewMouseMotionListener
    class ViewMouseMotionListener extends MouseMotionAdapter
    {
        // When you drag a mouse, all the following mouse positions are
        // relative to the view where you first start the dragging (so we
        // might have value that is less than zero), and the dragging event
        // is still send back to the view where you first start dragging.

        public void mouseDragged(MouseEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("DEViseCanvas(" + view.viewName +
                                   ").ViewMouseMotionListener.mouseDragged()");
            }

            Point p = event.getPoint();
            isMouseDragged = true;

            doMouseDragged(p);
        }

        public void mouseMoved(MouseEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("DEViseCanvas(" + view.viewName +
                                   ").ViewMouseMotionListener.mouseMoved()");
            }

            // Bug fix -- see notes at variable declaration.
            if (buttonIsDown) {
                mouseDragged(event);
                return;
            }

            isMouseDragged = false;

            // the position of this event will be relative to this view
            // and will not exceed the range of this view, ie, p.x >= 0 &&
            // p.x < view.width, p.y >= 0 && p.y < view.height

            Point p = event.getPoint();

            doMouseMoved(p);
        }
    }
    // end of class ViewMouseMotionListener

    //
    // Note: we don't absolutely need this class; it's here just to help
    // debug the problem with input focus sometimes improperly getting
    // taken away from a DEViseCanvas.
    //
    class ViewFocusListener extends FocusAdapter
    {
        public void focusGained(FocusEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("View " + view.curlyName + " gained focus");
            }
        }

        public void focusLost(FocusEvent event)
        {
            if (DEBUG >= 2) {
                System.out.println("View " + view.curlyName + " lost focus");
                System.out.println("_mouseIsInCanvas = " + _mouseIsInCanvas);
            }

            if (_mouseIsInCanvas) {
                if (DEBUG >= 1) {
                    System.err.println("WARNING: view " + view.viewName +
                                       " lost focus while mouse is in it!");
                }
            }
        }
    } // end of class ViewFocusListener

    // Update the shape of the mouse cursor based on whether the mouse
    // is on a DEVise cursor, etc.
    public synchronized void checkMousePos(Point mouseLoc,
                                           boolean checkDispatcher) throws YError
    {
        if (DEBUG >= 3) {
            System.out.println("DEViseCanvas(" + view.viewName +
                               ").checkMousePos(" + mouseLoc.x + ", " + mouseLoc.y + ")");
        }

        // initialize value
        whichCursorSide = DEViseCursor.sideNone;
        selectedCursor = null;
        isInViewDataArea = false;
        activeView = null;

        if (checkMousePos(mouseLoc, view)) { // activeView will not be null

            // Set mouse cursor according to the toolbar state, whether
            // we're on a DEVise cursor, etc.
            if (checkDispatcher && jsc.dispatcher.getStatus() ==
                    DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                // dispatcher still busy
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.waitCursor, this);

            } else if (!isInViewDataArea) {
                // inside the view but not within the data area of that view
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.defaultCursor, this);

            } else if (selectedCursor != null && jsc.toolBar.doCursorOps()) {
                // on a DEVise cursor -- set mouse cursor according to
                // where we are on the DEVise cursor
                setMouseCursorByDEViseCursor();

            } else if (selectedCursor == null && jsc.toolBar.doNormal() &&
                       activeView.isRubberBand) {
                // we can draw a rubberband line to zoom
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.rbCursor, this);

            } else if (jsc.toolBar.doZoomOnly() && !activeView.isRubberBand) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doCursorFill() &&
                       (!activeView.isCursorMove ||
                        activeView.getFirstCursor() == null)) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doDrillDown() && !activeView.isDrillDown) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doHome() && !activeView.isKey) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doToggleFilter() && !activeView.isKey) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doIncreaseSymSize() && !activeView.isKey) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doDecreaseSymSize() && !activeView.isKey) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else if (jsc.toolBar.doNormal() &&
                       (!activeView.isCursorMove ||
                        activeView.getFirstCursor() == null) &&
                       !activeView.isRubberBand &&
                       !_hasJavaSym) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.disabledCursor, this);

            } else {
                jsc.mouseCursor.setToPermanentCursor(this);
            }


            if (activeView != null && activeView.pileBaseView != null) {
                activeView = activeView.pileBaseView;
            }

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            // show the data at current mouse position

            if (activeView != null && jsc.viewInfo != null) {

                // Added check here for mouseLoc being null, because sometimes
                // (infrequently) we get null pointers in the line calling
                // jsc.viewInfo.updateInfo below.  The problem is, I have never
                // been able to reproduce it reliably, and I can't *really*
                // understand how we could get here with mouseLoc equal to null.
                // But I can't see what else could be the proble, either.
                // wenger, 2003-04-01.  *Not* April Fool's!!

                // Note: the extra checks still didn't fix the problem.
                // Could it be that another thread occasionally sets something
                // to null between the check and the updateInfo call below?
                // wenger, 2003-04-25.
                if (mouseLoc != null && isInViewDataArea) {
                    jsc.viewInfo.updateInfo(activeView.viewName,
                                            activeView.getX(mouseLoc.x), activeView.getY(mouseLoc.y));
                } else {
                    jsc.viewInfo.updateInfo();
                }
            }

        } else { // activeView is null and all other values will be initialized value before
            // I don't think we ever get here...  wenger 2007-08-03.
            whichCursorSide = DEViseCursor.sideNone;
            selectedCursor = null;
            isInViewDataArea = false;
            activeView = null;

            Cursor tmpCursor = null;
            if (!checkDispatcher || jsc.dispatcher.getStatus() ==
                    DEViseCmdDispatcher.STATUS_RUNNING_NON_HB) {
                jsc.mouseCursor.setTemporaryCursor(
                    jsc.mouseCursor.waitCursor, this);
            } else {
                jsc.mouseCursor.setToPermanentCursor(this);
            }

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            jsc.viewInfo.updateInfo();
        }
    }

    private void setMouseCursorByDEViseCursor()
    {
        switch (whichCursorSide) {
        case DEViseCursor.sideNone:
            // Not on a cursor
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.rbCursor, this);
            break;

        case DEViseCursor.sideMiddle:
            // Inside a cursor, you can move this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideMiddle");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.moveCursor, this);
            break;

        case DEViseCursor.sideLeft:
            // On left side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideLeft");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.lrsCursor, this);
            break;

        case DEViseCursor.sideRight:
            // On right side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideRight");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.rrsCursor, this);
            break;

        case DEViseCursor.sideTop:
            // On top side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideTop");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.trsCursor, this);
            break;

        case DEViseCursor.sideBottom:
            // On bottom side of a cursor, you can resize this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideBottom");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.brsCursor, this);
            break;

        case DEViseCursor.sideTopLeft:
            // On left-top corner of a cursor, you can resize
            // this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideTopLeft");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.tlrsCursor, this);
            break;

        case DEViseCursor.sideBottomLeft:
            // On left-bottom corner of a cursor, you can resize
            // this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideBottomLeft");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.blrsCursor, this);
            break;

        case DEViseCursor.sideTopRight:
            // On right-top corner of a cursor, you can resize
            // this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideTopRight");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.trrsCursor, this);
            break;

        case DEViseCursor.sideBottomRight:
            // 0n right-bottom corner of a cursor, you can resize
            // this cursor.
            if (DEBUG >= 3) System.out.println(
                    "whichCursorSide = DEViseCursor.sideBottomRight");
            jsc.mouseCursor.setTemporaryCursor(
                jsc.mouseCursor.brrsCursor, this);
            break;

        default:
            throw new YError("Illegal whichCursorSide value: " +
                             whichCursorSide);
        }
    }

    // Returns some kind of information about p in terms of what it's
    // on or in.  However, the logic is so complicated I haven't figured out
    // any easy way to summarize it.
    // It also has side effects of setting activeView, selectedCursor,
    // whichCursorSide, and isInViewDataArea.
    // RKW 2000-05-12.
    //TEMP -- rename this??
    public synchronized boolean checkMousePos(Point p, DEViseView v)
    {
        if (!v.inView(p)) {
            return false;
        }

        if (!v.inViewDataArea(p)) {
            if (v.pileBaseView != null) {
                // v is a non-base view in a pile.
                return false;
            } else {
                activeView = v;
                selectedCursor = null;
                whichCursorSide = DEViseCursor.sideNone;
                isInViewDataArea = false;
                return true;
            }
        }

        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);
            if (checkMousePos(p, vv)) {
                return true;
            }
        }

        if (v.isCursorMove && jsc.toolBar.doCursorOps()) {
            for (int i = 0; i < v.viewCursors.size(); i++) {
                DEViseCursor cursor = (DEViseCursor)v.viewCursors.elementAt(i);
                int cursorSide = cursor.inCursor(p);
                if (cursorSide != DEViseCursor.sideNone) {
                    activeView = v;
                    whichCursorSide = cursorSide;
                    if (DEBUG >= 3) System.out.println(
                            "Setting selected cursor to " + cursor.name);
                    selectedCursor = cursor;
                    isInViewDataArea = true;
                    return true;
                }
            }
        }

        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            if (checkMousePos(p, vv)) {
                return true;
            }
        }

        if (v.pileBaseView != null) {
            // v is a non-base view in a pile.
            return false;
        } else {
            activeView = v;
            isInViewDataArea = true;
            selectedCursor = null;
            whichCursorSide = DEViseCursor.sideNone;
            return true;
        }
    }

    public void create3DViewer()
    {
        return;
    }

    //TEMP -- document this
    public void clear3DViewer()
    {
    }

    //TEMP -- document this
    public Object saveJmolState()
    {
        return null;
    }

    //TEMP -- document this
    public void restoreJmolState(Object jmState)
    {
    }

    // 3D drill-down
    public void drillDown3D(Point p)
    {
    }

    private static String fitStringToWidth(String str, int width,
                                           FontMetrics fm)
    {

        final String suffix = "...";
        final int suffixLen = suffix.length();
        int strWidth;
        while ((strWidth = fm.stringWidth(str)) > width &&
                str.length() > suffixLen) {
            float factor = (float)width / (float)strWidth;

            int newLen = (int)(factor * str.length());
            newLen = Math.min(newLen, str.length() - suffixLen - 1);
            newLen = Math.max(newLen, 0);

            str = str.substring(0, newLen) + suffix;
        }

        return str;
    }
}

// this class is used to create XOR of part of image
class XORFilter extends RGBImageFilter
{
    Color color = Color.white;
    int red, green, blue;
    int _width, _height;
    int _drawType;

    public static final int typeFilledRect = 1;
    public static final int typeFrame = 2;
    public static final int typeFrameAndDots = 3;

    public XORFilter(Color c, int width, int height, int drawType)
    {
        if (c != null) {
            color = c;
        }

        red = color.getRed() << 16;
        green = color.getGreen() << 8;
        blue = color.getBlue();

        _width = width;
        _height = height;

        //System.out.println("r = " + red + " g = " + green + " b = " + blue);

        if (drawType == typeFilledRect) {
            canFilterIndexColorModel = true;
        }
        _drawType = drawType;
    }

    public int filterRGB(int x, int y, int rgb) throws YError
    {
        int rgbOut = rgb;
        if (_drawType == typeFilledRect) {
            rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
                      ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
        } else if (_drawType == typeFrame) {
            if (x < 3 || y < 3 || x > _width - 4 || y > _height - 4) {
                rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
                          ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
            }
        } else if (_drawType == typeFrameAndDots) {
            if (x < 3 || y < 3 || x > _width - 4 || y > _height - 4) {
                rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
                          ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
            } else if ((x % 6 == 0) && (y % 6 == 0)) {
                rgbOut = ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ red) |
                          ((rgb & 0xff00) ^ green) | ((rgb & 0xff) ^ blue));
            }
        } else {
            throw new YError("Illegal draw type (" + _drawType + ")");
        }

        return rgbOut;
    }
}
