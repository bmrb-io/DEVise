import  java.util.*;

public final class Globals
{
    // global constants
    public static final boolean ISDEBUG = true, ISLOG = false;
    public static final int IDSIZE = 6;
    public static final String ERRORID = "-1    ";
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2, API_CTL = 3, API_JAVA = 5;
        
    // global variables
    public static int CMDPORT = 6100, IMGPORT = 6400, DATAPORT = 6124;
    public static boolean ISAPPLET = false;
    
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
            //}  else if (testChar == ' ' || testChar == '\n' | testChar == '\t')  {
            } else if (testChar == ' ') {
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
}

