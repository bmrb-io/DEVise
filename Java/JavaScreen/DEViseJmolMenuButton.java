// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2006
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Create button and corresponding menu for Jmol functions.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2006/06/23 19:52:41  wenger
// Merged devise_jmol_br_1 thru devise_jmol_br_2 to the trunk.
//
// Revision 1.2  2006/05/26 16:22:16  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.11  2006/06/27 20:49:30  wenger
// Added menu option to highlight by color instead of selection
// halo; slight cleanup of how the Jmol popup menu is generated;
// other minor cleanups.
//
// Revision 1.1.2.10  2006/06/23 17:08:02  wenger
// Implemented setting Jmol menu checkboxes to the right state;
// tried help stuff, but both the HelpDialog and AboutDialog
// generate security violations when running as an applet.
//
// Revision 1.1.2.9  2006/06/19 22:10:02  wenger
// Got Jmol vibration menu items working (committing with a *temporary*
// change to load a molecule that does something when you turn on
// vibration).
//
// Revision 1.1.2.8  2006/06/19 20:32:44  wenger
// Got Jmol animation menu items working (committing with a *temporary*
// change to load a molecule that does something when you run animations).
//
// Revision 1.1.2.7  2006/06/15 19:54:30  wenger
// Most (but not all) Jmol menus in the JS now working; still need to
// update the JS's Jmol menus as the state of Jmol changes, etc.;
// fixed a null pointer exception if closing coordinate session after
// destroying the tree dialog.
//
// Revision 1.1.2.6  2006/06/14 18:35:29  wenger
// Changed Jmol menus to use string constants so subsequent comparisons
// are safe (don't have to worry about typos causing mismatches).
//
// Revision 1.1.2.5  2006/06/14 16:32:02  wenger
// Added new DEViseButton class to force the colors and font we want
// for buttons; cleaned up things in jsdevisec (made public members
// private, etc.); started on getting more of the Jmol menus actually
// working.
//
// Revision 1.1.2.4  2006/06/13 19:12:18  wenger
// Jmol-related menus are in place, but most not actually working
// yet.
//
// Revision 1.1.2.3  2006/06/12 22:05:02  wenger
// Improved fonts -- larger, buttons/menus are now sans-serif.
//
// Revision 1.1.2.2  2006/06/06 21:48:22  wenger
// Added Jmol menus to the JavaScreen Jmol menu button (only the View
// menu is functional at this time).
//
// Revision 1.1.2.1  2006/05/23 18:17:50  wenger
// Added initial Jmol menu with a menu item to show the tree selection
// window; destroying and re-creating the window currently doesn't
// preserve the existing selection, although I started on provision for
// that.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.util.*;
import  java.text.*;
import  java.awt.*;
import  java.awt.event.*;


import org.jmol.api.JmolViewer;

