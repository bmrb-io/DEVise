import java.awt.*;
import java.awt.event.*;
import java.util.*;

public final class YGlobals
{
    // global constants
    public static final String IDNO = new String("   No   "), IDYES = new String(" Yes "), IDOK = new String("   OK   "), IDCANCEL = new String("Cancel");
    public static final int MBXOK = 0, MBXYESNO = 1, MBXOKCANCEL = 2, MBXYESNOCANCEL = 3;

    // global variables
    public static boolean ISDEBUG = false, ISLOG = false, ISGUI = false, ISAPPLET = false;
    public static YDebug DebugInfo = null;
    public static YLogFile LogFile = null;

    // global functions
    public static void printMsg(String msg)
    {
        System.out.println(msg);
    }

    public static String showMsg(Frame frame, String msg, String title, int style, boolean isCenterScreen)
    {
        YMSGDlg dlg = new YMSGDlg(frame, msg, title, style, isCenterScreen);
        dlg.show();
        return dlg.getResult();
    }
    
    public static String showMsg(Frame frame, String msg, String title, int style)
    {
        return showMsg(frame, msg, title, style, false);
    }
    
    public static String showMsg(Frame frame, String msg, String title)
    {
        return showMsg(frame, msg, title, YGlobals.MBXOK, false);
    }

    public static String showMsg(Frame frame, String msg)
    {
        return showMsg(frame, msg, "Program Message", YGlobals.MBXOK, false);
    }

    public static String[] parseStr(String str, String delim)
    {
        return parseStr(str, delim, false);
    }

    public static String[] parseStr(String str)
    {
        return parseStr(str, " ", false);
    }

    public static String[] parseStr(String str, String delim, boolean returnDelim)
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

    public static String[] parseString(String inputStr, char startChar, char endChar)
    {
        return parseString(inputStr, startChar, endChar, false);
    }

    public static String[] parseString(String inputStr, char startChar, char endChar, boolean keep)
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
}

final class YMSGDlg extends Dialog
{
    private Panel panel1 = new Panel();
    private Panel panel2 = new Panel();
    private Panel panel3 = new Panel();
    private Label label[] = null;
    private Button button[] = null;
    private String result = null;
    private Color buttonbgcolor = new Color(64, 96, 0);
    private Color buttonfgcolor = Color.white;
    private Font buttonfont = new Font("Serif", Font.PLAIN, 14);
    private Color bgcolor = new Color(64, 96, 0);
    private Color fgcolor = Color.white;
    private Font font = new Font("Serif", Font.PLAIN, 14);

    public YMSGDlg(Frame frame, String msg, String title, int style, boolean isCenterScreen)
    {
        super(frame, title, true);

        boolean isStyleCorrect = true;

        switch (style)  {
        case YGlobals.MBXOK:
            button = new Button[1];
            button[0] = new Button(YGlobals.IDOK);
            break;
        case YGlobals.MBXYESNO:
            button = new Button[2];
            button[0] = new Button(YGlobals.IDYES);
            button[1] = new Button(YGlobals.IDNO);
            break;
        case YGlobals.MBXOKCANCEL:
            button = new Button[2];
            button[0] = new Button(YGlobals.IDOK);
            button[1] = new Button(YGlobals.IDCANCEL);
            break;
        case YGlobals.MBXYESNOCANCEL:
            button = new Button[3];
            button[0] = new Button(YGlobals.IDYES);
            button[1] = new Button(YGlobals.IDNO);
            button[2] = new Button(YGlobals.IDCANCEL);
            break;
        default:
            isStyleCorrect = false;
            button = new Button[1];
            button[0] = new Button(YGlobals.IDOK);
            break;
        }

        if (isStyleCorrect)  {
            if (msg == null)  {
                label = new Label[1];
                label[0] = new Label("Null Message Given!");
            }  else if (msg.equals(""))  {
                label = new Label[1];
                label[0] = new Label("No Message Given!");
            }  else  {
                String[] msgStr = YGlobals.parseStr(msg, "\n");
                if (msgStr == null) {
                    button = new Button[1];
                    button[0] = new Button(YGlobals.IDOK);
                    label = new Label[1];
                    label[0] = new Label("Invalid Message given!");
                } else {
                    label = new Label[msgStr.length];
                    for (int i = 0; i < msgStr.length; i++)
                        label[i] = new Label(msgStr[i]);
                }    
            }
        }  else  {
            label = new Label[1];
            label[0] = new Label("Invalid Dialog Style!");
        }

        setBackground(bgcolor);
        setForeground(fgcolor);
        setFont(font);

        // building the panel that display messages
        panel1.setLayout(new GridLayout(0, 1, 0, 0));
        for (int i = 0; i < label.length; i++)
            panel1.add(label[i]);

        // building the panel that display buttons
        panel2.setLayout(new GridLayout(1, 0, 10, 0));
        for (int i = 0; i < button.length; i++)  {
            button[i].setBackground(buttonbgcolor);
            button[i].setForeground(buttonfgcolor);
            button[i].setFont(buttonfont);
            button[i].setActionCommand(button[i].getLabel());
            panel2.add(button[i]);
        }

        // add panel1 and panel2
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();

        // panel3 act as an holder of panel2
        panel3.setLayout(gridbag);
        gridbag.setConstraints(panel2, c);
        panel3.add(panel2);

        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.gridheight = 1;
        c.fill = GridBagConstraints.HORIZONTAL;
        c.insets = new Insets(10, 10, 10, 10);
        c.ipadx = 0;
        c.ipady = 0;
        //c.anchor = GridBagConstraints.CENTER;
        //c.weightx = 1.0;
        //c.weighty = 1.0;

        setLayout(gridbag);
        gridbag.setConstraints(panel1, c);
        add(panel1);
        c.insets = new Insets(0, 10, 5, 10);
        gridbag.setConstraints(panel3, c);
        add(panel3);

        pack();
        setResizable(false);
        
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
    }

    public String getResult()
    {
        return result;
    }
}

