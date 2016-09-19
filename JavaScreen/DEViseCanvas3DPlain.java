// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// "Plain" (non-Jmol) 3D implementation-specific aspects of DEViseCanvas.

// This version of the 3D stuff is *not* molecular-biology specific --
// it just understands arbitrary sticks and balls.

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4.20.1  2011/04/20 17:15:07  wenger
// Changed the DEViseGenericTree.setSelection() method and the YLogGUI
// p() and pn() methods to only actually update the GUI in the event
// dispatched thread, to hopefully cure problems with incorrect 3D
// highlight updating in the s2pred visualization, and null pointer
// problems when showing the log window.  (I actually meant to do some
// earlier commits to this branch with more of the debug code in place,
// but I forgot to do that.)
//
// Revision 1.4  2007/08/03 20:17:27  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.3.2.1  2007/07/25 18:25:17  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.3  2006/12/08 16:24:36  wenger
// Merged V1_8b0_br_1 thru V1_8b0_br_2 to the trunk (took some manual
// changes to merge the DEViseCanvas.java stuff correctly).
//
// Revision 1.2  2006/05/26 16:22:15  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.2  2006/03/31 22:41:15  wenger
// Finished splitting up DEViseCanvas class.
//
// Revision 1.1.2.1  2006/03/30 20:51:26  wenger
// Partially done splitting up the DEViseCanvas class.
//

// ========================================================================

// DEViseCanvas3DPlain.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.awt.event.*;
import  java.util.*;

public class DEViseCanvas3DPlain extends DEViseCanvas3D
{
    //===================================================================
    // PUBLIC METHODS

    // v is base view if there is a pile in this canvas.
    public DEViseCanvas3DPlain (DEViseView v, Image img)
    {
	super(v, img);
    }

    public void create3DViewer()
    {
        if (view.viewGDatas.size() == 0) {
            return;
        }

        createCrystal();
    }

    public void clear3DViewer()
    {
        crystal = null;
    }

    // 3D drill-down
    public void drillDown3D(Point p)
    {
	jsc.pn("Mouse point: " + p.x + " " + p.y);
	Vector atomList = new Vector();
	Float f;
	String s = null;

	s = crystal.drillDown3D(p, atomList);
	if (atomList.size() > 0) {
	    jsc.pn(s);
	} else {
	    jsc.pn("No atom.");
        }

	if (atomList.size() > 0) {
	    String cmd = DEViseCommands.SHOW_RECORDS3D + " " + 
	    activeView.getCurlyName() + " " + atomList.size();
	    
	    for (int i=0; i<atomList.size(); i++) {
		DEViseAtomInCrystal atom = (DEViseAtomInCrystal) atomList.elementAt(i);
		for (int j=0; j<3; j++) {
		    f = new Float(atom.pos[j]);
		    cmd = cmd + " " + f.toString();
		}
	    }

	    jscreen.guiAction = true;
	    dispatcher.start(cmd);
	}
	
	isInViewDataArea = true;
	selectedCursor = null;
	whichCursorSide = DEViseCursor.sideNone;
	isMouseDragged = false;
	repaint();
    }

    //===================================================================
    // PROTECTED METHODS

    protected synchronized boolean paintCrystal(Graphics gc)
    {
        if (view.viewDimension != 3) {
            return false;
        }

	if (jsc.jsValues.debug._debugLevel >= 2) {
            jsc.pn("Crystals have been painted " +
	      jsc.jsValues.canvas.crystalPaintCount++ + " times");
        }

        paintBackground(gc);
	if (crystal != null) {
            crystal.paint(this, gc, isMouseDragged);
	}

        return true;
    }

