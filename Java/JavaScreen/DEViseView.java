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
// Revision 1.6  1998/08/14 17:48:07  hongyu
// *** empty log message ***
//
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
    DEViseGData[] viewGData = null;
    DEViseCursor[] viewCursor = null;

    public DEViseView(jsdevisec what, String name, Rectangle loc)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        viewName = name;
        viewLoc = loc;
    }

    public String getName()
    {
        return viewName;
    }

    public Rectangle getLoc()
    {
        return viewLoc;
    }

    public boolean isCurrent(Point p)
    {
        if (p.x >= viewLoc.x && p.x < viewLoc.x + viewLoc.width && p.y >= viewLoc.y && p.y < viewLoc.y + viewLoc.height)
            return true;
        else
            return false;
    }

    public void setGData(Vector gdata)
    {
        if (gdata == null || gdata.size() == 0) {
            viewGData = null;
            return;
        }

        viewGData = new DEViseGData[gdata.size()];
        for (int i = 0; i < viewGData.length; i++)
            viewGData[i] = (DEViseGData)gdata.elementAt(i);            
    }

    public DEViseGData[] getGData()
    {
        return viewGData;
    }

    public void setCursor(DEViseCursor cursor)
    {
        if (cursor == null) {
            viewCursor = null;
            return;
        }

        viewCursor = new DEViseCursor[1];
        viewCursor[0] = cursor;
    }

    public DEViseCursor[] getCursor()
    {
        return viewCursor;
    }
}
