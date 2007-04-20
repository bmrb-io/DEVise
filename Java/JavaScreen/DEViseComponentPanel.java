// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2007
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
// Revision 1.10.38.2  2007/04/20 16:59:58  wenger
// Fixed the problem with the JavaScreen buttons showing up with the
// wrong font; improved handling of color arguments.
//
// Revision 1.10.38.1  2007/03/16 17:12:46  adayton
// Add UI package
//
// Revision 1.10  2001/05/11 20:36:06  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.9  2001/04/18 16:04:52  wenger
// Changed the JSA version of the JavaScreen to have two rows of buttons
// because we're running out of space; changed JS version to 4.1.
//
// Revision 1.8  2001/01/08 20:31:51  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.7.4.1  2000/11/21 01:51:31  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.7  2000/06/22 20:51:11  wenger
// Changed colors and font in drill-down dialog to make text more
// readable.
//
// Revision 1.6  2000/06/12 22:13:56  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
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

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;

public class DEViseComponentPanel extends Panel
{
    public static final String LAYOUT_HORIZONTAL = "Horizontal";
    public static final String LAYOUT_VERTICAL = "Vertical";

    public static final int ALIGN_CENTER = 0;
    public static final int ALIGN_LEFT = 1;
    public static final int ALIGN_RIGHT = 2;
    
    public jsdevisec jsc;

    public DEViseComponentPanel(Component component[], String style, int gap,
      int align, jsdevisec js)
    {
	jsc = js;

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
            // panel.setLayout(new GridLayout(0, 1, 0, gap));
            panel.setLayout(new GridLayout(0, 0, 0, gap));
        }  else if (style.equals(LAYOUT_HORIZONTAL))  {
	    int rows = component.length > 5 ? 2 : 1;
            panel.setLayout(new GridLayout(rows, 0, gap, 0));
        } else {
	    System.out.println("Illegal style value: " + style);
	}

        int i;
        for (i = 0; i < component.length; i++) {
            if ((component[i] instanceof TextField) ||
	      (component[i] instanceof TextArea)) {
                component[i].setBackground(jsc.jsValues.uiglobals.textBg);
                component[i].setForeground(jsc.jsValues.uiglobals.textFg);
                component[i].setFont(jsc.jsValues.uiglobals.textFont);
            } else if (!(component[i] instanceof DEViseButton)) {
                component[i].setBackground(jsc.jsValues.uiglobals.bg);
                component[i].setForeground(jsc.jsValues.uiglobals.fg);
		// font2 for drill-down dialog.  RKW 2000-06-22.
                component[i].setFont(jsc.jsValues.uiglobals.font2);
                if (component[i] instanceof Label) {
                    ((Label)component[i]).setAlignment(Label.CENTER);
	        }
            }

            panel.add(component[i]);
        }

        add(panel);
    }

    public DEViseComponentPanel(Component component[], String style, int gap, jsdevisec js)
    {
        this(component, style, gap, ALIGN_CENTER, js);
    }

    public DEViseComponentPanel(Component component[], String style, jsdevisec js)
    {
        this(component, style, 10, ALIGN_CENTER, js);
    }

    public DEViseComponentPanel(Component component[], jsdevisec js)
    {
        this(component, LAYOUT_HORIZONTAL, 10, ALIGN_CENTER, js);
    }
}
