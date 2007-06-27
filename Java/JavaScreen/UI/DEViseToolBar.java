// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2007
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
	private DEViseToolBarButton[] infoButtonSet = null;

	private DEViseToolBarButton normalButton = null;
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

	private DEViseToolBarButton zoomInXButton = null;
	private DEViseToolBarButton zoomInYButton = null;
	private DEViseToolBarButton zoomOutXYButton = null;
	private DEViseToolBarButton zoomOutXButton = null;
	private DEViseToolBarButton zoomOutYButton = null;
	private DEViseToolBarButton[] zoomButtonSet = null;
	
	private boolean infoButtonSetEnabled = true;
	private boolean viewButtonSetEnabled = true;
	private boolean threeDButtonSetEnabled = true;
	private boolean filterToolsButtonSetEnabled = true;
	private boolean zoomButtonSetEnabled = true;
	
    public DEViseToolBar(DEViseJSValues jsValues) {

		drillDownButton = new DEViseToolBarButton(
		  "Drill down (show data records)", "drill-down", jsValues);
		helpButton = new DEViseToolBarButton("Show help for selected view",
		  "show_view_help", jsValues);
	    infoButtonSet = new DEViseToolBarButton[] {
		    drillDownButton,
			helpButton,
	    };

		normalButton = new DEViseToolBarButton(
		  "Select cursor/zoom in XY/3D rotate", "crosshairs", jsValues);
		cursorFillButton = new DEViseToolBarButton(
		  "Fill selected view with cursor", "full_screen_filter",
		  jsValues);
		increaseSymSizeButton = new DEViseToolBarButton("Increase symbol size",
		  "increase_symbol_size", jsValues);
		decreaseSymSizeButton = new DEViseToolBarButton("Decrease symbol size",
		  "decrease_symbol_size", jsValues);
	    viewButtonSet = new DEViseToolBarButton[] {
			normalButton,
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
		toggleFilterButton = new DEViseToolBarButton("Toggle visual filter",
		  "toggle_filter", jsValues);
	    filterToolsButtonSet = new DEViseToolBarButton[] {
			homeButton,
			toggleFilterButton
	    };

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
	
	private void addDefaultButtons() {
		if(infoButtonSetEnabled) {
		    addButtonSet( infoButtonSet );
		}
		if(viewButtonSetEnabled) {
			addButtonSet( viewButtonSet );
		}
		if(threeDButtonSetEnabled) {
		    addButtonSet( threeDButtonSet );
		}
		if(filterToolsButtonSetEnabled) {
		    addButtonSet( filterToolsButtonSet );
		}
		if(zoomButtonSetEnabled) {
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

	public boolean doCursorFill() {
		return selectedButton == cursorFillButton;
	}

	public boolean doDrillDown() {
		return selectedButton == drillDownButton;
	}

	public boolean doViewHelp() {
		return selectedButton == helpButton;
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
		  (selectedButton == zoomOutXYButton) ||
		  (selectedButton == threeDRotateButton);
	}

	public boolean doZoomX() {
		return (selectedButton == zoomInXButton) ||
		  (selectedButton == zoomOutXButton);
	}
	public boolean doZoomY() {
		return (selectedButton == zoomInYButton) ||
		  (selectedButton == zoomOutYButton);
	}

	public boolean doZoomIn() {
		return (selectedButton == normalButton) ||
		  (selectedButton == zoomInXButton) ||
		  (selectedButton == zoomInYButton) ||
		  (selectedButton == threeDRotateButton);
	}

	public boolean doZoomOut() {
		return (selectedButton == zoomOutXYButton) ||
		  (selectedButton == zoomOutXButton) ||
		  (selectedButton == zoomOutYButton);
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
