// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2012
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Need to write description.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6.16.2  2012/04/30 19:07:56  wenger
// Fixed toolbar icons for data download; added mouse cursor icon for
// data download, and set the mouse cursor when we go into data download
// mode; added data download info to the JavaScreen help page; updated
// JS & DEVise version histories; put view names back into the download
// files because I think piles are too confusing otherwise.
//
// Revision 1.6.16.1  2012/04/12 23:27:29  wenger
// Initial implementation of command framework for JavaScreen data saving
// (the command doesn't really do anything on the devised end yet).  Note
// that the toolbar icons for data saving are just a copy of the icons
// for drill down right now -- I want to get things implemented on the
// devised end before I put time into farting around making new icons.
//
// Revision 1.6  2009/09/09 21:50:24  wenger
// Fixed JavaScreen bugs 960 and 983 (problems with the mouse cursor
// not changing correctly according to the toolbar mode and the state
// of the active view).
//
// Revision 1.5  2008/02/13 22:38:25  wenger
// Added a button to show the Jmol selection trees without having to
// go into a menu.
//
// Revision 1.4  2007/08/27 19:16:47  wenger
// Merged andyd_gui_br_7 thru andyd_gui_br_8 to trunk.
//
// Revision 1.3  2007/08/03 20:17:35  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.2  2007/06/27 17:48:11  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.1.2.20  2007/08/27 18:35:40  wenger
// Implemented "click to zoom" feature in toolbar zoom modes (zooms in or
// out 50% and re-centers).
//
// Revision 1.1.2.19  2007/08/24 16:51:52  wenger
// Removed elipses from "View Help" menu item; renamed "Toggle visual
// filter" tooltip to "Toggle axis ranges" (hopefully clearer).
//
// Revision 1.1.2.18  2007/08/03 19:21:24  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.1.2.17  2007/07/27 18:25:35  wenger
// "Toggle text display" and "move cursor" toolbar icons are temporarily
// hidden (until they're functional).
//
// Revision 1.1.2.16  2007/07/26 19:18:21  adayton
// Added toolbar buttons for move cursor and toggle text display
//
// Revision 1.1.2.15  2007/07/25 18:25:26  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.1.2.14  2007/07/24 21:50:38  wenger
// We now create (but don't use yet) mouse cursors corresponding to the
// various toolbar states.
//
// Revision 1.1.2.13  2007/07/20 21:21:52  wenger
// Changed the "Fill selected view with cursor" tooltip to "Expand DEVise
// cursor to fill view".
//
// Revision 1.1.2.12  2007/07/20 20:56:01  wenger
// 3D toolbar buttons are temporarily not shown because they are not
// yet functional.
//
// Revision 1.1.2.11  2007/06/22 21:49:44  wenger
// Added disabled images for toolbar buttons; 3D buttons now stay
// disabled even when we have a 3D view because they don't actually
// work yet.
//
// Revision 1.1.2.10  2007/06/21 14:41:29  wenger
// Tried changing the DEViseScreen to extend a JPanel instead of a Panel
// (this fixed the problem with toolbar tooltips being obscured, but
// caused the JS to lock up really a lot, so I have temporarily changed
// it back); changed some of the tooltip text; put code into
// DEViseToolBar.java to make sure graphics code is getting called from
// the AWT Event thread.
//
// Revision 1.1.2.9  2007/06/20 15:16:32  wenger
// Changed "Jmol" buttons to "3D", since they should also apply to the
// non-Jmol 3D views; 3D toolbar buttons are now disabled if there's no
// Jmol view (they should also be enable for a non-Jmol 3D view).
//
// Revision 1.1.2.8  2007/06/19 20:49:54  wenger
// Toolbar now works for the various zoom modes and for enlarging/reducing
// symbol size; removed buttons for Y-only zoom modes (not supported right
// now).
//
// Revision 1.1.2.7  2007/06/19 19:32:40  wenger
// Toolbar now works for help, home, cursor fill, and toggling visual
// filters; increased the spacing between the "sections" of icons.
//
// Revision 1.1.2.6  2007/06/18 19:57:32  wenger
// Toolbar works for drill-down (including Jmol); we go back to "normal"
// mode after drilling down; drill-down in Jmol is now disabled by
// default; removed Jmol menu options to enable/disable drill-down;
// removed unneeded utils stuff from 'make clean' target.
//
// Revision 1.1.2.5  2007/06/15 20:46:17  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.1.2.4  2007/06/15 14:27:11  adayton
// Updated some of the button behaviours and made some improvements to the icons
//
// Revision 1.1.2.3  2007/06/06 14:13:14  wenger
// Got JavaScreen Makefile to work with Andy's latest changes; added
// headers to the UI java files.
//

// ------------------------------------------------------------------------

