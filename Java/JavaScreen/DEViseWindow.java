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
    
    boolean isCurrent = false, isPopup = false, isMouseDragged = false, isKeyPressed = false;
    int lastKey = KeyEvent.VK_UNDEFINED;
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
        
    public DEViseView getCurrentView()
    {
        return currentView;
    }
    
    public Vector getAllViews()
    {
        return allViews;
    }
        
    public DEViseView getView(String name)
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
            YDebug.println("At this moment, the image size for a window can not be changed during a session!");
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
    public void setCurrent(boolean flag)
    {
        isCurrent = flag;
        if (isCurrent == false)
            currentView = null;
        
        requestFocus();    
        repaint();
    }
    
    public void setCurrentView(Point point)
    {   
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
        
        repaint();
    }
    
    protected void processEvent(AWTEvent event)
    {
        int status = dispatcher.getStatus();
        
        if (status != 2) {
            if (status > 2) {
                jsc.startDispatcher();
            }
                        
            super.processEvent(event);
        }                    
    }    
    /*    
    public void updateGData(String[] data)
    {
        gdataName.removeAllElements();
        gdataValue.removeAllElements();
        
        if (data.length > 1) {
            int i = 1;
            while (i < data.length) {
                gdataName.addElement(data[i++]);
                gdataValue.addElement(data[i++]);
            }
        }    
            
        jsc.viewControl.updateControl(); 
    }  
    */    
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
                    if (userAction == 1) {
                        g.drawImage(croppedImage, cropP.x, cropP.y, this);
                    } else if (userAction == 2) {
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
                        
                        /*
                        if (jscreen.isGrid) {
                            Rectangle rect = currentView.getLoc();
                            int x1 = x0 + w, y1 = y0 + h, step = currentView.getStep();
                            x0 = (x0 / step) * step;
                            y0 = (y0 / step) * step;
                            if (dim.width - x1 < step)
                                x1 = dim.width;
                            else 
                                x1 = (x1 / 20 + 1) * 20;
                            if (imageHeight - y1 < 20)
                                y1 = imageHeight;
                            else
                                y1 = (y1 / 20 + 1) * 20;
                        
                            w = x1 - x0;
                            h = y1 - y0;                        
                        }
                        */
                        
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
        //boolean isKeyPressed;
        //int lastKey;
        
        public void keyPressed(KeyEvent event)
        {
            isKeyPressed = true;
            lastKey = event.getKeyCode();    
        }
        
        public void keyReleased(KeyEvent event)
        {
            isKeyPressed = false;
        }
        
        public void keyTyped(KeyEvent event)
        {
            char key = event.getKeyChar();
            
            //YDebug.println("this key is pressed: " + key);
            
            userAction = 0;
            repaint();
            
            dispatcher.addCmd("JAVAC_KeyAction {" + currentView.getName() + "} " + (int)key);            
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
            //sp = adjustPoint(event.getPoint());
            sp = event.getPoint();
            ep = sp;
            
            setCurrentView(sp);
            
            if (!isCurrent) {
                jscreen.setCurrentWindow(DEViseWindow.this);
            } else {
                requestFocus();
            }
            
            if (currentView == null)
                jsc.viewInfo.updateInfo(windowName, null, sp.x, sp.y);
            else 
                jsc.viewInfo.updateInfo(windowName, currentView.getName(), sp.x, sp.y);
            
            if (isKeyPressed && lastKey == KeyEvent.VK_ESCAPE)
                isPopup = true;
            else
                isPopup = false;
            
            isMouseDragged = false;
        }
            
        public void mouseReleased(MouseEvent event)
        {
            if (currentView != null) {
                ep = adjustViewPoint(event.getPoint());
                if (isPopup && sp.x == ep.x && sp.y == ep.y && isMouseDragged == false) {
                    userAction = 0;
                    repaint();                    
                    dispatcher.addCmd("JAVAC_MouseAction_Click {" + currentView.getName() + "} " + sp.x + " " + sp.y);
                } else {
                    if (isMouseDragged == true) {
                        if (sp.x == ep.x || sp.y == ep.y) {
                            userAction = 0;
                            repaint();
                        }  else {
                            userAction = 0;
                            repaint(); 
                            //dispatcher.addCmd("JAVAC_MouseAction_RubberBand {" + currentView.getName() + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);                
                            dispatcher.addCmd("JAVAC_MouseAction_RubberBand {" + windowName + "} " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);                
                        }
                    }
                }
            }
        }
            
        public void mouseClicked(MouseEvent event)
        {               
            if (currentView != null) {            
                if (event.getClickCount() > 1) {
                    Rectangle cursorLoc = currentView.getCursorLoc(event.getPoint());
                    if (cursorLoc != null) {
                        CropImageFilter croppedFilter = new CropImageFilter(cursorLoc.x, cursorLoc.y, cursorLoc.width, cursorLoc.height);
                        FilteredImageSource source = new FilteredImageSource(winImage.getSource(), croppedFilter);
                        croppedImage = createImage(source);
                        source = new FilteredImageSource(croppedImage.getSource(), new XORFilter());
                        croppedImage = createImage(source);
                        cropP.x = cursorLoc.x;
                        cropP.y = cursorLoc.y;
                        
                        userAction = 1;
                        repaint();

                        dispatcher.addCmd("JAVAC_MouseAction_DoubleClick {" + currentView.getName() + "} " + cursorLoc.x + " " + cursorLoc.y);                
                    } else {
                        userAction = 0;
                        repaint();
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
