import java.awt.*;
import java.awt.event.*;
import java.util.*;

public final class YGlobals
{
    // global constants
    public static final String YIDNO = new String("No"), YIDYES = new String("Yes"),
                               YIDOK = new String("OK"), YIDCANCEL = new String("Cancel"),
                               YIDRETRY = new String("Retry"), YIDABORT = new String("Abort"),
                               YIDIGNORE = new String("Ignore");
    public static final int YMBXOK = 0, YMBXYESNO = 1, YMBXOKCANCEL = 2, YMBXYESNOCANCEL = 3,
                            YMBXRETRYCANCEL = 4, YMBXABORTRETRYIGNORE = 5;

    // global variables
    public static boolean YISDEBUG = false, YISLOG = false, YISGUI = false, YISAPPLET = false;
    public static YDebug DebugInfo = null;

    static {
        DebugInfo = new YDebug();
    }

    // global functions
    public static void start()
    {
        DebugInfo = new YDebug();
    }

    public static void close()
    {
        YDebug.close();
    }

    public static void Ypmsg(String msg)
    {
        System.out.print(msg);
    }

    public static void Ypnmsg(String msg)
    {
        System.out.println(msg);
    }

    public static String Yshowmsg(Frame frame, String msg, String title, int style, boolean isCenterScreen, boolean isModal)
    {
        YMSGDlg dlg = new YMSGDlg(frame, msg, title, style, isCenterScreen, isModal);
        //dlg.show();
        return dlg.getResult();
    }

    public static String Yshowmsg(Frame frame, String msg, String title, int style, boolean isCenterScreen)
    {
        return Yshowmsg(frame, msg, title, style, isCenterScreen, true);
    }

    public static String Yshowmsg(Frame frame, String msg, String title, int style)
    {
        return Yshowmsg(frame, msg, title, style, false, true);
    }

    public static String Yshowmsg(Frame frame, String msg, int what)
    {
        return Yshowmsg(frame, msg, "Confirm", YMBXYESNO, false, true);
    }

    public static String Yshowmsg(Frame frame, String msg, int what, boolean isCenterScreen, boolean isModal)
    {
        return Yshowmsg(frame, msg, "Confirm", YMBXYESNO, isCenterScreen, isModal);
    }

    public static String Yshowmsg(Frame frame, String msg)
    {
        return Yshowmsg(frame, msg, "Program Message", YMBXOK, false, true);
    }

    public static String Yshowmsg(Frame frame, String msg, boolean isCenterScreen, boolean isModal)
    {
        return Yshowmsg(frame, msg, "Program Message", YMBXOK, isCenterScreen, isModal);
    }

    public static void Ydebugpn(String msg, int id)
    {
        YDebug.println(msg, id);
    }

    public static void Ydebugpn(String msg)
    {
        Ydebugpn(msg, 0);
    }

    public static void Ydebugp(String msg, int id)
    {
        YDebug.print(msg, id);
    }

    public static void Ydebugp(String msg)
    {
        Ydebugp(msg, 0);
    }

    public static char Ytochar(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset)
            return (char)0;

        int v1 = data[0 + offset];
        int v2 = data[1 + offset];

