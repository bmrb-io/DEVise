// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2002
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class holds view properties (such as location, cursors,
// pile information, and so on).  Methods of this class are mostly
// called by the DEViseCanvas class.

// There is one instance of this class for each view in the session.
// (This includes view symbols, piled views, etc.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.71  2002/05/01 21:28:59  wenger
// Merged V1_7b0_br thru V1_7b0_br_1 to trunk.
//
// Revision 1.70.2.3  2002/06/04 14:22:02  sjlong
// Fixed bug 783 (in aart1_EmEn.ds - mouse position does not work in main view)\nFixed bug 784 (in aart1_EmEn.ds - the axis labels are sometimes not correct)
//
// Revision 1.70.2.2  2002/05/17 17:13:10  wenger
// Did some cleanup of the JavaScreen axis labeling code (including fixing
// a bug that prevented the BMRB histogram sessions from working).
//
// Revision 1.70.2.1  2002/03/30 19:16:01  xuk
// Fix bugs for displaying axis labels for very small values.
//
// Revision 1.70  2002/03/27 22:11:52  xuk
// Fixed bug 760.
//
// Revision 1.69  2002/03/26 16:35:51  wenger
// Implemented kludge fix for bug 760; found bug 761.
//
// Revision 1.68  2002/02/20 18:08:35  xuk
// Improvement on axis label drawing.
//
// Revision 1.67  2002/02/19 22:13:32  xuk
// Improvement for label drawing.
//
// Revision 1.66  2002/02/18 22:08:00  xuk
// Modification for axis label drawing.
//
// Revision 1.65  2002/02/13 17:33:28  xuk
// Drawing axis labels according to the string length in pixels.
//
// Revision 1.64  2002/02/12 20:56:44  xuk
// Improvement for drawing axis labels.
// Added getDatelabel() for different date format.
//
// Revision 1.63  2002/02/06 23:08:54  xuk
// Draw the axis labels in the JavaScreen.
// Added new functions: getXlabel(), getYlabel(), roundUp();
// Changed updateDataRange().
//
// Revision 1.62  2001/12/13 21:35:33  wenger
// Added flexibility to enable/disable mouse location display individually
// for X and Y axes (needed for peptide-cgi session improvements requested
// by John Markley).
//
// Revision 1.61  2001/11/28 21:56:20  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.60.6.1  2001/11/21 23:35:36  wenger
// Fixed bug 729 (JavaScreen problem displaying mouse location for date
// axes).
//
// Revision 1.60  2001/09/26 15:11:59  wenger
// Fixed bugs 694 (JS rubberband line now reflects X-only zoom) and 695
// (JS now properly deals with disallowed cursor movement).
//
// Revision 1.59  2001/05/11 20:36:10  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.58  2001/05/03 19:34:38  xuk
// Added two factors for displaying mouse position.
//
// Revision 1.57  2001/05/03 16:26:42  xuk
// Added multiply factor for displaying mouse postion.
// Changed updateDateRange() function to set factor argument.
//
// Revision 1.56  2000/08/07 20:21:24  wenger
// Fixed problems with DEViseView.getFirstCursor() method; cleanup of
// DEViseCanvas CVS log, and other minor cleanup.
//
// Revision 1.55  2000/08/07 17:46:14  wenger
// Fixed bug 608 (NullPointerExceptions in family medicine sessions).
//
// Revision 1.54  2000/08/07 04:45:25  venkatan
// YError that occurs when mouse-clicked in misc/FamMed_visits.tk is fixed.
//
// Revision 1.53  2000/07/21 16:26:19  venkatan
// *** empty log message ***
//
// Revision 1.52  2000/07/21 16:17:44  venkatan
// Mouse Format display :  "-" is now recognised.
//
// Revision 1.51  2000/07/20 22:38:26  venkatan
// Mouse Location Format display:
// 1. Both X and Y axis formats are recognised.
// 2. "-" is recognised for "" String.
// 3. %.0f is now recognised.
//
// Revision 1.50  2000/07/20 16:53:54  wenger
// Fixed bug 602 (problem dragging cursors in non-base piled views).
//
// Revision 1.49  2000/07/20 16:26:07  venkatan
// Mouse Location Display format - is now controlled by printf type
// format strings specified by the VIEW_DATA_AREA command
//
// Revision 1.48  2000/06/27 16:44:45  wenger
// Considerably cleaned up and simplified the cursor-related code; moved
// cursor grid implementation from the DEViseCanvas class to the DEViseCursor
// class.
//
// Revision 1.47  2000/06/21 18:37:30  wenger
// Removed a bunch of unused code (previously just commented out).
//
// Revision 1.46  2000/06/12 22:11:31  wenger
// Commented out unused gridx and gridy.
//
// Revision 1.45  2000/05/04 15:53:32  wenger
// Added consistency checking, added comments, commented out unused code
// in DEViseScreen.java, DEViseCanvas.java, DEViseView.java,
// DEViseCmdDispatcher.java.
//
// Revision 1.44  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.43  2000/04/07 22:44:10  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.42  2000/04/05 15:42:24  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.41  2000/04/05 06:25:39  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.40  2000/03/31 19:29:17  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.39  2000/03/23 16:26:15  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.38  2000/02/14 10:45:23  hongyu
// *** empty log message ***
//
// Revision 1.37  1999/12/07 23:18:21  hongyu
// *** empty log message ***
//
// Revision 1.36  1999/10/10 08:49:53  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.35  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.34  1999/08/03 05:56:50  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.33  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.32  1999/06/23 20:59:17  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseView.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.util.*;
import  java.text.*;

// This class represent a image view in DEVise
public class DEViseView
{
    private static final int DEBUG = 0;

    public jsdevisec jsc = null;

    public DEViseCanvas canvas = null;

    public String viewName = null;
    public String curlyName = null;

    public Rectangle viewLoc = null;
    public Rectangle viewLocInCanvas = null;
    public float viewZ = 0.0f; // for piled views
    public int viewDimension = 0; // number of dimensions
    public int viewBg, viewFg;

    // viewDataLoc is the location relative to this view
    public Rectangle viewDataLoc = null;

    public float viewDataXMin = 0.0f, viewDataXMax = 0.0f;
    public float viewDataYMin = 0.0f, viewDataYMax = 0.0f;

    // data type could be "real" or "date" or "none"
    public String viewDataXType = null, viewDataYType = null;

    //ven
    public String viewInfoFormatX = null;
    public String viewInfoFormatY = null;

    // Show value meanings: 0 = none; 1 = xy; 2 = x; 3 = y;
    // values are this way for backwards compatibility.
    private int showMouseLoc = 1;
    private boolean showMouseLocX = true;
    private boolean showMouseLocY = true;

    public String viewTitle = null;
    public int viewDTX, viewDTY; // title X and Y location
    public Font viewDTFont = null;

    public DEViseView parentView = null;

    // Base view of a piled view.  Note that this is null in the base view
    // itself.
    public DEViseView pileBaseView = null;

    public Vector viewPiledViews = new Vector();
    public Vector viewChilds = new Vector();
    public Vector viewGDatas = new Vector();
    public Vector viewCursors = new Vector();

    public boolean isRubberBand, isCursorMove, isDrillDown, isKey;

    // data units per pixel in the X and Y dimensions
    public float dataXStep, dataYStep;

    // public float gridx, gridy; // mouse movement grid -- not used

    public boolean isFirstTime = true;

    private static final boolean _debug = false;
    
    // mouse position multiply factors
    float factorX = 1; 
    float factorY = 1;

    // label drawing info.
    int labelXDraw = 0;
    int labelYDraw = 0;
    int fontTypeX = 0;
    int fontTypeY = 0;
    int fontSizeX = 0;
    int fontSizeY = 0;
    int fontBoldX = 0;
    int fontBoldY = 0;
    int fontItalicX = 0;
    int fontItalicY = 0;

    public DEViseView(jsdevisec panel, String pn, String name,
      String piledname, String title, Rectangle loc, float Z, int dim,
      int bg, int fg, Rectangle dl, String xt, String yt, float gx, float gy,
      int rb, int cm, int dd, int ky)
    {
        jsc = panel;

        viewName = name;
        viewTitle = title;
        viewLoc = loc;
        viewZ = Z;
        viewDimension = dim;
        viewBg = bg;
        viewFg = fg;

        viewDataLoc = dl;
        //viewDataLoc.x += DEViseUIGlobals.screenEdge.width;
        //viewDataLoc.y += DEViseGlobals.
        viewDataXType = xt;
        viewDataYType = yt;

        parentView = jsc.jscreen.getView(pn);

	// Note that this will be null for the base view, because in this
	// constructor the DEViseScreen object doesn't know about this
	// view yet.
        pileBaseView = jsc.jscreen.getView(piledname);

        viewLocInCanvas = getLocInCanvas();

        // gridx = gx; // mouse movement grid -- not used
        // gridy = gy; // mouse movement grid -- not used

        isRubberBand = (rb == 1);

        isCursorMove = (cm == 1);

        isDrillDown = (dd == 1);

        isKey = (ky == 1);

        curlyName = "{" + viewName + "}";
    }

    protected void finalize() {
    	if (_debug) {
            System.out.println("DEViseView(" + viewName + ").finalize()");
        }
	removeAllGData();
    }

    // Returns view name enclosed by curly brackets.
    public String getCurlyName()
    {
        return curlyName;
    }

    public void setShowMouseLoc(int showMouseLocVal) {
        showMouseLoc = showMouseLocVal;
	showMouseLocX = ((showMouseLoc == 1) || (showMouseLoc == 2));
	showMouseLocY = ((showMouseLoc == 1) || (showMouseLoc == 3));
    }

    // Add a child view to this view.
    public void addChild(DEViseView view) throws YError
    {
        if (view != null) {
	    if (view.parentView != this) {
	        throw new YError("Adding child view (" + view.viewName +
		  ") to wrong parent(" + viewName + ")");
	    }

	    if (viewChilds.contains(view)) {
	        throw new YError("Child view " + view.viewName +
		  " is already in parent (" + viewName + ")");
	    } else {
                viewChilds.addElement(view);
	    }
        }
    }

    // Add a view to the base view of a pile.
    public void addPiledView(DEViseView view) throws YError
    {
	if (pileBaseView != null) {
	    throw new YError("Adding piled view (" + view.viewName +
	      ") to non-base view (" + viewName + ")");
	}

        if (view != null) {
	    if (view.pileBaseView != this) {
	        throw new YError("Adding piled view (" + view.viewName +
		  ") to wrong base view (" + viewName + ")");
	    }
	    if (viewPiledViews.contains(view)) {
	        throw new YError("Pile " + viewName +
		  "already contains view " + view.viewName);
	    }

            for (int i = 0; i < viewPiledViews.size(); i++) {
                DEViseView v = (DEViseView)viewPiledViews.elementAt(i);
                if (view.viewZ < v.viewZ) {
                    viewPiledViews.insertElementAt(view, i);
                    return;
                }
            }

            viewPiledViews.addElement(view);
        }
    }

    // Add a cursor to this view.  Returns true iff cursor exists and is
    // not already in this view.
    public boolean addCursor(DEViseCursor cursor) throws YError
    {
        if (cursor == null) {
            return false;
        }

	if (cursor.parentView != this) {
	    throw new YError("Adding cursor (" + cursor.name +
	      ") to wrong view (" + viewName + ")");
	}

        for (int i = 0; i < viewCursors.size(); i++) {
            DEViseCursor c = (DEViseCursor)viewCursors.elementAt(i);
            if (c.name.equals(cursor.name)) {
                if (c.isSame(cursor)) {
                    return false;
                } else {
                    viewCursors.removeElementAt(i);
                }
            }
        }

        cursor.image = null;

        viewCursors.addElement(cursor);

        return true;
    }

    // Add a GData record to this view.
    public void addGData(DEViseGData gdata)
    {
	// Note: I'm not checking GData.parentView here for speed.
	// RKW 2000-05-3.
        if (gdata != null) {
            viewGDatas.addElement(gdata);
        }
    }

    // Remove a child view from this view.
    public void removeChild(DEViseView view)
    {
        if (view != null) {
            if (canvas != null) {
                if (canvas.activeView == view) {
                    canvas.activeView = null;
                }
            }

            viewChilds.removeElement(view);
        }
    }

    // Removed a piled view from its base view.
    public void removePiledView(DEViseView view) throws YError
    {
	if (pileBaseView != null) {
	    throw new YError("Removing piled view from non-base view (" +
	      viewName + ")");
	}
        if (view != null) {
            viewPiledViews.removeElement(view);
        }
    }

    // Remove the given cursor from this view.
    public void removeCursor(DEViseCursor cursor)
    {
        if (cursor != null) {
            viewCursors.removeElement(cursor);
        }
    }

    // Remove the named cursor from this view; returns true iff the cursor
    // is in this view.
    public boolean removeCursor(String name)
    {
        if (name != null) {
            for (int i = 0; i < viewCursors.size(); i++) {
                DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(i);
                if (cursor.name.equals(name)) {
                    viewCursors.removeElement(cursor);
                    return true;
                }
            }

            return false;
        } else {
            return false;
        }
    }

    // Remove all GData records from this view.
    public void removeAllGData()
    {
        if (_debug) {
            System.out.println("DEViseView(" + viewName +
	      ").removeAllGData()");
        }
        viewGDatas = new Vector();
    }

    // Remove all cursors from this view.
    public void removeAllCursor()
    {
        viewCursors.removeAllElements();
    }

    // Remove all child views from this view.
    public void removeAllChild()
    {
        if (canvas != null) {
            if (canvas.activeView != this) {
                canvas.activeView = null;
            }
        }

        viewChilds.removeAllElements();
    }

    // Paint the axis labels for this view.
    //TEMP -- make sure this works right for piled views, especially
    // piled child views
    public void paintAxisLabels(Graphics gc, boolean isChildView)
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseView(" + viewName +
	      ").printAxisLabels(" + isChildView + ")");
	}

	Rectangle loc;

	Color labelColor = new Color(viewFg);
	if (DEBUG >= 2) {
	    System.out.println("  labelColor: " + labelColor);
	}
	gc.setColor(labelColor);

	//
	// Compensate for the location of the child view relative to the
	// parent, if this is a child view.
	//
	if (isChildView) {
            loc = new Rectangle(viewLoc.x + viewDataLoc.x,
                                viewLoc.y + viewDataLoc.y,
                                viewDataLoc.width,
                                viewDataLoc.height);
	} else {
	    loc = viewDataLoc;
	}

	if (labelYDraw == 1) {
	    int currentYPos = 0;
	    float currentY = 0;
	    String labelY = null;
	    float step = 0;
	    int width = 0;
	    float displayY = 0;

	    gc.setFont(DEViseFonts.getFont(fontSizeY, fontTypeY,
	      fontBoldY, fontItalicY));

	    if ((viewDataYType.toLowerCase()).equals("real")) {
		// round up the step
		step = 40 * dataYStep;
		step = roundUp(step);
		if (viewDataYMin >= 0) {
		    // round up the min
		    currentY = getMin(viewDataYMin, step);
		    currentYPos = loc.y + loc.height - 
			(int)((currentY - viewDataYMin) / dataYStep);

		    while (currentYPos >= loc.y + 10) {
			displayY = currentY * factorY;

			labelY = getYLabel(displayY);
			if (DEBUG >= 2) {
			    System.out.println("  Drawing Y label: " + labelY);
			}

			width = gc.getFontMetrics().stringWidth(labelY);
			gc.drawString(labelY, loc.x-10-width, currentYPos+5); 
			gc.drawLine(loc.x-5, currentYPos, loc.x-2, currentYPos);

   			currentY += step;
			currentYPos = loc.y + loc.height - 
			    (int)((currentY - viewDataYMin) / dataYStep);
		    }
		} else { // min < 0
		    // drawing upward
		    currentY = 0;
		    currentYPos = loc.y + loc.height - 
			(int)((0 - viewDataYMin) / dataYStep);

		    while (currentYPos >= loc.y + 10) {
			displayY = currentY * factorY;

			labelY = getYLabel(displayY);
			if (DEBUG >= 2) {
			    System.out.println("  Drawing Y label: " + labelY);
			}
			width = gc.getFontMetrics().stringWidth(labelY);
			gc.drawString(labelY, loc.x-10-width, currentYPos+5); 
			gc.drawLine(loc.x-5, currentYPos, loc.x-2, currentYPos);
   			currentY += step;

			currentYPos = loc.y + loc.height - 
			    (int)((currentY - viewDataYMin) / dataYStep);

		    }		
    
		    // drawing downward
		    currentY = 0 - step;
		    currentYPos = loc.y + loc.height - 
			(int)((currentY - viewDataYMin) / dataYStep);
		    	
		    while (currentYPos <= loc.y + loc.height) {
			displayY = currentY * factorY;

			labelY = getYLabel(displayY);
			if (DEBUG >= 2) {
			    System.out.println("  Drawing Y label: " + labelY);
			}
			width = gc.getFontMetrics().stringWidth(labelY);

			gc.drawString(labelY, loc.x-10-width, currentYPos+5); 
			gc.drawLine(loc.x-5, currentYPos, loc.x-2, currentYPos);
			//currentYPos += 40;	    
			currentY -= step;
			currentYPos = loc.y + loc.height - 
			    (int)((currentY - viewDataYMin) / dataYStep);
		    }		
		}
	    } else { // for "date"
		while (currentYPos >= loc.y + 20) {
		    currentY = viewDataYMin + (currentYPos - loc.x) *  dataYStep;
		    displayY = currentY * factorY;
		    labelY = getYLabel(displayY);
		    if (DEBUG >= 2) {
		        System.out.println("  Drawing Y label: " + labelY);
		    }
		    
		    gc.drawString(labelY, loc.x-40, currentYPos+5); 
		    gc.drawLine(loc.x-5, currentYPos, loc.x-2, currentYPos);
		    currentYPos -= 40;	    
		}
	    }    
	}   
	
	if (labelXDraw == 1) {
	    int currentXPos = loc.x;
	    float currentX = 0;
	    String labelX = null;
	    float step = 0;
	    int width = 0;
	    float displayX = 0;

	    gc.setFont(DEViseFonts.getFont(fontSizeX, fontTypeX,
	      fontBoldX, fontItalicX));
	    
	    if ((viewDataXType.toLowerCase()).equals("real")) {
		// round up the step
		step = 100 * dataXStep;
		step = roundUp(step);

		if (viewDataXMin >= 0) {
		    // round up the min
		    currentX = getMin(viewDataXMin, step);
		    currentXPos = loc.x + 
			(int)((currentX - viewDataXMin) / dataXStep);
		    
		    while (currentXPos <= loc.x + loc.width) {
			displayX = currentX * factorX;

			labelX = getXLabel(displayX);
		        if (DEBUG >= 2) {
		            System.out.println("  Drawing X label: " + labelX);
		        }
			width = gc.getFontMetrics().stringWidth(labelX);	
			gc.drawString(labelX, currentXPos-width/2+1, loc.y+loc.height+15); 
			gc.drawLine(currentXPos, loc.y+loc.height, 
				    currentXPos, loc.y+loc.height+3);

			currentX += step;
			currentXPos = loc.x + 
			    (int)((currentX - viewDataXMin) / dataXStep);
		    }
		} else { // min < 0
		    // drawing upward
		    currentX = 0;
		    currentXPos = loc.x + 
			(int)((0 - viewDataXMin) / dataXStep);
		    
		    while (currentXPos <= loc.x + loc.width) {
			displayX = currentX * factorX;

			labelX = getXLabel(displayX);
		        if (DEBUG >= 2) {
		            System.out.println("  Drawing X label: " + labelX);
		        }
			width = gc.getFontMetrics().stringWidth(labelX);	
			gc.drawString(labelX, currentXPos-width/2+1, loc.y+loc.height+15); 
			gc.drawLine(currentXPos, loc.y+loc.height, 
				    currentXPos, loc.y+loc.height+3);

			currentX += step;
			currentXPos = loc.x + 
			    (int)((currentX - viewDataXMin) / dataXStep);
		    }		
			
		    // drawing downward
		    currentX = 0 - step;
		    currentXPos = loc.x + 
			(int)((currentX - viewDataXMin) / dataXStep);
		    
		    while (currentXPos >= loc.x) {
			displayX = currentX * factorX;
			
			labelX = getXLabel(displayX);
		        if (DEBUG >= 2) {
		            System.out.println("  Drawing X label: " + labelX);
		        }
			width = gc.getFontMetrics().stringWidth(labelX);       
			gc.drawString(labelX, currentXPos-width/2+1, loc.y+loc.height+15); 
			gc.drawLine(currentXPos, loc.y+loc.height, 
				    currentXPos, loc.y+loc.height+3);
			currentX -= step;
			currentXPos = loc.x + 
			    (int)((currentX - viewDataXMin) / dataXStep);
		    }		
		}
	    } else { // for "date"
		fontSizeX = (fontSizeX <= 12) ? fontSizeX : 12; 

		displayX = viewDataXMin * factorX;
		labelX = getXLabel(displayX);
		if (DEBUG >= 2) {
		    System.out.println("  Drawing X label: " + labelX);
		}
		gc.drawString(labelX, loc.x, loc.y+loc.height+15); 
		gc.drawLine(loc.x, loc.y+loc.height, 
			    loc.x, loc.y+loc.height+3);
		
		displayX = viewDataXMax * factorX;
		labelX = getXLabel(displayX);
		if (DEBUG >= 2) {
		    System.out.println("  Drawing X label: " + labelX);
		}
		width = gc.getFontMetrics().stringWidth(labelX);
		gc.drawString(labelX, loc.x+loc.width-width, loc.y+loc.height+15); 
		gc.drawLine(loc.x+loc.width, loc.y+loc.height, 
			    loc.x+loc.width, loc.y+loc.height+3);
	    }    
	}   
    }

    // Get the canvas corresponding to this view (note that if this view
    // is a child view or a non-base piled view, the canvas returned is
    // not "owned" by this view).
    public DEViseCanvas getCanvas()
    {
        if (canvas == null) {
            if (parentView == null) {
                if (pileBaseView == null) { // this is a top view, but the canvas for it has not yet been set
                    return null;
                } else {
                    return pileBaseView.getCanvas();
                }
            } else {
                return parentView.getCanvas();
            }
        } else {
            return canvas;
        }
    }

    // Returns the location of this view relative to its canvas.
    // Relative to upper left corner of canvas?
    public Rectangle getLocInCanvas()
    {
        if (canvas == null) {
            Rectangle loc = null;

            if (parentView == null) {
                if (pileBaseView == null) {
                    // this is a top view, but the canvas for it has not yet been set
                    return new Rectangle(0, 0, viewLoc.width, viewLoc.height);
                } else {
                    loc = pileBaseView.getLocInCanvas();
                }
            } else {
                loc = parentView.getLocInCanvas();
                loc.x = loc.x + viewLoc.x;
                loc.y = loc.y + viewLoc.y;
                loc.width = viewLoc.width;
                loc.height = viewLoc.height;
            }

            return loc;
        } else {
            return new Rectangle(0, 0, viewLoc.width, viewLoc.height);
        }
    }

    // Returns the location of this view relative to the entire JavaScreen.
    // Relative to upper left corner of screen?
    public Rectangle getLocInScreen()
    {
        DEViseCanvas canvas = getCanvas();
        Rectangle loc = viewLocInCanvas;

        if (canvas == null) {
            DEViseView v = this;
            while (v.parentView != null) {
                v = v.parentView;
            }
            return new Rectangle(loc.x + v.viewLoc.x, loc.y + v.viewLoc.y,
	      viewLoc.width, viewLoc.height);
        } else {
            return new Rectangle(loc.x + canvas.view.viewLoc.x,
	      loc.y + canvas.view.viewLoc.y, viewLoc.width, viewLoc.height);
        }
    }

    // Update the data range of the given axis.
     void updateDataRange(String axis, float min, float max, 
			  String format, float factor, int label, 
			  int type, int size, int bold, int italic)
      throws YError
    {
       	if (axis.equals("X")) {
            viewDataXMin = min;
            viewDataXMax = max;
            viewInfoFormatX = format;
	    //  viewInfoFormatX = "-";

            dataXStep = 0.0f;
            if (viewDataLoc.width > 0)
                dataXStep = (viewDataXMax - viewDataXMin) / viewDataLoc.width;
	    factorX = factor;
	    labelXDraw = label;
	    fontTypeX = type;
	    fontSizeX = size;
	    fontBoldX = bold;
	    fontItalicX = italic;
	
	} else if (axis.equals("Y")) {
            viewDataYMin = min;
            viewDataYMax = max;
            viewInfoFormatY = format;
            //  viewInfoFormatY = "-";

            dataYStep = 0.0f;
            if (viewDataLoc.height > 0)
                dataYStep = (viewDataYMax - viewDataYMin) / viewDataLoc.height;
	    factorY = factor;
	    labelYDraw = label;
	    fontTypeY = type;
	    fontSizeY = size;
	    fontBoldY = bold;
	    fontItalicY = italic;
        } else {
	    throw new YError("Illegal axis type (" + axis + ")");
	}

    }

    // Note: getX and getY should probably be combined into a single
    // method. RKW 2000-05-03.

    // Convert mouse X coordinate (in pixels) to a String.
    // x is relative to this view's canvas

    public String getX(int x)
    {
	if(!showMouseLocX || viewDimension == 3 ||
	  (viewInfoFormatX != null && viewInfoFormatX.equals("-"))){
	   return "";
        }
        Rectangle loc = viewLocInCanvas;

        if (x < loc.x || x > (loc.x + loc.width)) {
            return "";
        }

        x -= loc.x;

        float xstep = dataXStep;
        // x0 represent the value at the left side of that pixel x
        float x0 = (x - viewDataLoc.x) * xstep + viewDataXMin;

	x0 *= factorX;

        if ((viewDataXType.toLowerCase()).equals("date")) {
            x0 *= 1000.0f; // convert to millisec????
            Date date = new Date((long)x0);
            DateFormat format = DateFormat.getDateInstance(DateFormat.SHORT);
            Calendar cal = Calendar.getInstance();
            cal.setTime(date);
            String time = " " + cal.get(Calendar.HOUR_OF_DAY) + ":"
                             + cal.get(Calendar.MINUTE) + ":"
                             + cal.get(Calendar.SECOND);
            return (format.format(date) + time);
        } else {
	    // for small values less than 1.0E-3
	    float abs = Math.abs(x0);
	    if (abs < 0.001) {
		String label = new Float(x0).toString();
		int dot = label.indexOf('.');
		int e = label.indexOf('E');
		if (e-dot > 4) 
		    label = label.substring(0, dot+3).concat(label.substring(e, label.length())); 
		return label;
	    }

	    // TEMP -- round to the nearest thousandth?
            if (x0 > 0) {
                x0 = (float) Math.floor(x0 * 1000.0f + 0.5f) / 1000.0f;
            } else {
                x0 = (float) Math.ceil(x0 * 1000.0f - 0.5f) / 1000.0f;
            }

            //  return ("" + x0);
	    // Ven - modified for mouse display format
	    return DEViseViewInfo.viewParser(x0, viewInfoFormatX);

        }
    }

    // Convert mouse Y coordinate (in pixels) to a String.
    // y is relative to this view's canvas
    public String getY(int y)
    {
	if(!showMouseLocY || viewDimension == 3 ||
	  (viewInfoFormatY != null && viewInfoFormatY.equals("-"))){
	   return "";
        }
        Rectangle loc = viewLocInCanvas;

        if (y < loc.y || y > (loc.y + loc.height)) {
            return "";
        }

        y -= loc.y;

        float ystep = dataYStep;
        // y0 represent the value at the top side of that pixel
        float y0 = viewDataYMax - (y - viewDataLoc.y) * ystep;

	y0 *= factorY;

        if ((viewDataYType.toLowerCase()).equals("date")) {
            y0 *= 1000.0f; // convert to millisec????
            Date date = new Date((long)y0);
            DateFormat format = DateFormat.getDateInstance(DateFormat.SHORT);
            Calendar cal = Calendar.getInstance();
            cal.setTime(date);
            String time = " " + cal.get(Calendar.HOUR_OF_DAY) + ":"
                             + cal.get(Calendar.MINUTE) + ":"
                             + cal.get(Calendar.SECOND);
            return (format.format(date) + time);
        } else {
	    // for small values less than 1.0E-3
	    float abs = Math.abs(y0);
	    if (abs < 0.001) {
		String label = new Float(y0).toString();
		int dot = label.indexOf('.');
		int e = label.indexOf('E');
		if (e-dot > 4) 
		    label = label.substring(0, dot+3).concat(label.substring(e, label.length())); 
		return label;
	    }

	    // TEMP -- round to the nearest thousandth?
            if (y0 > 0) {
                y0 = (long)(y0 * 1000.0f + 0.5f) / 1000.0f;
            } else {
                y0 = (long)(y0 * 1000.0f - 0.5f) / 1000.0f;
            }

            // return ("" + y0);
	    return DEViseViewInfo.viewParser(y0, viewInfoFormatY);
        }
    }

    public double pixel2DataX(int pixelX)
    {
	if (pileBaseView != null) {
	    return pileBaseView.pixel2DataX(pixelX);
	} else {
            return (pixelX - viewDataLoc.x) * dataXStep + viewDataXMin;
	}
    }

    public double pixel2DataY(int pixelY)
    {
	if (pileBaseView != null) {
	    return pileBaseView.pixel2DataY(pixelY);
	} else {
            return viewDataYMax - (pixelY - viewDataLoc.y) * dataYStep;
        }
    }

    public int data2PixelX(double dataX)
    {
	if (pileBaseView != null) {
	    return pileBaseView.data2PixelX(dataX);
	} else {
            return (int)Math.round((dataX - viewDataXMin) /
	      dataXStep + viewDataLoc.x);
        }
    }

    public int data2PixelY(double dataY)
    {
	if (pileBaseView != null) {
	    return pileBaseView.data2PixelY(dataY);
	} else {
            return (int)Math.round((viewDataYMax - dataY) /
	      dataYStep + viewDataLoc.y);
        }
    }

    // check whether or not the point (relative to this view's canvas) is within the view area
    public boolean inView(Point p)
    {
        Rectangle loc = viewLocInCanvas;

        //jsc.pn("view name" + viewName + " = p " + p.x + " " + p.y + "  loc " + loc.x + " " + loc.y + " " + loc.width + " " + loc.height + "  viewloc" + viewLoc.x + " " + viewLoc.y);
        if (p.x >= loc.x && p.y >= loc.y && p.x <= loc.x + loc.width && p.y <= loc.y + loc.height) {
            return true;
        } else {
            return false;
        }
    }

    // check whether or not the point (relative to this view's canvas) is
    // within the view's data area
    public boolean inViewDataArea(Point p)
    {
        Rectangle loc = viewLocInCanvas;

        int x0 = p.x - loc.x - viewDataLoc.x, y0 = p.y - loc.y - viewDataLoc.y;

        if (x0 >= 0 && y0 >= 0 && x0 <= viewDataLoc.width && y0 <= viewDataLoc.height) {
            return true;
        } else {
            return false;
        }
    }

    // ADD COMMENT -- what is this doing?
    public int translateX(int x, int mode)
    {
        return translateX(x, mode, 0);
    }

    // ADD COMMENT -- what is this doing?
    public int translateX(int x, int mode, int width)
    {
        Rectangle loc = viewLocInCanvas;

        if (mode == 1) { // adjust position x so it is within the range of data area of this view
            if (x < loc.x + viewDataLoc.x) {
                x = loc.x + viewDataLoc.x;
            } else if (x + width > loc.x + viewDataLoc.x + viewDataLoc.width) {
                x = loc.x + viewDataLoc.x + viewDataLoc.width - width;
            }
        } else if (mode == 2) { // adjust position x so it is relative to this view
            x = x - loc.x;
        }

        return x;
    }

    // ADD COMMENT -- what is this doing?
    public int translateY(int y, int mode)
    {
        return translateY(y, mode, 0);
    }

    // ADD COMMENT -- what is this doing?
    public int translateY(int y, int mode, int height)
    {
        Rectangle loc = viewLocInCanvas;

        if (mode == 1) { // adjust position y so it is within the range of data area of this view
            if (y < loc.y + viewDataLoc.y) {
                y = loc.y + viewDataLoc.y;
            } else if (y + height > loc.y + viewDataLoc.y + viewDataLoc.height) {
                y = loc.y + viewDataLoc.y + viewDataLoc.height - height;
            }
        } else if (mode == 2) { // adjust position y so it is relative to this view
            y = y - loc.y;
        }

        return y;
    }

    // Get this first cursor in this view or this view's pile.  Note that
    // this method should only be called on a base view.
    public DEViseCursor getFirstCursor() throws YError
    {
        if (_debug) {
	    System.out.println("DEViseView(" + viewName +
	      ").getFirstCursor()");
	}

        if (pileBaseView != null) {
            throw new YError("Calling getFirstCursor() on non-base view (" +
	      viewName + ")");
	}

	DEViseCursor cursor = doGetFirstCursor();

	if (_debug) {
	    String cursorName = (cursor != null) ? cursor.name : "null";
	    System.out.println("  getFirstCursor() returns: " + cursorName);
        }

	return cursor;
    }

    private DEViseCursor doGetFirstCursor()
    {
        if (_debug) {
	    System.out.println("DEViseView(" + viewName +
	      ").doGetFirstCursor()");

	}

	// Get any cursor in this view.
	if (viewCursors.size() > 0) {
            DEViseCursor cursor = (DEViseCursor)viewCursors.elementAt(0);
            return cursor;
        }

	// Return if this is a non-base view.
	// Note: we shouldn't really need to do this, because if this is a
	// non-base view viewPiledViews.size(); however, doing this seems to
	// avoid some lockups (maybe thread problems).  RKW 2000-08-07.
        if (pileBaseView != null) {
	    return null;
	}

	// Check cursors in child views of this view.
        for (int i = 0; i < viewPiledViews.size(); i++) {
            DEViseView v = (DEViseView)viewPiledViews.elementAt(i);
            DEViseCursor cursor = v.doGetFirstCursor();
            if (cursor != null) {
                return cursor;
            }
        }

        return null;
    }

    //TEMP -- why are x and y labels calculated differently? RKW 2002-03-26

    // get the label for Y axis
    public String getYLabel(float y)
    {
        boolean done = false;        

	if ((viewDataYType.toLowerCase()).equals("date")) {
	    return getDateLabel(y, viewInfoFormatY);
	} else {
	    int length = 0;
	    float abs = Math.abs(y);
	    String labelY = new Float(y).toString();

	    if (abs > 99999) { // |y| >= 1,000,000
		int y0 = (int)(y);
		labelY = DEViseViewInfo.viewParser(y0, viewInfoFormatY);

		int e = labelY.indexOf('E');
                // This 'while' statement will eliminate any unnecessary trailing 0s as
                // well as an unecessary decimal point
                done = false;
		while (labelY.charAt(e-1) == '0' 
		       || labelY.charAt(e-1) == '.') {
                        
                    // Once we get to the decimal point, we are done cleaning up the number
                    if(labelY.charAt(e-1) == '.') {
			done = true;
                    }

		    labelY = labelY.substring(0, e-1).concat(labelY.substring(e, labelY.length()));
		    e = e-1;

                    if(done) break;
		}

		length = labelY.length();
	    } else if (abs >= 10) { // |y| >= 10
		length = (int)(Math.log(abs) / Math.log(10) + 0.0001) + 1;
	    } else if (abs < 0.01) { // |y| < 0.01
		int e = labelY.indexOf('E');
		if (e != -1) 

                    // This 'while' statement will eliminate any unnecessary trailing 0s as
                    // well as an unecessary decimal point
		    
		    while (labelY.charAt(e-1) == '0' 
			   || labelY.charAt(e-1) == '.') {
                        
                        // Once we get to the decimal point, we are done cleaning up the number
                        if(labelY.charAt(e-1) == '.') {
			    done = true;
                        }
			labelY = labelY.substring(0, e-1).concat(labelY.substring(e, labelY.length()));
			e = e-1;
 
			if(done) break;
		    }
		length = labelY.length();
	    } else {
		length = Math.min(labelY.length(), 4);
	    }
		
	    if (y < 0) { // "-"
		    length ++;
	    }

	    // Kludge fix for bug 760.  RKW 2002-03-26.
	    length = Math.min(length, labelY.length());

	    labelY = labelY.substring(0, length);

	    while ( (labelY.charAt(length-1) == '.') || 
		(labelY.indexOf('.') != -1 && labelY.charAt(length-1) == '0') ){
		    labelY = labelY.substring(0, length-1);
		    length--;
	    }

	    return labelY;
	}
    }

    // get the label for X axis
    public String getXLabel(float x)
    {
        boolean done = false;

	if ((viewDataXType.toLowerCase()).equals("date")) {
	    return getDateLabel(x, viewInfoFormatX);
	} else {
	    int length = 0;
	    float abs = Math.abs(x);
	    String labelX = new Float(x).toString();

	    if (abs > 99999) {
		int x0 = (int)(x);
		labelX = DEViseViewInfo.viewParser(x0, viewInfoFormatX);
		int e = labelX.indexOf('E');


                // This 'while' statement will eliminate any unnecessary trailing 0s as
                // well as an unecessary decimal point
                done = false;
		while (labelX.charAt(e-1) == '0' 
		       || labelX.charAt(e-1) == '.') {
                        
                    // Once we get to the decimal point, we are done cleaning up the number
                    if(labelX.charAt(e-1) == '.') {
			done = true;
                    }

		    labelX = labelX.substring(0, e-1).concat(labelX.substring(e, labelX.length()));
		    e = e-1;
                  
                    if(done) break;
		}
		length = labelX.length();
	    } else if (abs >= 10) { // |x| >= 10
		length = (int)(Math.log(abs) / Math.log(10) + 0.0001) + 1;
	    } else if (abs < 0.01) { // |x| < 0.01
		int e = labelX.indexOf('E');
		if (e != -1) 

                    // This 'while' statement will eliminate any unnecessary trailing 0s as
                    // well as an unecessary decimal point
                    //done = false;
		    while (labelX.charAt(e-1) == '0' 
			   || labelX.charAt(e-1) == '.') {

                        // Once we get to the decimal point, we are done cleaning up the number
                        if(labelX.charAt(e-1) == '.') {
			    done = true;
                        }

			labelX = labelX.substring(0, e-1).concat(labelX.substring(e, labelX.length()));
			e = e-1;

                        if(done) break;
		    }
		length = labelX.length();
	    } else {
		length = Math.min(labelX.length(), 4);
	    }

	    if (x < 0) { // "-"
		length ++;
	    }

	    // Kludge fix for bug 760.  RKW 2002-03-26.
	    length = Math.min(length, labelX.length());

	    labelX = labelX.substring(0, length);

	    while ( (labelX.charAt(length-1) == '.') || 
		(labelX.indexOf('.') != -1 && labelX.charAt(length-1) == '0') ){
		    labelX = labelX.substring(0, length-1);
		    length--;
	    }
	    return labelX;
	}
    }

    // round up the steps on axises
    public float roundUp(float f)
    {
	float factor = 0;
	float log = 0;
	float temp = 0;

	if (f >= 1)
	    log = (int)(Math.log(f) / Math.log(10) + 0.0001);
	else 
	    log = (int)(Math.log(f) / Math.log(10) + 0.0001) - 1;
	factor = (float)Math.pow(10, log);
	temp = f / factor; 
	
	if (temp >= 7.5) 
	    f = (float)((10+0.0000001) * factor);
	else if (temp >= 3.5)
	    f = (float)((5+0.0000001) * factor);
	else if (temp >= 1.5)
	    f = (float)((2+0.0000001) * factor);
	else 
	    f = (float)((1+0.0000001) * factor);
	return f;
    }

    // get the proper min value on axises
    public float getMin(float min, float step)
    {
	int factor = 0;
	float f = 0;

	factor = (int)(min / step);
	f = factor * step;

	if (f < min)
	    f += step;
	return f;
    }

    // get date label according to cftime format
    public String getDateLabel(float f, String format)
    {
	f *= 1000.0f; 
	Date date = new Date((long)f);
	Calendar cal = Calendar.getInstance();
	cal.setTime(date);
	int i = 0;
	char c;
	String result = new String("");

	while (i < format.length()) {
	    String st = "";
	    c = format.charAt(i);

	    if (c == 'b') {
		int mon = cal.get(Calendar.MONTH);
		
		switch (mon+1) {
		case 1: st = "Jan"; break;
		case 2: st = "Feb"; break;
		case 3: st = "March"; break;
		case 4: st = "April"; break;
		case 5: st = "May"; break;
		case 6: st = "June"; break;
		case 7: st = "July"; break;
		case 8: st = "Aug"; break;
		case 9: st = "Sep"; break;
		case 10: st = "Oct"; break;
		case 11: st = "Nov"; break;
		case 12: st = "Dec"; break;
		}		
	    } 
	    else if (c == 'd') {
		st = new Integer(cal.get(Calendar.DAY_OF_MONTH)).toString();
	    }
	    else if (c == 'Y') {
		st = new Integer(cal.get(Calendar.YEAR)).toString();
	    }
	    else if (c == 'T') {
		st = cal.get(Calendar.HOUR_OF_DAY) + ":"
		    + cal.get(Calendar.MINUTE) + ":"
		    + cal.get(Calendar.SECOND);
	    }
	    else if (c == 'm') {
		st = new Integer(cal.get(Calendar.MONTH)+1).toString();
	    }
	    else if (c == 'H') {
		st = new Integer(cal.get(Calendar.HOUR_OF_DAY)).toString();
	    }
	    else if (c == 'M') {
		st = new Integer(cal.get(Calendar.MINUTE)).toString();
	    }
	    else if (c == ':') {
		st = ":";
	    }
	    else if (c == ' ') {
		st = " ";
	    }
	    else if (c == '-') {
		st = "-";
	    }

	    result = result.concat(st);
	    i++;
	}

	return result;
    }

}
