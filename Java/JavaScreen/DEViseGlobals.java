import java.util.*;
import java.awt.*;

public final class DEViseGlobals
{
    // global constants
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2, API_CTL = 3, API_JAVA = 5;
    public static final int DEFAULTCMDPORT = 6666, ERRORID = -1;
    public static final String DEFAULTUSER = new String("nobody");
    public static final String DEFAULTPASS = new String("none");
    public static final String DEFAULTHOST = new String("localhost");
    public static final int DEFAULTCMDSOCKETTIMEOUT = 300000; // 5 minutes
    public static final int DEFAULTIMGSOCKETTIMEOUT = 900000; // 15 minutes    

    // global variables
    public static Dimension SCREENSIZE = new Dimension(640, 480);
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
    public static String[] parseString(String inputStr, boolean keep)
    {
        return YGlobals.parseString(inputStr, '{', '}', keep);
    }
    
    public static String[] parseString(String inputStr)
    {
        return YGlobals.parseString(inputStr, '{', '}', false);
    }    
    
    public static String[] parseStr(String inputStr)
    {
        return YGlobals.parseStr(inputStr, "\n");
    }
}

