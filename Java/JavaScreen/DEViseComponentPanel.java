// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2000/03/23 16:26:13  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.3  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseComponentPanel.java

import  java.awt.*;

public class DEViseComponentPanel extends Panel
{
    public DEViseComponentPanel(Component component[], String style, int gap, int align)
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

        // building the panel that display components
        Panel panel = new Panel();
        if (style.equals("Vertical"))  {
            panel.setLayout(new GridLayout(0, 1, 0, gap));
        }  else  {
            panel.setLayout(new GridLayout(1, 0, gap, 0));
        }

        int i;
        for (i = 0; i < component.length; i++) {
            if (component[i] instanceof Button) {
                component[i].setBackground(DEViseUIGlobals.bg);
                component[i].setForeground(DEViseUIGlobals.fg);
                component[i].setFont(DEViseUIGlobals.font);
                ((Button)component[i]).setActionCommand(((Button)component[i]).getLabel());
            } else if ((component[i] instanceof TextField) || (component[i] instanceof TextArea)) {
                component[i].setBackground(DEViseUIGlobals.textBg);
                component[i].setForeground(DEViseUIGlobals.textFg);
                component[i].setFont(DEViseUIGlobals.textFont);
            } else {
                component[i].setBackground(DEViseUIGlobals.bg);
                component[i].setForeground(DEViseUIGlobals.fg);
                component[i].setFont(DEViseUIGlobals.font);
                if (component[i] instanceof Label)
                    ((Label)component[i]).setAlignment(Label.CENTER);
            }

            panel.add(component[i]);
        }

        add(panel);
    }

    public DEViseComponentPanel(Component component[], String style, int gap)
    {
        this(component, style, gap, 0);
    }

    public DEViseComponentPanel(Component component[], String style)
    {
        this(component, style, 10, 0);
    }

    public DEViseComponentPanel(Component component[])
    {
        this(component, "Horizontal", 10, 0);
    }
}
