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

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.18.4.2  2000/11/22 17:44:00  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.18.4.1  2000/11/21 01:51:34  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.18  2000/04/05 06:25:40  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.17  2000/03/23 16:26:17  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.16  1999/12/10 15:37:01  wenger
// Added standard headers to source files.
//
// ========================================================================


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