        return (char)((v1 << 8) + (v2 << 0));
    }

    public static char Ytochar(byte[] data)
    {
        return Ytochar(data, 0);
    }

    public static short Ytoshort(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset)
            return 0;

        int v1 = data[0 + offset];
        int v2 = data[1 + offset];

        return (short)((v1 << 8) + (v2 << 0));
    }

    public static short Ytoshort(byte[] data)
    {
        return Ytoshort(data, 0);
    }

    public static int YtoUshort(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset)
            return 0;

        byte b1 = data[0 + offset];
        byte b2 = data[1 + offset];
        byte b3 = -128;
        int v1, v2;

        if (b1 < 0) {
            v1 = -b1 + 128;
        } else {
            v1 = b1;
        }

        if (b2 < 0) {
            v2 = -b2 + 128;
        } else {
            v2 = b2;
        }

        return ((v1 << 8) + (v2 << 0));
    }

    public static int YtoUshort(byte[] data)
    {
        return YtoUshort(data, 0);
    }

    public static int Ytoint(byte[] data, int offset)
    {
        if (data == null || data.length < 4 + offset)
            return 0;

        int v1 = data[0 + offset];
        int v2 = data[1 + offset];
        int v3 = data[2 + offset];
        int v4 = data[3 + offset];

        return (char)((v1 << 24) + (v2 << 16) + (v3 << 8) + (v1 << 0));
    }

    public static int Ytoint(byte[] data)
    {
        return Ytoint(data, 0);
    }

    public static long Ytolong(byte[] data, int offset)
    {
        if (data == null || data.length < 8 + offset)
            return 0;

        return ( (((long)Ytoint(data, offset)) << 32) + (Ytoint(data, offset + 4) & 0xFFFFFFFFL));
    }

    public static long Ytolong(byte[] data)
    {
        return Ytolong(data, 0);
    }

    public static float Ytofloat(byte[] data, int offset)
    {
        if (data == null || data.length < 4 + offset)
            return 0.0f;

        return Float.intBitsToFloat(Ytoint(data, offset));
    }

    public static float Ytofloat(byte[] data)
    {
        return Ytofloat(data, 0);
    }

    public static double Ytodouble(byte[] data, int offset)
    {
        if (data == null || data.length < 8 + offset)
            return 0.0;

        return Double.longBitsToDouble(Ytolong(data, offset));
    }

    public static double Ytodouble(byte[] data)
    {
        return Ytodouble(data, 0);
    }

    public static String[] Yparsestr(String str, String delim, boolean returnDelim)
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
            if (returnDelim) {
                outStr[i] = (String)tokenList.elementAt(i) + delim;
            } else {
                outStr[i] = (String)tokenList.elementAt(i);
            }
        }

        return outStr;
    }

    public static String[] Yparsestr(String str, String delim)
    {
        return Yparsestr(str, delim, false);
    }

    public static String[] Yparsestr(String str)
    {
        return Yparsestr(str, " ", false);
    }

    public static String[] Yparsestring(String inputStr, char startChar, char endChar, boolean keep)
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
        byte isInit = 0, isSpace = 1, isStartChar = 2, isSpecialChar = 3, isChar = 4;
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

    public static String[] Yparsestring(String inputStr, char startChar, char endChar)
    {
        return Yparsestring(inputStr, startChar, endChar, false);
    }

}

final class YMSGDlg extends Dialog
{
    private Label label[] = null;
    private Button button[] = null;
    private String result = null;
    private Color bgcolor = new Color(64, 96, 0);
    private Color fgcolor = Color.white;
    private Font font = new Font("Serif", Font.PLAIN, 14);

