// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$

// ========================================================================

// DEViseGData.java

import java.awt.*;
import java.awt.event.*;
import java.net.*;

public class DEViseGData
{
    public DEViseView parentView = null;
    public String viewname = null;
    public int x = 0, y = 0, width = 0, height = 0;
    public String[] data = null;
    public Component symbol = null;
    public boolean isJavaSymbol = false; 
    public int symbolType = 0;
    
    // GData format: <x> <y> <z> <color> <size> <pattern> <orientation> <symbol type> <shape attr 0> ... <shape attr 9>
    public DEViseGData(String name, String gdata, double xm, double xo, double ym, double yo) throws YException
    {
        if (name == null)
            throw new YException("Invalid parent view for GData!");

        viewname = name;

        data = DEViseGlobals.parseStr(gdata, " ");
        if (data == null || data.length != 18)
            throw new YException("Invalid GData + {" + gdata + "}");

        double x0 = 0, y0 = 0, size = 0;
        try {
            x0 = (Double.valueOf(data[0])).doubleValue();
            x = (int)(x0 * xm + xo);
            y0 = (Double.valueOf(data[1])).doubleValue();
            y = (int)(y0 * ym + yo);
            size = (Double.valueOf(data[4])).doubleValue();
            symbolType = Integer.parseInt(data[7]);
            
            width = (int)(size * xm);
            height = (int)(size * ym);
            if (width < 0)
                width = -width;
            if (height < 0)
                height = -height;
            x = x - width / 2;
            y = y - height / 2;
            if (x < 0)
                x = 0;
            if (y < 0)
                y = 0;
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData!");
        }

        if (symbolType == 15) { // check symbol type
            isJavaSymbol = true;
            
            Button button = new Button(data[11]);
            button.setActionCommand(data[10]);
            button.setFont(new Font("Monospaced", Font.PLAIN, 10));
            button.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (DEViseGlobals.isApplet) {
                            if (DEViseGlobals.browser != null) {
                                try {
                                    URL url = new URL(event.getActionCommand());
                                    //DEViseGlobals.browser.showDocument(url, "_blank");
                                    DEViseGlobals.browser.showDocument(url, "_parent");
                                } catch (MalformedURLException e) {
                                    //YGlobals.Ydebugpn("Invalid URL {" + event.getActionCommand() + "}");
                                }
                            }
                        }
                    }
                });

            symbol = button;
        } else if (symbolType == 12 || symbolType == 16) {
            isJavaSymbol = false;
        } else {
            isJavaSymbol = false;
        }
    }

    public Rectangle getBounds()
    {
        return new Rectangle(x, y, width, height);
    }

    public Rectangle getBoundsInScreen()
    {
        if (parentView != null) {
            Rectangle loc = parentView.getBoundsInScreen();
            Rectangle r = new Rectangle(x + loc.x, y + loc.y, width, height);
            if (r.x < loc.x) {
                r.width = r.width + r.x - loc.x;
                r.x = loc.x;
            } else if (r.x > loc.x + loc.width - r.width) {
                r.width = r.width + r.x - loc.x - loc.width + r.width;
                r.x = loc.x + loc.width - r.width;
            }
            if (r.y < loc.y) {
                r.height = r.height + r.y - loc.y;
                r.y = loc.y;
            } else if (r.y > loc.y + loc.height - r.height) {
                r.height = r.height + r.y - loc.y - loc.height + r.height;
                r.y = loc.y + loc.height - r.height;
            }

            return r;
        } else {
            return getBounds();
        }
    }
}