package JavaScreen.UI;

import javax.swing.*;
import JavaScreen.*;
import java.awt.Dimension;
import java.awt.event.*;

public final class DEViseToolBar extends JToolBar {
	
	static final int MIN_WIDTH = 24;
	static final int MIN_HEIGHT = 24;
	static final int SEPARATOR_HEIGHT = 15;
	private DEViseToolBarButton selectedButton;
	private DEViseCanvas3DJmol jmolCanvas = null;
	
	// default button sets
	// these should probably be defined somewhere else, but let's
	// leave them here for the time being
	
	private DEViseToolBarButton drillDownButton = null;
	private DEViseToolBarButton helpButton = null;
	private DEViseToolBarButton dataDownloadButton = null;
	private DEViseToolBarButton toggleTextDisplayButton = null;
	private DEViseToolBarButton[] infoButtonSet = null;

	private DEViseToolBarButton normalButton = null;
	private DEViseToolBarButton moveCursorButton = null;
	private DEViseToolBarButton cursorFillButton = null;
	private DEViseToolBarButton increaseSymSizeButton = null;
	private DEViseToolBarButton decreaseSymSizeButton = null;
	private DEViseToolBarButton[] viewButtonSet = null;

	private DEViseToolBarButton threeDRotateButton = null;
	private DEViseToolBarButton threeDTranslateButton =  null;
	private DEViseToolBarButton threeDZoomButton =  null;
	private DEViseToolBarButton[] threeDButtonSet = null;

	private DEViseToolBarButton homeButton = null;
	private DEViseToolBarButton toggleFilterButton = null;
	private DEViseToolBarButton[] filterToolsButtonSet = null;

	private DEViseToolBarButton zoomInXYButton = null;
	private DEViseToolBarButton zoomInXButton = null;
	private DEViseToolBarButton zoomInYButton = null;
	private DEViseToolBarButton zoomOutXYButton = null;
	private DEViseToolBarButton zoomOutXButton = null;
	private DEViseToolBarButton zoomOutYButton = null;
	private DEViseToolBarButton[] zoomButtonSet = null;
	
	private boolean showInfoButtonSet = true;
	private boolean showViewButtonSet = true;
	private boolean showThreeDButtonSet = false;
	private boolean showFilterToolsButtonSet = true;
	private boolean showZoomButtonSet = true;

	private DEViseMouseCursor mouseCursor;
	
    public DEViseToolBar(DEViseJSValues jsValues, DEViseMouseCursor mc) {

	    mouseCursor = mc;

			drillDownButton = new DEViseToolBarButton(
			  "Drill down (show data records)", "drill-down", jsValues);
			helpButton = new DEViseToolBarButton("Show help for selected view",
			  "show_view_help", jsValues);
			dataDownloadButton = new DEViseToolBarButton(
			  "Download data for selected view", "data_download", jsValues);
			toggleTextDisplayButton = new DEViseToolBarButton(
				"Toggle text display for selected view",
				"toggle_text_display", jsValues);
	    infoButtonSet = new DEViseToolBarButton[] {
		    drillDownButton,
				helpButton,
				dataDownloadButton,
				// Temporarily hidden until it's functional.
				//toggleTextDisplayButton,
	    };

		normalButton = new DEViseToolBarButton(
		  "Select cursor/zoom in XY/3D rotate", "crosshairs", jsValues);
		moveCursorButton = new DEViseToolBarButton(
			"Move cursor without zooming", "move_cursor", jsValues);
		cursorFillButton = new DEViseToolBarButton(
		  "Expand DEVise cursor to fill view", "full_screen_filter",
		  jsValues);
		increaseSymSizeButton = new DEViseToolBarButton("Increase symbol size",
		  "increase_symbol_size", jsValues);
		decreaseSymSizeButton = new DEViseToolBarButton("Decrease symbol size",
		  "decrease_symbol_size", jsValues);
	    viewButtonSet = new DEViseToolBarButton[] {
			normalButton,
			// Temporarily hidden until it's functional.
			// moveCursorButton,
			cursorFillButton,
			increaseSymSizeButton,
			decreaseSymSizeButton,
	    };

		//TEMPTOOLBAR -- get these to work
		//TEMPTOOLBAR -- enable these for non-Jmol 3D
		threeDRotateButton = new DEViseToolBarButton("3D Rotate",
		  "jmol_rotate", jsValues);
		threeDTranslateButton = new DEViseToolBarButton("3D Translate",
		  "jmol_translate", jsValues);
		threeDZoomButton = new DEViseToolBarButton("3D Zoom", "jmol_zoom",
		  jsValues);
	    threeDButtonSet = new DEViseToolBarButton[] {
			threeDRotateButton,
			threeDTranslateButton,
			threeDZoomButton
	    };
		setThreeDEnabled(false);

		homeButton = new DEViseToolBarButton(
		  "Home (show all data)", "home_filter", jsValues);
		toggleFilterButton = new DEViseToolBarButton("Toggle axis ranges",
		  "toggle_filter", jsValues);
	    filterToolsButtonSet = new DEViseToolBarButton[] {
			homeButton,
			toggleFilterButton
	    };

		zoomInXYButton = new DEViseToolBarButton("Zoom in XY",
		  "zoom_in-all", jsValues);
		zoomInXButton = new DEViseToolBarButton("Zoom in X",
		  "zoom_in-x", jsValues);
		zoomInYButton = new DEViseToolBarButton("Zoom in Y",
		  "zoom_in-y", jsValues);
		zoomOutXYButton = new DEViseToolBarButton("Zoom out XY",
		  "zoom_out-all", jsValues);
		zoomOutXButton = new DEViseToolBarButton("Zoom out X",
		  "zoom_out-x", jsValues);
		zoomOutYButton = new DEViseToolBarButton("Zoom out Y",
		  "zoom_out-y", jsValues);
	    zoomButtonSet = new DEViseToolBarButton[] {
			zoomInXYButton,
			zoomInXButton,
/* Not yet supported by DEVise.
			zoomInYButton,
*/
			zoomOutXYButton,
			zoomOutXButton,
/* Not yet supported by DEVise.
			zoomOutYButton
*/
	    };

		setFloatable(false);
		setOrientation(VERTICAL);
		setMinimumSize(new Dimension(MIN_WIDTH, MIN_HEIGHT));
		setBorder(BorderFactory.createEmptyBorder());
        addDefaultButtons();

		// set the 'crosshairs' button as selected by default
		setNormal();
    }

