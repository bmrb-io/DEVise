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

// There is once instance of this class for each GData record the
// JavaScreen is handling.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.34  2000/05/22 17:52:49  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//
// Revision 1.33  2000/05/04 17:40:28  wenger
// Added new text object feature: GData Z value specifies max size of
// font in points (if > 1).  (Allows me to fix problems with BMRB 4096
// protein session.)
//
// Revision 1.32  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.31  2000/04/07 22:43:13  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.30  2000/04/05 15:42:23  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.29  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.28  2000/04/03 22:24:53  wenger
// Added named constants for GData symbol types; 3D GData symbols are now
// differentiated by symbol type instead of string; removed some commented-
// out code.
//
// Revision 1.27  2000/03/31 19:29:16  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.26  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.25  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.24  1999/11/03 15:29:44  wenger
// Fixed text alignment problems.
//
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

    public jsdevisec jsc = null; //TEMP -- do we really need this for *every* GData record???
    private DEViseView parentView = null; //TEMP -- do we really need this for *every* GData record???

    //public Rectangle GDataLoc = null;
    public Rectangle GDataLocInScreen = null;

    //TEMP -- why do we need x, y, etc, *and* GDataLoc
    // x, y, z are x0, y0, z0 converted from data units to pixels.
    public int x = 0, y = 0, z = 0, width = 0, height = 0;
    // x0, y0, z0 are the values in the GData (data units).
    public float x0, y0, z0, x1, y1, z1;

    //public String[] data = null;
    public Component symbol = null;
    public boolean isJavaSymbol = false;
    public int symbolType = 0;

    // Values for symbolType.
    public static final int _symOval = 4;
    public static final int _symSegment = 7;
    public static final int _symText = 12;
    public static final int _symEmbeddedTk = 15;
    public static final int _symFixedText = 16;

    public String string = null;
    public Color color = null;
    public Font font = null;
    public int outline = 0;

    private static int _gdCount = 0;
    private static final boolean _debug = false;

    // GData format: <x> <y> <z> <color> <size> <pattern> <orientation>
    // <symbol type> <shape attr 0> ... <shape attr 14>
    public DEViseGData(jsdevisec panel, DEViseView view, String gdata, float xm,
      float xo, float ym, float yo) throws YException
    {
	_gdCount++;
	if (_debug) {
            System.out.println("DEViseGData constructor " + _gdCount);
            System.out.println("Free memory: " +
	      Runtime.getRuntime().freeMemory() + "/" +
	      Runtime.getRuntime().totalMemory());
	}

        jsc = panel;

        parentView = view;
        if (parentView == null) {
            throw new YException("Invalid parent view for GData!");
        }

        String[] data = DEViseGlobals.parseStringGD(gdata, '{', '}', false);
        if (data == null || data.length != 23)
            throw new YException("Invalid GData + {" + gdata + "}");
        gdata = null;

        float size = 0;
        try {
	    // ADD COMMENT about conversion here
            x0 = (Float.valueOf(data[0])).floatValue();
            x = (int)(x0 * xm + xo);
            y0 = (Float.valueOf(data[1])).floatValue();
            y = (int)(y0 * ym + yo);
            z0 = (Float.valueOf(data[2])).floatValue();
	    z = (int)z0;
            size = (Float.valueOf(data[4])).floatValue();
            symbolType = Integer.parseInt(data[7]);
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData!");
        }

	//TEMP -- perhaps this should be done when the symbol is
	// drawn, instead of now
        if (symbolType == _symEmbeddedTk) { // check symbol type
	    EmbeddedTk(data, size, xm, ym);

	} else if (symbolType == _symText) {
	    TextLabel(data, size, xm, ym);

        } else if (symbolType == _symFixedText) {
	    FixedTextLabel(data, size);

        } else if (symbolType == _symOval) {
	    Oval(data, size, xm, ym);

	} else if (symbolType == _symSegment) {
            Segment(data, size, xm, ym);

        } else {
	    DefaultSymbol(data, size, xm, ym);
        }

        //GDataLoc = new Rectangle(x, y, width, height);
        GDataLocInScreen = getLocInScreen();
    }

    protected void finalize() {
	_gdCount--;
	if (_debug) {
            System.out.println("DEViseGData.finalize() " + _gdCount);
            System.out.println("Free memory: " +
	      Runtime.getRuntime().freeMemory() + "/" +
	      Runtime.getRuntime().totalMemory());
        }
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

    protected void TextLabel(String[] data, float size, float xm, float ym)
      throws YException
    {
        isJavaSymbol = false;

        string = data[8];

        float w = 0.0f, h = 0.0f;
        int align, ff, fw, fs;

	// Note: format is ignored for now.  RKW 1999-11-03.

        // default font is courier, regular, nonitalic
        try {
            w = (Float.valueOf(data[10])).floatValue();
            h = (Float.valueOf(data[11])).floatValue();

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

        color = DEViseUIGlobals.convertColor(data[3]);

        // Note: not using the default font here is probably less
        // efficient, but the previous version of this code failed
        // if we needed different fonts for different symbols.
        // RKW 1999-11-02.
        if (w < 0.0f) {
	    // Note: Z value in GData (if > 1) is max point size of font.
            font = DEViseUIGlobals.getFont(string, height, ff, fw, fs, (int)z0);
        } else {
	    // Note: Z value in GData (if > 1) is max point size of font.
            font = DEViseUIGlobals.getFont(string, width, height, ff, fw, fs, (int)z0);
        }

        if (color == null || font == null) {
            string = null;
            return;
        }

	AlignText(align);

        x = parentView.viewLocInCanvas.x + x;
        y = parentView.viewLocInCanvas.y + y;
    }

    protected void FixedTextLabel(String[] data, float size) throws YException
    {
        isJavaSymbol = false;

        string = data[8];

        float w = 0.0f, h = 0.0f;
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

        color = DEViseUIGlobals.convertColor(data[3]);

        // Note: not using the default font here is probably less
        // efficient, but the previous version of this code failed
        // if we needed different fonts for different symbols.
        // RKW 1999-11-02.
        int fsize;
        if (size > 1.0f) {
            fsize = (int)(size + 0.25f);
        } else {
            fsize = (int)(size * DEViseUIGlobals.screenSize.height + 0.25f);
        }
        font = DEViseUIGlobals.getFont(fsize, ff, fw, fs);

        if (color == null || font == null) {
            string = null;
            return;
        }

	AlignText(align);

        x = parentView.viewLocInCanvas.x + x;
        y = parentView.viewLocInCanvas.y + y;
    }

    protected void EmbeddedTk(String[] data, float size, float xm, float ym)
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
        button.setFont(DEViseFonts.getFont(10, DEViseFonts.MONOSPACED, 0, 0));
        button.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent event)
                {
                    if (DEViseUIGlobals.isApplet) {
                        if (DEViseUIGlobals.browser != null) {
                            try {
                                URL url = new URL(event.getActionCommand());
                                //DEViseUIGlobals.browser.showDocument(url, "_blank");
                                DEViseUIGlobals.browser.showDocument(url, "_parent");
                            } catch (MalformedURLException e) {
                                //YGlobals.Ydebugpn("Invalid URL {" + event.getActionCommand() + "}");
                            }
                        }
                    }
                }
            });

        symbol = button;
    }

    protected void Oval(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0)
            width = -width;
        if (height < 0)
            height = -height;

        color = DEViseUIGlobals.convertColor(data[3]);

	// ADD COMMENT -- is this the name of the element??
        string = data[10];
    }

    protected void Segment(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0)
            width = -width;
        if (height < 0)
            height = -height;

        color = DEViseUIGlobals.convertColor(data[3]);

        x1 = x0 + size * (Float.valueOf(data[8])).floatValue();
        y1 = y0 + size * (Float.valueOf(data[9])).floatValue();
        z1 = z0 + size * (Float.valueOf(data[10])).floatValue();
        //z1 = (int)(z0 + size * ym * z1 );
    }

    protected void DefaultSymbol(String[] data, float size, float xm, float ym)
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