public class DEViseJmolMenuButton extends DEViseButton
  implements ActionListener, ItemListener
{
    private static final int DEBUG = 0;

    private Button thisButton;
    private DEViseCanvas3DJmol jmolCanvas;
    private PopupMenu jmolMenu;

    private CheckboxMenuItem perspectiveDepth;
    private CheckboxMenuItem showAxes;
    private CheckboxMenuItem showBoundBox;
    private CheckboxMenuItem showHydrogens;
    // private CheckboxMenuItem showVectors;
    private CheckboxMenuItem showMeasurements;

    // Put all menu strings here so we don't have to worry about typos
    // when we check for equality in actionPerformed().
    private static final String STR_3_PIXELS = "3 pixels";
    private static final String STR_ALL = "All";
    private static final String STR_AMINO = "Amino";
    private static final String STR_ANG_005 = "0.05 A";
    private static final String STR_ANG_010 = "0.10 A";
    private static final String STR_ANG_015 = "0.15 A";
    private static final String STR_ANG_020 = "0.20 A";
    private static final String STR_ANG_005_VECTOR = "0.05 A vector";
    private static final String STR_ANG_010_BOND = "0.10 A bond";
    private static final String STR_ANG_010_VECTOR = "0.10 A vector";
    private static final String STR_ANG_015_BOND = "0.15 A bond";
    private static final String STR_ANG_020_BOND = "0.20 A bond";
    private static final String STR_ANGSTROMS = "Angstroms 1E-10";
    private static final String STR_ANIMATE = "Animate";
    private static final String STR_ATOM = "Atom";
    private static final String STR_AXES = "Axes";
    private static final String STR_BOND = "Bond";
    private static final String STR_BOTTOM = "Bottom";
    private static final String STR_BOUND_BOX = "Bounding Box";
    private static final String STR_BY_COLOR = "By Color";
    private static final String STR_CARBON = "Carbon";
    private static final String STR_CENTERED = "Centered";
    private static final String STR_DEFINE_CTR = "Define Center";
    private static final String STR_DISPLAY = "Display";
    private static final String STR_DISTANCE = "Distance";
    private static final String STR_EDIT = "Edit";
    private static final String STR_FIRST_FREQ = "First frequency";
    private static final String STR_FRONT = "Front";
    private static final String STR_GO_TO_NEXT = "Go to next frame";
    private static final String STR_GO_TO_PREV = "Go to previous frame";
    private static final String STR_HELP = "Help";
    private static final String STR_HETERO = "Hetero";
    private static final String STR_HIGHLIGHT = "Highlight";
    private static final String STR_HYDROGEN = "Hydrogen";
    private static final String STR_HYDROGENS = "Hydrogens";
    private static final String STR_JMOL = "Jmol";
    private static final String STR_LABEL = "Label";
    private static final String STR_LEFT = "Left";
    private static final String STR_LOOP = "Loop";
    private static final String STR_MEASUREMENTS = "Measurements";
    private static final String STR_NAME = "Name";
    private static final String STR_NANOMETERS = "Nanometers 1E-9";
    private static final String STR_NEXT_FREQ = "Next frequency";
    private static final String STR_NITROGEN = "Nitrogen";
    private static final String STR_NONE = "None";
    private static final String STR_NONE_ATOM = "None atom";
    private static final String STR_NONE_BOND = "None bond";
    private static final String STR_NONE_LABEL = "None label";
    private static final String STR_NONE_SELECT = "None select";
    private static final String STR_NONE_VECTOR = "None vector";
    private static final String STR_NUCLEIC = "Nucleic";
    private static final String STR_NUMBER = "Number";
    private static final String STR_ON = "On";
    private static final String STR_ONCE = "Once";
    private static final String STR_OXYGEN = "Oxygen";
    private static final String STR_PALINDROME = "Palindrome";
    private static final String STR_PCT_100 = "100%";
    private static final String STR_PCT_150 = "150%";
    private static final String STR_PCT_200 = "200%";
    private static final String STR_PCT_400 = "400%";
    private static final String STR_PCT_800 = "800%";
    private static final String STR_PERS_DEPTH = "Perspective Depth";
    private static final String STR_PHOSPHORUS = "Phosphorus";
    private static final String STR_PICOMETERS = "Picometers 1E-12";
    private static final String STR_PREV_FREQ = "Previous frequency";
    private static final String STR_REWIND = "Rewind to first frame";
    private static final String STR_RIGHT = "Right";
    private static final String STR_SCALE_02 = "Scale 0.2";
    private static final String STR_SCALE_05 = "Scale 0.5";
    private static final String STR_SCALE_10 = "Scale 1";
    private static final String STR_SCALE_20 = "Scale 2";
    private static final String STR_SCALE_50 = "Scale 5";
    private static final String STR_SEL_TREES = "Selection trees...";
    private static final String STR_SELECT = "Select";
    private static final String STR_SELECT_ALL = "Select All";
    private static final String STR_START_VIBE = "Start vibration";
    private static final String STR_STOP_ANIMATION = "Stop animation";
    private static final String STR_STOP_VIBE = "Stop vibration";
    private static final String STR_SULFUR = "Sulfur";
    private static final String STR_SYMBOL = "Symbol";
    private static final String STR_TOOLS = "Tools";
    private static final String STR_TOP = "Top";
    private static final String STR_UPPER_RIGHT = "Upper right";
    private static final String STR_VANDER_15 = "15% vanderWaals";
    private static final String STR_VANDER_20 = "20% vanderWaals";
    private static final String STR_VANDER_25 = "25% vanderWaals";
    private static final String STR_VANDER_100 = "100% vanderWaals";
    private static final String STR_VECTOR = "Vector";
    private static final String STR_VECTORS = "Vectors";
    private static final String STR_VIBRATE = "Vibrate";
    private static final String STR_VIEW = "View";
    private static final String STR_WATER = "Water";
    private static final String STR_WIREFRAME = "Wireframe";
    private static final String STR_WITH_HALOS = "With Halos";
    private static final String STR_ZOOM = "Zoom";

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public DEViseJmolMenuButton(DEViseJSValues jsValues)
    {
        super(STR_JMOL, jsValues);

	thisButton = this;

	jmolMenu = new PopupMenu();
	MenuItem treeMenuItem;

	//
	// Edit menu.
	//
	PopupMenu editMenu = new PopupMenu(STR_EDIT);
	jmolMenu.add(editMenu);

	addMenuItem(editMenu, STR_SELECT_ALL);

	editMenu.add(createSelectMenu());

	//
	// Display menu.
	//
	PopupMenu displayMenu = new PopupMenu(STR_DISPLAY);
	jmolMenu.add(displayMenu);

	displayMenu.add(createSelectMenu());

	PopupMenu atomMenu = new PopupMenu(STR_ATOM);
	displayMenu.add(atomMenu);

	treeMenuItem = addMenuItem(atomMenu, STR_NONE);
	treeMenuItem.setActionCommand(STR_NONE_ATOM);
	addMenuItem(atomMenu, STR_VANDER_15);
	addMenuItem(atomMenu, STR_VANDER_20);
	addMenuItem(atomMenu, STR_VANDER_25);
	addMenuItem(atomMenu, STR_VANDER_100);

	PopupMenu bondMenu = new PopupMenu(STR_BOND);
	displayMenu.add(bondMenu);

	treeMenuItem = addMenuItem(bondMenu, STR_NONE);
	treeMenuItem.setActionCommand(STR_NONE_BOND);
	addMenuItem(bondMenu, STR_WIREFRAME);
	treeMenuItem = addMenuItem(bondMenu, STR_ANG_010);
	treeMenuItem.setActionCommand(STR_ANG_010_BOND);
	treeMenuItem = addMenuItem(bondMenu, STR_ANG_015);
	treeMenuItem.setActionCommand(STR_ANG_015_BOND);
	treeMenuItem = addMenuItem(bondMenu, STR_ANG_020);
	treeMenuItem.setActionCommand(STR_ANG_020_BOND);

	PopupMenu labelMenu = new PopupMenu(STR_LABEL);
	displayMenu.add(labelMenu);

	treeMenuItem = addMenuItem(labelMenu, STR_NONE);
	treeMenuItem.setActionCommand(STR_NONE_LABEL);
	addMenuItem(labelMenu, STR_SYMBOL);
	addMenuItem(labelMenu, STR_NAME);
	addMenuItem(labelMenu, STR_NUMBER);
	labelMenu.add(new MenuItem("-"));
	addMenuItem(labelMenu, STR_CENTERED);
	addMenuItem(labelMenu, STR_UPPER_RIGHT);

	PopupMenu vectorMenu = new PopupMenu(STR_VECTOR);
	displayMenu.add(vectorMenu);

	treeMenuItem = addMenuItem(vectorMenu, STR_NONE);
	treeMenuItem.setActionCommand(STR_NONE_VECTOR);
	addMenuItem(vectorMenu, STR_ON);
	addMenuItem(vectorMenu, STR_3_PIXELS);
	treeMenuItem = addMenuItem(vectorMenu, STR_ANG_005);
	treeMenuItem.setActionCommand(STR_ANG_005_VECTOR);
	treeMenuItem = addMenuItem(vectorMenu, STR_ANG_010);
	treeMenuItem.setActionCommand(STR_ANG_010_VECTOR);
	vectorMenu.add(new MenuItem("-"));
	addMenuItem(vectorMenu, STR_SCALE_02);
	addMenuItem(vectorMenu, STR_SCALE_05);
	addMenuItem(vectorMenu, STR_SCALE_10);
	addMenuItem(vectorMenu, STR_SCALE_20);
	addMenuItem(vectorMenu, STR_SCALE_50);

	vectorMenu.add(new MenuItem("-"));
	PopupMenu zoomMenu = new PopupMenu(STR_ZOOM);
	displayMenu.add(zoomMenu);

	addMenuItem(zoomMenu, STR_PCT_100);
	addMenuItem(zoomMenu, STR_PCT_150);
	addMenuItem(zoomMenu, STR_PCT_200);
	addMenuItem(zoomMenu, STR_PCT_400);
	addMenuItem(zoomMenu, STR_PCT_800);

	displayMenu.add(new MenuItem("-"));
	perspectiveDepth = addCheckboxMenuItem(displayMenu, STR_PERS_DEPTH);
	showAxes = addCheckboxMenuItem(displayMenu, STR_AXES);
	showBoundBox = addCheckboxMenuItem(displayMenu, STR_BOUND_BOX);

	displayMenu.add(new MenuItem("-"));
	showHydrogens = addCheckboxMenuItem(displayMenu, STR_HYDROGENS);
	// showVectors = addCheckboxMenuItem(displayMenu, STR_VECTORS);
	showMeasurements = addCheckboxMenuItem(displayMenu, STR_MEASUREMENTS);

	//
	// View menu.
	//
	PopupMenu viewMenu = new PopupMenu(STR_VIEW);
	jmolMenu.add(viewMenu);

	addMenuItem(viewMenu, STR_SEL_TREES);

	PopupMenu highlightMenu = new PopupMenu(STR_HIGHLIGHT);
	viewMenu.add(highlightMenu);
	addMenuItem(highlightMenu, STR_WITH_HALOS);
	addMenuItem(highlightMenu, STR_BY_COLOR);

	addMenuItem(viewMenu, STR_FRONT);
	addMenuItem(viewMenu, STR_TOP);
	addMenuItem(viewMenu, STR_BOTTOM);
	addMenuItem(viewMenu, STR_RIGHT);
	addMenuItem(viewMenu, STR_LEFT);
	addMenuItem(viewMenu, STR_DEFINE_CTR);

	//
	// Tools menu.
	//
	PopupMenu toolsMenu = new PopupMenu(STR_TOOLS);
	jmolMenu.add(toolsMenu);

	PopupMenu distanceMenu = new PopupMenu(STR_DISTANCE);
	toolsMenu.add(distanceMenu);

	addMenuItem(distanceMenu, STR_NANOMETERS);
	addMenuItem(distanceMenu, STR_ANGSTROMS);
	addMenuItem(distanceMenu, STR_PICOMETERS);

	PopupMenu animateMenu = new PopupMenu(STR_ANIMATE);
	toolsMenu.add(animateMenu);

	addMenuItem(animateMenu, STR_ONCE);
	addMenuItem(animateMenu, STR_LOOP);
	addMenuItem(animateMenu, STR_PALINDROME);
	addMenuItem(animateMenu, STR_STOP_ANIMATION);
	animateMenu.add(new MenuItem("-"));
	addMenuItem(animateMenu, STR_REWIND);
	addMenuItem(animateMenu, STR_GO_TO_NEXT);
	addMenuItem(animateMenu, STR_GO_TO_PREV);

	PopupMenu vibrateMenu = new PopupMenu(STR_VIBRATE);
	toolsMenu.add(vibrateMenu);

	addMenuItem(vibrateMenu, STR_START_VIBE);
	addMenuItem(vibrateMenu, STR_STOP_VIBE);
	vibrateMenu.add(new MenuItem("-"));
	addMenuItem(vibrateMenu, STR_FIRST_FREQ);
	addMenuItem(vibrateMenu, STR_NEXT_FREQ);
	addMenuItem(vibrateMenu, STR_PREV_FREQ);

	//
	// Help menu.
	//
	// addMenuItem(jmolMenu, STR_HELP);

	add(jmolMenu);
	addActionListener(this);
    }

    //-------------------------------------------------------------------
    public void show(DEViseCanvas3DJmol canvas)
    {
	jmolCanvas = canvas;
        super.show();
    }

    //-------------------------------------------------------------------
    public void actionPerformed(ActionEvent event)
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJmolMenuButton.actionPerformed(" +
	      event.getActionCommand() + ")");
	}

	JmolViewer viewer = null;
	if (jmolCanvas != null) viewer = jmolCanvas.get3DViewer();
	if (viewer == null) {
	    System.err.println("No Jmol viewer available!");
	    return;
	}

	setCheckboxes(viewer);

    	if (event.getActionCommand().equals(STR_JMOL)) {
	    // This is a special case -- the Jmol button was clicked, so
	    // we need to show the menu (as opposed to a menu item being
	    // selected).
	    jmolMenu.show(thisButton, DEViseMainButtons.MENU_X_OFFSET,
	      DEViseMainButtons.MENU_Y_OFFSET);

	} else if (event.getActionCommand().equals(STR_3_PIXELS)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vectors 3");

	} else if (event.getActionCommand().equals(STR_ALL)) {
	    viewer.selectAll();

	} else if (event.getActionCommand().equals(STR_AMINO)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select amino");

	} else if (event.getActionCommand().equals(STR_ANG_005_VECTOR)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vectors 0.05");

	} else if (event.getActionCommand().equals(STR_ANG_010_BOND)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "wireframe .1");

	} else if (event.getActionCommand().equals(STR_ANG_010_VECTOR)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vectors 0.10");

	} else if (event.getActionCommand().equals(STR_ANG_015_BOND)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "wireframe .15");

	} else if (event.getActionCommand().equals(STR_ANG_020_BOND)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "wireframe .2");

	} else if (event.getActionCommand().equals(STR_ANGSTROMS)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "set measure angstroms");

	} else if (event.getActionCommand().equals(STR_BOTTOM)) {
	    jmolCanvas.bottom();

	} else if (event.getActionCommand().equals(STR_BY_COLOR)) {
	    jmolCanvas.setHighlightWithHalos(false);

	} else if (event.getActionCommand().equals(STR_CARBON)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select carbon");

	} else if (event.getActionCommand().equals(STR_CENTERED)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "set labeloffset 0 0");

	} else if (event.getActionCommand().equals(STR_DEFINE_CTR)) {
	    jmolCanvas.defineCenter();

	} else if (event.getActionCommand().equals(STR_FIRST_FREQ)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame 1");

	} else if (event.getActionCommand().equals(STR_FRONT)) {
	    jmolCanvas.front();

	} else if (event.getActionCommand().equals(STR_GO_TO_NEXT)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame next");

	} else if (event.getActionCommand().equals(STR_GO_TO_PREV)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame prev");

/*
// Both HelpDialog and AboutDialog generate security violations when
// running as an applet.
	} else if (event.getActionCommand().equals(STR_HELP)) {
*/

	} else if (event.getActionCommand().equals(STR_HETERO)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select hetero");

	} else if (event.getActionCommand().equals(STR_HYDROGEN)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select hydrogen");

	} else if (event.getActionCommand().equals(STR_LEFT)) {
	    jmolCanvas.left();

	} else if (event.getActionCommand().equals(STR_LOOP)) {
	    jmolCanvas.jmolEvalStringErr(viewer,
	      "frame 1; anim mode loop; anim on");

	} else if (event.getActionCommand().equals(STR_NAME)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "label %a");

	} else if (event.getActionCommand().equals(STR_NANOMETERS)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "set measure nanometers");

	} else if (event.getActionCommand().equals(STR_NEXT_FREQ)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame next");

	} else if (event.getActionCommand().equals(STR_NITROGEN)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select nitrogen");

	} else if (event.getActionCommand().equals(STR_NONE_ATOM)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "cpk off");

	} else if (event.getActionCommand().equals(STR_NONE_BOND)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "wireframe off");

	} else if (event.getActionCommand().equals(STR_NONE_LABEL)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "label off");

	} else if (event.getActionCommand().equals(STR_NONE_SELECT)) {
	    viewer.clearSelection();

	} else if (event.getActionCommand().equals(STR_NONE_VECTOR)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vectors off");

	} else if (event.getActionCommand().equals(STR_NUCLEIC)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select nucleic");

	} else if (event.getActionCommand().equals(STR_NUMBER)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "label %i");

	} else if (event.getActionCommand().equals(STR_ON)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vectors on");

	} else if (event.getActionCommand().equals(STR_ONCE)) {
	    jmolCanvas.jmolEvalStringErr(viewer,
	      "frame 1; anim mode once; anim on");

	} else if (event.getActionCommand().equals(STR_OXYGEN)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select oxygen");

	} else if (event.getActionCommand().equals(STR_PALINDROME)) {
	    jmolCanvas.jmolEvalStringErr(viewer,
	      "frame 1; anim mode palindrome; anim on");

	} else if (event.getActionCommand().equals(STR_PCT_100)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "zoom 100");

	} else if (event.getActionCommand().equals(STR_PCT_150)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "zoom 150");

	} else if (event.getActionCommand().equals(STR_PCT_200)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "zoom 200");

	} else if (event.getActionCommand().equals(STR_PCT_400)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "zoom 400");

	} else if (event.getActionCommand().equals(STR_PCT_800)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "zoom 800");

	} else if (event.getActionCommand().equals(STR_PHOSPHORUS)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select phosphorus");

	} else if (event.getActionCommand().equals(STR_PICOMETERS)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "set measure picometers");

	} else if (event.getActionCommand().equals(STR_PREV_FREQ)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame prev");

	} else if (event.getActionCommand().equals(STR_REWIND)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame 1");

	} else if (event.getActionCommand().equals(STR_RIGHT)) {
	    jmolCanvas.right();

	} else if (event.getActionCommand().equals(STR_SCALE_02)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vector scale 0.2");

	} else if (event.getActionCommand().equals(STR_SCALE_05)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vector scale 0.5");

	} else if (event.getActionCommand().equals(STR_SCALE_10)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vector scale 1");

	} else if (event.getActionCommand().equals(STR_SCALE_20)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vector scale 2");

	} else if (event.getActionCommand().equals(STR_SCALE_50)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vector scale 5");

	} else if (event.getActionCommand().equals(STR_SEL_TREES)) {
	    jmolCanvas.showTrees();

	} else if (event.getActionCommand().equals(STR_SELECT_ALL)) {
	    viewer.selectAll();

	} else if (event.getActionCommand().equals(STR_START_VIBE)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "frame 1; vibration on");

	} else if (event.getActionCommand().equals(STR_STOP_ANIMATION)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "anim off");

	} else if (event.getActionCommand().equals(STR_STOP_VIBE)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "vibration off");

	} else if (event.getActionCommand().equals(STR_SULFUR)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select sulfur");

	} else if (event.getActionCommand().equals(STR_SYMBOL)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "label %e");

	} else if (event.getActionCommand().equals(STR_TOP)) {
	    jmolCanvas.top();

	} else if (event.getActionCommand().equals(STR_UPPER_RIGHT)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "set labeloffset 4 4");

	} else if (event.getActionCommand().equals(STR_VANDER_15)) {
	    viewer.setPercentVdwAtom(15);

	} else if (event.getActionCommand().equals(STR_VANDER_20)) {
	    viewer.setPercentVdwAtom(20);

	} else if (event.getActionCommand().equals(STR_VANDER_25)) {
	    viewer.setPercentVdwAtom(25);

	} else if (event.getActionCommand().equals(STR_VANDER_100)) {
	    viewer.setPercentVdwAtom(100);

	} else if (event.getActionCommand().equals(STR_WATER)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "select water");

	} else if (event.getActionCommand().equals(STR_WIREFRAME)) {
	    jmolCanvas.jmolEvalStringErr(viewer, "wireframe on");

	} else if (event.getActionCommand().equals(STR_WITH_HALOS)) {
	    jmolCanvas.setHighlightWithHalos(true);

	} else {
	    System.err.println("Unknown action: " + event.getActionCommand());
	}
    }

    //-------------------------------------------------------------------
    public void itemStateChanged(ItemEvent event)
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJmolMenuButton.itemStateChanged(" +
	      event.paramString() + ")");
	}

	JmolViewer viewer = null;
	if (jmolCanvas != null) viewer = jmolCanvas.get3DViewer();
	if (viewer == null) {
	    System.err.println("No Jmol viewer available!");
	    return;
	}

        String command = event.getItem().toString();

	if (command.equals(STR_AXES)) {
	    viewer.setShowAxes(event.getStateChange() == ItemEvent.SELECTED);

	} else if (command.equals(STR_BOUND_BOX)) {
	    viewer.setShowBbcage(event.getStateChange() == ItemEvent.SELECTED);

	} else if (command.equals(STR_HYDROGENS)) {
	    viewer.setShowHydrogens(
	      event.getStateChange() == ItemEvent.SELECTED);

	} else if (command.equals(STR_MEASUREMENTS)) {
	    viewer.setShowMeasurements(
	      event.getStateChange() == ItemEvent.SELECTED);

	} else if (command.equals(STR_PERS_DEPTH)) {
	    viewer.setPerspectiveDepth(
	      event.getStateChange() == ItemEvent.SELECTED);

/*
	} else if (command.equals(STR_VECTORS)) {
*/

	} else {
	    System.err.println("Unknown item state change: " + command);
	}
    }

    //===================================================================
    // PRIVATE METHODS

    private PopupMenu createSelectMenu()
    {
	PopupMenu selectMenu = new PopupMenu(STR_SELECT);

	addMenuItem(selectMenu, STR_ALL);

	MenuItem treeMenuItem = addMenuItem(selectMenu, STR_NONE);
	treeMenuItem.setActionCommand(STR_NONE_SELECT);

	addMenuItem(selectMenu, STR_HYDROGEN);
	addMenuItem(selectMenu, STR_CARBON);
	addMenuItem(selectMenu, STR_NITROGEN);
	addMenuItem(selectMenu, STR_OXYGEN);
	addMenuItem(selectMenu, STR_PHOSPHORUS);
	addMenuItem(selectMenu, STR_SULFUR);
	selectMenu.add(new MenuItem("-"));
	addMenuItem(selectMenu, STR_AMINO);
	addMenuItem(selectMenu, STR_NUCLEIC);
	addMenuItem(selectMenu, STR_HETERO);
	addMenuItem(selectMenu, STR_WATER);

	return selectMenu;
    }

    //-------------------------------------------------------------------
    private MenuItem addMenuItem(Menu menu, String itemName)
    {
	MenuItem treeMenuItem = new MenuItem(itemName);
	treeMenuItem.addActionListener(this);
	menu.add(treeMenuItem);

	return treeMenuItem;
    }

    //-------------------------------------------------------------------
    private CheckboxMenuItem addCheckboxMenuItem(Menu menu, String itemName)
    {
	CheckboxMenuItem treeMenuItem = new CheckboxMenuItem(itemName);
	treeMenuItem.addItemListener(this);
	menu.add(treeMenuItem);

	return treeMenuItem;
    }

    //-------------------------------------------------------------------
    // Update the setting of the menu checkboxes according to the
    // current Jmol viewer state.
    private void setCheckboxes(JmolViewer viewer)
    {
	perspectiveDepth.setState(viewer.getPerspectiveDepth());
	showAxes.setState(viewer.getShowAxes());
	showBoundBox.setState(viewer.getShowBbcage());
    	showHydrogens.setState(viewer.getShowHydrogens());
	// showVectors.setState(viewer.?());
	showMeasurements.setState(viewer.getShowMeasurements());
    }
}
