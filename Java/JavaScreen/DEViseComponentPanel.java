// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
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

// ========================================================================

// DEViseComponentPanel.java

package JavaScreen;

import  java.awt.*;
import  javax.swing.*;

// Note: if this extends Panel rather than JPanel, buttons embedded in
// the visualization don't work.  wenger 2011-08-25.
public class DEViseComponentPanel extends JPanel
{
    public static final String LAYOUT_HORIZONTAL = "Horizontal";
    public static final String LAYOUT_VERTICAL = "Vertical";

    public static final int ALIGN_CENTER = 0;
    public static final int ALIGN_LEFT = 1;
    public static final int ALIGN_RIGHT = 2;

    public jsdevisec jsc;

    public DEViseComponentPanel(Component components[], String style, int gap,
                                int align, jsdevisec js)
    {
        jsc = js;

        if (components == null) {
            return;
        }

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
            int rows = components.length > 5 ? 2 : 1;
            panel.setLayout(new GridLayout(rows, 0, gap, 0));
        } else {
            System.out.println("Illegal style value: " + style);
        }

        int i;
        // We leave off the last component (the feedback button) so that
        // it can later be put in on the far right side.
        for (i = 0; i < components.length-1; i++) {
            if ((components[i] instanceof TextField) ||
                    (components[i] instanceof TextArea)) {
                components[i].setBackground(jsc.jsValues.uiglobals.textBg);
                components[i].setForeground(jsc.jsValues.uiglobals.textFg);
                components[i].setFont(jsc.jsValues.uiglobals.textFont);
            } else if (!(components[i] instanceof DEViseButton)) {
                components[i].setBackground(jsc.jsValues.uiglobals.bg);
                components[i].setForeground(jsc.jsValues.uiglobals.fg);
                // font2 for drill-down dialog.  RKW 2000-06-22.
                components[i].setFont(jsc.jsValues.uiglobals.font2);
                if (components[i] instanceof Label) {
                    ((Label)components[i]).setAlignment(Label.CENTER);
                }
            }

            panel.add(components[i]);
        }

        add(panel);
    }

    public DEViseComponentPanel(Component components[], String style, int gap, jsdevisec js)
    {
        this(components, style, gap, ALIGN_CENTER, js);
    }

    public DEViseComponentPanel(Component components[], String style, jsdevisec js)
    {
        this(components, style, 10, ALIGN_CENTER, js);
    }

    public DEViseComponentPanel(Component components[], jsdevisec js)
    {
        this(components, LAYOUT_HORIZONTAL, 10, ALIGN_CENTER, js);
    }
}
