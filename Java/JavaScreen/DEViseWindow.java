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
public class DEViseWindow extends Canvas
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

    boolean isCurrent = false, isFirstTime = true, isMouseDragged = false,
            isKeyPressed = false, isViewFirstTime = true;
    int lastKeyPressed = KeyEvent.VK_UNDEFINED;
    char lastKeyTyped = KeyEvent.CHAR_UNDEFINED;
    int userAction = 0;
    Point sp = new Point(), ep = new Point(), cropP = new Point();

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

        requestFocus();
        repaint();
    }

    public synchronized void setCurrentView(Point point)
    {
        DEViseView old = currentView;

        currentView = null;
        if (point == null) {
            isViewFirstTime = true;
            return;
        }

        int number = allViews.size();
        DEViseView view = null;

        for (int i = 0; i < number; i++) {
            view = (DEViseView)allViews.elementAt(i);
            if (view.isCurrent(point)) {
                currentView = view;
                if (currentView == old)
                    isViewFirstTime = false;
                else
                    isViewFirstTime = true;

                break;
            }
        }

        repaint();
    }

    protected void processEvent(AWTEvent event)
    {
        if (dispatcher.getStatus() == 0)
            return;

        super.processEvent(event);
    }

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

        userAction = 1;
        repaint();
    }

    public void eraseCursor()
    {
        croppedImage = null;
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
        if (winImage != null)  {
            g.drawImage(winImage, 0, 0, this);

            if (isCurrent) {
                if (currentView != null) {
                    // It is guranteed in other place that cropP, ep and sp
                    // will within the rectangle of currentView
                    if (croppedImage != null) {
                        g.drawImage(croppedImage, cropP.x, cropP.y, this);
                    }

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
                    } else {
                        ;// do nothing
                    }

                    Color oldColor = g.getColor();
                    g.setColor(Color.red);
                    Rectangle rect = currentView.getLoc();
                    g.drawRect(rect.x, rect.y, rect.width, rect.height);
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
        public void keyPressed(KeyEvent event)
        {
            isKeyPressed = true;
            lastKeyPressed = event.getKeyCode();
        }

        public void keyReleased(KeyEvent event)
        {
            isKeyPressed = false;
        }

        public void keyTyped(KeyEvent event)
        {
            lastKeyTyped = event.getKeyChar();

            if (lastKeyTyped != KeyEvent.CHAR_UNDEFINED) {
                YGlobals.Ydebugpn("Key \'" + lastKeyTyped + "\' is pressed");

                if (dispatcher.getStatus() < 0) {
                     YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                     dispatcher.startDispatcher();
                     return;
                }

                userAction = 0;
                repaint();

                dispatcher.insertCmd("JAVAC_KeyAction {" + currentView.getName() + "} " + (int)lastKeyTyped);
	        }
        }
    }
    // end of class ViewKeyListener

    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter
    {
        public void mousePressed(MouseEvent event)
        {
            // The starting point will be in this window, otherwise this event
            // will not be catched by this window
            sp = event.getPoint();
            ep = sp;

            setCurrentView(sp);

            if (!isCurrent) {
                isFirstTime = true;
                jscreen.setCurrentWindow(DEViseWindow.this);
            } else {
                isFirstTime = false;
                requestFocus();
            }

            if (currentView == null) {
                jsc.viewInfo.updateInfo(windowName, null, sp.x, sp.y);
            } else {
                jsc.viewInfo.updateInfo(windowName, currentView.getName(), sp.x, sp.y);
            }

            isMouseDragged = false;
        }

        public void mouseReleased(MouseEvent event)
        {
            if (currentView != null && !isFirstTime) {
                ep = adjustViewPoint(event.getPoint());
                if (sp.x == ep.x && sp.y == ep.y && isMouseDragged == false) {
                    userAction = 0;
                    repaint();

                    if (dispatcher.getStatus() < 0) {
                         YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                         dispatcher.startDispatcher();
                         return;
                    }

                    dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + sp.x + " " + sp.y);
                } else {
                    if (isMouseDragged == true) {
                        if (sp.x == ep.x || sp.y == ep.y) {
                            userAction = 0;
                            repaint();

                            if (isViewFirstTime) {
                                if (dispatcher.getStatus() < 0) {
                                     YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                                     dispatcher.startDispatcher();
                                     return;
                                }

                                dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + ep.x + " " + ep.y);
                            }
                        }  else {
                            userAction = 0;
                            repaint();

                            if (dispatcher.getStatus() < 0) {
                                 YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                                 dispatcher.startDispatcher();
                                 return;
                            }

                            if (isViewFirstTime) {
                                dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + ep.x + " " + ep.y);
                            } else {
                                dispatcher.insertCmd("JAVAC_MouseAction_RubberBand {" + windowName + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);
                            }
                        }
                    }
                }
            }
        }

        public void mouseClicked(MouseEvent event)
        {
            if (currentView != null && !isFirstTime) {
                if (event.getClickCount() > 1) {
                    Point p = event.getPoint();
                    if (currentView.isCurrent(p)) {
                        userAction = 0;
                        repaint();

                        if (dispatcher.getStatus() < 0) {
                             YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                             dispatcher.startDispatcher();
                             return;
                        }

                        if (isViewFirstTime)
                            dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + p.x + " " + p.y);
                        else
                            dispatcher.insertCmd("JAVAC_MouseAction_DoubleClick {" + currentView.getName() + "} " + p.x + " " + p.y);
                    }
                }
            } else {
                userAction = 0;
                repaint();
            }
        }
    }
    // end of class ViewMouseListener

    // start of class ViewMouseMotionListener
    class ViewMouseMotionListener extends MouseMotionAdapter
    {
        public void mouseDragged(MouseEvent event)
        {
            if (currentView != null && !isFirstTime && !isViewFirstTime) {
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
            Point point = adjustPoint(event.getPoint());
            if (isCurrent)
                jsc.viewInfo.updateInfo(point.x, point.y);
        }
    }
    // end of class ViewMouseMotionListener
}


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
