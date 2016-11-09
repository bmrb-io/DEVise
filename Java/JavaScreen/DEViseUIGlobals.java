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

// This class contains GUI-related constants and utility functions.
// It was split out from DEViseGlobals.java so that the jspop wouldn't
// need an X server to run.

// ------------------------------------------------------------------------

// This class contains a number of constants and utility functions.
// Note: all members are static.

// ------------------------------------------------------------------------

package JavaScreen;

import java.util.*;
import java.awt.*;
import java.applet.*;

public final class DEViseUIGlobals
{
    // global variables
    public static final String javaScreenTitle =
        "DEVise JavaScreen -- Version " + DEViseGlobals.VERSION;

    public static final int COLOR_MIN = 0; // min. integer RGB value
    public static final int COLOR_MAX = 255; // max. integer RGB value

    // DEVise font name
    public static final String[] DEViseFont = {"Monospaced", "Serif",
                                               "SanSerif"
                                              };

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

    // size = size in points
    // ff = font family (0 = "Monospaced", 1 = "Serif", 2 = "SanSerif")
    // fw = font weight (0 = plain, 1 = bold)
    // fs = font style (0 = plain, 1 = italic)
    public static Font getFont(int size, int ff, int fw, int fs)
    {
        return DEViseFonts.getFont(size, ff, fw, fs);
    }

    // Return a font that fits the given string within the given height
    // (specified in pixels).
    // ff = font family (0 = "Monospaced", 1 = "Serif", 2 = "SanSerif")
    // fw = font weight (0 = plain, 1 = bold)
    // fs = font style (0 = plain, 1 = italic)
    public static Font getFont(String str, int height, int ff, int fw, int fs, int maxPointSize)
    {
        return getFont(str, -1, height, ff, fw, fs, maxPointSize);
    }

    // Return a font that fits the given string within the given width and
    // height (specified in pixels).
    // ff = font family (0 = "Monospaced", 1 = "Serif", 2 = "SanSerif")
    // fw = font weight (0 = plain, 1 = bold)
    // fs = font style (0 = plain, 1 = italic)
    public static Font getFont(String str, int width, int height, int ff, int fw, int fs, int maxPointSize)
    {
        if (str == null || str.length() == 0 || height < 1) {
            return null;
        }

        boolean checkWidth = true;
        if (width < 0) {
            checkWidth = false;
        }

        // as of right now, I only recognize Courier (Monospaced), Times
        // (Serif) and Helvetica (SansSerif).
        // Default is Courier.
        if (ff < 0 || ff >= DEViseFont.length) {
            ff = 1;
        }

        int minSize = 1, maxSize = 100, fontSize = 12;
        Font okFont = null;

        if (maxPointSize > 1) maxSize = maxPointSize;
        if (fontSize > maxSize) fontSize = maxSize;

        int fontstyle = ((fw == 0)?Font.PLAIN:Font.BOLD) +
                        ((fs == 0)?Font.PLAIN:Font.ITALIC);

        Toolkit tk = Toolkit.getDefaultToolkit();
        Font font = DEViseFonts.getFont(fontSize, ff, fw, fs);
        FontMetrics fm = tk.getFontMetrics(font);
        int h = fm.getHeight(), w = fm.stringWidth(str);

        if (h > height || (checkWidth && w > width)) {
            // First guess is too big.

            while (fontSize > minSize) {
                fontSize--;
                font = DEViseFonts.getFont(fontSize, ff, fw, fs);
                if (font == null) {
                    continue;
                }

                fm = tk.getFontMetrics(font);
                h = fm.getHeight();
                if (h < height) {
                    if (checkWidth) {
                        w = fm.stringWidth(str);
                        if (w < width) {
                            // Current font is small enough.
                            okFont = font;
                            break;
                        }
                    } else {
                        // Current font is small enough.
                        okFont = font;
                        break;
                    }
                }
            }
        } else {
            // First guess is not too big.

            okFont = font;

            while (fontSize < maxSize) {
                fontSize++;
                font = DEViseFonts.getFont(fontSize, ff, fw, fs);
                if (font == null) {
                    continue;
                }

                fm = tk.getFontMetrics(font);
                h = fm.getHeight();

                if (h > height) {
                    // Current font is too big.
                    break;
                } else {
                    if (checkWidth) {
                        w = fm.stringWidth(str);
                        if (w > width) {
                            // Current font is too big.
                            break;
                        }
                    }

                    okFont = font;
                }
            }
        }

        return okFont;
    }
}
