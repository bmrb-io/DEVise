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
// Revision 1.11  1998/06/11 15:07:47  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*;

public class DEViseViewControl extends Panel
{   
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    
    Label title = null;
    Component [] component = null;
    ComponentPanel panel = null;
     
    public DEViseViewControl(jsdevisec what)
    {
        jsc = what;
        jscreen = jsc.jscreen;
      
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        //setLayout(new GridLayout(1, 0));
        setLayout(new BorderLayout());
        
        DEViseView view = null;
        DEViseWindow win = jscreen.getCurrentWindow();
        if (win != null)
            view = win.getCurrentView();
            
        if (view == null) {
            title = new Label("GData For Current View");
            component = new Component[1];
            component[0] = title;
        }  else {
            title = new Label("GData For Current View");
            component = new Component[1 + view.getGData().size()];
            component[0] = title;
            if (!view.getGData().isEmpty()) {
                int howMany = view.getGData().size();         
                Button [] button = new Button[howMany];
                for (int i = 0; i < howMany; i++) {
                    button[i] = new Button((String)view.getGData().elementAt(i));
                    component[i + 1] = button[i];
                }
            }
        }
        
        panel = new ComponentPanel(component, "Vertical", 2);
        add(panel, BorderLayout.CENTER);
    }

    public void updateControl()
    {
        remove(panel);
        
        DEViseView view = null;
        DEViseWindow win = jscreen.getCurrentWindow();
        if (win != null)
            view = win.getCurrentView();
            
        if (view == null) {
            title = new Label("GData For Current View");
            component = new Component[1];
            component[0] = title;
        } else {
            title = new Label("GData For Current View");
            component = new Component[1 + view.getGData().size()];
            component[0] = title;
            if (!view.getGData().isEmpty()) {
                int howMany = view.getGData().size();         
                Button [] button = new Button[howMany];
                for (int i = 0; i < howMany; i++) {
                    button[i] = new Button((String)view.getGData().elementAt(i));
                    component[i + 1] = button[i];
                }
            }
        }

        panel = new ComponentPanel(component, "Vertical", 2);
        add(panel);
        validate();
    }
}