    public YMSGDlg(Frame frame, String msg, String title, int style, boolean isCenterScreen, boolean isModal)
    {
        super(frame, title, isModal);

        boolean isStyleCorrect = true;

        switch (style) {
        case YGlobals.YMBXOK:
            button = new Button[1];
            button[0] = new Button(YGlobals.YIDOK);
            result = YGlobals.YIDOK;
            break;
        case YGlobals.YMBXYESNO:
            button = new Button[2];
            button[0] = new Button(YGlobals.YIDYES);
            button[1] = new Button(YGlobals.YIDNO);
            result = YGlobals.YIDNO;
            break;
        case YGlobals.YMBXOKCANCEL:
            button = new Button[2];
            button[0] = new Button(YGlobals.YIDOK);
            button[1] = new Button(YGlobals.YIDCANCEL);
            result = YGlobals.YIDCANCEL;
            break;
        case YGlobals.YMBXYESNOCANCEL:
            button = new Button[3];
            button[0] = new Button(YGlobals.YIDYES);
            button[1] = new Button(YGlobals.YIDNO);
            button[2] = new Button(YGlobals.YIDCANCEL);
            result = YGlobals.YIDCANCEL;
            break;
        case YGlobals.YMBXRETRYCANCEL:
            button = new Button[2];
            button[0] = new Button(YGlobals.YIDRETRY);
            button[1] = new Button(YGlobals.YIDCANCEL);
            result = YGlobals.YIDCANCEL;
            break;
        case YGlobals.YMBXABORTRETRYIGNORE:
            button = new Button[3];
            button[0] = new Button(YGlobals.YIDABORT);
            button[1] = new Button(YGlobals.YIDRETRY);
            button[2] = new Button(YGlobals.YIDIGNORE);
            result = YGlobals.YIDIGNORE;
            break;
        default:
            isStyleCorrect = false;
            button = new Button[1];
            button[0] = new Button(YGlobals.YIDOK);
            result = YGlobals.YIDOK;
            break;
        }

        if (isStyleCorrect) {
            if (msg == null) {
                label = new Label[1];
                label[0] = new Label("No Message Given!");
                button = new Button[1];
                button[0] = new Button(YGlobals.YIDOK);
                result = YGlobals.YIDOK;
            } else if (msg.equals("")) {
                label = new Label[1];
                label[0] = new Label("No Message Given!");
                button = new Button[1];
                button[0] = new Button(YGlobals.YIDOK);
                result = YGlobals.YIDOK;
            } else {
                String[] msgStr = YGlobals.Yparsestr(msg, "\n");
                if (msgStr == null) {
                    button = new Button[1];
                    button[0] = new Button(YGlobals.YIDOK);
                    result = YGlobals.YIDOK;
                    label = new Label[1];
                    label[0] = new Label("Invalid message given!");
                } else {
                    label = new Label[msgStr.length];
                    for (int i = 0; i < msgStr.length; i++) {
                        if (msgStr[i] != null) {
                            label[i] = new Label(msgStr[i]);
                        } else {
                            label[i] = new Label("");
                        }
                    }
                }
            }
        }  else  {
            label = new Label[1];
            label[0] = new Label("Unsupported dialog style given!");
        }

        setBackground(bgcolor);
        setForeground(fgcolor);
        setFont(font);

        // building the panel that display messages
        Panel panel1 = new Panel();
        panel1.setLayout(new GridLayout(0, 1, 0, 0));
        for (int i = 0; i < label.length; i++) {
            panel1.add(label[i]);
        }

        // building the panel that display buttons
        Panel panel2 = new Panel();
        panel2.setLayout(new GridLayout(1, 0, 10, 0));
        for (int i = 0; i < button.length; i++) {
            button[i].setBackground(bgcolor);
            button[i].setForeground(fgcolor);
            button[i].setFont(font);
            button[i].setActionCommand(button[i].getLabel());
            panel2.add(button[i]);
        }

        // set new gridbag layout
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = 1;
        c.fill = GridBagConstraints.NONE;
        c.insets = new Insets(10, 10, 0, 0);
        c.ipadx = 0;
        c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        //c.weightx = 1.0;
        //c.weighty = 1.0;

        setLayout(gridbag);

        gridbag.setConstraints(panel1, c);
        add(panel1);

        c.insets = new Insets(10, 10, 5, 10);
        gridbag.setConstraints(panel2, c);
        add(panel2);

        pack();
        //setResizable(false);

        // reposition the dialog
        Point parentLoc = null;
        Dimension parentSize = null;
        if (isCenterScreen) {
            Toolkit kit = Toolkit.getDefaultToolkit();
            parentSize = kit.getScreenSize();
            parentLoc = new Point(0, 0);
        } else {
            parentLoc = frame.getLocation();
            parentSize = frame.getSize();
        }

        Dimension mysize = getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        // event handler
        for (int i = 0; i < button.length; i++)  {
            button[i].addActionListener(new ActionListener()
                    {
                        public void actionPerformed(ActionEvent event)  {
                            result = event.getActionCommand();
                            dispose();
                        }
                    });
        }

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        show();
    }

    public String getResult()
    {
        return result;
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            dispose();
        }

        super.processEvent(event);
    }
}

