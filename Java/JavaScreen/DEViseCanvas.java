// DEViseCanvas.java
// last updated on 04/20/99

import  java.awt.*;
import  java.applet.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;
import  java.net.*;

public class DEViseCanvas extends Container
{
    private jsdevisec jsc = null;
    private DEViseScreen jscreen = null;
    private DEViseCmdDispatcher dispatcher = null;

    public DEViseView view = null;
    public DEViseView activeView = null;
    
    //public Vector allCursors = new Vector();
    //public Vector allGDatas = new Vector();
    
    private Image image = null;

    private Dimension canvasDim = null;
    private Rectangle canvasLoc = null;

    private Image offScrImg = null;
        
    // indicate whether or not mouse is dragged
    boolean isMouseDragged = false;

    int mousePosition = 0;

    Point sp = new Point(), ep = new Point(), op = new Point();


    public DEViseCanvas(DEViseView v, Image img)
    {
        view = v;
        jsc = view.jsc;
        jscreen = jsc.jscreen;
        dispatcher = jsc.dispatcher;

        image = img;

        if (image != null) {
            canvasDim = new Dimension(image.getWidth(this), image.getHeight(this));
            canvasLoc = new Rectangle(view.viewLoc);
            if (canvasLoc.width != canvasDim.width || canvasLoc.height != canvasDim.height) {
                canvasLoc.width = canvasDim.width;
                canvasLoc.height = canvasDim.height;
                view.updateLoc(canvasLoc);
            }
        }

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
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

    public Rectangle getBoundsInScreen()
    {
        //return new Rectangle(canvasLoc.x + DEViseGlobals.screenEdge.width, canvasLoc.y + DEViseGlobals.screenEdge.height, canvasLoc.width, canvasLoc.height);
        return view.getBoundsInScreen();
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
        if (img == null) {
            return null;
        } else {
            /*            
            int waittime = 0;
            while (img.getWidth(this) < 0 || img.getHeight(this) < 0) {
                try {
                    Thread.sleep(50);                    
                } catch (InterruptedException e) {
                    waittime += 50;
                    if (waittime > 1000) {
                        jsc.pn("Can not finish creating image with 1 seconds");
                        return null;
                    }
                }
            }
            */            
        }
            
        return img;
    }

    public synchronized void updateImage(Image img)
    {
        image = img;

        if (image != null) {
            canvasDim.width = image.getWidth(this);
            canvasDim.height = image.getHeight(this);
            if (canvasLoc.width != canvasDim.width || canvasLoc.height != canvasDim.height) {
                canvasLoc.width = canvasDim.width;
                canvasLoc.height = canvasDim.height;
                view.updateLoc(canvasLoc);
            }
        }

        // Necessary to draw correct image because double-buffering is used
        offScrImg = null;

        // Need to rebuild all cursor images, including cursors for child view and piled view
        Rectangle loc = null;
        if (view.viewCursors.size() != 0) {
            for (int i = 0; i < view.viewCursors.size(); i++) {
                DEViseCursor cursor = view.getCursor(i);
                loc = cursor.getLoc();
                cursor.image = getCroppedXORImage(loc);
            }
        }
        
        if (view.viewChilds.size() != 0) {
            for (int i = 0; i < view.viewChilds.size(); i++) {
                DEViseView v = view.getChild(i);
                for (int j = 0; j < v.viewCursors.size(); j++) {
                    DEViseCursor cursor = v.getCursor(j);
                    loc = cursor.getLoc();
                    // convert loc to be relative to this view
                    loc.x = loc.x + v.viewLoc.x;
                    loc.y = loc.y + v.viewLoc.y;
                    cursor.image = getCroppedXORImage(loc);
                }
            }
        }        
    }

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (g == null) {
            jsc.pn("Null graphics received in view update for view " + view.getCurlyName());
            return;
        }

        if (offScrImg == null) {
            offScrImg = createImage(canvasDim.width, canvasDim.height);
            if (offScrImg == null) {
                jsc.pn("Can not create off screen image for view " + view.getCurlyName());
                return;
            }
        }

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (image != null) {
            g.drawImage(image, 0, 0, this);
        } else {
            g.setColor(new Color(view.viewBg));
            g.fillRect(0, 0, canvasDim.width, canvasDim.height);
        }

        // draw all the cursors, include the cursors for child view and piled view
        Rectangle loc = null;
        
        if (view.viewCursors.size() != 0) {
            for (int i = 0; i < view.viewCursors.size(); i++) {
                DEViseCursor cursor = view.getCursor(i);
                // check to see if this cursor is selected
                if ((mousePosition == 2 || mousePosition == 3) && view.whichCursor == i) {
                    continue;
                }

                if (cursor.image == null) {
                    loc = cursor.getLoc();
                    cursor.image = getCroppedXORImage(loc);
                }

                if (cursor.image == null) { // Can not create the image for cursor, so skip this cursor
                    jsc.pn("Can not create image for cursor in view " + view.getCurlyName());
                } else {
                    g.drawImage(cursor.image, cursor.x, cursor.y, this);
                }
            }
        }
        
        if (view.viewChilds.size() != 0) {
            for (int i = 0; i < view.viewChilds.size(); i++) {
                DEViseView v = view.getChild(i);
                for (int j = 0; j < v.viewCursors.size(); j++) {
                    DEViseCursor cursor = v.getCursor(j);

                    // check to see if this cursor is selected
                    if ((mousePosition == 6 || mousePosition == 7) && view.whichCursor == j) {
                        continue;
                    }

                    loc = cursor.getLoc();
                    // convert loc to be relative to this view
                    loc.x = loc.x + v.viewLoc.x;
                    loc.y = loc.y + v.viewLoc.y;
                    
                    if (cursor.image == null) {                        
                        cursor.image = getCroppedXORImage(loc);
                    }
                    
                    if (cursor.image == null) { // Can not create the image for cursor, so skip this cursor
                        jsc.pn("Can not create image for cursor in child view " + v.getCurlyName() + " of view " + view.getCurlyName());
                    } else {
                        g.drawImage(cursor.image, loc.x, loc.y, this);
                    }                                        
                }
            }
        }        
        
        if (activeView != null && activeView == jscreen.getCurrentView()) {
            if (isMouseDragged) {
                if (mousePosition == 2 || mousePosition == 3 || mousePosition == 6 || mousePosition == 7) { // draw cursor movement box
                    loc = new Rectangle(activeView.getCursor(activeView.whichCursor).getLoc());
                    loc.x = activeView.xtoparent(loc.x);
                    loc.y = activeView.ytoparent(loc.y);
                    int edge = 1;
                    g.setColor(Color.yellow);
                    g.drawRect(loc.x, loc.y, loc.width, loc.height);
                    g.drawRect(loc.x + edge, loc.y + edge, loc.width - edge * 2, loc.height - edge * 2);
                    edge++;
                    g.setColor(Color.red);
                    g.drawRect(loc.x + edge, loc.y + edge, loc.width - edge * 2, loc.height - edge * 2);
                    edge++;
                    g.drawRect(loc.x + edge, loc.y + edge, loc.width - edge * 2, loc.height - edge * 2);
                } else if (mousePosition == 1 || mousePosition == 5) { // draw rubber band
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

                    if (w < 3) w = 3;
                    if (h < 3) h = 3;
                    loc = new Rectangle(x0, y0, w, h);
                    g.setColor(Color.yellow);
                    g.drawRect(loc.x, loc.y, loc.width, loc.height);
                    int edge = 1;
                    g.setColor(Color.red);
                    g.drawRect(loc.x + edge, loc.y + edge, loc.width - edge * 2, loc.height - edge * 2);
                }
            }

            // draw highlight box indicate this view is the current view
            loc = new Rectangle(activeView.viewLoc);
            if (activeView.parentView == null) {
                loc.x = 0;
                loc.y = 0;
            }

            g.setColor(Color.red);
            g.drawRect(loc.x, loc.y, loc.width, loc.height);
            g.drawRect(loc.x + 1, loc.y + 1, loc.width - 2, loc.height - 2);
        }
    }

