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
// Revision 1.11  1998/06/11 15:07:48  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*;

public class DEViseViewInfo extends Panel
{   
    private jsdevisec jsc = null;
    
    TextField winName = new TextField(15);
    TextField viewName = new TextField(15);
    TextField mouseX = new TextField(4), mouseY = new TextField(4);
    TextField recordValue = new TextField(15);
    TextField recordName = new TextField(15);
     
    public DEViseViewInfo(jsdevisec what)
    {
        jsc = what;
      
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();       
        setLayout(gridbag); 
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        //c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.NONE;
        c.insets = new Insets(5, 10, 5, 0);
        //c.ipadx = 0;
        //c.ipady = 0;
        //c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        //setLayout(new FlowLayout(FlowLayout.CENTER));
        winName.setBackground(DEViseGlobals.textbgcolor);
        winName.setForeground(DEViseGlobals.textfgcolor);
        winName.setFont(DEViseGlobals.textfont);
        viewName.setBackground(DEViseGlobals.textbgcolor);
        viewName.setForeground(DEViseGlobals.textfgcolor);
        viewName.setFont(DEViseGlobals.textfont);
        mouseX.setBackground(DEViseGlobals.textbgcolor);
        mouseX.setForeground(DEViseGlobals.textfgcolor);
        mouseX.setFont(DEViseGlobals.textfont);
        mouseY.setBackground(DEViseGlobals.textbgcolor);
        mouseY.setForeground(DEViseGlobals.textfgcolor);
        mouseY.setFont(DEViseGlobals.textfont);
        recordName.setBackground(DEViseGlobals.textbgcolor);
        recordName.setForeground(DEViseGlobals.textfgcolor);
        recordName.setFont(DEViseGlobals.textfont);
        recordValue.setBackground(DEViseGlobals.textbgcolor);
        recordValue.setForeground(DEViseGlobals.textfgcolor);
        recordValue.setFont(DEViseGlobals.textfont);
        
        c.insets = new Insets(5, 15, 5, 0);
        Label label = new Label("Active Window:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(winName, c);
        add(winName);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Active View:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(viewName, c);
        add(viewName);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Mouse X:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(mouseX, c);
        add(mouseX);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Mouse Y:");
        gridbag.setConstraints(label, c);
        add(label);
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.insets = new Insets(5, 0, 5, 15);
        gridbag.setConstraints(mouseY, c);
        add(mouseY);
        c.gridwidth = 1;        
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Record Name:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 10);
        gridbag.setConstraints(recordName, c);
        add(recordName);
        c.insets = new Insets(5, 15, 5, 0);
        label = new Label("Record Value:");
        gridbag.setConstraints(label, c);
        add(label);
        c.insets = new Insets(5, 0, 5, 15);
        gridbag.setConstraints(recordValue, c);
        add(recordValue);
    }

    public void updateInfo(String wname, String vname, int x, int y, String rname, String rvalue)
    {   
        if (wname == null)
            winName.setText("");
        else
            winName.setText(wname);

        if (vname == null)
            viewName.setText("");
        else
            viewName.setText(vname);
                
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));

        if (rname == null)
            recordName.setText("");
        else
            recordName.setText(rname);

        if (rvalue == null)
            recordValue.setText("");
        else
            recordValue.setText(rvalue);
        
        validate();
    } 
    
    public void updateInfo(String wname, String vname, int x, int y)
    {
        updateInfo(wname, vname, x, y, null, null);
    }
    
    public void updateInfo(int x, int y)
    { 
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));
        validate();
    }
    
    public void updateInfo(String rname, String rvalue)
    {
        if (rname == null)
            recordName.setText("");
        else
            recordName.setText(rname);

        if (rvalue == null)
            recordValue.setText("");
        else
            recordValue.setText(rvalue);
        
        validate();
    }
    
    public void updateInfo()
    {   
        winName.setText("");
        viewName.setText("");
        mouseX.setText("");
        mouseY.setText("");
        recordName.setText("");
        recordValue.setText("");
        validate();
    }    
}
