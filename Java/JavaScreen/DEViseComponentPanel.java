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

// This class is used purely as a container for layout of other widgets.

// Hongyu says that is was needed at least partly as a workaround for
// problems in early versions of Java, and I'm leaving it in at least
// for now.  RKW 2000-06-12.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
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
    public static final String LAYOUT_HORIZONTAL = "Horizontal";
    public static final String LAYOUT_VERTICAL = "Vertical";

    public static final int ALIGN_CENTER = 0;
    public static final int ALIGN_LEFT = 1;
    public static final int ALIGN_RIGHT = 2;

    public DEViseComponentPanel(Component component[], String style, int gap,
      int align)
    {
        if (component == null)
            return;

        if (align == ALIGN_CENTER) {
            setLayout(new FlowLayout(FlowLayout.CENTER));
        } else if (align == ALIGN_LEFT) {
            setLayout(new FlowLayout(FlowLayout.LEFT));
        } else if (align == ALIGN_RIGHT) {
            setLayout(new FlowLayout(FlowLayout.RIGHT));
        } else {
	    System.out.println("Illegal align value: " + align);
        }

        // building the panel that display components
        Panel panel = new Panel();
        if (style.equals(LAYOUT_VERTICAL))  {
            panel.setLayout(new GridLayout(0, 1, 0, gap));
        }  else if (style.equals(LAYOUT_HORIZONTAL))  {
            panel.setLayout(new GridLayout(1, 0, gap, 0));
        } else {
	    System.out.println("Illegal style value: " + style);
	}

        int i;
        for (i = 0; i < component.length; i++) {
            if (component[i] instanceof Button) {
                component[i].setBackground(DEViseUIGlobals.bg);
                component[i].setForeground(DEViseUIGlobals.fg);
                component[i].setFont(DEViseUIGlobals.font);
                ((Button)component[i]).setActionCommand(
		  ((Button)component[i]).getLabel());
            } else if ((component[i] instanceof TextField) ||
	      (component[i] instanceof TextArea)) {
                component[i].setBackground(DEViseUIGlobals.textBg);
                component[i].setForeground(DEViseUIGlobals.textFg);
                component[i].setFont(DEViseUIGlobals.textFont);
            } else {
                component[i].setBackground(DEViseUIGlobals.bg);
                component[i].setForeground(DEViseUIGlobals.fg);
                component[i].setFont(DEViseUIGlobals.font);
                if (component[i] instanceof Label) {
                    ((Label)component[i]).setAlignment(Label.CENTER);
	        }
            }

            panel.add(component[i]);
        }

        add(panel);
    }

    public DEViseComponentPanel(Component component[], String style, int gap)
    {
        this(component, style, gap, ALIGN_CENTER);
    }

    public DEViseComponentPanel(Component component[], String style)
    {
        this(component, style, 10, ALIGN_CENTER);
    }

    public DEViseComponentPanel(Component component[])
    {
        this(component, LAYOUT_HORIZONTAL, 10, ALIGN_CENTER);
    }
}
