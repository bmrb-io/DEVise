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
// Revision 1.23  1999/11/03 08:00:49  hongyu
// *** empty log message ***
//
// Revision 1.22  1999/11/02 21:37:39  wenger
// Made separate methods for constructing the different symbol types, without
// really changing the functionality, to clean things up and make fixes to
// text alignment, etc., easier to do.
//
// Revision 1.21  1999/10/28 17:48:46  wenger
// Fixed various JavaScreen/devised bugs (incorrect location of GData symbols
// in JS; incorrect location of child views; extra destroys of child GData
// views).
//
// Revision 1.20  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.19  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.18  1999/08/19 07:21:06  hongyu
// *** empty log message ***
//
// Revision 1.17  1999/08/17 06:15:16  hongyu
// *** empty log message ***
//
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

    public Rectangle GDataLoc = null;
    public Rectangle GDataLocInScreen = null;

    public int x = 0, y = 0, z = 0, width = 0, height = 0;
    public double x0, y0, z0;

    public String[] data = null;
    public Component symbol = null;
    public boolean isJavaSymbol = false;
    public int symbolType = 0;

    public String string = null;
    public Color color = null;
    public Font font = null;
    public int outline = 0;

    // GData format: <x> <y> <z> <color> <size> <pattern> <orientation> <symbol type> <shape attr 0> ... <shape attr 14>
    public DEViseGData(jsdevisec panel, String name, String gdata, double xm, double xo, double ym, double yo) throws YException
    {
        jsc = panel;
        viewname = name;

        parentView = jsc.jscreen.getView(viewname);
        if (parentView == null) {
            throw new YException("Invalid parent view for GData!");
        }

        data = DEViseGlobals.parseString(gdata);
        if (data == null || data.length != 23)
            throw new YException("Invalid GData + {" + gdata + "}");

        //double x0 = 0, y0 = 0, z0 = 0, size = 0;
        double size = 0;
        try {
            x0 = (Double.valueOf(data[0])).doubleValue();
            x = (int)(x0 * xm + xo);
            y0 = (Double.valueOf(data[1])).doubleValue();
            y = (int)(y0 * ym + yo);
            z0 = (Double.valueOf(data[2])).doubleValue();
            z = (int)(y0 * ym + yo);
            size = (Double.valueOf(data[4])).doubleValue();
            symbolType = Integer.parseInt(data[7]);
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData!");
        }

        if (symbolType == 15) { // check symbol type
	    // 15 is Embedded Tk window
	    EmbeddedTk(size, xm, ym);

	} else if (symbolType == 12) {
	    // 12 is Text Label
	    TextLabel(size, xm, ym);

        } else if (symbolType == 16) {
	    // 16 is Fixed Text Label
	    FixedTextLabel(size);

        } else if (symbolType == 4) {
	    // 4 is Oval
	    Oval(size, xm, ym);

        } else {
	    DefaultSymbol(size, xm, ym);
        }

        GDataLoc = new Rectangle(x, y, width, height);
        GDataLocInScreen = getLocInScreen();
    }

    public Rectangle getLocInScreen()
    {
        Rectangle loc = parentView.getLocInScreen();

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
    }

    protected void TextLabel(double size, double xm, double ym)
      throws YException
    {
        isJavaSymbol = false;

        string = data[8];

        double w = 0.0, h = 0.0;
        int align, ff, fw, fs;

	// Note: format is ignored for now.  RKW 1999-11-03.

        // default font is courier, regular, nonitalic
        try {
            w = (Double.valueOf(data[10])).doubleValue();
            h = (Double.valueOf(data[11])).doubleValue();

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

        width = (int)(w * size * xm);
        height = (int)(h * size * ym);
        if (width < 0) {
            width = -width;
	}
        if (height < 0) {
            height = -height;
	}

        color = DEViseGlobals.convertColor(data[3]);

        // Note: not using the default font here is probably less
        // efficient, but the previous version of this code failed
        // if we needed different fonts for different symbols.
        // RKW 1999-11-02.
        if (w < 0.0) {
            font = DEViseGlobals.getFont(string, height, ff, fw, fs);
        } else {
            font = DEViseGlobals.getFont(string, width, height, ff, fw, fs);
        }

        if (color == null || font == null) {
            string = null;
            return;
        }

	AlignText(align);
            
        x = parentView.viewLocInCanvas.x + x;
        y = parentView.viewLocInCanvas.y + y;
    }

    protected void FixedTextLabel(double size) throws YException
    {
        isJavaSymbol = false;

        string = data[8];

        double w = 0.0, h = 0.0;
        int align, ff, fw, fs;

        // default font is courier, regular, nonitalic
        try {
            if (data[10].equals("")) {
                align = 0;
            } else {
                align = Integer.parseInt(data[10]);
                if (align < -4 || align > 4) {
                    align = 0;
                }
            }

            if (data[11].equals("")) {
                ff = 0;
            } else {
                ff = Integer.parseInt(data[11]);
            }

            if (data[12].equals("")) {
                fw = 0;
            } else {
                fw = Integer.parseInt(data[12]);
            }

            if (data[13].equals("")) {
                fs = 0;
            } else {
                fs = Integer.parseInt(data[13]);
            }
        } catch (NumberFormatException e) {
            throw new YException("Invalid Gdata!");
        }

        width = 0;
        height = 0;

        color = DEViseGlobals.convertColor(data[3]);

        // Note: not using the default font here is probably less
        // efficient, but the previous version of this code failed
        // if we needed different fonts for different symbols.
        // RKW 1999-11-02.
        int fsize;
        if (size > 1.0) {
            fsize = (int)(size + 0.25);
        } else {
            fsize = (int)(size * DEViseGlobals.screenSize.height + 0.25);
        }
        font = DEViseGlobals.getFont(fsize, ff, fw, fs);

        if (color == null || font == null) {
            string = null;
            return;
        }

	AlignText(align);
            
        x = parentView.viewLocInCanvas.x + x;
        y = parentView.viewLocInCanvas.y + y;
    }

    protected void EmbeddedTk(double size, double xm, double ym)
    {
        isJavaSymbol = true;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0) {
            width = -width;
	}
        if (height < 0) {
            height = -height;
	}

        x = x - width / 2;
        y = y - height / 2;
        /* Don't force the entire rectangle to be within the view.
         * RKW 1999-10-28.
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        */

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
    }

    protected void Oval(double size, double xm, double ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0)
            width = -width;
        if (height < 0)
            height = -height;

        color = DEViseGlobals.convertColor(data[3]);
        
        string = data[10];
    }

    protected void DefaultSymbol(double size, double xm, double ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0) {
            width = -width;
        }
        if (height < 0) {
            height = -height;
        }

        x = x - width / 2;
        y = y - height / 2;
        /* Don't force the entire symbol to be within the view.
         * RKW 1999-10-28.
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        */
    }

    protected void AlignText(int align)
    {
        Toolkit tk = Toolkit.getDefaultToolkit();
        FontMetrics fm = tk.getFontMetrics(font);
        int ac = fm.getAscent(), dc = fm.getDescent();
        int sw = fm.stringWidth(string);

        switch (align) {
        case -4: // northwest
            y = y + ac;
            break;
        case -3: // west
            y = y + (ac - dc) / 2;
            break;
        case -2: // southwest
            y = y - dc;
            break;
        case -1: // north
            x = x - sw / 2;
            y = y + ac;
            break;
        case 0: // center
            x = x - sw / 2;
            y = y + (ac - dc) / 2;
            break;
        case 1: // south
            x = x - sw / 2;
            y = y - dc;
            break;
        case 2: // northeast
            x = x - sw;
            y = y + ac;
            break;
        case 3: // east
            x = x - sw;
            y = y + (ac - dc) / 2;
            break;
        case 4: // southeast
            x = x - sw;
            y = y - dc;
            break;
        }
    }
}
