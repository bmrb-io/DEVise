// DEViseComponentPanel.java
// last updated on 04/11/99

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
                component[i].setBackground(DEViseGlobals.bg);
                component[i].setForeground(DEViseGlobals.fg);
                component[i].setFont(DEViseGlobals.font);
                ((Button)component[i]).setActionCommand(((Button)component[i]).getLabel());
            } else if ((component[i] instanceof TextField) || (component[i] instanceof TextArea)) {
                component[i].setBackground(DEViseGlobals.textBg);
                component[i].setForeground(DEViseGlobals.textFg);
                component[i].setFont(DEViseGlobals.textFont);
            } else {
                component[i].setBackground(DEViseGlobals.bg);
                component[i].setForeground(DEViseGlobals.fg);
                component[i].setFont(DEViseGlobals.font);
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
