// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

//
// ========================================================================

package JavaScreen;

public final class YGlobals
{
    // math constants
    public final static float e = (float)Math.E;
    public final static float pi = (float)Math.PI;
    public final static float rad = (float)(180.0 / Math.PI);

    // control constants
    private static final boolean isdebug = true;

    public static void ythrow(boolean condition, String msg, String where, int id) throws YException
    {
        if (isdebug) {
            if (!condition) {
                throw new YException(msg, where, id);
            }
        }
    }

    public static void ythrow(boolean condition, String msg, String where) throws YException
    {
        if (isdebug) {
            if (!condition) {
                throw new YException(msg, where);
            }
        }
    }

    public static void ythrow(boolean condition, String msg) throws YException
    {
        ythrow(condition, msg, null);
    }

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
        yassert(condition, msg, null);
    }

    // Calendar.getInstance() will return a calendar object in default timezone and default
    // locale which is initialized use current time. Or you can use Calendar.getInstance(timezone)
    // Calendar.getInstance(locale) and Calendar.getInstance(timezone, locale) to use other
    // timezone and locale. And all information about this time can be retrieve by Calendar.get(int)
    // method use specific code, such as Calendar.DAY_OF_MONTH.
    // Other useful class or method related to time manipulation is: System.currentTimeMillis(),
    // java.util.Date, java.util.TimeZone, java.util.Locale and java.text.DateFormat

}

