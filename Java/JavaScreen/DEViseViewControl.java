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
    YImageCanvas canvas = null;
    TrafficLight light = null;    

    boolean isEditable = true;

    public DEViseViewControl(jsdevisec what, Vector array)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        // the length of array should be 3
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
        boolean isCanvas = false;
        if (images != null && images.size() > 4) {
            try {
                canvas = new YImageCanvas((Image)images.elementAt(4)); 
                isCanvas = true;
            } catch (YException e) {
                canvas = null;
            }
        }                    
        
        Panel lowPanel = new Panel();
        if (isCanvas) {
            lowPanel.add(canvas);
        }
        lowPanel.add(count);
        
        isCanvas = false;
        if (images != null && images.size() > 6) {
            try {
                light = new TrafficLight((Image)images.elementAt(5), (Image)images.elementAt(6));
                isCanvas = true;
            } catch (YException e) {
                light = null;
            }
        }
        
        if (isCanvas) {
            add(light, BorderLayout.NORTH);
        }
        
        add(lowPanel, BorderLayout.CENTER);
        add(panel, BorderLayout.SOUTH);

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
    
    // type = 0, idle
    // type = 1, sending
    // type = 2, waiting 
    // type = 3, receiving
    // type = 4, processing
    public void updateImage(int type, int isOn)
    {   
        if (light != null) {
            light.updateImage(type, isOn);
            validate();
        }
    }
    
    public void updateCount(int number)
    {
        count.setText("" + number);
        validate();
    }        
}

class TrafficLight extends Panel
{          
    Image onImage = null, offImage = null;
    YImageCanvas [] canvas = new YImageCanvas[4];
    Label [] label = new Label[4];
    String[] c = {"S", "W", "R", "P"};    
    
    public TrafficLight(Image offi, Image oni) throws YException
    {
        onImage = oni;
        offImage = offi;
        
        for (int i = 0; i < 4; i++) {
            canvas[i] = new YImageCanvas(onImage);
            if (!canvas[i].setImage(offImage))
                throw new YException("Invalid Image!");
            label[i] = new Label(c[i]);            
        } 
        
        setFont(new Font("Monospaced", Font.BOLD, 14));
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        
        setLayout(new GridLayout(2, 4));
        for (int i = 0; i < 4; i++)
            add(label[i]);
        for (int i = 0; i < 4; i++)
            add(canvas[i]);                    
    } 
    
    public void updateImage(int type, int isOn)
    {  
        if (type < 0 || type > 4)
            return;
        
        if (type == 0) {
            for (int i = 0; i < 4; i++) {
                canvas[i].setImage(offImage);
            } 
        } else {             
            if (isOn == 1) {
                canvas[type - 1].setImage(onImage);
            } else {
                canvas[type - 1].setImage(offImage);
            }
        }               
    }       
}
    
