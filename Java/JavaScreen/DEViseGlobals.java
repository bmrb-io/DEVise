// DEViseGlobals.java
// last updated on 04/11/99

import java.util.*;
import java.awt.*;
import java.applet.*;

public final class DEViseGlobals
{
    // global constants
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2,
                              API_CTL = 3, API_JAVA = 5, API_IMAGE = 6,
                              API_DATA = 7;
    public static final int DEFAULTCMDPORT = 6666, DEFAULTIMGPORT = 6688;
    public static final int DEFAULTID = -1;
    public static final String DEFAULTUSER = new String("guest");
    public static final String DEFAULTPASS = new String("guest");
    public static final String DEFAULTHOST = new String("localhost");

    // global variables
    public static boolean isApplet = false, inBrowser = false;

    public static int cmdport = 0, imgport = 0;
    public static int connectionID = -1;
    public static String username = null, password = null, hostname = null;

    public static AppletContext browser = null;

    public static Dimension actualScreenSize = new Dimension(0, 0);
    public static Dimension screenSize = new Dimension(0, 0);
    public static Dimension screenEdge = new Dimension(2, 2);
    public static Dimension rubberBandLimit = new Dimension(4, 4);

    public static Color bg = new Color(64, 96, 0);
    public static Color fg = Color.white;
    public static Font font = new Font("Serif", Font.PLAIN, 14);
    public static Color textBg = new Color(160, 160, 160);
    public static Color textFg = Color.black;
    public static Font textFont = new Font("Serif", Font.PLAIN, 14);

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
}

