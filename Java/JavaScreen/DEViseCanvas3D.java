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

// ========================================================================

// DEViseCanvas3D.java

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
