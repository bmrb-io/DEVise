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

// $Id$

// $Log$
// Revision 1.2  2000/05/04 17:40:28  wenger
// Added new text object feature: GData Z value specifies max size of
// font in points (if > 1).  (Allows me to fix problems with BMRB 4096
// protein session.)
//
// Revision 1.1  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//

// This class contains a number of constants and utility functions.
// Note: all members are static.

// ------------------------------------------------------------------------


import java.util.*;
import java.awt.*;
import java.applet.*;

public final class DEViseUIGlobals
{
    // global variables
    public static boolean isApplet = false, inBrowser = false;
    public static final String javaScreenTitle = "DEVise JavaScreen -- Version " + DEViseGlobals.VERSION;

    public static AppletContext browser = null;

    public static Dimension maxScreenSize = new Dimension(0, 0);
    public static Dimension minScreenSize = new Dimension(0, 0);
    public static Dimension screenSize = new Dimension(0, 0);
    public static Dimension rubberBandLimit = new Dimension(4, 4);

    public static Color bg = new Color(64, 96, 0);
    public static Color fg = Color.white;
    public static Color screenBg = new Color(64, 192, 0);
    public static Font font = new Font("Serif", Font.PLAIN, 14);
    public static Color textBg = new Color(160, 160, 160);
    public static Color textFg = Color.black;
    public static Font textFont = new Font("Serif", Font.PLAIN, 14);

    // DEVise font name
    public static String[] DEViseFont = {"Monospaced", "Serif", "SanSerif"};

    public static Cursor defaultCursor = new Cursor(Cursor.DEFAULT_CURSOR);
    public static Cursor waitCursor = new Cursor(Cursor.WAIT_CURSOR);
    public static Cursor rbCursor = new Cursor(Cursor.CROSSHAIR_CURSOR);
    public static Cursor moveCursor = new Cursor(Cursor.MOVE_CURSOR);
    public static Cursor lrsCursor = new Cursor(Cursor.W_RESIZE_CURSOR);
    public static Cursor rrsCursor = new Cursor(Cursor.E_RESIZE_CURSOR);
    public static Cursor trsCursor = new Cursor(Cursor.N_RESIZE_CURSOR);
    public static Cursor brsCursor = new Cursor(Cursor.S_RESIZE_CURSOR);
    public static Cursor tlrsCursor = new Cursor(Cursor.NW_RESIZE_CURSOR);
    public static Cursor blrsCursor = new Cursor(Cursor.SW_RESIZE_CURSOR);
    public static Cursor trrsCursor = new Cursor(Cursor.NE_RESIZE_CURSOR);
    public static Cursor brrsCursor = new Cursor(Cursor.SE_RESIZE_CURSOR);

    public static Color convertColor(String str)
    {
        if (str == null || str.length() != 13 || !str.startsWith("#")) {
            return null;
        }

        String red = str.substring(1, 3), green = str.substring(5, 7), blue = str.substring(9, 11);
        try {
            int r = (Integer.valueOf(red, 16)).intValue(), g = (Integer.valueOf(green, 16)).intValue(), b = (Integer.valueOf(blue, 16)).intValue();
            Color color = new Color(r, g, b);
            return color;
        } catch (NumberFormatException e) {
            return null;
        }
    }

    // ADD COMMENT -- what are units for size? points?
    // TEMP -- info for ff (font family)
    // TEMP -- what are fw, fs?
    public static Font getFont(int size, int ff, int fw, int fs)
    {
        if (size < 4) {
            return null;
        }

	// TEMP -- should have defined constants for font family
        if (ff < 0 || ff > 2) {
            ff = 1;
        }

        int fontstyle = ((fw == 0)?Font.PLAIN:Font.BOLD) + ((fs == 0)?Font.PLAIN:Font.ITALIC);

        return new Font(DEViseFont[ff], fontstyle, size);
    }

    public static Font getFont(String str, int height, int ff, int fw, int fs, int maxPointSize)
    {
        return getFont(str, -1, height, ff, fw, fs, maxPointSize);
    }

    // ADD COMMENT -- what are width and height?
    public static Font getFont(String str, int width, int height, int ff, int fw, int fs, int maxPointSize)
    {
        if (str == null || str.length() == 0 || height < 1) {
            return null;
        }

        boolean checkWidth = true;
        if (width < 0) {
            checkWidth = false;
        }

        // as of right now, I only recognize courier(Monospaced), times(Serif) and Helvetica(SansSerif)
        // default is courier
        if (ff < 0 || ff > 2) {
            ff = 1;
        }

        int minSize = 1, maxSize = 100, fontSize = 12, okSize = -1;
	if (maxPointSize > 1) maxSize = maxPointSize;
        int fontstyle = ((fw == 0)?Font.PLAIN:Font.BOLD) + ((fs == 0)?Font.PLAIN:Font.ITALIC);

        Toolkit tk = Toolkit.getDefaultToolkit();
        Font font = new Font(DEViseFont[ff], fontstyle, fontSize);
        FontMetrics fm = tk.getFontMetrics(font);
        int h = fm.getHeight(), w = fm.stringWidth(str);

        if (h > height || (checkWidth && w > width)) {
            okSize = -1;

            while (fontSize > minSize) {
                fontSize--;
                font = new Font(DEViseFont[ff], fontstyle, fontSize);
                if (font == null) {
                    continue;
                }
                fm = tk.getFontMetrics(font);
                h = fm.getHeight();
                if (h < height) {
                    if (checkWidth) {
                        w = fm.stringWidth(str);
                        if (w < width) {
                            okSize = fontSize;
                            break;
                        }
                    } else {
                        okSize = fontSize;
                        break;
                    }
                }
            }

            if (okSize > 0) {
                return new Font(DEViseFont[ff], fontstyle, okSize);
            } else {
                return null;
            }
        } else {
            okSize = fontSize;

            while (fontSize < maxSize) {
                fontSize++;
                font = new Font(DEViseFont[ff], fontstyle, fontSize);
                if (font == null) {
                    continue;
                }
                fm = tk.getFontMetrics(font);
                h = fm.getHeight();

                if (h > height) {
                    break;
                } else {
                    if (checkWidth) {
                        w = fm.stringWidth(str);
                        if (w > width) {
                            break;
                        }
                    }

                    okSize = fontSize;
                }
            }

            return new Font(DEViseFont[ff], fontstyle, okSize);
        }
    }
}
