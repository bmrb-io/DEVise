// YGlobals.java
// last updated on 07/30/99


public final class YGlobals
{   
    // math constants
    public final static double e = Math.E;
    public final static double pi = Math.PI;    
    public final static double rad = 180.0 / Math.PI;    
    
    // control constants
    public static boolean isdebug = true;
    public static boolean isapplet = false;
    public static boolean inbrowser = false;

    public static void yassert(boolean condition, String msg, String where)
    {
        if (isdebug) {
            if (!condition) {
                throw new YError(msg, where);
            }
        }
    }

    public static void yassert(boolean condition, String msg)
    {
        if (isdebug) {
            if (!condition) {
                throw new YError(msg);
            }
        }
    }
}
           