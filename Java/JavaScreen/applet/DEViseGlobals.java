import  java.util.*;
import  java.awt.*; 
import  java.awt.event.*;

// Defines global constants, global variables and global functions used in DEVise Client
public final class DEViseGlobals
{
    // global constants
    public static final boolean ISDEBUG = true, ISLOG = true;
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2, API_CTL = 3, API_JAVA = 5;
    public static final String NO_OP = new String("   No   "), YES_OP = new String(" Yes "), OK_OP = new String("   OK   "), CANCEL_OP = new String("Cancel");
    public static final int OP_OK = 0, OP_YESNO = 1, OP_OKCANCEL = 2, OP_YESNOCANCEL = 3;
    //public static final Cursor WAITCURSOR = new Cursor(Cursor.WAIT_CURSOR);
        
    // global variables
    public static String DEVISEHOST = new String("localhost");
    public static int CMDPORT = 6100, IMGPORT = 6123, DATAPORT = 6124;
    public static Color uibgcolor = new Color(64, 96, 0);    
    public static Color uifgcolor = Color.white;
    public static Color dialogbgcolor = new Color(64, 96, 0);    
    public static Color dialogfgcolor = Color.white;
    public static Color textbgcolor = new Color(160, 160, 160);    
    public static Color textfgcolor = Color.black;
    public static Color buttonbgcolor = new Color(64, 96, 0);    
    public static Color buttonfgcolor = Color.white;
    public static Font uifont = new Font("Serif", Font.PLAIN, 14);
    public static Font dialogfont = new Font("Serif", Font.PLAIN, 14);
    public static Font textfont = new Font("Serif", Font.PLAIN, 14);
    public static Font buttonfont = new Font("Serif", Font.PLAIN, 14);
    
    // global functions    
    public static String[] parseStr(String inputStr, boolean keepCurly)
    {
        // return null if unsuccessful
        String[] outputStr = null;
        
        // Check empty string
        if (inputStr.equals(""))  {
            outputStr = new String[1];
            outputStr[0] = new String("");
            return outputStr;
        }

        Vector strBuffer = new Vector();
        byte isInit = 0, isWhitespace = 1, isCurly = 2, isCurlyChar = 3, isChar = 4, state = 0;
        char testChar;
        StringBuffer buffer = new StringBuffer("");
        int i;
        
        for (i = 0; i < inputStr.length(); i++)  {
            testChar = inputStr.charAt(i);
            if (testChar == '{')  {
                if (state == isInit || state == isWhitespace)  {
                    if (keepCurly)
                        buffer.append(testChar);
                }  else if (state == isCurly || state == isCurlyChar)  {
                    return null;
                }  else if (state == isChar)  {                    
                    strBuffer.addElement(buffer.toString());
                    buffer = new StringBuffer("");
                }  else  {
                    return null;
                }
                state = isCurly;
            }  else if (testChar == '}')  {
                if (state == isInit || state == isWhitespace || state == isChar)  {
                    return null;
                }  else if (state == isCurlyChar || state == isCurly)  {
                    if (keepCurly)
                        buffer.append(testChar);
                    strBuffer.addElement(buffer.toString());
                    buffer = new StringBuffer("");
                }  else  {
                    return null;
                }
                state = isWhitespace;
            }  else if (testChar == ' ' || testChar == '\n' | testChar == '\t')  {
                if (state == isCurlyChar || state == isCurly)  {
                    buffer.append(testChar);
                    state = isCurlyChar;
                }  else if (state == isInit || state == isWhitespace)  {
                    state = isWhitespace;
                }  else if (state == isChar)  {
                    strBuffer.addElement(buffer.toString());
                    buffer = new StringBuffer("");
                    state = isWhitespace;
                }                
            }  else  {
                if (state == isInit || state == isWhitespace || state == isChar)  {
                    buffer.append(testChar);
                    state = isChar;
                }  else if (state == isCurly || state == isCurlyChar)  {
                    buffer.append(testChar);                
                    state = isCurlyChar;
                }
            }
        }
        
        if (state == isChar)  {
            strBuffer.addElement(buffer.toString());
        }  else if (state == isWhitespace)  {
            if (strBuffer.size() == 0)  {
                strBuffer.addElement(buffer.toString());
            }
        }  else  {
            return null;
        }
        
        outputStr = new String[strBuffer.size()];
        for (i = 0; i < strBuffer.size(); i++)  {
            outputStr[i] = (String)strBuffer.elementAt(i);
        }
        
        return outputStr;
    }
    
