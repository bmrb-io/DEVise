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
import  java.applet.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;
import  java.net.*;

// This class represent a image view in DEVise
public class DEViseWindow extends Container
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    DEViseCmdDispatcher dispatcher = null;

    String windowName = null;
    Rectangle windowLoc = null;
    Dimension windowDim = null;
    Image winImage = null;
    Image offScrImg = null;
    Vector allViews = null;
    DEViseView currentView = null;

    boolean isCurrent = false, isViewFirstTime = true, isMouseDragged = false;
    int userAction = 0;
    Point sp = new Point(), ep = new Point(), op = new Point();

    DEViseCursor[] viewCursor = null;
    Image[] cursorImage = null; 
    boolean isWithinCursor = false, isMovingCursor = false;
    int whichCursor = 0, whichSide = 0;

    public DEViseWindow(jsdevisec what, String name, Rectangle loc, Image img, Vector views)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        dispatcher = jsc.dispatcher;
        windowName = name;
        windowLoc = loc;
        winImage = img;
        allViews = views;
        
        if (allViews != null) {
            for (int i = 0; i < allViews.size(); i++) {
                DEViseView view = (DEViseView)allViews.elementAt(i);
                view.setWindow(this);
            }
        }
        
        int imageWidth = winImage.getWidth(this);
        int imageHeight = winImage.getHeight(this);
        windowDim = new Dimension(imageWidth, imageHeight);
        windowLoc.height = imageHeight;
        windowLoc.width = imageWidth;

        this.enableEvents(AWTEvent.MOUSE_EVENT_MASK); 
        this.enableEvents(AWTEvent.KEY_EVENT_MASK);
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

        DEViseView view = null;
        for (int i = 0; i < allViews.size(); i++) {
            view = (DEViseView)allViews.elementAt(i);
            if ((view.getName()).equals(name)) {
                break;
            } else {
                view = null;
            }
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

    protected void processMouseEvent(MouseEvent event)
    {   
        int id = event.getID();
        if (id == MouseEvent.MOUSE_CLICKED|| id == MouseEvent.MOUSE_DRAGGED || id == MouseEvent.MOUSE_PRESSED || id == MouseEvent.MOUSE_RELEASED) {        
            if (dispatcher.getStatus() == 0) {
                YGlobals.Yshowinfo(jsc, "Java Screen still talking to the Server!\nPlease wait or press STOP button!");                
                return;
            }
        }
        
        super.processMouseEvent(event);
    }

    protected void processKeyEvent(KeyEvent event)
    {
        if (dispatcher.getStatus() == 0) {
            YGlobals.Yshowinfo(jsc, "Java Screen still talking to the Server!\nPlease wait or press STOP button!");
            return;
        }
        
        super.processKeyEvent(event);
    }
    
    public void updateCursor()
    {
        viewCursor = null;
        cursorImage = null;
        
        Vector tmp = new Vector();
        for (int i = 0; i < allViews.size(); i++) {
            DEViseView view = (DEViseView)allViews.elementAt(i);
            DEViseCursor[] data = view.getCursor();
            if (data != null) {
                for (int j = 0; j < data.length; j++) {
                    tmp.addElement(data[j]);
                }
            }
        }
        
        if (tmp.size() != 0) {        
            viewCursor = new DEViseCursor[tmp.size()];
            
            for (int i = 0; i < viewCursor.length; i++) 
                viewCursor[i] = (DEViseCursor)tmp.elementAt(i);
        }
        
        buildCursorImage();
        
        repaint();         
    } 
    
    private void buildCursorImage()
    {
        if (viewCursor == null || viewCursor.length == 0) {
            cursorImage = null;
            return;
        }
        
        cursorImage = new Image[viewCursor.length];
        for (int i = 0; i < cursorImage.length; i++) {                
            CropImageFilter croppedFilter = new CropImageFilter(viewCursor[i].x, viewCursor[i].y, viewCursor[i].width, viewCursor[i].height);
            FilteredImageSource source = new FilteredImageSource(winImage.getSource(), croppedFilter);
            
            cursorImage[i] = createImage(source);
            if (cursorImage[i] == null) {
                YGlobals.Ydebugpn("Can not create image for cursor!");
                cursorImage = null;
                break;
            }
            
            source = new FilteredImageSource(cursorImage[i].getSource(), new XORFilter());

            cursorImage[i] = createImage(source);            
            if (cursorImage[i] == null) {
                YGlobals.Ydebugpn("Can not create image for cursor!");
                cursorImage = null;
                break;
            }            
        }
    }
    
    private int checkCursor(Point p)
    {
        if (viewCursor == null || currentView == null) {
            whichCursor = 0;            
            return 0;
        }
        
        for (int i = 0; i < viewCursor.length; i++) {
            if (p.x >= viewCursor[i].x && p.x <= (viewCursor[i].x + viewCursor[i].width)    
                && p.y >= viewCursor[i].y && p.y <= (viewCursor[i].y + viewCursor[i].height)) {

                whichCursor = i;
                                
                if (p.x == viewCursor[i].x) {
                    if (p.y == viewCursor[i].y) {
                        whichSide = 5;
                        return 2;
                    } else if (p.y == viewCursor[i].y + viewCursor[i].height) {
                        whichSide = 6;
                        return 2;
                    } else {
                        whichSide = 1;
                        return 2;
                    }
                } else if (p.x == viewCursor[i].x + viewCursor[i].width) {
                    if (p.y == viewCursor[i].y) {
                        whichSide = 7;
                        return 2;
                    } else if (p.y == viewCursor[i].y + viewCursor[i].height) {
                        whichSide = 8;
                        return 2;
                    } else {
                        whichSide = 2;
                        return 2;
                    }
                } else {
                    if (p.y == viewCursor[i].y) {
                        whichSide = 3;
                        return 2;
                    } else if (p.y == viewCursor[i].y + viewCursor[i].height) {
                        whichSide = 4;
                        return 2;
                    } else {
                        return 1;
                    }
                }
            }
        }
        
        whichCursor = 0;
        return 0;
    }
    
    private void updateCursorPosition(Point p)
    {    
        if (viewCursor == null || currentView == null)
            return;
        
        Rectangle loc = currentView.getLoc();
        
        if (isWithinCursor) {    
            int cx = p.x + viewCursor[whichCursor].x;
            if (cx < loc.x) {
                cx = loc.x;
            } else if (cx > loc.x + loc.width - viewCursor[whichCursor].width) {
                cx = loc.x + loc.width - viewCursor[whichCursor].width;
            }
            int cy = p.y + viewCursor[whichCursor].y;
            if (cy < loc.y) {
                cy = loc.y;
            } else if (cy > loc.y + loc.height - viewCursor[whichCursor].height) {
                cy = loc.y + loc.height - viewCursor[whichCursor].height;
            }
        
            viewCursor[whichCursor].x = cx;
            viewCursor[whichCursor].y = cy;
        } else if (isMovingCursor) {
            int tmpx, tmpy, tx, ty, cx, cy;
            boolean isXChange = false, isYChange = false;
            
            switch (whichSide) {
            case 1: // left side
                tmpx = viewCursor[whichCursor].x + viewCursor[whichCursor].width;
                cx = p.x + viewCursor[whichCursor].x;
                
                if (cx < loc.x) {
                    cx = loc.x;
                } else if (cx > loc.x + loc.width - viewCursor[whichCursor].width) {
                    cx = loc.x + loc.width - viewCursor[whichCursor].width;
                }
                
                if (cx >= tmpx) {
                    viewCursor[whichCursor].x = tmpx;
                    viewCursor[whichCursor].width = cx - tmpx;
                    if (viewCursor[whichCursor].width == 0) {
                        viewCursor[whichCursor].width = 1;
                    }
                    
                    whichSide = 2;
                } else {
                    viewCursor[whichCursor].x = cx;
                    viewCursor[whichCursor].width = tmpx - cx;
                }
            
                break;
            case 2: // right side
                tmpx = viewCursor[whichCursor].x + viewCursor[whichCursor].width;
                cx = p.x + tmpx;
                if (cx < loc.x) {
                    cx = loc.x;
                } else if (cx > loc.x + loc.width) {
                    cx = loc.x + loc.width;
                }
                
                tx = viewCursor[whichCursor].x;
                if (tx >= cx) {
                    viewCursor[whichCursor].x = cx;
                    viewCursor[whichCursor].width = tx - cx;
                    if (viewCursor[whichCursor].width == 0) {
                        viewCursor[whichCursor].width = 1;
                    }
                    
                    whichSide = 1;
                } else {
                    viewCursor[whichCursor].width = cx - tx;
                }
            
                break;
            case 3: // top side
                tmpy = viewCursor[whichCursor].y + viewCursor[whichCursor].height;
                cy = p.y + viewCursor[whichCursor].y;
                
                if (cy < loc.y) {
                    cy = loc.y;
                } else if (cy > loc.y + loc.height - viewCursor[whichCursor].height) {
                    cy = loc.y + loc.height - viewCursor[whichCursor].height;
                }
                
                if (cy >= tmpy) {
                    viewCursor[whichCursor].y = tmpy;
                    viewCursor[whichCursor].height = cy - tmpy;
                    if (viewCursor[whichCursor].height == 0) {
                        viewCursor[whichCursor].height = 1;
                    }
                    
                    whichSide = 4;
                } else {
                    viewCursor[whichCursor].y = cy;
                    viewCursor[whichCursor].height = tmpy - cy;
                }
            
                break;
            case 4: // bottom side
                tmpy = viewCursor[whichCursor].y + viewCursor[whichCursor].height;
                cy = p.y + tmpy;

                if (cy < loc.y) {
                    cy = loc.y;
                } else if (cy > loc.y + loc.height) {
                    cy = loc.y + loc.height;
                }                
                
                ty = viewCursor[whichCursor].y;
                if (ty >= cy) {
                    viewCursor[whichCursor].y = cy;
                    viewCursor[whichCursor].height = ty - cy;
                    if (viewCursor[whichCursor].height == 0) {
                        viewCursor[whichCursor].height = 1;
                    }
                    
                    whichSide = 3;
                } else {
                    viewCursor[whichCursor].height = cy - ty;
                }
                
                //YGlobals.Ydebugpn("c.width " + viewCursor[whichCursor].width + " c.height " + viewCursor[whichCursor].height
                //                  + " c.x " + viewCursor[whichCursor].x + " c.y " + viewCursor[whichCursor].y
                //                  + " p.x " + p.x + " p.y " + p.y + " loc.x " + loc.x + " loc.y " + loc.y
                //                  + " loc.width " + loc.width + " loc.height" + loc.height);
                break;
            case 5: // left top corner
                tmpx = viewCursor[whichCursor].x + viewCursor[whichCursor].width;
                cx = p.x + viewCursor[whichCursor].x;
                isXChange = false;
                isYChange = false;
                
                if (cx < loc.x) {
                    cx = loc.x;
                } else if (cx > loc.x + loc.width - viewCursor[whichCursor].width) {
                    cx = loc.x + loc.width - viewCursor[whichCursor].width;
                }
                
                if (cx >= tmpx) {
                    viewCursor[whichCursor].x = tmpx;
                    viewCursor[whichCursor].width = cx - tmpx;
                    if (viewCursor[whichCursor].width == 0) {
                        viewCursor[whichCursor].width = 1;
                    }
                    
                    isXChange = true;
                } else {
                    viewCursor[whichCursor].x = cx;
                    viewCursor[whichCursor].width = tmpx - cx;
                }
            
                tmpy = viewCursor[whichCursor].y + viewCursor[whichCursor].height;
                cy = p.y + viewCursor[whichCursor].y;
                
                if (cy < loc.y) {
                    cy = loc.y;
                } else if (cy > loc.y + loc.height - viewCursor[whichCursor].height) {
                    cy = loc.y + loc.height - viewCursor[whichCursor].height;
                }
                
                if (cy >= tmpy) {
                    viewCursor[whichCursor].y = tmpy;
                    viewCursor[whichCursor].height = cy - tmpy;
                    if (viewCursor[whichCursor].height == 0) {
                        viewCursor[whichCursor].height = 1;
                    }
                    
                    isYChange = true;
                } else {
                    viewCursor[whichCursor].y = cy;
                    viewCursor[whichCursor].height = tmpy - cy;
                }
                
                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            whichSide = 8;
                        } else {
                            whichSide = 7;
                        }
                    } else {
                        whichSide = 6;
                    }
                } 
                                
                break;
            case 6: // left bottom corner
                tmpx = viewCursor[whichCursor].x + viewCursor[whichCursor].width;
                cx = p.x + viewCursor[whichCursor].x;
                isXChange = false;
                isYChange = false;
                
                if (cx < loc.x) {
                    cx = loc.x;
                } else if (cx > loc.x + loc.width - viewCursor[whichCursor].width) {
                    cx = loc.x + loc.width - viewCursor[whichCursor].width;
                }
                
                if (cx >= tmpx) {
                    viewCursor[whichCursor].x = tmpx;
                    viewCursor[whichCursor].width = cx - tmpx;
                    if (viewCursor[whichCursor].width == 0) {
                        viewCursor[whichCursor].width = 1;
                    }
                    
                    isXChange = true;
                } else {
                    viewCursor[whichCursor].x = cx;
                    viewCursor[whichCursor].width = tmpx - cx;
                }
            
                tmpy = viewCursor[whichCursor].y + viewCursor[whichCursor].height;
                cy = p.y + tmpy;

                if (cy < loc.y) {
                    cy = loc.y;
                } else if (cy > loc.y + loc.height) {
                    cy = loc.y + loc.height;
                }                
                
                ty = viewCursor[whichCursor].y;
                if (ty >= cy) {
                    viewCursor[whichCursor].y = cy;
                    viewCursor[whichCursor].height = ty - cy;
                    if (viewCursor[whichCursor].height == 0) {
                        viewCursor[whichCursor].height = 1;
                    }
                    
                    isYChange = true;
                } else {
                    viewCursor[whichCursor].height = cy - ty;
                }
                
                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            whichSide = 7;
                        } else {
                            whichSide = 8;
                        }
                    } else {
                        whichSide = 5;
                    }
                } 
                                
                break;
            case 7: // right top corner
                tmpx = viewCursor[whichCursor].x + viewCursor[whichCursor].width;
                cx = p.x + tmpx;
                isXChange = false;
                isYChange = false;

                if (cx < loc.x) {
                    cx = loc.x;
                } else if (cx > loc.x + loc.width) {
                    cx = loc.x + loc.width;
                }
                
                tx = viewCursor[whichCursor].x;
                if (tx >= cx) {
                    viewCursor[whichCursor].x = cx;
                    viewCursor[whichCursor].width = tx - cx;
                    if (viewCursor[whichCursor].width == 0) {
                        viewCursor[whichCursor].width = 1;
                    }
                    
                    isXChange = true;
                } else {
                    viewCursor[whichCursor].width = cx - tx;
                }
            
                tmpy = viewCursor[whichCursor].y + viewCursor[whichCursor].height;
                cy = p.y + viewCursor[whichCursor].y;
                
                if (cy < loc.y) {
                    cy = loc.y;
                } else if (cy > loc.y + loc.height - viewCursor[whichCursor].height) {
                    cy = loc.y + loc.height - viewCursor[whichCursor].height;
                }
                
                if (cy >= tmpy) {
                    viewCursor[whichCursor].y = tmpy;
                    viewCursor[whichCursor].height = cy - tmpy;
                    if (viewCursor[whichCursor].height == 0) {
                        viewCursor[whichCursor].height = 1;
                    }
                    
                    isYChange = true;
                } else {
                    viewCursor[whichCursor].y = cy;
                    viewCursor[whichCursor].height = tmpy - cy;
                }
                
                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            whichSide = 6;
                        } else {
                            whichSide = 5;
                        }
                    } else {
                        whichSide = 8;
                    }
                } 
                                
                break;
            case 8: // right bottom corner
                tmpx = viewCursor[whichCursor].x + viewCursor[whichCursor].width;
                cx = p.x + tmpx;
                isXChange = false;
                isYChange = false;

                if (cx < loc.x) {
                    cx = loc.x;
                } else if (cx > loc.x + loc.width) {
                    cx = loc.x + loc.width;
                }
                
                tx = viewCursor[whichCursor].x;
                if (tx >= cx) {
                    viewCursor[whichCursor].x = cx;
                    viewCursor[whichCursor].width = tx - cx;
                    if (viewCursor[whichCursor].width == 0) {
                        viewCursor[whichCursor].width = 1;
                    }
                    
                    isXChange = true;
                } else {
                    viewCursor[whichCursor].width = cx - tx;
                }

                tmpy = viewCursor[whichCursor].y + viewCursor[whichCursor].height;
                cy = p.y + tmpy;

                if (cy < loc.y) {
                    cy = loc.y;
                } else if (cy > loc.y + loc.height) {
                    cy = loc.y + loc.height;
                }                
                
                ty = viewCursor[whichCursor].y;
                if (ty >= cy) {
                    viewCursor[whichCursor].y = cy;
                    viewCursor[whichCursor].height = ty - cy;
                    if (viewCursor[whichCursor].height == 0) {
                        viewCursor[whichCursor].height = 1;
                    }
                    
                    isYChange = true;
                } else {
                    viewCursor[whichCursor].height = cy - ty;
                }
                
                if (isXChange || isYChange) {
                    if (isXChange) {
                        if (isYChange) {
                            whichSide = 5;
                        } else {
                            whichSide = 6;
                        }
                    } else {
                        whichSide = 7;
                    }
                } 
                                
                break;
            }
        }
        
        //YGlobals.Ydebugpn("p.x " + p.x + " p.y " + p.y + " c.x " + viewCursor[whichCursor].x
        //+ " c.y " + viewCursor[whichCursor].y + " c.w " + viewCursor[whichCursor].width      
        //+ " c.h " + viewCursor[whichCursor].height + " v.x " + loc.x + " v.y " + loc.y
        //+ " v.w " + loc.width + " v.h " + loc.height + " cy " + cy + " cx " + cx);
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
            
            if (viewCursor != null) {
                if (cursorImage != null) {
                    for (int i = 0; i < cursorImage.length; i++) {
                        if ((isWithinCursor || isMovingCursor) && whichCursor == i) {                            
                        } else {
                            g.drawImage(cursorImage[i], viewCursor[i].x, viewCursor[i].y, this);
                        }
                    }
                }
            }

            if (isCurrent) {
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
                        userAction = 0;
                    } else if (userAction == 3) {
                        Color oldColor = g.getColor();
                        g.setColor(Color.yellow);
                        g.drawRect(viewCursor[whichCursor].x, viewCursor[whichCursor].y, viewCursor[whichCursor].width, viewCursor[whichCursor].height);
                        g.setColor(oldColor);
                        userAction = 0;
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
                } else {                 
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
                    repaint();
                    dispatcher.insertCmd("JAVAC_KeyAction {" + currentView.getName() + "} " + actualKey);
                    setCursor(DEViseGlobals.waitcursor);
                }
            }
        }
    }
    // end of class ViewKeyListener

    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter
    {
        WaitThread wt = new WaitThread(dispatcher, DEViseWindow.this);

        // event sequence: 1. mousePressed 2. mouseReleased 3. mouseClicked
        public void mousePressed(MouseEvent event)
        {
            // The starting point will be in this window, otherwise this event
            // will not be catched by this window

            // Each mouse click will be here once, so double click actually will enter
            // this twice
            sp = event.getPoint();
            ep.x = op.x = sp.x;
            op.y = ep.y = sp.y;

            //YGlobals.Ydebugpn("Mouse pressed with click counts " + event.getClickCount() + "!");
            isMouseDragged = false; 
            int state = checkCursor(ep);
            if (state == 1) {                
                isWithinCursor = true;
                //updateCursorPosition(new Point(0, 0));
                userAction = 3;
            } else if (state == 2) {
                isMovingCursor = true;
                userAction = 3;
            }
                
            jscreen.setClickWindow(DEViseWindow.this);

            repaint();
        }

        public void mouseReleased(MouseEvent event)
        {
            // Each mouse click will be here once, so double click actually will enter
            // this twice. Also, this event will always reported with each mouse click
            // and before the mouseClick event is reported.

            //YGlobals.Ydebugpn("Mouse released!");
            
            if (isMouseDragged) {
                if (currentView != null) {
                    // clear the mouse drag box
                    if (isWithinCursor || isMovingCursor) {
                        isWithinCursor = false;
                        isMovingCursor = false;
                        ep = adjustViewPoint(event.getPoint());
                        updateCursorPosition(new Point(ep.x - op.x, ep.y - op.y));
                        buildCursorImage();
                        repaint();
                        int cx = viewCursor[whichCursor].x + viewCursor[whichCursor].width / 2;
                        int cy = viewCursor[whichCursor].y + viewCursor[whichCursor].height / 2;                        
                        if (isViewFirstTime) {
                            dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + cx + " " + cy
                                    + "\nJAVAC_MouseAction_Click {" + currentView.getName() + "} " + cx + " " + cy);
                        } else {
                            dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + cx + " " + cy);
                        }    
                    } else {
                        repaint();

                        ep = adjustViewPoint(event.getPoint());
                        if (sp.x == ep.x || sp.y == ep.y) {
                            dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + ep.x + " " + ep.y);
                        } else {
                            if (isViewFirstTime) {
                                dispatcher.insertCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + ep.x + " " + ep.y
                                                 + "\nJAVAC_MouseAction_RubberBand {" + windowName + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);
                                isViewFirstTime = false;
                                setCursor(DEViseGlobals.waitcursor);
                            } else {
                                dispatcher.insertCmd("JAVAC_MouseAction_RubberBand {" + windowName + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);
                                setCursor(DEViseGlobals.waitcursor);
                            }
                        }
                    }
                } else {
                    jscreen.updateWindowPos(DEViseWindow.this, 0, 0, true);
                }
            } else {
                isWithinCursor = false;
                repaint();
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
                        isViewFirstTime = false;  
                        setCursor(DEViseGlobals.waitcursor);
                    } else {
                        dispatcher.insertCmd("JAVAC_MouseAction_DoubleClick {" + currentView.getName() + "} " + p.x + " " + p.y);
                        setCursor(DEViseGlobals.waitcursor);
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
            Point p = event.getPoint();
            isMouseDragged = true;
            
            if (currentView != null) {
                ep = adjustViewPoint(p);
                jsc.viewInfo.updateInfo(ep.x, ep.y);
                
                if (isWithinCursor || isMovingCursor) {
                    updateCursorPosition(new Point(ep.x - op.x, ep.y - op.y));
                    op.x = ep.x;
                    op.y = ep.y;
                    userAction = 3;
                } else {                   
                    userAction = 2;
                }
                
                repaint();
            } else {
                jscreen.updateWindowPos(DEViseWindow.this, p.x - sp.x, p.y - sp.y, false);
            }
        }

        public void mouseMoved(MouseEvent event)
        {    
            Point p = event.getPoint(); 
            if (!isCurrent) {
                jscreen.setCurrentWindow(DEViseWindow.this); 
                jscreen.setFocusToCurrentWindow();
            }

            if (currentView != null && currentView.isCurrent(p)) {
                //isViewFirstTime = false;
                jsc.viewInfo.updateInfo(p.x, p.y);

                if (jsc.dispatcher.getStatus() != 0) {
                    int state = checkCursor(p);
                    if (state == 0) {
                        setCursor(DEViseGlobals.pointercursor);
                    } else if (state == 1) {
                        setCursor(DEViseGlobals.handcursor);
                    } else if (state == 2) {
                        setCursor(DEViseGlobals.movecursor);
                    }
                } else {
                    setCursor(DEViseGlobals.waitcursor);
                }
                    
                return;
            } else {
                if (jscreen.getClickWindow() != DEViseWindow.this) {
                    isViewFirstTime = true;
                }
                currentView = null;
            }

            for (int j = 0; j < allViews.size(); j++) {
                DEViseView view = (DEViseView)allViews.elementAt(j);
                if (view.isCurrent(p)) {                   
                    currentView = view;

                    if (jsc.dispatcher.getStatus() != 0) {
                        int state = checkCursor(p);
                        if (state == 0) {
                            setCursor(DEViseGlobals.pointercursor);
                        } else if (state == 1) {
                            setCursor(DEViseGlobals.handcursor);
                        } else if (state == 2) {
                            setCursor(DEViseGlobals.movecursor);
                        }
                    } else {
                        setCursor(DEViseGlobals.waitcursor);
                    }
                    
                    jsc.viewInfo.updateInfo(getName(), view.getName(), p.x, p.y);
                    repaint();
                    return;
                }                                
            }
            
            if (jsc.dispatcher.getStatus() != 0) {
                setCursor(DEViseGlobals.movecursor);
            } else {
                setCursor(DEViseGlobals.waitcursor);
            }
            
            jsc.viewInfo.updateInfo(getName(), null, p.x, p.y);
            repaint();
        }
    }
    // end of class ViewMouseMotionListener
}

class WaitThread implements Runnable
{
    private DEViseCmdDispatcher dispatcher = null;
    private DEViseWindow window = null;
    private boolean dc = false;
    private String view = null;
    private int viewx, viewy;

    public WaitThread(DEViseCmdDispatcher dp, DEViseWindow win)
    {
        dispatcher = dp;
        window = win;
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
            if (window.isViewFirstTime) { 
                dispatcher.insertCmd("JAVAC_MouseAction_Click {" + view + "} " + viewx + " " + viewy
                    + "\nJAVAC_MouseAction_Click {" + view + "} " + viewx + " " + viewy);
                window.isViewFirstTime = false;
                window.setCursor(DEViseGlobals.waitcursor);
            } else {
                dispatcher.insertCmd("JAVAC_MouseAction_Click {" + view + "} " + viewx + " " + viewy);
                window.setCursor(DEViseGlobals.waitcursor);
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
