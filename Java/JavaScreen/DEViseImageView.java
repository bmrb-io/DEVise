/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
 */
import  java.awt.*;
import  java.awt.event.*;
import  java.awt.image.*;
import  java.util.*;

// This class represent a image view in DEVise
public class DEViseImageView extends Canvas
{   
    private jsdevisec jsc = null;    
    private String viewName = null;
    private Rectangle viewLoc = null;
    private Vector viewNames = null;
    private Vector viewLocs = null;
    public Vector gdataName = new Vector();
    public Vector gdataValue = new Vector();
    
    private Image image = null;
    private Image offScrImg = null;
    private Image croppedImage = null;
    private int imageWidth = 0, imageHeight = 0;      
    private Dimension minSize = new Dimension(0, 0);        
    
    // cp: current mouse position
    Point cp = new Point(), sp = new Point(), ep = new Point(), cip = new Point();
    boolean isSelected = false;
    int mouseAction = 0;
    int x0, y0, w, h;
        
    public DEViseImageView(jsdevisec what, String name, Rectangle loc, Image img, Vector vname, Vector rect) throws DEViseException
    {
        if (what == null || name == null || loc == null || img == null || vname == null || rect == null)
            throw new DEViseException("NULL argument in DEViseImageView constructor!");
            
        jsc = what;
        viewName = name;
        viewLoc = loc;
        image = img;
        viewNames = vname;
        viewLocs = rect;
        
        imageWidth = image.getWidth(this);
        imageHeight = image.getHeight(this);        
        minSize = new Dimension(imageWidth, imageHeight);
            
        //this.enableEvents(AWTEvent.MOUSE_EVENT_MASK);
        addMouseListener(new ViewMouseListener());
        addMouseMotionListener(new ViewMouseMotionListener());
    }
        
    public Dimension getPreferredSize() 
    {   
        return minSize;
    }
    
    public Dimension getMinimumSize() 
    {
        return minSize;
    }

    protected void processEvent(AWTEvent event)
    {
        if (jsc.getStatus())
            super.processEvent(event);    
    }    
    
    public synchronized void setMouseAction(int what)
    {
        mouseAction = what;
    }
    
    public synchronized int getMouseAction()
    {
        return mouseAction;
    }
    
    public synchronized void setNewImage(Image img)
    {   
        if (img == null)  {
            DEViseDebugInfo.println("NULL argument in setNewImage!");
            return;
        }
            
        image = img;
        imageWidth = image.getWidth(this);
        imageHeight = image.getHeight(this);
        minSize = new Dimension(imageWidth, imageHeight);
        
        setMouseAction(0);
        repaint();
    }
    
    public synchronized void setSelected(boolean flag)
    {
        isSelected = flag;
        setMouseAction(0);
        repaint();
    }

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
    
    public String getViewName()
    {
        return viewName;
    } 
    
    public synchronized void setViewName(String name)
    {
        viewName = name;
    }           

    public void update(Graphics g)
    {
        if (offScrImg == null)
            offScrImg = createImage(imageWidth, imageHeight);

        Graphics og = offScrImg.getGraphics();
        paint(og);
        g.drawImage(offScrImg, 0, 0, this);
        og.dispose();
    }
    
    public void paint(Graphics g) 
    {
        if (image != null)  {
            g.drawImage(image, 0, 0, this);
        
            if (isSelected)  {
                g.drawRect(0, 0, imageWidth - 1, imageHeight - 1);
                
                int ma = getMouseAction();
                if (ma == 1) {
                    g.drawImage(croppedImage, cip.x, cip.y, this);
                } else if (ma == 2) {
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
                    
                    if (jsc.isGrid) {
                        int x1 = x0 + w, y1 = y0 + h;                    
                        x0 = (x0 / 20) * 20;
                        y0 = (y0 / 20) * 20;
                        if (imageWidth - x1 < 20)
                            x1 = imageWidth;
                        else 
                            x1 = (x1 / 20 + 1) * 20;
                        if (imageHeight - y1 < 20)
                            y1 = imageHeight;
                        else
                            y1 = (y1 / 20 + 1) * 20;
                        
                        w = x1 - x0;
                        h = y1 - y0;                        
                    }
                    
                    g.drawRect(x0, y0, w, h);              
                }  else {
                    // do nothing
                }
            }
            
        }        
    }                                     
                                     