    public static String showMsg(Frame frame, String msg, String title, int style)
    {
        DEViseMSGDlg dlg = new DEViseMSGDlg(frame, msg, title, style);
        dlg.show();
        return dlg.getResult();                
    }
    
    public static String showMsg(Frame frame, String msg, String title)
    {
        DEViseMSGDlg dlg = new DEViseMSGDlg(frame, msg, title, DEViseGlobals.OP_OK);
        dlg.show();
        return dlg.getResult();                
    }
                
}

final class DEViseMSGDlg extends Dialog
{
    private Panel panel1 = new Panel();
    private Panel panel2 = new Panel();
    private Panel panel3 = new Panel();
    private Label label[] = null;
    private Button button[] = null;
    private String result = null;
    
    public DEViseMSGDlg(Frame frame, String msg, String title, int style)
    {
        super(frame, title, true);
        
        boolean isStyleCorrect = true;
        int i;
        
        switch (style)  {
        case DEViseGlobals.OP_OK:
            button = new Button[1];
            button[0] = new Button(DEViseGlobals.OK_OP);
            break;
        case DEViseGlobals.OP_YESNO:
            button = new Button[2];
            button[0] = new Button(DEViseGlobals.YES_OP);
            button[1] = new Button(DEViseGlobals.NO_OP);            
            break;
        case DEViseGlobals.OP_OKCANCEL:
            button = new Button[2];
            button[0] = new Button(DEViseGlobals.OK_OP);
            button[1] = new Button(DEViseGlobals.CANCEL_OP);            
            break;
        case DEViseGlobals.OP_YESNOCANCEL:
            button = new Button[3];
            button[0] = new Button(DEViseGlobals.YES_OP);
            button[1] = new Button(DEViseGlobals.NO_OP);
            button[2] = new Button(DEViseGlobals.CANCEL_OP);
            break;
        default: 
            isStyleCorrect = false;
            button = new Button[1];
            button[0] = new Button(DEViseGlobals.OK_OP);
            break;
        }
        
        if (isStyleCorrect)  {
            if (msg == null)  {
                label = new Label[1];
                label[0] = new Label("No Message Given!");
            }  else if (msg.equals(""))  {
                label = new Label[1];
                label[0] = new Label("No Message Given!");
            }  else  {
                StringTokenizer msgStr = new StringTokenizer(msg, "\n");
                int msgNum = msgStr.countTokens();
                label = new Label[msgNum];
                try  {
                    String tmpMsg = null;
                    for (i = 0; i < msgNum; i++)  {
                        tmpMsg = msgStr.nextToken();
                        if (tmpMsg.equals(""))  {
                            label[i] = new Label(" ");
                        }  else  {
                            label[i] = new Label(tmpMsg);
                        }
                    }                    
                }  catch (NoSuchElementException e)  {
                    button = new Button[1];
                    button[0] = new Button(DEViseGlobals.OK_OP);                   
                    label = new Label[1];
                    label[0] = new Label("DEVise Error #0801 - DEVise Internal Error!");
                }                            
            }             
        }  else  {
            label = new Label[1];
            label[0] = new Label("DEVise Error #0802 - Unsupported Dialog Style!");
        }
        
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
                        
        // building the panel that display messages
        panel1.setLayout(new GridLayout(0, 1, 0, 0));        
        for (i = 0; i < label.length; i++)  
            panel1.add(label[i]);
        
        // building the panel that display buttons
        panel2.setLayout(new GridLayout(1, 0, 10, 0));
        for (i = 0; i < button.length; i++)  {
            button[i].setBackground(DEViseGlobals.buttonbgcolor);
            button[i].setForeground(DEViseGlobals.buttonfgcolor);
            button[i].setFont(DEViseGlobals.buttonfont);
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
        Point parentLoc = frame.getLocation();
        Dimension mysize = getSize();
        Dimension parentSize = frame.getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);                
        
        // event handler 
        for (i = 0; i < button.length; i++)  {
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

