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

// $Id$

// $Log$
// Revision 1.32  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.31  2000/01/17 07:50:17  hongyu
// *** empty log message ***
//
// Revision 1.30  1999/12/10 15:30:13  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// Revision 1.29  1999/12/08 15:34:12  wenger
// Changed version to 3.1.
//
// Revision 1.28  1999/12/07 23:24:26  hongyu
// *** empty log message ***
//
// Revision 1.27  1999/10/10 19:59:26  hongyu
// *** empty log message ***
//
// Revision 1.26  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.25  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.24  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.23  1999/08/17 06:15:17  hongyu
// *** empty log message ***
//
// Revision 1.22  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.20  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.18  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseGlobals.java

import java.util.*;
import java.awt.*;
import java.applet.*;

public final class DEViseGlobals
{
    // global constants
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2,
                              API_CTL = 3, API_JAVA = 5, API_IMAGE = 6,
                              API_DATA = 7;
    public static final int DEFAULTCMDPORT = 6666, DEFAULTIMGPORT = 6644, JSSPORT = 1688, JSPOPPORT = 1689;
    public static final String JSPOPHOST = new String("localhost");
    public static final String VERSION = new String("3.2");
    public static final String PROTOCOL_VERSION = new String("4.0");
    public static final int DEFAULTID = -1;
    public static final String DEFAULTUSER = new String("guest");
    public static final String DEFAULTPASS = new String("guest");
    public static final String DEFAULTHOST = new String("localhost");

    // global variables
    public static boolean isApplet = false, inBrowser = false;
    public static final String javaScreenTitle = "DEVise JavaScreen -- Version " + DEViseGlobals.VERSION;

    public static int cmdport = 0, imgport = 0;
    public static int connectionID = -1;
    public static String username = null, password = null, hostname = null;

    public static AppletContext browser = null;

    public static Dimension maxScreenSize = new Dimension(0, 0);
    public static Dimension minScreenSize = new Dimension(0, 0);
    public static Dimension screenSize = new Dimension(0, 0);
    public static Dimension rubberBandLimit = new Dimension(4, 4);

    public static Color bg = new Color(64, 96, 0);
    public static Color fg = Color.white;
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

    // global functions
    public static String[] parseString(String inputStr, char startChar,
                                       char endChar, boolean keep)
    {
        String[] outputStr = null;

        // Check empty string
        if (inputStr.equals(""))  {
            outputStr = new String[1];
            outputStr[0] = new String("");
            return outputStr;
        }

        Vector strBuffer = new Vector();
        // isInit: denote the starting state
        // isSpace: denote the space character ' '
        // isStartChar: denote the startChar
        // isSpecialChar: denote any character between startChar and endChar
        // isChar: denote any other character
        byte isInit = 0, isSpace = 1, isStartChar = 2, isSpecialChar = 3,
             isChar = 4;
        byte state = isInit; // denote the state of previous character
        char testChar;
        StringBuffer buffer = new StringBuffer("");
        int i;

        for (i = 0; i < inputStr.length(); i++) {
            testChar = inputStr.charAt(i);

            if (testChar == startChar) {
                if (state == isInit || state == isSpace) {
                    if (keep) {
                        buffer.append(testChar);
                    }
                } else if (state == isStartChar || state == isSpecialChar) {
                    return null;
                } else if (state == isChar) {
                    strBuffer.addElement(buffer.toString());
                    buffer = new StringBuffer("");
                }

                state = isStartChar;
            } else if (testChar == endChar) {
                if (state == isInit || state == isSpace || state == isChar) {
                    return null;
                } else if (state == isSpecialChar || state == isStartChar) {
                    if (keep)
                        buffer.append(testChar);
                    strBuffer.addElement(buffer.toString());
                    buffer = new StringBuffer("");
                }

                state = isSpace;
            } else if (testChar == ' ') {
                if (state == isSpecialChar || state == isStartChar)  {
                    buffer.append(testChar);
                    state = isSpecialChar;
                }  else if (state == isInit || state == isSpace)  {
                    state = isSpace;
                }  else if (state == isChar)  {
                    strBuffer.addElement(buffer.toString());
                    buffer = new StringBuffer("");
                    state = isSpace;
                }
            } else {
                if (state == isInit || state == isSpace || state == isChar) {
                    buffer.append(testChar);
                    state = isChar;
                } else if (state == isStartChar || state == isSpecialChar) {
                    buffer.append(testChar);
                    state = isSpecialChar;
                }
            }
        }

        if (state == isChar) {
            strBuffer.addElement(buffer.toString());
        } else if (state == isSpace) {
            if (strBuffer.size() == 0) {
                strBuffer.addElement(buffer.toString());
            }
        } else {
            return null;
        }

        outputStr = new String[strBuffer.size()];
        for (i = 0; i < strBuffer.size(); i++) {
            outputStr[i] = (String)strBuffer.elementAt(i);
        }

        return outputStr;
    }

    public static String[] parseString(String inputStr, char startChar, char endChar)
    {
        return DEViseGlobals.parseString(inputStr, startChar, endChar, false);
    }

    public static String[] parseString(String inputStr, boolean keep)
    {
        return DEViseGlobals.parseString(inputStr, '{', '}', keep);
    }

    public static String[] parseString(String inputStr)
    {
        return DEViseGlobals.parseString(inputStr, '{', '}', false);
    }

    public static String[] parseStr(String str, String delim, boolean keep)
    {
        String[] outStr = null;

        if (str == null)
            return null;

        StringTokenizer token = new StringTokenizer(str, delim, false);
        Vector tokenList = new Vector();
        while (token.hasMoreTokens()) {
            try {
                tokenList.addElement(token.nextToken());
            } catch (NoSuchElementException e) {
                // this should not be happening
            }
        }

        outStr = new String[tokenList.size()];
        for (int i = 0; i < outStr.length; i++) {
            if (keep) {
                outStr[i] = (String)tokenList.elementAt(i) + delim;
            } else {
                outStr[i] = (String)tokenList.elementAt(i);
            }
        }

        return outStr;
    }

    public static String[] parseStr(String inputStr, String delim)
    {
        return DEViseGlobals.parseStr(inputStr, delim, false);
    }

    public static String[] parseStr(String inputStr)
    {
        return DEViseGlobals.parseStr(inputStr, "\n", false);
    }

    public static int toUshort(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset)
            return 0;

        int v1 = (int)data[0 + offset] & 0x000000FF;
        int v2 = (int)data[1 + offset] & 0x000000FF;

        return ((v1 << 8) + (v2 << 0));
    }

    public static int toUshort(byte[] data)
    {
        return DEViseGlobals.toUshort(data, 0);
    }

    public static long getCurrentTime()
    {
        return System.currentTimeMillis();
    }

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

    public static Font getFont(int size, int ff, int fw, int fs)
    {
        if (size < 4) {
            return null;
        }

        if (ff < 0 || ff > 2) {
            ff = 1;
        }

        int fontstyle = ((fw == 0)?Font.PLAIN:Font.BOLD) + ((fs == 0)?Font.PLAIN:Font.ITALIC);

        return new Font(DEViseFont[ff], fontstyle, size);
    }

    public static Font getFont(String str, int height, int ff, int fw, int fs)
    {
        return getFont(str, -1, height, ff, fw, fs);
    }

    public static Font getFont(String str, int width, int height, int ff, int fw, int fs)
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

