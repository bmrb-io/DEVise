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
// Revision 1.6  1998/08/14 17:48:09  hongyu
// *** empty log message ***
//
// Revision 1.2  1998/06/11 15:07:48  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import  java.awt.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;

// This class represent a image view in DEVise
public class DEViseWindow extends Component
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    DEViseCmdDispatcher dispatcher = null;

    String windowName = null;
    Rectangle windowLoc = null;
    Dimension windowDim = null;
    Image winImage = null;
    Image offScrImg = null;
    Image croppedImage = null;
    Vector allViews = null;
    DEViseView currentView = null;

    boolean isCurrent = false, isViewFirstTime = true, isMouseDragged = false;
    int userAction = 0;
    Point sp = new Point(), ep = new Point(), cropP = new Point();

    Rectangle[] viewGData = null;
    Image[] GDataImage = null;

    public DEViseWindow(jsdevisec what, String name, Rectangle loc, Image img, Vector views)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        dispatcher = jsc.dispatcher;
        windowName = name;
        windowLoc = loc;
        winImage = img;
        allViews = views;

        int imageWidth = winImage.getWidth(this);
        int imageHeight = winImage.getHeight(this);
        windowDim = new Dimension(imageWidth, imageHeight);
        windowLoc.height = imageHeight;
        windowLoc.width = imageWidth;

        //this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        addMouseListener(new ViewMouseListener());
        addMouseMotionListener(new ViewMouseMotionListener());
        addKeyListener(new ViewKeyListener());
    }

    public Dimension getPreferredSize()
    {
        return windowDim;
    }

    public Dimension getMinimumSize()
    {
        return windowDim;
    }

    public String getName()
    {
        return windowName;
    }

    public Rectangle getLoc()
    {
        return windowLoc;
    }

    public Dimension getDim()
    {
        return windowDim;
    }

    public synchronized DEViseView getCurrentView()
    {
        return currentView;
    }

    public synchronized Vector getAllViews()
    {
        return allViews;
    }

    public synchronized DEViseView getView(String name)
    {
        if (name == null)
            return null;

        int number = allViews.size();
        DEViseView view = null;

        for (int i = 0; i < number; i++) {
            view = (DEViseView)allViews.elementAt(i);
            if (view.getName().equals(name))
                break;
        }

        return view;
    }

    // At this moment, the image size for a window can not be changed during
    // a session
    public synchronized void setImage(Image img)
    {
        if (img == null)
            return;

        int imageWidth = img.getWidth(this);
        int imageHeight = img.getHeight(this);
        if (imageWidth != windowDim.width || imageHeight != windowDim.height) {
            YGlobals.Ydebugpn("Different sized image received for window " + windowName);
            return;
        }

        winImage = img;
        // Necessary to draw correct image because double-buffering is used
        offScrImg = null;

        repaint();
    }

    // While a moust click occured in this window, the 'setCurrentWindow' of
    // DEViseScreen will be called, and 'setCurrentWindow' will call this
    // this function
    public synchronized void setCurrent(boolean flag)
    {
        isCurrent = flag;

        if (isCurrent == false)
            currentView = null;

        repaint();
    }

    public synchronized void setCurrentView(Point point)
    {
        if (point == null) {
            return;
        }

        if (currentView != null && currentView.isCurrent(point)) {
            isViewFirstTime = false;
            jsc.viewInfo.updateInfo(point.x, point.y);
            return;
        } else {
            isViewFirstTime = true;
        }

        currentView = null;
        int number = allViews.size();
        DEViseView view = null;

        for (int i = 0; i < number; i++) {
            view = (DEViseView)allViews.elementAt(i);
            if (view.isCurrent(point)) {
                currentView = view;
                break;
            }
        }

        if (currentView == null) {
            jsc.viewInfo.updateInfo(windowName, null, point.x, point.y);
        } else {
            jsc.viewInfo.updateInfo(windowName, currentView.getName(), point.x, point.y);
        }
    }

    //protected void processEvent(AWTEvent event)
    //{
    //    if (dispatcher.getStatus() == 0)
    //        return;
    //    super.processEvent(event);
    //}

    public void drawCursor(Rectangle loc)
    {
        if (loc == null)
            return;

        // Should also check that loc is within the range of current view
        CropImageFilter croppedFilter = new CropImageFilter(loc.x, loc.y, loc.width, loc.height);
        FilteredImageSource source = new FilteredImageSource(winImage.getSource(), croppedFilter);
        croppedImage = createImage(source);
        source = new FilteredImageSource(croppedImage.getSource(), new XORFilter());
        croppedImage = createImage(source);
        cropP.x = loc.x;
        cropP.y = loc.y;

        repaint();
    }

    public void eraseCursor()
    {
        croppedImage = null;
        repaint();
    }

    public void updateGData(Vector gdata)
    {
        if (gdata == null || gdata.size() == 0) {
            GDataImage = null;
            viewGData = null;
            return;
        }

        viewGData = new Rectangle[gdata.size()];
        for (int i = 0; i < viewGData.length; i++) {
            viewGData[i] = (Rectangle)gdata.elementAt(i);
        }
    }

    // Enable reliable light weight component paint
    public void repaint()
    {
        Graphics g = getGraphics();

        if (g == null)
            return;
        else
            update(g);
    }

    // Enable double-buffering
    public void update(Graphics g)
    {
        if (offScrImg == null)
            offScrImg = createImage(windowDim.width, windowDim.height);

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }

    public void paint(Graphics g)
    {
        if (winImage != null) {
            g.drawImage(winImage, 0, 0, this);

            if (viewGData != null) {
                /*
                Color oldColor = g.getColor();
                g.setColor(Color.yellow);

                for (int i = 0; i < viewGData.length; i++) {
                    g.fillRect(viewGData[i].x - viewGData[i].width / 2, viewGData[i].y - viewGData[i].height / 2, viewGData[i].width, viewGData[i].height);
                }

                g.setColor(oldColor);
                */
                if (GDataImage == null) {
                    GDataImage = new Image[viewGData.length];
                    for (int i = 0; i < viewGData.length; i++) {
                        GDataImage[i] = createImage(viewGData[i].width, viewGData[i].height);
                        if (GDataImage[i] == null) {
                            YGlobals.Ydebugpn("Can not create image for GData!");
                            GDataImage = null;
                            break;
                        }

                        Graphics tg = GDataImage[i].getGraphics();
                        tg.setColor(Color.yellow);
                        tg.fillRect(0, 0, viewGData[i].width, viewGData[i].height);
                        tg.dispose();
                    }
                }

                if (GDataImage != null) {
                    for (int i = 0; i < GDataImage.length; i++) {
                        g.drawImage(GDataImage[i], viewGData[i].x - viewGData[i].width / 2, viewGData[i].y - viewGData[i].height / 2, this);
                    }
                }

            }

            if (croppedImage != null) {
                g.drawImage(croppedImage, cropP.x, cropP.y, this);
            }

            if (isCurrent) {
                jscreen.requestFocus();
                requestFocus();

                if (currentView != null) {
                    // It is guranteed in other place that cropP, ep and sp
                    // will within the rectangle of currentView
                    if (userAction == 2) {
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

                        g.drawRect(x0, y0, w, h);
                    }

                    Color oldColor = g.getColor();
                    g.setColor(Color.red);
                    Rectangle rect = currentView.getLoc();
                    int w = rect.width, h = rect.height, x0 = rect.x, y0 = rect.y;

                    if ((w + x0) < windowDim.width) {
                        w += 1;
                    } else {
                        w = windowDim.width - x0;
                    }

                    if ((h + y0) < windowDim.height) {
                        h += 1;
                    } else {
                        h = windowDim.height - y0;
                    }

                    g.drawRect(x0, y0, w - 1, h - 1);
                    g.setColor(oldColor);
                }  else {
                    Color oldColor = g.getColor();
                    g.setColor(Color.red);
                    g.drawRect(0, 0, windowDim.width - 1, windowDim.height - 1);
                    g.setColor(oldColor);
                }
            }
        }
    }

    public Point adjustPoint(Point p)
    {
        if (p == null)
            return new Point(0, 0);

        if (p.x < 0)
            p.x = 0;
        if (p.x >= windowDim.width)
            p.x = windowDim.width - 1;
        if (p.y < 0)
            p.y = 0;
        if (p.y >= windowDim.height)
            p.y = windowDim.height - 1;

        return p;
    }

    public Point adjustViewPoint(Point p)
    {
        // It is guranteed in other place that currentView will not be null
        Rectangle loc = currentView.getLoc();
        if (p == null)
            return new Point(loc.x, loc.y);

        if (p.x < loc.x)
            p.x = loc.x;
        if (p.x >= loc.x + loc.width)
            p.x = loc.x + loc.width - 1;
        if (p.y < loc.y)
            p.y = loc.y;
        if (p.y >= loc.y + loc.height)
            p.y = loc.y + loc.height - 1;

        return p;
    }

    // start of class ViewKeyListener
    class ViewKeyListener extends KeyAdapter
    {
        // event sequence: 1. keypressed 2.keytyped 3.keyreleased
        public void keyPressed(KeyEvent event)
        {
            //YGlobals.Ydebugpn("Key " + event.getKeyCode() + " pressed!");
            //YGlobals.Ydebugpn("Character " + event.getKeyChar() + " pressed!");
        }

        public void keyReleased(KeyEvent event)
        {
            //YGlobals.Ydebugpn("Key " + event.getKeyCode() + " released!");
            //YGlobals.Ydebugpn("Character " + event.getKeyChar() + " released!");

            char keyChar = event.getKeyChar();
            int keyCode = event.getKeyCode();
            int actualKey = 0;

            if (currentView != null) {
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

                if (actualKey != 0) {
                    //userAction = 0;
                    //repaint();
                    dispatcher.insertCmd("JAVAC_KeyAction {" + currentView.getName() + "} " + actualKey);
                }
            }
        }
    }
    // end of class ViewKeyListener

    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter
    {
        WaitThread wt = new WaitThread(dispatcher);

        // event sequence: 1. mousePressed 2. mouseReleased 3. mouseClicked
        public void mousePressed(MouseEvent event)
        {
            // The starting point will be in this window, otherwise this event
            // will not be catched by this window

            // Each mouse click will be here once, so double click actually will enter
            // this twice
            sp = event.getPoint();
            ep = sp;

            //YGlobals.Ydebugpn("Mouse pressed with click counts " + event.getClickCount() + "!");

            if (!isCurrent) {
                jscreen.setCurrentWindow(DEViseWindow.this);
            }

            isMouseDragged = false;

            setCurrentView(sp);

            userAction = 0;
            repaint();
        }

        public void mouseReleased(MouseEvent event)
        {
            // Each mouse click will be here once, so double click actually will enter
            // this twice. Also, this event will always reported with each mouse click
            // and before the mouseClick event is reported.

            //YGlobals.Ydebugpn("Mouse released!");

            if (currentView != null && isMouseDragged) {
                // clear the mouse drag box
                userAction = 0;
                repaint();

                ep = adjustViewPoint(event.getPoint());
                if (sp.x == ep.x || sp.y == ep.y) {
                    dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + ep.x + " " + ep.y);
                } else {
                    if (isViewFirstTime) {
                        dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + ep.x + " " + ep.y
                                             + "\nJAVAC_MouseAction_RubberBand {" + windowName + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);
                    } else {
                        dispatcher.insertCmd("JAVAC_MouseAction_RubberBand {" + windowName + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);
                    }
                }
            }
        }

        public void mouseClicked(MouseEvent event)
        {
            // before this event is reported, mousePressed and mouseReleased events
            // will be reported

            //YGlobals.Ydebugpn("Mouse Clicked with click count " + event.getClickCount() + "!");

            if (currentView != null && !isMouseDragged) {
                if (event.getClickCount() > 1) {
                    Point p = event.getPoint();

                    wt.setDC(true);
                    if (isViewFirstTime) {
                        dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + p.x + " " + p.y
                                             + "\nJAVAC_MouseAction_DoubleClick {" + currentView.getName() + "} " + p.x + " " + p.y);
                    } else {
                        dispatcher.insertCmd("JAVAC_MouseAction_DoubleClick {" + currentView.getName() + "} " + p.x + " " + p.y);
                    }
                } else {
                    Point p = event.getPoint();

                    wt.setDC(false);
                    wt.setValue(currentView.getName(), p.x, p.y);
                    (new Thread(wt)).start();
                }
            }
        }

    }
    // end of class ViewMouseListener

    // start of class ViewMouseMotionListener
    class ViewMouseMotionListener extends MouseMotionAdapter
    {
        public void mouseDragged(MouseEvent event)
        {
            //YGlobals.Ydebugpn("Mouse Dragged!");

            if (currentView != null) {
                isMouseDragged = true;

                ep = adjustViewPoint(event.getPoint());
                jsc.viewInfo.updateInfo(ep.x, ep.y);

                userAction = 2;
                repaint();
            } else {
                Point point = adjustPoint(event.getPoint());
                jsc.viewInfo.updateInfo(point.x, point.y);
            }
        }

        public void mouseMoved(MouseEvent event)
        {
            //YGlobals.Ydebugpn("Mouse Moved!");

            Point point = adjustPoint(event.getPoint());

            if (isCurrent)
                jsc.viewInfo.updateInfo(point.x, point.y);
        }
    }
    // end of class ViewMouseMotionListener
}

class WaitThread implements Runnable
{
    private DEViseCmdDispatcher dispatcher = null;
    private boolean dc = false;
    private String view = null;
    private int viewx, viewy;

    public WaitThread(DEViseCmdDispatcher dp)
    {
        dispatcher = dp;
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

    public void setValue(String name, int x, int y)
    {
        view = name;
        viewx = x;
        viewy = y;
    }

    public void run()
    {
        if (!getDC()) {
            dispatcher.insertCmd("JAVAC_MouseAction_Click {" + view + "} " + viewx + " " + viewy);
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