    protected void processMouseEvent(MouseEvent event)
    {
        int id = event.getID();
        if (id == MouseEvent.MOUSE_CLICKED || id == MouseEvent.MOUSE_DRAGGED || id == MouseEvent.MOUSE_PRESSED || id == MouseEvent.MOUSE_RELEASED) {
            if (dispatcher.getStatus() != 0) {
                if (id == MouseEvent.MOUSE_PRESSED) {
                    jsc.showMsg("JavaScreen still talking to the Server!\nPlease try again later or press STOP button!");
                }

                return;
            }
        }

        super.processMouseEvent(event);
    }

    protected void processKeyEvent(KeyEvent event)
    {
        if (dispatcher.getStatus() != 0) {
            if (event.getID() == KeyEvent.KEY_PRESSED) {
                jsc.showMsg("Java Screen still talking to the Server!\nPlease try again later or press STOP button!");
            }

            return;
        }

        super.processKeyEvent(event);
    }

    // start of class ViewKeyListener
    class ViewKeyListener extends KeyAdapter
    {
        // event sequence: 1. keypressed 2.keytyped 3.keyreleased
        public void keyReleased(KeyEvent event)
        {
            //jsc.pn("Key " + event.getKeyCode() + " released!");
            //jsc.pn("Character " + event.getKeyChar() + " released!");

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
                case KeyEvent.VK_F1:
                    actualKey = 0x50000 + 1;
                    break;
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
                if (activeView.isFirstTime) {
                    cmd = cmd + "JAVAC_MouseAction_Click " + activeView.getCurlyName() + " " +  0 + " " + 0 + "\n";
                }

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
        WaitThread wt = new WaitThread(jsc);

        // event sequence: 1. mousePressed 2. mouseReleased 3. mouseClicked
        public void mousePressed(MouseEvent event)
        {
            // The starting point will be in this view, otherwise this event
            // will not be catched by this view
            // Each mouse click will be here once, so double click actually will enter
            // this twice
            // the position of this event will be relative to this view, and will not
            // exceed the range of this view, ie, p.x >= 0 && p.x < view.width ...
            sp = event.getPoint();
            ep.x = op.x = sp.x;
            ep.y = op.y = sp.y;

            isMouseDragged = false;

            mousePosition = view.checkPos(sp);

            // set the current view
            if (mousePosition >= 0 && mousePosition < 4) {
                if (jscreen.getCurrentView() != view) {
                    jscreen.setCurrentView(view);
                }

                activeView = view;
            } else if (mousePosition >= 4 && mousePosition < 8) {
                DEViseView v = view.getChild(view.whichChild);

                if (jscreen.getCurrentView() != v) {
                    jscreen.setCurrentView(v);
                }

                activeView = v;
            }

            // show the data at current mouse position, if any
            String name = activeView.viewName, xpos = activeView.getX(sp.x), ypos = activeView.getY(sp.y);
            jsc.viewInfo.updateInfo(name, xpos, ypos);

            if (mousePosition == 0 || mousePosition == 4) {
                // inside the view but not within the data area of that view
                // you can move the view if you want
                setCursor(DEViseGlobals.defaultCursor);
                jsc.lastCursor = DEViseGlobals.defaultCursor;
            } else if (mousePosition == 1 || mousePosition == 5) {
                // inside the date area but not within any cursor, you can draw
                // rubber band now if you want
                setCursor(DEViseGlobals.rbCursor);
                jsc.lastCursor = DEViseGlobals.rbCursor;
            } else if (mousePosition == 2 || mousePosition == 6) {
                // inside a cursor, so you can move cursor around now if you want
                setCursor(DEViseGlobals.moveCursor);
                jsc.lastCursor = DEViseGlobals.moveCursor;
            } else if (mousePosition == 3 || mousePosition == 7) {
                // on the edge of a cursor, so you can change the shape of the
                // cursor now if you want
                switch (activeView.whichSide) {
                case 1:
                    setCursor(DEViseGlobals.lrsCursor);
                    jsc.lastCursor = DEViseGlobals.lrsCursor;
                    break;
                case 2:
                    setCursor(DEViseGlobals.rrsCursor);
                    jsc.lastCursor = DEViseGlobals.rrsCursor;
                    break;
                case 3:
                    setCursor(DEViseGlobals.trsCursor);
                    jsc.lastCursor = DEViseGlobals.trsCursor;
                    break;
                case 4:
                    setCursor(DEViseGlobals.brsCursor);
                    jsc.lastCursor = DEViseGlobals.brsCursor;
                    break;
                case 5:
                    setCursor(DEViseGlobals.tlrsCursor);
                    jsc.lastCursor = DEViseGlobals.tlrsCursor;
                    break;
                case 6:
                    setCursor(DEViseGlobals.blrsCursor);
                    jsc.lastCursor = DEViseGlobals.blrsCursor;
                    break;
                case 7:
                    setCursor(DEViseGlobals.trrsCursor);
                    jsc.lastCursor = DEViseGlobals.trrsCursor;
                    break;
                case 8:
                    setCursor(DEViseGlobals.brrsCursor);
                    jsc.lastCursor = DEViseGlobals.brrsCursor;
                    break;
                }
            }

            //repaint();
        }

        public void mouseReleased(MouseEvent event)
        {
            // Each mouse click will be here once, so double click actually will enter
            // this twice. Also, this event will always reported with each mouse click
            // and before the mouseClick event is reported.

            // I am not processing mouseClick event here
            if (isMouseDragged) {
                Point p = event.getPoint();

                // 8 possible actions could happen here depends on mousePosition
                // 0: move view, disabled
                // 1: draw rubber band
                // 2: move cursor
                // 3: resize cursor
                // 4: move child view, disabled
                // 5: draw rubber band in child view
                // 6: move cursor in child view
                // 7: resize cursor in child view
                String cmd = "";

                if (mousePosition == 1 || mousePosition == 5) {
                    ep.x = activeView.adjustPosX(p.x);
                    ep.y = activeView.adjustPosY(p.y);

                    String xpos = activeView.getX(ep.x), ypos = activeView.getY(ep.y);
                    jsc.viewInfo.updateInfo(xpos, ypos);

                    int w = ep.x - sp.x, h = ep.y - sp.y;
                    if (w < 0)
                        w = -w;
                    if (h < 0)
                        h = -h;

                    if (w > DEViseGlobals.rubberBandLimit.width || h > DEViseGlobals.rubberBandLimit.height && activeView.isRubberBand) {
                        if (activeView.isFirstTime) {
                            cmd = cmd + "JAVAC_MouseAction_Click " + activeView.getCurlyName() + " " +  activeView.xtome(ep.x) + " " + activeView.ytome(ep.y) + "\n";
                        }

                        cmd = cmd + "JAVAC_MouseAction_RubberBand " + activeView.getCurlyName() + " "
                              + activeView.xtome(sp.x) + " " + activeView.ytome(sp.y) + " " + activeView.xtome(ep.x) + " " + activeView.ytome(ep.y);

                        jscreen.guiAction = true;
                        dispatcher.start(cmd);
                    }
                } else if (mousePosition == 2 || mousePosition == 6) {
                    ep.x = activeView.adjustPosX(p.x);
                    ep.y = activeView.adjustPosY(p.y);

                    String xpos = activeView.getX(ep.x), ypos = activeView.getY(ep.y);
                    jsc.viewInfo.updateInfo(xpos, ypos);

                    //activeView.updateCursorLoc(activeView.whichCursor, 1, ep.x - op.x, ep.y - op.y);
                    int dx = ep.x - op.x, dy = ep.y - op.y;
                    
                    if (activeView.whichCursor >= 0 && activeView.whichCursor < activeView.viewCursors.size()) {
                        DEViseCursor cursor = (DEViseCursor)activeView.viewCursors.elementAt(activeView.whichCursor);
                    
                        if (cursor.gridx > 0) {
                            dx = (dx / cursor.gridx) * cursor.gridx;
                        }
                    
                        if (cursor.gridy > 0) {
                            dy = (dy / cursor.gridy) * cursor.gridy;
                        }
                    
                        activeView.updateCursorLoc(activeView.whichCursor, 1, dx, dy);
                    }
                
                    //op.x = ep.x;
                    op.x = op.x + dx;
                    //op.y = ep.y;
                    op.y = op.y + dy;

                    if (activeView.isFirstTime) {
                        cmd = cmd + "JAVAC_MouseAction_Click " + activeView.getCurlyName() + " " +  activeView.xtome(ep.x) + " " + activeView.ytome(ep.y) + "\n";
                    }

                    DEViseCursor cursor = activeView.getCursor(activeView.whichCursor);
                    //cmd = cmd + "JAVAC_CursorChanged " + activeView.getCurlyName() + " "
                    cmd = cmd + "JAVAC_CursorChanged " + cursor.name + " "
                          + cursor.x + " "
                          + cursor.y + " "
                          + cursor.width + " "
                          + cursor.height;

                    cursor.image = null;

                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                } else if (mousePosition == 3 || mousePosition == 7) {
                    ep.x = activeView.adjustPosX(p.x);
                    ep.y = activeView.adjustPosY(p.y);

                    String xpos = activeView.getX(ep.x), ypos = activeView.getY(ep.y);
                    jsc.viewInfo.updateInfo(xpos, ypos);

                    activeView.updateCursorLoc(activeView.whichCursor, 2, ep.x - op.x, ep.y - op.y);

                    if (activeView.isFirstTime) {
                        cmd = cmd + "JAVAC_MouseAction_Click " + activeView.getCurlyName() + " " +  activeView.xtome(ep.x) + " " + activeView.ytome(ep.y) + "\n";
                    }

                    DEViseCursor cursor = activeView.getCursor(activeView.whichCursor);
                    //cmd = cmd + "JAVAC_CursorChanged " + activeView.getCurlyName() + " "
                    cmd = cmd + "JAVAC_CursorChanged " + cursor.name + " "
                          + cursor.x + " "
                          + cursor.y + " "
                          + cursor.width + " "
                          + cursor.height;

                    cursor.image = null;
                    
                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                } else {
                    // we disabled the view moving feature now
                }

                isMouseDragged = false;
                mousePosition = -1;

                repaint();
            }
        }

        public void mouseClicked(MouseEvent event)
        {
            // before this event is reported, mousePressed and mouseReleased events
            // will be reported
            // the position for this event will be within the range of this view

            if (!isMouseDragged && (mousePosition > 0 && mousePosition != 4)) {
                if (event.getClickCount() > 1 && activeView.isDrillDown) {
                    wt.setDC(true);

                    String cmd = "";

                    if (activeView.isFirstTime) {
                        cmd = cmd + "JAVAC_MouseAction_Click " + activeView.getCurlyName() + " " +  activeView.xtome(sp.x) + " " + activeView.ytome(sp.y) + "\n";
                    }

                    cmd = cmd + "JAVAC_MouseAction_DoubleClick " + activeView.getCurlyName() + " " + activeView.xtome(sp.x) + " " + activeView.ytome(sp.y);
                    
                    jscreen.guiAction = true;
                    dispatcher.start(cmd);
                } else {
                    wt.setDC(false);

                    wt.setValue(activeView, activeView.xtome(sp.x), activeView.ytome(sp.y));

                    (new Thread(wt)).start();
                }

                mousePosition = -1;

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
            // in this event, the position might exceed the range of this view
            Point p = event.getPoint();

            isMouseDragged = true;

            // 8 possible actions could happen here depends on mousePosition
            // 0: move view, disabled
            // 1: draw rubber band
            // 2: move cursor
            // 3: resize cursor
            // 4: move child view, disabled
            // 5: draw rubber band in child view
            // 6: move cursor in child view
            // 7: resize cursor in child view
            if (mousePosition == 1 || mousePosition == 5) {
                ep.x = activeView.adjustPosX(p.x);
                ep.y = activeView.adjustPosY(p.y);

                String xpos = activeView.getX(ep.x), ypos = activeView.getY(ep.y);
                jsc.viewInfo.updateInfo(xpos, ypos);

                repaint();
            } else if (mousePosition == 2 || mousePosition == 6) {
                ep.x = activeView.adjustPosX(p.x);
                ep.y = activeView.adjustPosY(p.y);

                String xpos = activeView.getX(ep.x), ypos = activeView.getY(ep.y);
                jsc.viewInfo.updateInfo(xpos, ypos);
                                    
                int dx = ep.x - op.x, dy = ep.y - op.y;
                
                if (activeView.whichCursor >= 0 && activeView.whichCursor < activeView.viewCursors.size()) {
                    DEViseCursor cursor = (DEViseCursor)activeView.viewCursors.elementAt(activeView.whichCursor);
                    
                    if (cursor.gridx > 0) {
                        dx = (dx / cursor.gridx) * cursor.gridx;
                    }
                    
                    if (cursor.gridy > 0) {
                        dy = (dy / cursor.gridy) * cursor.gridy;
                    }
                    
                    activeView.updateCursorLoc(activeView.whichCursor, 1, dx, dy);
                }
                
                //op.x = ep.x;
                op.x = op.x + dx;
                //op.y = ep.y;
                op.y = op.y + dy;

                repaint();
            } else if (mousePosition == 3 || mousePosition == 7) {
                ep.x = activeView.adjustPosX(p.x);
                ep.y = activeView.adjustPosY(p.y);

                String xpos = activeView.getX(ep.x), ypos = activeView.getY(ep.y);
                jsc.viewInfo.updateInfo(xpos, ypos);

                activeView.updateCursorLoc(activeView.whichCursor, 2, ep.x - op.x, ep.y - op.y);

                op.x = ep.x;
                op.y = ep.y;

                repaint();
            } else {
                // we disabled the view moving feature now
                return;
            }
        }

        // set this view as current view
        public void mouseMoved(MouseEvent event)
        {
            // the position of this event will be relative to this view and will not exceed
            // the range of this view, ie, p.x >= 0 && p.x < view.width, p.y >= 0 && p.y < view.height
            Point p = event.getPoint();

            int state = view.checkPos(p);

            // set the current view
            if (state >= 0 && state < 4) {
                if (jscreen.getCurrentView() != view) {
                    jscreen.setCurrentView(view);
                }

                activeView = view;
            } else if (state >= 4 && state < 8) {
                DEViseView v = view.getChild(view.whichChild);

                if (jscreen.getCurrentView() != v) {
                    jscreen.setCurrentView(v);
                }

                activeView = v;
            }

            // show the data at current mouse position, if any
            String name = activeView.viewName, xpos = activeView.getX(p.x), ypos = activeView.getY(p.y);
            jsc.viewInfo.updateInfo(name, xpos, ypos);

            // show correct cursor
            if (jsc.dispatcher.getStatus() == 0) {
                if (state == 0 || state == 4) {
                    // inside the view but not within the data area of that view
                    // you can move the view if you want
                    setCursor(DEViseGlobals.defaultCursor);
                    jsc.lastCursor = DEViseGlobals.defaultCursor;
                } else if (state == 1 || state == 5) {
                    // inside the date area but not within any cursor, you can draw
                    // rubber band now if you want
                    setCursor(DEViseGlobals.rbCursor);
                    jsc.lastCursor = DEViseGlobals.rbCursor;
                } else if (state == 2 || state == 6) {
                    // inside a cursor, so you can move cursor around now if you want
                    setCursor(DEViseGlobals.moveCursor);
                    jsc.lastCursor = DEViseGlobals.moveCursor;
                } else if (state == 3 || state == 7) {
                    // on the edge of a cursor, so you can change the shape of the
                    // cursor now if you want
                    switch (activeView.whichSide) {
                    case 1:
                        setCursor(DEViseGlobals.lrsCursor);
                        jsc.lastCursor = DEViseGlobals.lrsCursor;
                        break;
                    case 2:
                        setCursor(DEViseGlobals.rrsCursor);
                        jsc.lastCursor = DEViseGlobals.rrsCursor;
                        break;
                    case 3:
                        setCursor(DEViseGlobals.trsCursor);
                        jsc.lastCursor = DEViseGlobals.trsCursor;
                        break;
                    case 4:
                        setCursor(DEViseGlobals.brsCursor);
                        jsc.lastCursor = DEViseGlobals.brsCursor;
                        break;
                    case 5:
                        setCursor(DEViseGlobals.tlrsCursor);
                        jsc.lastCursor = DEViseGlobals.tlrsCursor;
                        break;
                    case 6:
                        setCursor(DEViseGlobals.blrsCursor);
                        jsc.lastCursor = DEViseGlobals.blrsCursor;
                        break;
                    case 7:
                        setCursor(DEViseGlobals.trrsCursor);
                        jsc.lastCursor = DEViseGlobals.trrsCursor;
                        break;
                    case 8:
                        setCursor(DEViseGlobals.brrsCursor);
                        jsc.lastCursor = DEViseGlobals.brrsCursor;
                        break;
                    }
                }
            } else {
                // dispatcher still busy waiting or processing server's response
                setCursor(DEViseGlobals.waitCursor);
            }

            repaint();
        }
    }
    // end of class ViewMouseMotionListener
}


// This class is used only for distinguish between doulbe-click and single-click
class WaitThread implements Runnable
{
    private jsdevisec jsc = null;
    private DEViseCmdDispatcher dispatcher = null;
    private boolean dc = false;
    private DEViseView view = null;
    private int viewx, viewy;

    public WaitThread(jsdevisec j)
    {   
        jsc = j;
        dispatcher = jsc.dispatcher;
    }

    public synchronized boolean getDC()
    {
        try {
            wait(300);
        } catch (InterruptedException e) {
        }

        return dc;
    }

    public synchronized void setDC(boolean flag)
    {
        dc = flag;

        if (dc)
            notifyAll();
    }

    public void setValue(DEViseView v, int x, int y)
    {
        view = v;
        viewx = x;
        viewy = y;
    }

    public void run()
    {
        if (!getDC() && view.isCursorMove) {
            String cmd = "";
            if (view.isFirstTime) {
                cmd = cmd + "JAVAC_MouseAction_Click " + view.getCurlyName() + " " +  viewx + " " + viewy + "\n";
            }

            cmd = cmd + "JAVAC_MouseAction_Click " + view.getCurlyName() + " " + viewx + " " + viewy;
            
            jsc.jscreen.guiAction = true;
            dispatcher.start(cmd);
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
