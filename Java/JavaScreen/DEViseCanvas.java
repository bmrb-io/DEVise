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
// Revision 1.29  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.28  1999/12/10 15:30:12  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// $Log$
// Revision 1.29  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.27  1999/12/07 23:18:20  hongyu
// *** empty log message ***
//
// Revision 1.25  1999/12/06 23:22:22  hongyu
// *** empty log message ***
//
// Revision 1.24  1999/12/02 19:47:23  hongyu
// *** empty log message ***
//
// Revision 1.23  1999/11/11 18:11:51  hongyu
// *** empty log message ***
//
// Revision 1.22  1999/11/03 08:00:48  hongyu
// *** empty log message ***
//
// Revision 1.21  1999/11/01 17:40:15  hongyu
// *** empty log message ***
//
// Revision 1.20  1999/10/14 15:08:53  hongyu
// *** empty log message ***
//
// Revision 1.19  1999/10/12 22:00:03  hongyu
// *** empty log message ***
//
// Revision 1.17  1999/10/12 00:08:42  hongyu
// *** empty log message ***
//
// Revision 1.16  1999/10/10 08:49:51  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.15  1999/09/24 17:11:46  hongyu
// adding support for 3-d molecule view
//
// Revision 1.14  1999/08/24 08:45:52  hongyu
// *** empty log message ***
//
// Revision 1.13  1999/08/19 07:20:51  hongyu
// *** empty log message ***
//
// Revision 1.12  1999/08/17 06:15:16  hongyu
// *** empty log message ***
//
// Revision 1.11  1999/08/03 05:56:35  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.9  1999/07/27 17:11:17  hongyu
// *** empty log message ***
//
// Revision 1.7  1999/06/23 20:59:14  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseCanvas.java

import  java.awt.*;
import  java.applet.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;
import  java.net.*;
import  java.io.*;

public class DEViseCanvas extends Container
{
    private jsdevisec jsc = null;
    private DEViseScreen jscreen = null;
    private DEViseCmdDispatcher dispatcher = null;
    public int posZ = 0;

    private static int javaCanvasSizeCorrection = 0;
    public int oldHighlightAtomIndex = -1, highlightAtomIndex = -1;
    public int action3d = 0;
    public Point point = new Point();
    DEViseCrystal crystal = null;

    public DEViseView view = null;
    public DEViseView activeView = null;

    private Dimension canvasDim = null;

    private Image image = null;
    private Image offScrImg = null;
    private boolean isImageUpdated = false;

    boolean isMouseDragged = false, isInViewDataArea = false, isInDataArea = false;
    DEViseCursor selectedCursor = null;
    int whichCursorSide = -1;

    public String helpMsg = null;
    public int helpMsgX = -1, helpMsgY = -1;

    Point sp = new Point(), ep = new Point(), op = new Point();
    public static int lastKey = KeyEvent.VK_UNDEFINED;

    public static DEViseCanvas sourceCanvas = null;
    public static boolean isInterative = false;