	public void setJmolCanvas(DEViseCanvas3DJmol canvas) {
		jmolCanvas = canvas;
		setThreeDEnabled(canvas != null);
	}

	public DEViseCanvas3DJmol getJmolCanvas() {
		return jmolCanvas;
	}
	
	private void addDefaultButtons() {
		if(showInfoButtonSet) {
		    addButtonSet( infoButtonSet );
		}
		if(showViewButtonSet) {
			addButtonSet( viewButtonSet );
		}
		if(showThreeDButtonSet) {
		    addButtonSet( threeDButtonSet );
		}
		if(showFilterToolsButtonSet) {
		    addButtonSet( filterToolsButtonSet );
		}
		if(showZoomButtonSet) {
		    addButtonSet( zoomButtonSet );
		}		
	}
	
	private void addButtonSet(DEViseToolBarButton[] buttonSet) {
		for(int i=0; i<buttonSet.length; i++) {
			this.add(buttonSet[i]);
			buttonSet[i].addActionListener(new DEViseToolBarButtonAction(this));
		}
		this.addSeparator(new Dimension(0, SEPARATOR_HEIGHT));	
	}

	private void setThreeDEnabled(boolean enabled) {
		// TEMPTOOLBAR Don't enable these until we get them to actually work.
		if (enabled) return;

		Runnable doSet3DEnabled = new DoSet3DEnabled(enabled);
		SwingUtilities.invokeLater(doSet3DEnabled);
	}

	class DoSet3DEnabled implements Runnable {
		private boolean _enabled;
		public DoSet3DEnabled(boolean enabled) {
			_enabled = enabled;
		}
		public void run() {
			threeDRotateButton.setEnabled(_enabled);
			threeDTranslateButton.setEnabled(_enabled);
			threeDZoomButton.setEnabled(_enabled);
		}
	}

	protected DEViseToolBarButton getSelectedButton() {
	    return selectedButton;
	}

	public boolean doNormal() {
		return selectedButton == normalButton;
	}

	public boolean doCursorFill() {
		return selectedButton == cursorFillButton;
	}

	public boolean doDrillDown() {
		return selectedButton == drillDownButton;
	}

	public boolean doViewHelp() {
		return selectedButton == helpButton;
	}

	public boolean doDataDownload() {
		return selectedButton == dataDownloadButton;
	}

	public boolean doHome() {
		return selectedButton == homeButton;
	}

	public boolean doToggleFilter() {
		return selectedButton == toggleFilterButton;
	}

	public boolean doIncreaseSymSize() {
		return selectedButton == increaseSymSizeButton;
	}

	public boolean doDecreaseSymSize() {
		return selectedButton == decreaseSymSizeButton;
	}

	public boolean doZoomXY() {
		return (selectedButton == normalButton) ||
		  (selectedButton == zoomInXYButton) ||
		  (selectedButton == zoomOutXYButton) ||
		  (selectedButton == threeDRotateButton);//TEMPTOOLBAR -- do we want this??????
	}

