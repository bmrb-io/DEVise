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

import  java.awt.*;
import  java.io.*;

public final class YDebugInfo extends Frame
{
    private static TextArea textArea = null;
    private static boolean isLog = false;
    private static boolean isDebug = true;
    
    public YDebugInfo(boolean arg1, boolean arg2)
    {   
        isDebug = arg1;
        isLog = arg2;
        
        if (isDebug) {            
            textArea = new TextArea(10, 50);            
            textArea.setFont(new Font("Monospaced", Font.PLAIN, 14));
            textArea.setEditable(false);
            add("Center", textArea);

            setTitle("Debug Information");
            pack();
            show();
        }        
    }        
   
    public static void println(String msg, boolean islog)
    {   
        if (isDebug)
            textArea.append(msg + "\n");
        
        if (islog) 
            YLogFile.println(msg);
    }

    public static void println(String msg)
    {   
        if (isDebug)
            textArea.append(msg + "\n");
        
        if (isLog) 
            YLogFile.println(msg);
    }
    
    public static void print(String msg, boolean islog)
    {
        if (isDebug)
            textArea.append(msg);
        
        if (islog)
            YLogFile.print(msg);
    }
        
    public static void print(String msg)
    {
        if (isDebug)
            textArea.append(msg);
        
        if (isLog)
            YLogFile.print(msg);
    }
    
    protected void finalize()
    {
        dispose();
    }
}
