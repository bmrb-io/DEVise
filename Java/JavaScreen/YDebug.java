// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1992-1998
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================
//
// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.
//

//
// Description of module.
//

//
// $Id$
//
// $Log$
//
// ------------------------------------------------------------------------

import java.io.*;

public final class YDebug
{
    private static boolean isDebug = true;
    private static boolean isLog = false;
    
    public YDebug(boolean arg1, boolean arg2)
    {   
        isDebug = arg1;
        isLog = arg2;
    }        
   
    public static void println(String msg, boolean islog)
    {   
        if (isDebug) 
            System.out.println(msg);        
        
        if (islog) 
            YLogFile.println(msg);
    }
    
    public static void println(String msg)
    {   
        if (isDebug) 
            System.out.println(msg);
        
        if (isLog)
            YLogFile.println(msg);
    }

    public static void print(String msg, boolean islog)
    {
        if (isDebug)
            System.out.print(msg);
        
        if (islog) 
            YLogFile.print(msg);
    }
        
    public static void print(String msg)
    {   
        if (isDebug) 
            System.out.print(msg);
        
        if (isLog)
            YLogFile.print(msg);
    }
}
