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
// Revision 1.2  1998/06/11 15:07:47  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.util.*;

public class DEViseView
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    String viewName = null;
    Rectangle viewLoc = null;
    Vector allCursors = null;
    Vector GData = null;
    
    public DEViseView(jsdevisec what, String name, Rectangle loc, Vector cursorloc, Vector gdata)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        viewName = name;
        viewLoc = loc;
        
        if (cursorloc == null)    
            allCursors = new Vector();
        else
            allCursors = cursorloc;
            
        if (GData == null)
            GData = new Vector();
        else
            GData = gdata;
    }
    
    public String getName()
    {
        return viewName;
    }
    
    public Rectangle getLoc()
    {
        return viewLoc;
    }
    
    public Vector getAllCursors()
    {
        return allCursors;
    }
    
    public Vector getGData()
    {
        return GData;
    }
    
    public boolean isCurrent(Point p)
    {
        if (p.x >= viewLoc.x && p.x < viewLoc.x + viewLoc.width && p.y >= viewLoc.y && p.y < viewLoc.y + viewLoc.height)
            return true;
        else
            return false;
    }
    
    public Rectangle getCursorLoc(Point p)
    {   
        int number = allCursors.size();
        Rectangle rect = null, loc = null;
        
        for (int i = 0; i < number; i++) {
            loc = (Rectangle)allCursors.elementAt(i);
            if (p.x >= loc.x && p.x < loc.x + loc.width && p.y >= loc.y && p.y < loc.y + loc.height) {
                rect = loc;
                break;
            }
        }
        
        return rect;
    }
} 
