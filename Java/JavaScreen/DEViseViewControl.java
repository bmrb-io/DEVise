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
// Revision 1.15  1998/08/14 17:48:08  hongyu
// *** empty log message ***
//
// Revision 1.11  1998/06/11 15:07:47  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*; 
import java.util.*;

public class DEViseViewControl extends Panel
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null; 
    Vector images = null;

    public TextField screenX = new TextField(4);
    public TextField screenY = new TextField(4);
    public TextField count = new TextField(2);
    public Button setButton = new Button("Set");
    Label screenSizeTitle = new Label("Screen Size"); 
    canvas imgCanvas = null;

    boolean isEditable = true;

    public DEViseViewControl(jsdevisec what, Vector array)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        images = array;

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        //setLayout(new GridLayout(1, 0));
        setLayout(new BorderLayout());

        screenX.setText("" + jscreen.getScreenDim().width);
        screenY.setText("" + jscreen.getScreenDim().height);

        Component[] component = new Component[4];
        component[0] = screenSizeTitle;
        component[1] = screenX;
        component[2] = screenY;
        component[3] = setButton;
        ComponentPanel panel = new ComponentPanel(component, "Vertical", 10);
        
        count.setBackground(DEViseGlobals.textbgcolor);
        count.setForeground(DEViseGlobals.textfgcolor);
        count.setFont(DEViseGlobals.textfont);
        imgCanvas = new canvas(images);
        
        Panel upPanel = new Panel();
        upPanel.add(imgCanvas);
        upPanel.add(count);
        
        add(upPanel, BorderLayout.NORTH);
        add(panel, BorderLayout.CENTER);

        setButton.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    Dimension newDim = new Dimension((Integer.valueOf(screenX.getText())).intValue(), (Integer.valueOf(screenY.getText())).intValue());
                    if (newDim.width > (int)(1.0 * DEViseGlobals.SCREENSIZE.width)
                        || newDim.height > (int)(1.0 * DEViseGlobals.SCREENSIZE.height)
                        || newDim.width < (int)(0.5 * DEViseGlobals.SCREENSIZE.width)
                        || newDim.height < (int)(0.5 * DEViseGlobals.SCREENSIZE.height)) {
                        YGlobals.Yshowmsg(jsc, "Your actual screen size is " + DEViseGlobals.SCREENSIZE.width
                                         + " x " + DEViseGlobals.SCREENSIZE.height
                                         + "\nYour Java Screen size can not exceed 100% of your actual screen size"
                                         + "\n and it can not be less than 50% of your actual screen size!");
                    } else {
                        jscreen.setScreenDim(newDim);
                    }
                }
            });
    }

    public void updateControl(boolean flag)
    {
        isEditable = flag;

        screenX.setEditable(isEditable);
        screenY.setEditable(isEditable);
        setButton.setEnabled(isEditable);

        validate();
    } 
    
    public void updateImage(int type, int number)
    {   
        imgCanvas.updateImage(type);
        count.setText("" + number);
        validate();
    }        
}

class canvas extends Canvas implements Runnable
{   
    Vector images = null;
    int imageStart = 4, imageEnd = 11, imageCount = 1;
    int imageWidth, imageHeight;
    int oldtype = 0;
    Dimension minSize = null;
    Image currentImg = null, offScrImg = null;
    Thread animator = null;
    boolean isAnimated = false;
    
    public canvas(Vector array)
    {   
        images = array;
        if (images != null && images.size() == 11) {
            currentImg = (Image)images.elementAt(imageStart);
            isAnimated = true;
        } else {
            isAnimated = false;
        }
        
        imageWidth = 32;
        imageHeight = 32;
        minSize = new Dimension(imageWidth, imageHeight);
    }
    
    public Dimension getPreferredSize()
    {
        return minSize;
    }

    public Dimension getMinimumSize()
    {
        return minSize;
    }
    
    public void updateImage(int type)
    {
        if (type < 0 || type > 2)
            return;
        
        if (!isAnimated)            
            return;
            
        if (type != oldtype) {
            if (animator != null) {
                if (animator.isAlive())
                    animator.stop();
                
                animator = null;
            }
            
            offScrImg = null;
            imageCount = 1;
            oldtype = type;                        
        
            int pos;
            if (oldtype == 0)
                pos = imageStart;                
            else 
                pos = imageStart + imageCount + (oldtype - 1) * 3;
            
            currentImg = (Image)images.elementAt(pos);
            
            if (oldtype == 0) {
                repaint();
            } else {
                animator = new Thread(this);
                animator.start();
            }
        }
    } 
    
    public void run()
    {
        Thread me = Thread.currentThread();
        
        while (animator == me) {
            repaint();
            
            imageCount++;
            if (imageCount > 3)
                imageCount = 1;
            
            int pos = imageStart + imageCount + (oldtype - 1) * 3;            
            currentImg = (Image)images.elementAt(pos);
            
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
            }
        }
    }
                
    // implement double-buffering, set offScrImg = null if you want to restart double-buffering
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
        if (currentImg != null) {
            g.drawImage(currentImg, 0, 0, this);
        } else {
            g.setColor(DEViseGlobals.uibgcolor);
            g.fillRect(0, 0, imageWidth, imageHeight);
        }
    }
}    