    public static int totalpaintcount = 0;

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
    }

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

    // loc is relative to this canvas
    public Image getCroppedXORImage(Rectangle loc)
    {
        if (image == null)
            return null;

        CropImageFilter croppedFilter = new CropImageFilter(loc.x, loc.y, loc.width, loc.height);
        FilteredImageSource source = new FilteredImageSource(image.getSource(), croppedFilter);
        Image img = createImage(source);
        if (img == null) {
            return null;
        }

        source = new FilteredImageSource(img.getSource(), new XORFilter());
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

    public void paint(Graphics gc)
    {
        if (DEViseCanvas.isInterative) {
            if (DEViseCanvas.sourceCanvas != this) {
                return;
            }
        }

        // draw 3D molecular view
        if (paintCrystal(gc)) {
            paintBorder(gc);
            paintHelpMsg(gc);
            return;
        }

        // draw the background image
        paintBackground(gc);

        // draw titles for all views, including view symbols but not piled views
        paintTitle(gc);

        // draw all the cursors
        paintCursor(gc);

        // draw all the GDatas
        paintGData(gc, view);

        // draw rubber band
        paintRubberBand(gc);

        // draw highlight border
        paintBorder(gc);

        paintHelpMsg(gc);
    }

    private void paintHelpMsg(Graphics gc)
    {
        if (helpMsg == null) {
            return;
        }

        Toolkit tk = Toolkit.getDefaultToolkit();
        FontMetrics fm = tk.getFontMetrics(DEViseGlobals.textFont);
        int ac = fm.getAscent(), dc = fm.getDescent(), height = ac + dc + 12;
        int width = fm.stringWidth(helpMsg) + 12;

        gc.setColor(Color.black);
        gc.drawRect(helpMsgX, helpMsgY, width - 1, height - 1);
        gc.setColor(new Color(255, 255, 192));
        gc.fillRect(helpMsgX + 1, helpMsgY + 1, width - 2, height - 2);
        gc.setColor(Color.black);
        gc.setFont(DEViseGlobals.textFont);
        gc.drawString(helpMsg, helpMsgX + 6, helpMsgY + height - dc - 6);

        //helpMsg = null;
    }

    private synchronized void paintBackground(Graphics gc)
    {
        if (image != null) {
            gc.drawImage(image, 0, 0, canvasDim.width, canvasDim.height, this);
        } else {
            gc.setColor(new Color(view.viewBg));
            gc.fillRect(0, 0, canvasDim.width, canvasDim.height);
        }
    }

    private synchronized boolean paintCrystal(Graphics gc)
    {
        if (view.viewDimension != 3) {
            return false;
        }

        //jsc.pn("crystal is been painted " + totalpaintcount++);

        if (crystal == null) {
            createCrystal();
            if (crystal == null) {
                paintBackground(gc);
                return true;
            }
        }

        if (action3d == 0) {
            paintBackground(gc);
            crystal.paint(this, gc, isMouseDragged);
        } else if (action3d == 1) {
            action3d = 0;

            if (oldHighlightAtomIndex >= 0) {
                DEViseAtomInCrystal atom = crystal.getAtom(oldHighlightAtomIndex);
                if (atom != null) {
                    atom.lastSelectedBondIndex = -1;
                }
                oldHighlightAtomIndex = -1;

                //gc.drawImage(offScrImg, 0, 0, this);
                paintBackground(gc);
                crystal.paint(this, gc, false);
            } else if (highlightAtomIndex >= 0) {
                DEViseAtomInCrystal atom = crystal.getAtom(highlightAtomIndex);
                oldHighlightAtomIndex = highlightAtomIndex;
                highlightAtomIndex = -1;

                if (atom != null) {
                    DEViseAtomInCrystal atom1 = null;

                    if (atom.lastSelectedBondIndex >= 0) {
                        atom1 = crystal.getAtom(atom.lastSelectedBondIndex);

                        if (atom1 != null) {
                            Color color = atom.type.XORcolor, color1 = atom1.type.XORcolor, oldcolor = gc.getColor();
                            int x = atom.drawX, y = atom.drawY, x1 = atom1.drawX, y1 = atom1.drawY, xm = (x + x1) / 2, ym = (y + y1) / 2;

                            gc.setColor(color);
                            gc.drawLine(x, y, xm, ym);
                            for (int k = 1; k < crystal.bondWidth + 1; k++) {
                                gc.drawLine(x, y - k, xm, ym - k);
                                gc.drawLine(x, y + k, xm, ym + k);
                                gc.drawLine(x - k, y, xm - k, ym);
                                gc.drawLine(x + k, y, xm + k, ym);
                            }

                            gc.setColor(color1);
                            gc.drawLine(xm, ym, x1, y1);
                            for (int k = 1; k < crystal.bondWidth + 1; k++) {
                                gc.drawLine(xm, ym - k, x1, y1 - k);
                                gc.drawLine(xm, ym + k, x1, y1 + k);
                                gc.drawLine(xm - k, ym, x1 - k, y1);
                                gc.drawLine(xm + k, ym, x1 + k, y1);
                            }

                            gc.setColor(oldcolor);
                        }
                    } else {
                        atom.type.paint(this, gc, atom.drawX, atom.drawY, atom.drawSize, 2);
                    }

                    if (atom.lastSelectedBondIndex >= 0) {
                        if (atom1 == null) {
                            atom.lastSelectedBondIndex = -1;
                            return true;
                        }
                    }

                    int w = 0, h = 0, x = point.x, y = point.y;
                    if (atom1 == null) {
                        w = 80;
                        h = 45;
                    } else {
                        w = 120;
                        h = 25;
                    }

                    if (x + w > canvasDim.width - 1) {
                        x = x - w;
                    }
                    if (y + h > canvasDim.height - 1) {
                        y = y - h;
                    }

                    Color oldcolor = gc.getColor();
                    Font oldfont = gc.getFont();

                    gc.setColor(Color.black);
                    gc.drawRect(x, y, w, h);
                    gc.setColor(Color.white);
                    gc.fillRect(x + 1, y + 1, w - 1, h - 1);
                    gc.setFont(new Font("Serif", Font.PLAIN, 12));
                    gc.setColor(atom.type.color);
                    if (atom1 == null) {
                        gc.drawString(atom.type.name + "(" + oldHighlightAtomIndex + ")", x + 5 , y + 10);
                        double[] pos = crystal.getPos(atom.pos);
                        String xs = Double.toString(pos[0]), ys = Double.toString(pos[1]), zs = Double.toString(pos[2]);
                        xs = xs.substring(0, (xs.length() > 6) ? 6 : xs.length());
                        ys = ys.substring(0, (ys.length() > 6) ? 6 : ys.length());
                        zs = zs.substring(0, (zs.length() > 6) ? 6 : zs.length());
                        gc.drawString("x = " + xs, x + 5, y + 23);
                        gc.drawString("y = " + ys, x + 5, y + 33);
                        gc.drawString("z = " + zs, x + 5, y + 43);
                    } else {
                        gc.drawString("" + atom.type.name + "(" + oldHighlightAtomIndex + ")" + "-" + atom1.type.name + "(" + atom.lastSelectedBondIndex + ")", x + 5, y + 13);
                        double[] pos1 = crystal.getPos(atom.pos), pos2 = crystal.getPos(atom1.pos);
                        double dx = pos1[0] - pos2[0], dy = pos1[1] - pos2[1], dz = pos1[2] - pos2[2];
                        double d = Math.sqrt(dx * dx + dy * dy + dz * dz);
                        String dd = Double.toString(d);
                        dd = dd.substring(0, (dd.length() > 6) ? 6 : dd.length());
                        gc.drawString("Length = " + dd, x + 5, y + 23);
                        atom.lastSelectedBondIndex = -1;
                    }

                    gc.setColor(oldcolor);
                    gc.setFont(oldfont);
                }
            }
        } else {
            action3d = 0;
        }

        return true;
    }

    private synchronized void paintBorder(Graphics gc)
    {
        if (activeView != null && activeView == jscreen.getCurrentView()) {
            Rectangle loc = activeView.viewLocInCanvas;

            gc.setColor(Color.red);
            gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
            gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3, loc.height - 3);
        }
    }

    // only top views get title drawn
    private synchronized void paintTitle(Graphics gc)
    {
        // handling child view
        for (int i = 0; i < view.viewChilds.size(); i++) {
            DEViseView v = (DEViseView)view.viewChilds.elementAt(i);

            if (v.viewDTFont != null && v.viewTitle != null) {
                gc.setColor(new Color(v.viewFg));
                gc.setFont(v.viewDTFont);
                gc.drawString(v.viewTitle, v.viewDTX, v.viewDTY);
            }
        }

        // handling itself
        if (view.viewDTFont != null && view.viewTitle != null) {
            gc.setColor(new Color(view.viewFg));
            gc.setFont(view.viewDTFont);
            gc.drawString(view.viewTitle, view.viewDTX, view.viewDTY);
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
            loc = cursor.getLocInCanvas();

            if (selectedCursor == cursor && isMouseDragged) { // draw cursor movement box
                if (loc.width < 4)
                    loc.width = 4;
                if (loc.height < 4)
                    loc.height = 4;

                gc.setColor(Color.yellow);
                gc.drawRect(loc.x, loc.y, loc.width - 1, loc.height - 1);
                gc.setColor(Color.red);
                gc.drawRect(loc.x + 1, loc.y + 1, loc.width - 3, loc.height - 3);

                continue;
            }

            if (cursor.image == null || isImageUpdated) {
                cursor.image = getCroppedXORImage(loc);
            }

            if (cursor.image != null) {
                gc.drawImage(cursor.image, loc.x, loc.y, this);
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
        // handling child views
        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);

            paintGData(gc, vv);
        }

        // handling itself
        DEViseGData gdata = null;
        for (int i = 0; i < v.viewGDatas.size(); i++) {
            gdata = (DEViseGData)v.viewGDatas.elementAt(i);

            if (gdata.string != null && (gdata.symbolType == 12 || gdata.symbolType == 16)) {
                gc.setColor(gdata.color);
                gc.setFont(gdata.font);
                gc.drawString(gdata.string, gdata.x, gdata.y);
            }
        }

        // handling piled views
        for (int i = 0; i < v.viewPiledViews.size(); i++) {
        //for (int i = v.viewPiledViews.size() - 1; i >= 0; i--) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            paintGData(gc, vv);
        }
    }

    private synchronized void paintRubberBand(Graphics gc)
    {
        if (isInViewDataArea && isMouseDragged && selectedCursor == null) {
            int x0, y0, w, h;

            if (sp.x > ep.x)  {
                x0 = ep.x;
                w = sp.x - x0;
            }  else  {
                x0 = sp.x;
                w = ep.x - x0;
            }

            if (sp.y > ep.y)  {
                y0 = ep.y;
                h = sp.y - y0;
            }  else  {
                y0 = sp.y;
                h = ep.y - y0;
            }

            if (w < 4)
                w = 4;
            if (h < 4)
                h = 4;

            gc.setColor(Color.yellow);
            gc.drawRect(x0, y0, w - 1, h - 1);
            gc.setColor(Color.red);
            gc.drawRect(x0 + 1, y0 + 1, w - 3, h - 3);
        }
    }

    protected void processMouseEvent(MouseEvent event)
    {
        int id = event.getID();

        jsc.jscreen.finalMousePosition.x = view.viewLoc.x + event.getX();
        jsc.jscreen.finalMousePosition.y = view.viewLoc.y + event.getY();

        if (id == MouseEvent.MOUSE_PRESSED || id == MouseEvent.MOUSE_RELEASED || id == MouseEvent.MOUSE_CLICKED) {
            if (dispatcher.getStatus() != 0) {
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
        int id = event.getID();

        jsc.jscreen.finalMousePosition.x = view.viewLoc.x + event.getX();
        jsc.jscreen.finalMousePosition.y = view.viewLoc.y + event.getY();

        if (id == MouseEvent.MOUSE_DRAGGED) {
            if (dispatcher.getStatus() != 0) {
                return;
            }
        }

        super.processMouseMotionEvent(event);
    }

    protected void processKeyEvent(KeyEvent event)
    {
        if (view.viewDimension != 3) {
            if (dispatcher.getStatus() != 0) {
                //if (event.getID() == KeyEvent.KEY_PRESSED) {
                //    jsc.showMsg("Java Screen still talking to the Server!\nPlease try again later or press STOP button!");
                //}

                return;
            }
        }

        super.processKeyEvent(event);
    }

    // start of class ViewKeyListener
    class ViewKeyListener extends KeyAdapter
    {
        // event sequence: 1. keypressed 2.keytyped 3.keyreleased
        public void keyPressed(KeyEvent event)
        {
            DEViseCanvas.lastKey = event.getKeyCode();
        }

        public void keyReleased(KeyEvent event)
        {
            DEViseCanvas.lastKey = KeyEvent.VK_UNDEFINED;

            if (view.viewDimension == 3) {
                return;
            }

            char keyChar = event.getKeyChar();
            int keyCode = event.getKeyCode();
            int actualKey = 0;

            if (keyChar != KeyEvent.CHAR_UNDEFINED) {
                actualKey = (int)keyChar;
            } else {
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
                //    actualKey = 0x50000 + 1;
                //    break;
                case KeyEvent.VK_F2:
                    actualKey = 0x50000 + 2;
                    break;
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
                default:
                    actualKey = 0;
                    break;
                }
            }

            if (actualKey != 0 && activeView != null && activeView.isKey) {
                String cmd = "";
                cmd = cmd + "JAVAC_KeyAction " + activeView.getCurlyName() + " " + actualKey;
                jscreen.guiAction = true;
                dispatcher.start(cmd);
            }
        }
    }
    // end of class ViewKeyListener

    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter
    {
        // event sequence: 1. mousePressed 2. mouseReleased 3. mouseClicked
        public void mousePressed(MouseEvent event)
        {
            // The starting point will be in this view, otherwise this event will not be catched

            // Each mouse click will be here once, so double click actually will enter this twice

            // The position of this event will be relative to this view, and will not exceed the
            // range of this view, ie, p.x >= 0 && p.x < view.width ...

            Point p = event.getPoint();
            ep.x = op.x = sp.x = p.x;
            ep.y = op.y = sp.y = p.y;

            if (view.viewDimension == 3) {
                jsc.lastCursor = DEViseGlobals.rbCursor;
                setCursor(jsc.lastCursor);

                activeView = view;
                jsc.viewInfo.updateInfo(activeView.viewName, activeView.getX(sp.x), activeView.getY(sp.y));
                if (jscreen.getCurrentView() != activeView) {
                    jscreen.setCurrentView(activeView);
                }

                return;
            }

            checkMousePos(sp, true);
        }

        public void mouseReleased(MouseEvent event)
        {
            // Each mouse click will be here once, so double click actually will enter
            // this twice. Also, this event will always reported with each mouse click
            // and before the mouseClick event is reported.

            DEViseCanvas.isInterative = false;

            if (view.viewDimension == 3) {
                DEViseCanvas.lastKey = KeyEvent.VK_UNDEFINED;
                isMouseDragged = false;
                repaint();
                return;
            }

            if (isMouseDragged && isInViewDataArea) {
                Point p = event.getPoint();
                String cmd = "";

                if (selectedCursor != null) { // move or resize cursor
                    ep.x = activeView.translateX(p.x, 1);
                    ep.y = activeView.translateY(p.y, 1);

                    jsc.viewInfo.updateInfo(activeView.getX(ep.x), activeView.getY(ep.y));

                    int dx = ep.x - sp.x, dy = ep.y - sp.y;
                    DEViseCursor cursor= selectedCursor;

                    if (whichCursorSide == 0) { // move cursor
                        if (cursor.gridx > 0) {
                            dx = (int)Math.round(Math.round((dx * activeView.dataXStep) / cursor.gridxx) * cursor.gridxx / activeView.dataXStep);
                        }

                        if (cursor.gridy > 0) {
                            dy = (int)Math.round(Math.round((dy * activeView.dataYStep) / cursor.gridyy) * cursor.gridyy / activeView.dataYStep);
                        }

                        whichCursorSide = cursor.updateCursorLoc(dx, dy, 1, whichCursorSide, true);
                    } else {
                        whichCursorSide = cursor.updateCursorLoc(dx, dy, 2, whichCursorSide, true);
                    }

                    cursor.image = null;

                    if (DEViseCanvas.lastKey != KeyEvent.VK_CONTROL) {
                        cmd = cmd + "JAVAC_CursorChanged " + cursor.name + " "
                              + cursor.x + " "
                              + cursor.y + " "
                              + cursor.width + " "
                              + cursor.height;

                        jscreen.guiAction = true;
                        dispatcher.start(cmd);
                    }
                } else { // rubber band
                    ep.x = activeView.translateX(p.x, 1);
                    ep.y = activeView.translateY(p.y, 1);

                    jsc.viewInfo.updateInfo(activeView.getX(ep.x), activeView.getY(ep.y));

                    int w = ep.x - sp.x, h = ep.y - sp.y;
                    if (w < 0)
                        w = -w;
                    if (h < 0)
                        h = -h;

                    if (w > DEViseGlobals.rubberBandLimit.width || h > DEViseGlobals.rubberBandLimit.height) {
                        cmd = cmd + "JAVAC_MouseAction_RubberBand " + activeView.getCurlyName() + " "
                              + activeView.translateX(sp.x, 2) + " " + activeView.translateY(sp.y, 2) + " " + activeView.translateX(ep.x, 2) + " " + activeView.translateY(ep.y, 2);

                        if (DEViseCanvas.lastKey == KeyEvent.VK_ALT) {
                            cmd = cmd + " 1";
                        } else {
                            cmd = cmd + " 0";
                        }

                        jscreen.guiAction = true;
                        dispatcher.start(cmd);
                    }
                }

                isInViewDataArea = false;
                selectedCursor = null;
                whichCursorSide = -1;
                isMouseDragged = false;
                DEViseCanvas.lastKey = KeyEvent.VK_UNDEFINED;

                repaint();
            }
        }

        public void mouseClicked(MouseEvent event)
        {
            if (view.viewDimension == 3) {
                DEViseCanvas.lastKey = KeyEvent.VK_UNDEFINED;
                return;
            }

            if (!isMouseDragged && isInViewDataArea) {
                String cmd = null;
                Point p = event.getPoint();

                if (DEViseCanvas.lastKey == KeyEvent.VK_SHIFT && activeView.isDrillDown) {
                    cmd = "JAVAC_ShowRecords " + activeView.getCurlyName() + " " + activeView.translateX(p.x, 2) + " " + activeView.translateY(p.y, 2);
                } else if (DEViseCanvas.lastKey == KeyEvent.VK_F1) {
                    if (helpMsg != null) {
                        helpMsg = null;
                    } else {
                        helpMsgX = activeView.translateX(p.x, 2);
                        helpMsgY = activeView.translateY(p.y, 2);
                        cmd = "JAVAC_GetViewHelp " + activeView.getCurlyName() + " " + helpMsgX + " " + helpMsgY;
                        jsc.jscreen.setLastMsgView(activeView);
                    }
                } else {
                    DEViseCursor cursor = activeView.getFirstCursor();

                    if (cursor != null && (cursor.isXMovable || cursor.isYMovable)) {
                        Rectangle loc = cursor.cursorLocInCanvas;

                        int dx = p.x - loc.x - loc.width / 2;
                        int dy = p.y - loc.y - loc.height / 2;

                        if (cursor.gridx > 0) {
                            dx = (int)Math.round(Math.round((dx * activeView.dataXStep) / cursor.gridxx) * cursor.gridxx / activeView.dataXStep);
                        }

                        if (cursor.gridy > 0) {
                            dy = (int)Math.round(Math.round((dy * activeView.dataYStep) / cursor.gridyy) * cursor.gridyy / activeView.dataYStep);
                        }

                        cursor.updateCursorLoc(dx, dy, 1, whichCursorSide, true);

                        cmd = "JAVAC_CursorChanged " + cursor.name + " "
                              + cursor.x + " "
                              + cursor.y + " "
                              + cursor.width + " "
                              + cursor.height;

                        cursor.image = null;
                    }
                }

                if (cmd != null) {
                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                }

                DEViseCanvas.lastKey = KeyEvent.VK_UNDEFINED;
                isInViewDataArea = false;
                selectedCursor = null;
                whichCursorSide = -1;
                isMouseDragged = false;

                repaint();
            }
        }
    }
    // end of class ViewMouseListener

    // start of class ViewMouseMotionListener
    class ViewMouseMotionListener extends MouseMotionAdapter
    {
        // When you drag a mouse, all the following mouse position is relative to the view
        // you first start the dragging(so might have value that is less than zero), and
        // the dragging event is still send back to the view where you first start dragging.

        public void mouseDragged(MouseEvent event)
        {
            Point p = event.getPoint();
            isMouseDragged = true;

            if (view.viewDimension == 3 && crystal != null) {
                int dx = p.x - op.x, dy = p.y - op.y;
                op.x = p.x;
                op.y = p.y;

                jsc.viewInfo.updateInfo(activeView.getX(p.x), activeView.getY(p.y));

                if (lastKey == KeyEvent.VK_ALT) {
                    crystal.translate(dx, dy);
                } else if (lastKey == KeyEvent.VK_CONTROL) {
                    crystal.scale(dx, dy);
                } else {
                    crystal.rotate(dx, dy);
                }

                DEViseCanvas.isInterative = true;
                DEViseCanvas.sourceCanvas = DEViseCanvas.this;
                repaint();
                return;
            }

            if (isInViewDataArea) {
                ep.x = activeView.translateX(p.x, 1);
                ep.y = activeView.translateY(p.y, 1);

                jsc.viewInfo.updateInfo(activeView.getX(ep.x), activeView.getY(ep.y));

                if (selectedCursor != null) {

                    DEViseCursor cursor = selectedCursor;

                    int dx = ep.x - sp.x, dy = ep.y - sp.y;

                    if (whichCursorSide == 0) { // move cursor
                        if (cursor.gridx > 0) {
                            dx = (int)Math.round(Math.round((dx * activeView.dataXStep) / cursor.gridxx) * cursor.gridxx / activeView.dataXStep);
                        }

                        if (cursor.gridy > 0) {
                            dy = (int)Math.round(Math.round((dy * activeView.dataYStep) / cursor.gridyy) * cursor.gridyy / activeView.dataYStep);
                        }

                        whichCursorSide = cursor.updateCursorLoc(dx, dy, 1, whichCursorSide, false);
                    } else {
                        whichCursorSide = cursor.updateCursorLoc(dx, dy, 2, whichCursorSide, false);
                    }
                }

                DEViseCanvas.isInterative = true;
                DEViseCanvas.sourceCanvas = DEViseCanvas.this;
                repaint();
            }
        }

        public void mouseMoved(MouseEvent event)
        {
            // the position of this event will be relative to this view and will not exceed
            // the range of this view, ie, p.x >= 0 && p.x < view.width, p.y >= 0 && p.y < view.height

            Point p = event.getPoint();

            if (view.viewDimension == 3 && crystal != null) {
                point.x = p.x;
                point.y = p.y;

                jsc.lastCursor = DEViseGlobals.rbCursor;
                setCursor(jsc.lastCursor);

                activeView = view;
                jsc.viewInfo.updateInfo(activeView.viewName, activeView.getX(p.x), activeView.getY(p.y));
                if (jscreen.getCurrentView() != activeView) {
                    jscreen.setCurrentView(activeView);
                }

		//int index = crystal.find(p.x, p.y);
		//if (index != oldHighlightAtomIndex) {
		//    action3d = 1;
		//    repaint();
		//    highlightAtomIndex = index;
		//    action3d = 1;
		//    repaint();
                //}

		        return;
            }

            checkMousePos(p, true);

            //DEViseCanvas.isInterative = true;
            //DEViseCanvas.sourceCanvas = DEViseCanvas.this;
            //repaint();
        }
    }
    // end of class ViewMouseMotionListener

    public synchronized void checkMousePos(Point p, boolean checkDispatcher)
    {
        // initialize value
        whichCursorSide = -1;
        selectedCursor = null;
        isInViewDataArea = false;
        activeView = null;

        if (checkMousePos(p, view)) { // activeView will not be null
            if (checkDispatcher && jsc.dispatcher.getStatus() != 0) {
                // dispatcher still busy
                jsc.lastCursor = DEViseGlobals.waitCursor;
            } else if (isInViewDataArea && selectedCursor == null) {
                // inside the date area but not within any cursor, you can draw rubber band or
                // get the records at that data point
                jsc.lastCursor = DEViseGlobals.rbCursor;
            } else if (isInViewDataArea && selectedCursor != null) {
                switch (whichCursorSide) {
                case 0: // inside a cursor, you can move that cursor
                    jsc.lastCursor = DEViseGlobals.moveCursor;
                    break;
                case 1: // on left side of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.lrsCursor;
                    break;
                case 2: // on right side of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.rrsCursor;
                    break;
                case 3: // on top side of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.trsCursor;
                    break;
                case 4: // on bottom side of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.brsCursor;
                    break;
                case 5: // on left-top corner of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.tlrsCursor;
                    break;
                case 6: // on left-bottom corner of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.blrsCursor;
                    break;
                case 7: // on right-top corner of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.trrsCursor;
                    break;
                case 8: // on right-bottom corner of a cursor, you can resize this cursor
                    jsc.lastCursor = DEViseGlobals.brrsCursor;
                    break;
                }
            } else { // inside the view but not within the data area of that view
                jsc.lastCursor = DEViseGlobals.defaultCursor;
            }

            setCursor(jsc.lastCursor);

            if (activeView.piledView != null) {
                activeView = activeView.piledView;
            }

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            // show the data at current mouse position
            jsc.viewInfo.updateInfo(activeView.viewName, activeView.getX(p.x), activeView.getY(p.y));

        } else { // activeView is null and all other values will be initialized value before
            whichCursorSide = -1;
            selectedCursor = null;
            isInViewDataArea = false;
            activeView = null;

            if (!checkDispatcher || jsc.dispatcher.getStatus() == 0) {
                jsc.lastCursor = DEViseGlobals.defaultCursor;
            } else {
                jsc.lastCursor = DEViseGlobals.waitCursor;
            }

            setCursor(jsc.lastCursor);

            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }

            jsc.viewInfo.updateInfo();
        }
    }

    public synchronized boolean checkMousePos(Point p, DEViseView v)
    {
        if (!v.inView(p)) {
            return false;
        }

        if (!v.inViewDataArea(p)) {
            if (v.piledView != null) {
                return false;
            } else {
                activeView = v;
                selectedCursor = null;
                whichCursorSide = -1;
                isInViewDataArea = false;
                return true;
            }
        }

        for (int i = 0; i < v.viewChilds.size(); i++) {
            DEViseView vv = (DEViseView)v.viewChilds.elementAt(i);
            if (checkMousePos(p, vv)) {
                return true;
            } else {
                continue;
            }
        }

        for (int i = 0; i < v.viewCursors.size(); i++) {
            DEViseCursor cursor = (DEViseCursor)v.viewCursors.elementAt(i);
            int status = cursor.inCursor(p);
            if (status < 0) {
                continue;
            } else {
                activeView = v;
                whichCursorSide = status;
                selectedCursor = cursor;
                isInViewDataArea = true;
                return true;
            }
        }

        for (int i = 0; i < v.viewPiledViews.size(); i++) {
            DEViseView vv = (DEViseView)v.viewPiledViews.elementAt(i);

            if (checkMousePos(p, vv)) {
                return true;
            } else {
                continue;
            }
        }

        if (v.piledView != null) {
            return false;
        } else {
            activeView = v;
            isInViewDataArea = true;
            selectedCursor = null;
            whichCursorSide = -1;
            return true;
        }
    }

    public void createCrystal()
    {
        if (view.viewDimension != 3) {
            return;
        }

        if (view.viewGDatas.size() == 0) {
            return;
        }

        if (crystal == null) {
            /*
            StringWriter writer = new StringWriter();
            writer.write("\"Scale Factor\" 1.0 1.0 1.0\n");
            writer.write("\"Base Vector\" 1 0 0 0 1 0 0 0 1\n");
            writer.write("Atoms " + view.viewGDatas.size() + "\n");

            for (int i = 0; i < view.viewGDatas.size(); i++) {
                DEViseGData gdata = (DEViseGData)view.viewGDatas.elementAt(i);
                writer.write(gdata.string + " " + gdata.x0 + " " + gdata.y0 + " " + gdata.z0 + "\n");
            }

            writer.write("\"Bond Limit\" 2.3 2.7\n");

            //writer.close();

            String string = writer.toString();

            try {
                createCrystalFromData(new StringReader(string));
                for (int i = 0; i < view.viewGDatas.size(); i++) {
                    DEViseGData gdata = (DEViseGData)view.viewGDatas.elementAt(i);
                    DEViseAtomInCrystal atom = crystal.getAtom(i);
                    atom.type.setColor(gdata.color);
                }
            } catch (YException e) {
                jsc.pn(e.getMessage());
                crystal = null;
		        return;
            }
            */
            int size = view.viewGDatas.size();
            //double[][] atomPos = new double[size][3];
            //double[][][] bondPos = new double[size][
            //String[] atomName = new String[size];
            Vector aPos = new Vector(size), aName = new Vector(size), aColor = new Vector(size), bPos = new Vector(size), bColor = new Vector(size);
            for (int i = 0; i < size; i++) {
                DEViseGData gdata = (DEViseGData)view.viewGDatas.elementAt(i);
                if (gdata.string.equals("bond")) {
                    double[][] pos = new double[2][3];
                    pos[0][0] = gdata.x0;
                    pos[0][1] = gdata.y0;
                    pos[0][2] = gdata.z0;
                    pos[1][0] = gdata.x1;
                    pos[1][1] = gdata.y1;
                    pos[1][2] = gdata.z1;
                    bPos.addElement(pos);
                    bColor.addElement(gdata.color);
                } else {
                    aName.addElement(gdata.string);
                    double[] pos = new double[3];
                    pos[0] = gdata.x0;
                    pos[1] = gdata.y0;
                    pos[2] = gdata.z0;
                    aPos.addElement(pos);
                    aColor.addElement(gdata.color);
                }
            }

            String[] atomName = null;
            double[][] atomPos = null;
            double[][][] bondPos = null;
            Color[] bondColor = null;
            Color[] atomColor = null;
            if (aName.size() > 0) {
                atomName = new String[aName.size()];
                for (int i = 0; i < atomName.length; i++) {
                    atomName[i] = (String)aName.elementAt(i);
                }
            }

            if (aPos.size() > 0) {
                atomPos = new double[aPos.size()][3];
                for (int i = 0; i < atomPos.length; i++) {
                    atomPos[i] = (double[])aPos.elementAt(i);
                }
            }

            if (aColor.size() > 0) {
                atomColor = new Color[aColor.size()];
                for (int i = 0; i < atomColor.length; i++) {
                    atomColor[i] = (Color)aColor.elementAt(i);
                }
            }

            if (bPos.size() > 0) {
                bondPos = new double[bPos.size()][2][3];
                for (int i = 0; i < bondPos.length; i++) {
                    bondPos[i] = (double[][])bPos.elementAt(i);
                }
            }

            if (bColor.size() > 0) {
                bondColor = new Color[bColor.size()];
                for (int i = 0; i < bondColor.length; i++) {
                    bondColor[i] = (Color)bColor.elementAt(i);
                }
            }

            try {
                DEVise3DLCS lcs = new DEVise3DLCS();
                crystal = new DEViseCrystal(canvasDim.width - 10, canvasDim.height - 10, 5, lcs, -1, -1, atomName, atomPos, atomColor, bondPos, bondColor);
            } catch (YException e) {
                jsc.pn(e.getMessage());
                crystal = null;
                return;
            }
        }

        if (view.viewPiledViews.size() > 0) {
            crystal.setSelect();

            for (int i = 0; i < view.viewPiledViews.size(); i++) {
		        DEViseView v = (DEViseView)view.viewPiledViews.elementAt(i);
		        for (int j = 0; j < v.viewGDatas.size(); j++) {
                    DEViseGData gdata = (DEViseGData)v.viewGDatas.elementAt(j);
                    if (gdata.string.equals("bond")) {
                        crystal.setSelect(gdata.x0, gdata.y0, gdata.z0, gdata.color, true);
                        crystal.setSelect(gdata.x1, gdata.y1, gdata.z1, gdata.color, true);
                    } else {
                        crystal.setSelect(gdata.x0, gdata.y0, gdata.z0, gdata.color, false);
                    }
                }
            }
        }
    }

    private void createCrystalFromData(Reader stream) throws YException
    {
        StreamTokenizer input = new StreamTokenizer(stream);
        input.eolIsSignificant(false);
        input.slashSlashComments(true);
        input.slashStarComments(true);
        // default
	    //input.wordChars('a', 'z');
	    //input.wordChars('A', 'Z');
	    //input.wordChars(128 + 32, 255);
	    //input.whitespaceChars(0, ' ');
	    //input.commentChar('/');
	    //input.quoteChar('"');
	    //input.quoteChar('\'');
	    //input.parseNumbers();
        input.commentChar('#');

        try {
            String title = null;
            boolean isEnd = false;

            // step 1: read information about the origin and base vector
            double[] origin = null, baseVector = null, scaleFactor = null;
            while (!isEnd) {
                switch(input.nextToken()) {
                    case StreamTokenizer.TT_EOF:
                        throw new YException("Unexpected end of input stream at line " + input.lineno());
                    case StreamTokenizer.TT_WORD:
                    case '"':
                    case '\'':
                        title = (input.sval).toLowerCase();

                        if (title.equals("origin")) {
                            origin = getData(input, 3);
                        } else if (title.equals("base vector")) {
                            baseVector = getData(input, 9);
                            isEnd = true;
                        } else if (title.equals("crystal parameter")) {
                            baseVector = getData(input, 6);
                            isEnd = true;
                        } else if (title.equals("scale factor")) {
                            scaleFactor = getData(input, 3);
                        } else {
                            throw new YException("Invalid token at line " + input.lineno());
                        }

                        break;
                    default:
                        throw new YException("Invalid token at line " + input.lineno());
                }
            }

            // step 2: read information about atoms
            isEnd = false;
            int numberOfAtoms = -1, unita = -1, unitb = -1, unitc = -1;
            double[][] atomPos = null;
            String[] atomName = null;
            while (!isEnd) {
                switch(input.nextToken()) {
                    case StreamTokenizer.TT_EOF:
                        throw new YException("Unexpected end of input stream at line " + input.lineno());
                    case StreamTokenizer.TT_WORD:
                    case '"':
                    case '\'':
                        title = (input.sval).toLowerCase();
                        if (title.equals("basis") || title.equals("atoms")) {
                            boolean isBasis = title.equals("basis");

                            numberOfAtoms = (int)(getData(input) + 0.5);

                            YGlobals.ythrow(numberOfAtoms > 0, "Number of atoms (" + numberOfAtoms + ") is invalid at line " + input.lineno());

                            atomPos = new double[numberOfAtoms][3];
                            atomName = new String[numberOfAtoms];

                            if (isBasis) {
                                unita = (int)(getData(input) + 0.5);
                                unitb = (int)(getData(input) + 0.5);
                                unitc = (int)(getData(input) + 0.5);

                                YGlobals.ythrow(unita > 0 && unitb > 0 && unitc > 0, "Invalid unit cell expansion (" + unita + "," + unitb + "," + unitc + ")");
                            }

                            for (int i = 0; i < numberOfAtoms; i++) {
                                atomName[i] = getString(input);
                                atomPos[i] = getData(input, 3);
                                /*
                                if (isBasis) {
                                    YGlobals.ythrow(atomPos[i][0] >= 0.0 && atomPos[i][0] < 1.0
                                                    && atomPos[i][1] >= 0.0 && atomPos[i][1] < 1.0
                                                    && atomPos[i][2] >= 0.0 && atomPos[i][2] < 1.0,
                                                    "Incorrect atom position for unit cell basis (valid atom position must be less than 1.0 and greater than or equals to 0.0) at line " + input.lineno());
                                }

                                for (int j = 0; j < i; j++) {
                                    YGlobals.ythrow(!(atomPos[i][0] == atomPos[j][0]
                                                    && atomPos[i][1] == atomPos[j][1]
                                                    && atomPos[i][2] == atomPos[j][2]),
                                                    "Atom position shared by more than one atoms at line " + input.lineno());
                                }
                                */
                            }

                            isEnd = true;
                        } else {
                            throw new YException("Invalid token at line " + input.lineno());
                        }

                        break;
                    default:
                        throw new YException("Invalid token at line " + input.lineno());
                }
            }

            // step 3: read information about bonds
            isEnd = false;
            double min = 0.0, max = 0.0;
            while (!isEnd) {
                switch(input.nextToken()) {
                    case StreamTokenizer.TT_EOF:
                        throw new YException("Unexpected end of input stream at line " + input.lineno());
                    case StreamTokenizer.TT_WORD:
                    case '"':
                    case '\'':
                        title = (input.sval).toLowerCase();

                        if (title.equals("bond limit")) {
                            min = getData(input);
                            max = getData(input);

                            YGlobals.ythrow(min > 0.0 && max > 0.0 && min < max, "Invalid bond limit (min = " + min + ", max = " + max + ")");

                            isEnd = true;
                        } else {
                            throw new YException("Invalid token at line " + input.lineno());
                        }

                        break;
                    default:
                        throw new YException("Invalid token at line " + input.lineno());
                }
            }

            // building crystal

            // baseVector will not will be null
            DEVise3DLCS lcs = null;
            if (baseVector.length == 6) {
                if (scaleFactor != null) {
                    baseVector[0] *= scaleFactor[0];
                    baseVector[1] *= scaleFactor[1];
                    baseVector[2] *= scaleFactor[2];
                }

                if (origin != null) {
                    lcs = new DEVise3DLCS(baseVector[0], baseVector[1], baseVector[2], baseVector[3], baseVector[4], baseVector[5], origin);
                } else {
                    lcs = new DEVise3DLCS(baseVector[0], baseVector[1], baseVector[2], baseVector[3], baseVector[4], baseVector[5]);
                }
            } else {
                if (scaleFactor != null) {
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            baseVector[i + j] *= scaleFactor[i];
                        }
                    }
                }

                if (origin != null) {
                    lcs = new DEVise3DLCS(baseVector, origin);
                } else {
                    lcs = new DEVise3DLCS(baseVector);
                }
            }

            if (unita > 0 && unitb > 0 && unitc > 0) {
                Vector an = new Vector(), ap = new Vector();
                double[] pos = null;

                for (int n = 0; n < atomName.length; n++) {
                    for (int i = 0; i <= unita; i++) {
                        for (int j = 0; j <= unitb; j++) {
                            for (int k = 0; k <= unitc; k++) {
                                pos = new double[3];

                                pos[0] = atomPos[n][0] + i;
                                pos[1] = atomPos[n][1] + j;
                                pos[2] = atomPos[n][2] + k;

                                if (pos[0] <= unita && pos[1] <= unitb && pos[2] <= unitc) {
                                    an.addElement(atomName[n]);
                                    ap.addElement(pos);
                                }
                            }
                        }
                    }
                }

                atomName = new String[an.size()];
                atomPos = new double[an.size()][3];

                for (int i = 0; i < an.size(); i++) {
                    atomName[i] = (String)an.elementAt(i);
                    atomPos[i] = (double[])ap.elementAt(i);
                }
            } else {
                lcs = new DEVise3DLCS();
                if (scaleFactor != null) {
                    lcs.scale(scaleFactor[0], scaleFactor[1], scaleFactor[2]);
                }
            }

            crystal = new DEViseCrystal(canvasDim.width - 10, canvasDim.height - 10, 5, lcs, min, max, atomName, atomPos);

            stream.close();
        } catch (IOException e) {
            throw new YException("IO error while reading from line " + input.lineno());
        }
    }

    private double[] getData(StreamTokenizer input, int number) throws YException, IOException
    {
        double[] data = new double[number];
        int count = 0, type;

        while (count < number) {
            type = input.nextToken();
            if (type == StreamTokenizer.TT_NUMBER) {
                data[count] = input.nval;
                count++;
            } else if (type == StreamTokenizer.TT_EOF) {
                throw new YException("Unexpected end of input stream at line " + input.lineno());
            } else {
                throw new YException("Invalid token at line " + input.lineno());
            }
        }

        return data;
    }

    private double getData(StreamTokenizer input) throws YException, IOException
    {
        int type;

        while (true) {
            type = input.nextToken();
            if (type == StreamTokenizer.TT_NUMBER) {
                return input.nval;
            } else if (type == StreamTokenizer.TT_EOF) {
                throw new YException("Unexpected end of input stream at line " + input.lineno());
            } else {
                throw new YException("Invalid token at line " + input.lineno());
            }
        }
    }

    private String[] getString(StreamTokenizer input, int number) throws YException, IOException
    {
        String[] data = new String[number];
        int count = 0, type;

        while (count < number) {
            type = input.nextToken();
            if (type == StreamTokenizer.TT_WORD || type == '"' || type == '\'') {
                data[count] = input.sval;
                count++;
            } else if (type == StreamTokenizer.TT_EOF) {
                throw new YException("Unexpected end of input stream at line " + input.lineno());
            } else {
                throw new YException("Invalid token at line " + input.lineno());
            }
        }

        return data;
    }

    private String getString(StreamTokenizer input) throws YException, IOException
    {
        int type;

        while (true) {
            type = input.nextToken();
            if (type == StreamTokenizer.TT_WORD || type == '"' || type == '\'') {
                return input.sval;
            } else if (type == StreamTokenizer.TT_EOF) {
                throw new YException("Unexpected end of input stream at line " + input.lineno());
            } else {
                throw new YException("Invalid token at line " + input.lineno());
            }
        }
    }
}

// this class is used to create XOR of part of image
class XORFilter extends RGBImageFilter
{
    public XORFilter()
    {
        canFilterIndexColorModel = true;
    }

    public int filterRGB(int x, int y, int rgb)
    {
        return ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ 0xff0000) | ((rgb & 0xff00) ^ 0xff00) | ((rgb & 0xff) ^ 0xff));
    }
}