    protected void resetCrystal()
    {
        if (crystal == null) return;

        crystal.totalShiftedX = 0;
        crystal.totalShiftedY = 0;
        crystal.totalX = 0.0f;
        crystal.totalY = 0.0f;
        crystal.totalZ = 0.0f;
        crystal.totalXRotation = 0.0f;
        crystal.totalYRotation = 0.0f;
        crystal.totalScaleFactor = 1.0f;
        crystal.resetAll(true);
        repaint();

        // send command to collaborations if necessary (only if we're
        // a leader!)
        if (jsc.specialID == -1 && jsc.isCollab) {
            String cmd = DEViseCommands.SET_3D_CONFIG + " "
	      + activeView.getCurlyName();
		
	    float[][] data = crystal.lcs.getData();
	    float[] origin = crystal.lcs.getOrigin();
	    Float f = null;
			
	    for (int i=0; i<3; i++) 
	        for (int j=0; j<3; j++) {
		    f = new Float(data[i][j]);
		    cmd = cmd + " {" + f.toString() + "}";
	        }
		
	    for (int i=0; i<3; i++) {
	        f = new Float(origin[i]);
	        cmd = cmd + " {" + f.toString() + "}";
	    }

	    cmd = cmd + " {" + crystal.shiftedX + "}" +
	      " {" + crystal.shiftedY + "}";
			

	    if (jsc.dispatcher.getStatus() ==
	      DEViseCmdDispatcher.STATUS_IDLE)
	        dispatcher.start(cmd);	
        }	
    }

    protected void doMouseReleased(MouseEvent event)
    {
	// Return here fixes empty data problems related to bug
	// 929.
	if (crystal == null) return;

	// send command to collaborations if necessary (only if
	// we're a leader!)
	//TEMP -- if this is the bug, it should get fixed on the 1.8 branch!!
	//TEMP if (jsc.specialID == -1 && jsc.isCollab) {
	//TEMP }
	if (jsc.specialID == -1) { //TEMP
	    // for 3D drill-down
	    if ((jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) 
		&& (activeView.isDrillDown) && (!isMouseDragged) 
		&& (isInViewDataArea)) {
		Point p = event.getPoint();			
		drillDown3D(p);
		return;
	    }

	    String cmd = DEViseCommands.SET_3D_CONFIG + " "
		+ activeView.getCurlyName();
	    
	    float[][] data = crystal.lcs.getData();
	    float[] origin = crystal.lcs.getOrigin();
	    Float f = null;
	    
	    for (int i=0; i<3; i++) 
		for (int j=0; j<3; j++) {
		    f = new Float(data[i][j]);
		    cmd = cmd + " {" + f.toString() + "}";
		}

	    for (int i=0; i<3; i++) {
		f = new Float(origin[i]);
		cmd = cmd + " {" + f.toString() + "}";
	    }

	    cmd = cmd + " {" + crystal.shiftedX + "}" +
		" {" + crystal.shiftedY + "}";

	    dispatcher.start(cmd);	
	}

        isMouseDragged = false;
        repaint();		

        return;
    }

