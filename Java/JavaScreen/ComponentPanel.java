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
// Revision 1.10  1998/06/11 15:07:43  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import  java.awt.*;

public class ComponentPanel extends Panel
{
    public ComponentPanel(Component component[], String style, int gap, int align)
    {   
        if (component == null)
            return;
        
        if (align == 0) {
            setLayout(new FlowLayout(FlowLayout.CENTER));
        } else if (align == 1) {
            setLayout(new FlowLayout(FlowLayout.LEFT));
        } else {
            setLayout(new FlowLayout(FlowLayout.RIGHT));
        }
        //GridBagLayout gridbag = new GridBagLayout();
        //GridBagConstraints c = new GridBagConstraints();       
        
        // building the panel that display components
        Panel panel = new Panel();
        if (style.equals("Vertical"))  {
            panel.setLayout(new GridLayout(0, 1, 0, gap));
        }  else  {
            panel.setLayout(new GridLayout(1, 0, gap, 0));
        }
        
        int i;
        for (i = 0; i < component.length; i++)  {
            if (component[i] instanceof Button)  {
                component[i].setBackground(DEViseGlobals.buttonbgcolor);
                component[i].setForeground(DEViseGlobals.buttonfgcolor);
                component[i].setFont(DEViseGlobals.buttonfont);
                ((Button)component[i]).setActionCommand(((Button)component[i]).getLabel());
            }  else if ((component[i] instanceof TextField) || (component[i] instanceof TextArea))  {
                component[i].setBackground(DEViseGlobals.textbgcolor);
                component[i].setForeground(DEViseGlobals.textfgcolor);
                component[i].setFont(DEViseGlobals.textfont);
            }  else  {
                component[i].setBackground(DEViseGlobals.uibgcolor);
                component[i].setForeground(DEViseGlobals.uifgcolor);
                component[i].setFont(DEViseGlobals.uifont);
                if (component[i] instanceof Label)
                    ((Label)component[i]).setAlignment(Label.CENTER);
            }
            
            panel.add(component[i]);
        }
        
        add(panel);
    }
    
    public ComponentPanel(Component component[], String style, int gap)
    {
        this(component, style, gap, 0);
    }
    
    public ComponentPanel(Component component[], String style)
    {
        this(component, style, 10, 0);        
    }
    
    public ComponentPanel(Component component[])
    {
        this(component, "Horizontal", 10, 0);
    }                
}
