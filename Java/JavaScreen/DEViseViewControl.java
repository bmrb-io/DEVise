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
import java.awt.*;
import java.awt.event.*;

public class DEViseViewControl extends Panel
{   
    private jsdevisec jsc = null;
    
    Label title = null;
    Component [] component = null;
    ComponentPanel panel = null;
     
    public DEViseViewControl(jsdevisec what)
    {
        jsc = what;
      
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        setLayout(new GridLayout(1, 0));
        
        DEViseImageView view = jsc.getCurrentView();
        if (view == null) {
            title = new Label("No Active View");
            component = new Component[1];
            component[0] = title;
        }  else {
            title = new Label("GData For Current View");
            component = new Component[1 + view.gdataValue.size()];
            component[0] = title;
            if (!view.gdataValue.isEmpty()) {
                int howMany = view.gdataValue.size();         
                Button [] button = new Button[howMany];
                for (int i = 0; i < howMany; i++) {
                    button[i] = new Button((String)view.gdataValue.elementAt(i));
                    component[i + 1] = button[i];
                }
            }
        }
        
        panel = new ComponentPanel(component, "Vertical", 2);
        add(panel);                            
    }

    public void updateControl()
    {
        remove(panel);

        DEViseImageView view = jsc.getCurrentView();
        if (view == null) {
            title = new Label("No Active View");
            component = new Component[1];
            component[0] = title;
        }  else {
            title = new Label("GData For Current View");
            component = new Component[1 + view.gdataValue.size()];
            component[0] = title;
            if (!view.gdataValue.isEmpty()) {
                int howMany = view.gdataValue.size();         
                Button [] button = new Button[howMany];
                for (int i = 0; i < howMany; i++) {
                    button[i] = new Button((String)view.gdataValue.elementAt(i));
                    component[i + 1] = button[i];
                }
            }
        }

        panel = new ComponentPanel(component, "Vertical", 2);
        add(panel);
        validate();
    }
}