    protected void doMouseDragged(Point p)
    {
        if (crystal != null) {
            int dx = p.x - op.x, dy = p.y - op.y;
            op.x = p.x;
            op.y = p.y;

            jsc.viewInfo.updateInfo(activeView.getX(p.x),
	      activeView.getY(p.y));

            if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_ALT || 
              jsc.jsValues.canvas.lastKey == KeyEvent.VK_SHIFT) {
                crystal.translate(dx, dy);
            } else if (jsc.jsValues.canvas.lastKey == KeyEvent.VK_CONTROL) {
                crystal.scale(dx, dy);
            } else {
                crystal.rotate(dx, dy);
            }

            jsc.jsValues.canvas.isInteractive = true;
            jsc.jsValues.canvas.sourceCanvas = this;
            repaint();
        }
    }

    protected void doMouseMoved(Point p)
    {
        if (crystal != null) {
            point.x = p.x;
            point.y = p.y;

            //  setCursor(jsc.mouseCursor.rbCursor); - Ven modified
            setCursor(jsc.mouseCursor.hdCursor);

            activeView = view;
            jsc.viewInfo.updateInfo(activeView.viewName,
	      activeView.getX(p.x), activeView.getY(p.y));
            if (jscreen.getCurrentView() != activeView) {
                jscreen.setCurrentView(activeView);
            }
        }
    }

    //===================================================================
    // PRIVATE METHODS

    // Create "crystal" (used for drawing in 3D views/piles).  Note that
    // in a 3D pile there is only one crystal for the entire pile.
    private void createCrystal()
    {
        if (crystal == null) {
            int size = view.viewGDatas.size();
            //float[][] atomPos = new float[size][3];
            //float[][][] bondPos = new float[size][
            //String[] atomName = new String[size];
            Vector aPos = new Vector(size), aName = new Vector(size),
			  aColor = new Vector(size), bPos = new Vector(size),
			  bColor = new Vector(size);
            for (int i = 0; i < size; i++) {
                DEViseGData gdata = (DEViseGData)view.viewGDatas.elementAt(i);
		if (gdata.symbolType == gdata._symSegment) {
                    float[][] pos = new float[2][3];
                    pos[0][0] = gdata.x0;
                    pos[0][1] = gdata.y0;
                    pos[0][2] = gdata.z0;
                    pos[1][0] = gdata.x1;
                    pos[1][1] = gdata.y1;
                    pos[1][2] = gdata.z1;
                    bPos.addElement(pos);
                    bColor.addElement(gdata.color);
                } else {
                    aName.addElement(gdata.string);
                    float[] pos = new float[3];
                    pos[0] = gdata.x0;
                    pos[1] = gdata.y0;
                    pos[2] = gdata.z0;
                    aPos.addElement(pos);
                    aColor.addElement(gdata.color);
                }
            }

            String[] atomName = null;
            float[][] atomPos = null;
            float[][][] bondPos = null;
            Color[] bondColor = null;
            Color[] atomColor = null;
            if (aName.size() > 0) {
                atomName = new String[aName.size()];
                for (int i = 0; i < atomName.length; i++) {
                    atomName[i] = (String)aName.elementAt(i);
                }
            }

            if (aPos.size() > 0) {
                atomPos = new float[aPos.size()][3];
                for (int i = 0; i < atomPos.length; i++) {
                    atomPos[i] = (float[])aPos.elementAt(i);
                }
            }

            if (aColor.size() > 0) {
                atomColor = new Color[aColor.size()];
                for (int i = 0; i < atomColor.length; i++) {
                    atomColor[i] = (Color)aColor.elementAt(i);
                }
            }

            if (bPos.size() > 0) {
                bondPos = new float[bPos.size()][2][3];
                for (int i = 0; i < bondPos.length; i++) {
                    bondPos[i] = (float[][])bPos.elementAt(i);
                }
            }

            if (bColor.size() > 0) {
                bondColor = new Color[bColor.size()];
                for (int i = 0; i < bondColor.length; i++) {
                    bondColor[i] = (Color)bColor.elementAt(i);
                }
            }

            try {
                DEVise3DLCS lcs = new DEVise3DLCS();
                crystal = new DEViseCrystal(canvasDim.width - 10, canvasDim.height - 10, 5, lcs, -1, -1, atomName, atomPos, atomColor, bondPos, bondColor);
            } catch (YException e) {
                jsc.pn(e.getMessage());
                crystal = null;
                return;
            }
	}

        if (view.viewPiledViews.size() > 0) {
            crystal.setSelect();

            for (int i = 0; i < view.viewPiledViews.size(); i++) {
	        DEViseView v = (DEViseView)view.viewPiledViews.elementAt(i);
		for (int j = 0; j < v.viewGDatas.size(); j++) {
		    DEViseGData gdata = (DEViseGData)v.viewGDatas.elementAt(j);
		    if (gdata.symbolType == gdata._symSegment) {
		        crystal.setSelect(gdata.x0, gdata.y0, gdata.z0,
			  gdata.color, true);
                        crystal.setSelect(gdata.x1, gdata.y1, gdata.z1,
			  gdata.color, true);
                    } else {
                        crystal.setSelect(gdata.x0, gdata.y0, gdata.z0,
			  gdata.color, false);
                    }
                }
            }
        }
    }
}