    // start of class ViewMouseListener
    class ViewMouseListener extends MouseAdapter       
    {     
        private MouseClickHandler mouseClickHandler = null;
        
        public void mousePressed(MouseEvent event)
        {                   
            sp = event.getPoint();
            ep = sp;

            if (!isSelected)  {
                jsc.setCurrentView(DEViseImageView.this);
                jsc.viewInfo.updateInfo(viewName, sp.x, sp.y);
            }                 
        }
            
        public void mouseReleased(MouseEvent event)
        {                
            ep = event.getPoint();

            if (sp.equals(ep)) {
                setMouseAction(0);
                repaint();
                return;
            }  else {
                jsc.setStatus(false);
                
                setMouseAction(0);
                repaint(); 
                jsc.channel.stop();
                jsc.channel.start("JAVAC_MouseAction_RubberBand " + viewName + " " + sp.x + " " + sp.y + " " + ep.x + " " + ep.y);                
            }
        }
            
        public void mouseClicked(MouseEvent event)
        {   
            cp = event.getPoint();
            
            if (event.getClickCount() > 1)  {
                if (mouseClickHandler != null)
                    mouseClickHandler.stop();                    
                mouseClickHandler = null;
                    
                jsc.setStatus(false);
                               
                cip.x = 0;
                cip.y = (cp.y / 20) * 20;

                CropImageFilter croppedFilter = new CropImageFilter(cip.x, cip.y, imageWidth, 20);
                FilteredImageSource source = new FilteredImageSource(image.getSource(), croppedFilter);
                croppedImage = createImage(source);
                source = new FilteredImageSource(croppedImage.getSource(), new XORFilter());
                croppedImage = createImage(source);
                
                setMouseAction(1);
                repaint();                
                jsc.channel.stop();
                jsc.channel.start("JAVAC_MouseAction_DoubleClick " + viewName + " " + cp.x + " " + cp.y);                
            }  else  {
                if (mouseClickHandler != null) {
                    mouseClickHandler.stop();
                    mouseClickHandler = null;
                }
                                
                mouseClickHandler = new MouseClickHandler(jsc, viewName, cp.x, cp.y);
                mouseClickHandler.start();
            }            
        }
    }
    // end of class ViewMouseListener
                                                                      
    // start of class ViewMouseMotionListener                                 
    class ViewMouseMotionListener extends MouseMotionAdapter
    {  
        public void mouseDragged(MouseEvent event)
        {
            ep = event.getPoint();
            jsc.viewInfo.updateInfo(ep.x, ep.y);
            
            setMouseAction(2);
            repaint();
        }
        
        public void mouseMoved(MouseEvent event)
        {
            ep = event.getPoint();
            if (isSelected) {
                jsc.viewInfo.updateInfo(ep.x, ep.y);
            }
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

class MouseClickHandler extends Thread
{   
    jsdevisec jsc = null;
    String viewName = null;
    int x = 0, y = 0;
     
    public MouseClickHandler(jsdevisec what, String name, int cpx, int cpy)
    {
        super();
        jsc = what;
        viewName = name;
        x = cpx;
        y = cpy;
    }
    
    public void run()
    {        
        try {
            sleep(300);
        } catch (InterruptedException e) {
        }
        
        jsc.setStatus(false);
        jsc.channel.stop();
        jsc.channel.start("JAVAC_MouseAction_Click " + viewName + " " + x + " " + y);
    }
}