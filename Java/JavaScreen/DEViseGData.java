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
// Revision 1.16  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.14  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.12  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseGData.java

import java.awt.*;
import java.awt.event.*;
import java.net.*;

public class DEViseGData
{   
    public static Font defaultFont = null;
    
    public jsdevisec jsc = null;
    public DEViseView parentView = null;
    public String viewname = null;
    public int x = 0, y = 0, width = 0, height = 0;
    public String[] data = null;
    public Component symbol = null;
    public boolean isJavaSymbol = false;
    public int symbolType = 0;

    public String string = null;
    public Color color = null;
    public Font font = null;
    public int outline = 0;

    // GData format: <x> <y> <z> <color> <size> <pattern> <orientation> <symbol type> <shape attr 0> ... <shape attr 9>
    public DEViseGData(jsdevisec j, String name, String gdata, double xm, double xo, double ym, double yo) throws YException
    {
        if (name == null)
            throw new YException("Invalid parent view for GData!");

        jsc = j;
        viewname = name;

        data = DEViseGlobals.parseString(gdata);
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
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData!");
        }

        if (symbolType == 15) { // check symbol type
            isJavaSymbol = true;

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

            string = data[8];

            double w = 0.0, h = 0.0;
            int align, ff, fw, fs;

            // default font is courier, regular, nonitalic
            try {
                if (symbolType == 12) {
                    w = (Double.valueOf(data[10])).doubleValue();
                    h = (Double.valueOf(data[11])).doubleValue();
                }

                if (data[12].equals("")) {
                    outline = 0;
                } else {
                    outline = Integer.parseInt(data[12]);
                }

                if (data[13].equals("")) {
                    align = 0;
                } else {
                    align = Integer.parseInt(data[13]);
                    if (align < -4 || align > 4) {
                        align = 0;
                    }
                }

                if (data[14].equals("")) {
                    ff = 0;
                } else {
                    ff = Integer.parseInt(data[14]);
                }

                if (data[15].equals("")) {
                    fw = 0;
                } else {
                    fw = Integer.parseInt(data[15]);
                }

                if (data[16].equals("")) {
                    fs = 0;
                } else {
                    fs = Integer.parseInt(data[16]);
                }
            } catch (NumberFormatException e) {
                throw new YException("Invalid Gdata!");
            }

            if (symbolType == 12) {
                width = (int)(w * size * xm);
                height = (int)(h * size * ym);
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
            } else {
                width = 0;
                height = 0;
            }

            color = DEViseGlobals.convertColor(data[3]);
            
            if (DEViseGData.defaultFont == null) {
                if (symbolType == 12) {
                    if (w < 0.0) {
                        font = DEViseGlobals.getFont(string, height, ff, fw, fs);
                    } else {
                        font = DEViseGlobals.getFont(string, width, height, ff, fw, fs);
                    }
                } else {
                    int fsize;
                    
                    if (size > 1.0) {
                        fsize = (int)(size + 0.25);
                    } else {
                        fsize = (int)(size * jsc.jscreen.getScreenDim().height + 0.25);
                    }
                    
                    font = DEViseGlobals.getFont(fsize, ff, fw, fs);
                }
            
                DEViseGData.defaultFont = font;
            } else {
                font = DEViseGData.defaultFont;
            }    
            
            if (color == null || font == null) {
                string = null;
                return;
            }

            // because java draw string from its baseline, so we have to make corrections
            Toolkit tk = Toolkit.getDefaultToolkit();
            FontMetrics fm = tk.getFontMetrics(font);
            int ac = fm.getAscent(), dc = fm.getDescent(), ld = fm.getLeading();
            ac = ac + ld / 2;
            dc = dc + ld / 2;
            int sh = fm.getHeight();
            int sw = fm.stringWidth(string);

            switch (align) {
            case -4:
                y = y + ac;
                break;
            case -3:
                y = y + height / 2 + ac - sh / 2;
                break;
            case -2:
                y = y + height - dc;
                break;
            case -1:
                y = y + ac;
                x = x + width / 2 - sw / 2;
                break;
            case 0:
                y = y + height / 2 + ac - sh / 2;
                x = x + width / 2 - sw / 2;
                break;
            case 1:
                y = y + height - dc;
                x = x + width / 2 - sw / 2;
                break;
            case 2:
                y = y + ac;
                x = x + width - sw;
                break;
            case 3:
                y = y + height / 2 + ac - sh / 2;
                x = x + width - sw;
                break;
            case 4:
                y = y + height - dc;
                x = x + width - sw;
                break;
            }
        } else {
            isJavaSymbol = false;

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
