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

    public DEViseViewInfo(jsdevisec what)
    {
        jsc = what;

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);

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

        setLayout(new FlowLayout(FlowLayout.LEFT));
        
        Label label = new Label("");
        add(label);
        label = new Label("Window:");
        add(label);
        add(winName);
        label = new Label("  ");
        add(label);
        label = new Label("View:");
        add(label);
        add(viewName);
        label = new Label("    ");
        add(label);
        label = new Label("MouseX:");
        add(label);
        add(mouseX);
        label = new Label("  ");
        add(label);
        label = new Label("MouseY:");
        add(label);
        add(mouseY);
    }

    public void updateInfo(String wname, String vname, int x, int y)
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

        validate();
    }

    public void updateInfo(int x, int y)
    {
        mouseX.setText(String.valueOf(x));
        mouseY.setText(String.valueOf(y));
        validate();
    }

    public void updateInfo()
    {
        winName.setText("");
        viewName.setText("");
        mouseX.setText("");
        mouseY.setText("");
 
        validate();
    }
}
