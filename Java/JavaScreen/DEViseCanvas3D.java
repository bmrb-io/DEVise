// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// 3D specific aspects of a DEViseCanvas that are not tied to either
// the Jmol or "plain" 3D implementations.

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.2.2  2007/08/03 19:21:15  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.2.2.1  2007/07/25 18:25:17  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.2  2006/05/26 16:22:15  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.2  2006/03/31 22:41:14  wenger
// Finished splitting up DEViseCanvas class.
//
// Revision 1.1.2.1  2006/03/30 20:51:25  wenger
// Partially done splitting up the DEViseCanvas class.
//

// ========================================================================

// DEViseCanvas3D.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.awt.event.*;

public class DEViseCanvas3D extends DEViseCanvas
{
    // v is base view if there is a pile in this canvas.
    public DEViseCanvas3D (DEViseView v, Image img)
    {
	super(v, img);
    }

    protected void doMousePressed()
    {
        // setCursor(jsc.mouseCursor.rbCursor);  - Ven modified
	// Not sure we need this -- we already have the hand cursor at
	// this point anyhow.  wenger 2007-07-25
        jsc.mouseCursor.setTemporaryCursor(jsc.mouseCursor.hdCursor, this);

        activeView = view;
        jsc.viewInfo.updateInfo(activeView.viewName,
          activeView.getX(sp.x), activeView.getY(sp.y));
        if (jscreen.getCurrentView() != activeView) {
            jscreen.setCurrentView(activeView);
        }
    }
}