	public boolean doZoomXOnly() {
		return (selectedButton == zoomInXButton) ||
		  (selectedButton == zoomOutXButton);
	}

	public boolean doZoomYOnly() {
		return (selectedButton == zoomInYButton) ||
		  (selectedButton == zoomOutYButton);
	}

	public boolean doZoomIn() {
		return (selectedButton == normalButton) ||
		  (selectedButton == zoomInXYButton) ||
		  (selectedButton == zoomInXButton) ||
		  (selectedButton == zoomInYButton) ||
		  (selectedButton == threeDRotateButton);//TEMPTOOLBAR -- do we want this?
	}

	public boolean doZoomOut() {
		return (selectedButton == zoomOutXYButton) ||
		  (selectedButton == zoomOutXButton) ||
		  (selectedButton == zoomOutYButton);
	}

	// Modes where zoom is the only allowed action.
	public boolean doZoomOnly() {
		return (selectedButton == zoomInXYButton) ||
		  (selectedButton == zoomInXButton) ||
		  (selectedButton == zoomInYButton) ||
		  (selectedButton == zoomOutXYButton) ||
		  (selectedButton == zoomOutXButton) ||
		  (selectedButton == zoomOutYButton);
	}

	public boolean doRubberband() {
		return (doZoomIn() || doZoomOut());
	}

	public boolean doCursorOps() {
		return selectedButton == normalButton;
		//TEMPTOOLBAR -- may need more stuff here eventually -- buttons for doing cursor stuff w/o zooming
	}

	public void setNormal() {
		Runnable doSetNormal = new Runnable() {
			public void run() {
				setSelectedButton(normalButton);
			}
		};
		SwingUtilities.invokeLater(doSetNormal);
	}

	protected DEViseToolBarButton setSelectedButton(
	  DEViseToolBarButton button) {

		if (selectedButton != null) {
			selectedButton.setSelected(false);
		}

		selectedButton = button;
		button.setSelected(true);

		// Set the mouse cursor according to the toolbar mode.
		if (button == drillDownButton) {
			mouseCursor.setPermanentCursor(mouseCursor.drillDownCursor, null);

		} else if (button == helpButton) {
			mouseCursor.setPermanentCursor(mouseCursor.helpCursor, null);

		} else if (button == dataDownloadButton) {
			mouseCursor.setPermanentCursor(mouseCursor.dataDownloadCursor, null);

		} else if (button == normalButton) {
			mouseCursor.setPermanentCursor(mouseCursor.defaultCursor, null);

		} else if (button == cursorFillButton) {
			mouseCursor.setPermanentCursor(mouseCursor.fillViewCursor, null);

		} else if (button == increaseSymSizeButton) {
			mouseCursor.setPermanentCursor(mouseCursor.incSymSizeCursor, null);

		} else if (button == decreaseSymSizeButton) {
			mouseCursor.setPermanentCursor(mouseCursor.decSymSizeCursor, null);

		} else if (button == threeDRotateButton) {
			mouseCursor.setPermanentCursor(mouseCursor.jmRotCursor, null);

		} else if (button == threeDTranslateButton) {
			mouseCursor.setPermanentCursor(mouseCursor.jmTransCursor, null);

		} else if (button == threeDZoomButton) {
			mouseCursor.setPermanentCursor(mouseCursor.jmZoomCursor, null);

		} else if (button == homeButton) {
			mouseCursor.setPermanentCursor(mouseCursor.homeCursor, null);

		} else if (button == toggleFilterButton) {
			mouseCursor.setPermanentCursor(mouseCursor.togFiltCursor, null);

		} else if (button == zoomInXYButton) {
			mouseCursor.setPermanentCursor(mouseCursor.zoomInCursor, null);

		} else if (button == zoomInXButton) {
			mouseCursor.setPermanentCursor(mouseCursor.zoomInCursor, null);

		} else if (button == zoomInYButton) {
			mouseCursor.setPermanentCursor(mouseCursor.zoomInCursor, null);

		} else if (button == zoomOutXYButton) {
			mouseCursor.setPermanentCursor(mouseCursor.zoomOutCursor, null);

		} else if (button == zoomOutXButton) {
			mouseCursor.setPermanentCursor(mouseCursor.zoomOutCursor, null);

		} else if (button == zoomOutYButton) {
			mouseCursor.setPermanentCursor(mouseCursor.zoomOutCursor, null);

		} else {
			System.err.println("Warning: unrecognized toolbar button");
			mouseCursor.setPermanentCursor(mouseCursor.defaultCursor, null);
		}

		if (button == drillDownButton) {
			if (jmolCanvas != null) {
				jmolCanvas.enableDrillDown();
			}
		} else {
			if (jmolCanvas != null) {
				jmolCanvas.disableDrillDown();
			}
		}

		return selectedButton;
	}
}
