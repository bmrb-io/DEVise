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

public class ComponentPanel extends Panel
{
    public ComponentPanel(Component component[], String style, int gap)
    {   
        if (component == null)
            return;
        
        setLayout(new FlowLayout(FlowLayout.LEFT));
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
        
        //setLayout(gridbag);
        //gridbag.setConstraints(panel, c);
        add(panel);
    }
    
    public ComponentPanel(Component component[], String style)
    {
        this(component, style, 10);        
    }
    
    public ComponentPanel(Component component[])
    {
        this(component, "Horizontal", 10);
    }                
}
