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
import java.awt.event.*;
import java.net.*;
import java.util.*;

public class DEViseView
{
    jsdevisec jsc = null;
    DEViseScreen jscreen = null;
    DEViseWindow window = null;
    String viewName = null;
    Rectangle viewLoc = null;
    DEViseGData[] viewGData = null;
    DEViseCursor[] viewCursor = null;
    Button[] GDataButton = null;

    public DEViseView(jsdevisec what, String name, Rectangle loc)
    {
        jsc = what;
        jscreen = jsc.jscreen;
        viewName = name;
        viewLoc = loc;
    }

    public void setWindow(DEViseWindow win)
    {
        window = win;
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

    public void updateGData(Vector gdata)
    {
        if (GDataButton != null) {
            for (int i = 0; i < GDataButton.length; i++) {
                jscreen.removeGDataItem(GDataButton[i]);
            }

            GDataButton = null;
        }

        if (gdata == null || gdata.size() == 0) {
            viewGData = null;

            return;
        }

        viewGData = new DEViseGData[gdata.size()];
        for (int i = 0; i < viewGData.length; i++) {
            viewGData[i] = (DEViseGData)gdata.elementAt(i);
        }

        GDataButton = new Button[viewGData.length];
        //YGlobals.Ydebugpn(viewName + " GDATA length " + GDataButton.length);
        for (int i = 0; i < GDataButton.length; i++) {
            GDataButton[i] = new Button(viewGData[i].getLabel());
            GDataButton[i].setActionCommand(viewGData[i].getLabel());
            GDataButton[i].setFont(new Font("Monospaced", Font.PLAIN, 10));
            GDataButton[i].addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (YGlobals.YISAPPLET) {
                            if (DEViseGlobals.browser != null) {
                                try {
                                    URL url = new URL(event.getActionCommand());
                                    DEViseGlobals.browser.showDocument(url, "_blank");
                                } catch (MalformedURLException e) {
                                }
                            }
                        }
                    }
                });

            jscreen.addGDataItem(GDataButton[i]);
            GDataButton[i].setBounds(viewGData[i].getBounds(window));
            //YGlobals.Ydebugpn(viewName + " counts=" + jscreen.getComponentCount());
        }
    }

    public void updateGData()
    {
        if (GDataButton == null)
            return;

        for (int i = 0; i < GDataButton.length; i++) {
            GDataButton[i].setBounds(viewGData[i].getBounds(window));
        }
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
