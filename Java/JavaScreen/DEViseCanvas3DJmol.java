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

// Jmol 3D implementation-specific aspects of DEViseCanvas.
// We use Jmol (http://jmol.sourceforge.net/) to do the 3D drawing,
// etc. for us.

// Note that this version of the 3D stuff is somewhat molecular-
// biology specific, because that's what Jmol is oriented towards.

// There are three trees (shown in a separate window) that control what
// is displayed and what is highlighted in the Jmol viewer.  These are
// as follows:
// - The Structure Display tree (selects what secondary structure(s) to
//   display)
// - The Atom Display tree (selects what specific atoms to display;
//   selection can occur by residue or by specific atom)
// - The Atom Selection tree (selects what specific atoms to highlight;
//   selection can occur by residue or by specific atom)
// Note that selection "trickles down" through the trees -- the Structure
// Display tree controls what is shown in the Atom Display tree.  The
// Atom Display tree controls what is shown in the Jmol viewer and
// the Atom Selection tree.  The Atom Selection tree only controls what
// is selected/highlighted in the Jmol viewer.
// The Atom Selection tree defaults to selecting the atoms that are
// selected in the DEVise highlight view of the 3D pile.  This selection
// can be overridden by the user.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.26.2.2  2011/06/03 23:21:28  wenger
// Cleaned up some of the junk that was just commented out previously.
//
// Revision 1.26.2.1  2011/06/03 23:10:50  wenger
// Working on getting embedded buttons in the JS working again -- big
// change so far is getting rid of the paint() method in DEViseScreen
// -- I think it was an error that that ever existed.  Lots of test/debug
// code in place right now as I play around with getting buttons to work.
//
// Revision 1.26  2011/04/21 15:34:16  wenger
// Merged js_highlight_thread_fix_br_0 thru js_highlight_thread_fix_br_1
// to trunk.
//
// Revision 1.25.2.1  2011/04/20 17:15:06  wenger
// Changed the DEViseGenericTree.setSelection() method and the YLogGUI
// p() and pn() methods to only actually update the GUI in the event
// dispatched thread, to hopefully cure problems with incorrect 3D
// highlight updating in the s2pred visualization, and null pointer
// problems when showing the log window.  (I actually meant to do some
// earlier commits to this branch with more of the debug code in place,
// but I forgot to do that.)
//
// Revision 1.25  2011/02/13 23:56:23  wenger
// Merged bug_1005_br_0 thru bug_1005_br_1 to trunk.
//
// Revision 1.24.4.10  2011/02/13 22:42:19  wenger
// Pretty much finished cleaning up the Jmol state preservation code.
//
// Revision 1.24.4.9  2011/02/13 02:43:00  wenger
// Got rid of debug output.
//
// Revision 1.24.4.8  2011/02/13 01:48:40  wenger
// The 'saving Jmol state on resizes' feature is now working at home
// -- the fix was putting the stuff that actually restores the Jmol
// state into the event dispatched thread.  (Lots of debug code still
// in place.)
//
// Revision 1.24.4.7  2011/02/09 18:31:44  wenger
// Okay, more test changes -- sleep in DEViseCanvas3DJmol.restoreJmoState()
// seems to fix things, but is not what I want for a "real" fix.
//
// Revision 1.24.4.6  2011/02/09 16:18:00  wenger
// Committing test changes, including sleep in
// DEViseCmdDispatcher.waitForCmds(), that seems to fix the problem with the Jmol restore state code hanging the JS on my laptop.
//
// Revision 1.24.4.5  2011/02/04 23:25:47  wenger
// Saving Jmol state: okay, trying to re-use the canvas didn't seem to
// work right (it didn't want to have the right size and location);
// got rid of temporary debug code.  Hmm -- maybe I can save the selection
// tree objects with the Jmol state...
//
// Revision 1.24.4.4  2011/02/04 22:39:47  wenger
// Saving Jmol state: this is pretty much working; leaving in a bunch
// of debug code and temporary comments for now (although I'm wondering
// if we could avoid destroying and re-creating the selection trees
// by saving and re-using the entire DEViseCanvas3DJmol object...).
//
// Revision 1.24.4.3  2011/01/21 22:10:23  wenger
// Removed javaGDatas from DEViseScreen because we never used it for
// anything; added some more debug output and comments to DEViseScreen;
// some temporary debug output for working on bug 1007.
//
// Revision 1.24.4.2  2011/01/21 19:37:50  wenger
// Cleaned up some of the temporary code.
//
// Revision 1.24.4.1  2011/01/21 19:18:37  wenger
// Initial part of fix of bug 1005 (Jmol loses state on resize) -- it's
// basically working, but needs cleanup because it relies on static
// variables in the DEViseCanvas3DJmol class, etc.; also, there's still
// some test code in place.
//
// Revision 1.24  2010/04/21 17:10:10  wenger
// Merged devise_dist_rest_1003_br_0 thru devise_dist_rest_1003_br_1 to trunk.
//
// Revision 1.23.4.13  2010/04/20 21:56:27  wenger
// Made restraints thinner.
//
// Revision 1.23.4.12  2010/04/19 16:30:38  wenger
// Protons are now initially displayed for distance restraint visualizations.
//
// Revision 1.23.4.11  2010/04/08 20:43:18  wenger
// Changed the JavaScreen to make a 3D Jmol canvas default to wireframe
// mode if the main view's name contains the string 'wireframe'.
//
// Revision 1.23.4.10  2010/04/08 17:08:14  wenger
// Oops -- one small change to compile without Jmol.jar.
//
// Revision 1.23.4.9  2010/04/06 22:25:42  wenger
// Made distance restraint lines thinner.
//
// Revision 1.23.4.8  2010/04/05 20:24:42  wenger
// Got coloring of restraints working (at least for violated/non-violated;
// I haven't tested ambiguous/non-ambiguous yet).
//
// Revision 1.23.4.7  2010/04/01 21:03:44  wenger
// Woo hoo!  Display of restraints is partly working!  (We're not yet
// differentiating violated vs. non-violated, ambiguous vs. non-
// ambiguous; and the code needs lots of checking and cleanup.)
//
// Revision 1.23.4.6  2010/03/29 18:17:46  wenger
// Got things to work as an applet with the latest Jmol version -- needed
// some more initialization in creating the JmolViewer object.  Added
// the jsdevisec.pnStackTrace() method, since we can't get a Java
// console with the lab's Firefox setup.
//
// Revision 1.23.4.5  2010/03/24 16:33:18  wenger
// Okay, I think I now have all of the existing functionality (except
// for top, bottom, right, and left) working with the new Jmol.  I'm
// also drawing three "dummy" restraints to test out how they'd look.
//
// Revision 1.23.4.4  2010/03/23 16:43:16  wenger
// Figured out how to draw and erase restraints (I have a couple of fake
// ones in right now); partway through re-creating all of the other JS
// functionality with the new Jmol (I don't have drill-down working yet).
//
// Revision 1.23.4.3  2010/03/19 21:28:14  wenger
// Got selection halos working again with new Jmol version.
//
// Revision 1.23.4.2  2010/03/19 20:42:27  wenger
// We don't seem to need the JmolPopup stuff anymore -- we still get a
// popup menu, even without it.
//
// Revision 1.23.4.1  2010/03/18 17:43:16  wenger
// Updated to latest production Jmol (11.8.21) -- got it to mostly work,
// but a bunch of stuff is commented out now, so things like highlighting
// in the 3D view don't work.
//
// Revision 1.23  2009/06/12 16:34:45  wenger
// Merged s2d_torsion_rest_0905_br_0 thru s2d_torsion_rest_0905_br_js1
// to the trunk (JavaScreen changes only -- mainly to get the fix that
// gets rid of the extra space stuck into atom names in the Atom
// Selection tree).
//
// Revision 1.22.4.1  2009/06/11 20:56:58  wenger
// Made changes to the JavaScreen client to allow highlighting by
// specific atom as well as the existing capability of highlighting
// by residue (this is for the torsion angle restraints visualization).
//
// Revision 1.22  2008/09/05 20:34:45  wenger
// (Hopefully, at least) fininshed JavaScreen changes for multiple
// entity assemblies -- we now send alphabetic chain IDs to Jmol,
// so that we can at least have 26 entity assemblies.
//
// Revision 1.21  2008/09/03 19:15:58  wenger
// Initial changes to JavaScreen client to support entity assembly
// IDs in 3D Jmol visualizations.  (Still needs some cleanup.)
//
// Revision 1.20  2008/08/12 21:16:52  wenger
// Changed "Molecule" to "Assembly" in selection trees; some other
// partially-finished changes related to multiple-entity fixes.
//
// Revision 1.19  2008/02/13 22:38:16  wenger
// Added a button to show the Jmol selection trees without having to
// go into a menu.
//
// Revision 1.18  2007/12/19 00:08:31  wenger
// Changed a bunch of JButton references to DEViseButtons to make things
// more consistent.
//
// Revision 1.17  2007/10/24 17:36:48  wenger
// Minor change to fix null pointer problem that sometimes shows up with
// Jmol visualizations.
//
// Revision 1.16  2007/09/10 22:10:36  wenger
// Mouse cursor now changes to the disabled cursor if the selected
// toolbar mode is not applicable in the current view; mouse cursor
// is default on non-view areas; changed Jmol views to not show the
// non-applicable mouse cursors (this only happened in some JVMs).
//
// Revision 1.15  2007/08/03 20:17:27  wenger
// Merged andyd_gui_br_6 thru andyd_gui_br_7 to trunk.
//
// Revision 1.14  2007/06/27 17:47:58  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.13  2007/04/25 20:43:50  wenger
// Greatly improved the documentation of the DEViseCanvas3DJmol class;
// minor improvements to other classes.
//
// Revision 1.12  2007/04/23 19:41:46  wenger
// Added some extra GUI-thread-related diagnostic output.
//
// Revision 1.11  2007/03/30 17:29:55  wenger
// Moved some GUI functions to the AWT-EventQueue thread (which is where
// they should be) to more correctly fix JavaScreen 5.8.0 lockup problems.
// (Note: there are probably many more that should be fixed.)
//
// Revision 1.10  2007/03/30 15:43:08  wenger
// (Hopefully) cured the lockups we've been seeing with JS 5.8.0 (removed
// a bunch of calls to validate() in the GUI); fixed up the client logging
// functionality somewhat; various improvements to debug output.
//
// Revision 1.9  2007/02/26 20:50:16  wenger
// Structure selection tree window colors now partially match the overall
// JavaScreen colors (I need to figure out how to get them to fully match).
//
// Revision 1.8  2007/02/23 19:21:59  wenger
// Structure selection tree window now comes up closer to the upper
// left corner of the screen.
//
// Revision 1.7.2.3  2007/08/03 19:21:15  wenger
// Mouse cursor now changes according to toolbar mode; fixed existing
// problems with mouse cursor being crosshairs cursor when it should be
// the default cursor; fixed problems with actions sometimes happening
// in the wrong toolbar mode; added "XY zoom in" button.
//
// Revision 1.7.2.2  2007/07/25 18:25:17  wenger
// Moved cursor handling from DEViseUIGlobals to the new
// UI/DEViseMouseCursor class, in preparation for changing the cursor
// according to the toolbar mode.
//
// Revision 1.7.2.1  2007/06/18 19:57:21  wenger
// Toolbar works for drill-down (including Jmol); we go back to "normal"
// mode after drilling down; drill-down in Jmol is now disabled by
// default; removed Jmol menu options to enable/disable drill-down;
// removed unneeded utils stuff from 'make clean' target.
//
// Revision 1.7  2006/12/13 22:42:17  wenger
// Added option to disable Jmol drill down (to avoid accidental drill-
// down); changed "Cancel" to "Close" in the Option dialog.
//
// Revision 1.6  2006/11/30 17:28:34  wenger
// Fixed bug 927 (Jmol double-click also drills down).
//
// Revision 1.5  2006/11/21 21:24:27  wenger
// Jmol drill-down in the JavaScreen mostly implemented (works on single
// click as opposed to shift-click).
//
// Revision 1.4  2006/06/29 21:01:20  wenger
// Merged devise_jmol_br_2 thru devise_jmol_br_3 to the trunk.
//
// Revision 1.3  2006/06/23 19:52:40  wenger
// Merged devise_jmol_br_1 thru devise_jmol_br_2 to the trunk.
//
// Revision 1.2  2006/05/26 16:22:15  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.25  2006/06/29 18:10:12  wenger
// Fixed the bug that moving the data selection cursor reset
// what was *displayed* in Jmol, not just what was highlighted.
//
// Revision 1.1.2.24  2006/06/27 20:49:28  wenger
// Added menu option to highlight by color instead of selection
// halo; slight cleanup of how the Jmol popup menu is generated;
// other minor cleanups.
//
// Revision 1.1.2.23  2006/06/23 18:13:07  wenger
// Minor Jmol-related cleanups.
//
// Revision 1.1.2.22  2006/06/23 17:08:01  wenger
// Implemented setting Jmol menu checkboxes to the right state;
// tried help stuff, but both the HelpDialog and AboutDialog
// generate security violations when running as an applet.
//
// Revision 1.1.2.21  2006/06/19 22:10:02  wenger
// Got Jmol vibration menu items working (committing with a *temporary*
// change to load a molecule that does something when you turn on
// vibration).
//
// Revision 1.1.2.20  2006/06/19 20:32:43  wenger
// Got Jmol animation menu items working (committing with a *temporary*
// change to load a molecule that does something when you run animations).
//
// Revision 1.1.2.19  2006/06/15 19:54:29  wenger
// Most (but not all) Jmol menus in the JS now working; still need to
// update the JS's Jmol menus as the state of Jmol changes, etc.;
// fixed a null pointer exception if closing coordinate session after
// destroying the tree dialog.
//
// Revision 1.1.2.18  2006/06/14 16:32:02  wenger
// Added new DEViseButton class to force the colors and font we want
// for buttons; cleaned up things in jsdevisec (made public members
// private, etc.); started on getting more of the Jmol menus actually
// working.
//
// Revision 1.1.2.17  2006/06/09 19:22:17  wenger
// Went back to using a JmolPanel embedded in the JavaScreen, instead of
// a top-level Jmol object in its own frame (doesn't work in browser because
// of security restrictions).
//
// Revision 1.1.2.16  2006/06/08 19:52:58  wenger
// Initial phase of having Jmol in its own window, mainly so we get the
// menus for free.  Still needs lots of cleanup.
//
// Revision 1.1.2.15  2006/06/06 21:48:19  wenger
// Added Jmol menus to the JavaScreen Jmol menu button (only the View
// menu is functional at this time).
//
// Revision 1.1.2.14  2006/05/26 21:19:23  wenger
// Jmol popup menus now working.
//
// Revision 1.1.2.13  2006/05/24 18:30:32  wenger
// Added button to reset highlight to default (based on data from
// DEVise); improved layout of tree GUI window.
//
// Revision 1.1.2.12  2006/05/23 18:17:49  wenger
// Added initial Jmol menu with a menu item to show the tree selection
// window; destroying and re-creating the window currently doesn't
// preserve the existing selection, although I started on provision for
// that.
//
// Revision 1.1.2.11  2006/05/22 18:59:55  wenger
// We now handle destruction of the Jmol tree window much better.
//
// Revision 1.1.2.10  2006/05/19 16:01:00  wenger
// DEViseCanvas3DJmol.nodesSelected now pretty much returns
// immediately, and the real work is done in DEViseCanvas3DJmol.paint(),
// so that when you select tree node(s) that shows up immediately.
// Also got the wait cursor to work in the tree frame, but not in
// the main frame for some reason.
//
// Revision 1.1.2.9  2006/05/18 20:49:00  wenger
// Added backbone/side chains/protons selection tree.
//
// Revision 1.1.2.8  2006/05/16 15:02:39  wenger
// Fixed (I think) tree GUI problems on Mozilla (really, pre-1.5 JVMs)
// (the tree GUI window was not showing up).
//
// Revision 1.1.2.7  2006/05/12 20:15:39  wenger
// Both structure trees are now in a single window.
//
// Revision 1.1.2.6  2006/05/12 14:50:26  wenger
// Now have two trees for a Jmol view: the first one selects which
// atoms are shown in the Jmol view and in the second tree; the
// second tree selects which atoms are highlighted in the Jmol view.
//
// Revision 1.1.2.5  2006/05/08 20:19:47  wenger
// Made the JavaScreen tree code itself generic (although the Jmol-
// related code that calls it is not).
//
// Revision 1.1.2.4  2006/04/14 15:27:17  wenger
// Atom selection GUI now feeds back into Jmol; default selection
// from DEVise data view is now fed into the atom selection GUI;
// selecting entire molecule works.  We still need a few more
// features.
//
// Revision 1.1.2.3  2006/04/11 19:43:21  wenger
// Atom selection tree GUI is in place, molecule is fed into it.
// Still needs the default *selection* fed into it, a way to get
// the selection back from the tree GUI to Jmol.
//
// Revision 1.1.2.2  2006/03/31 22:41:14  wenger
// Finished splitting up DEViseCanvas class.
//
// Revision 1.1.2.1  2006/03/30 20:51:26  wenger
// Partially done splitting up the DEViseCanvas class.
//

// ========================================================================

// DEViseCanvas3DJmol.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import  java.awt.*;
import  java.util.*;
import java.awt.event.*;

// Some Swing stuff needed for Jmol.
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSplitPane;
import javax.swing.JLabel;
import javax.swing.BoxLayout;
import javax.swing.Box;
import javax.swing.SwingUtilities;

// Import the Jmol stuff we need.
import org.jmol.api.*;
import org.jmol.adapter.smarter.SmarterJmolAdapter;
import org.jmol.api.JmolAdapter;
import org.jmol.api.JmolViewer;
import org.jmol.api.JmolStatusListener;
import org.jmol.viewer.JmolConstants;

public class DEViseCanvas3DJmol extends DEViseCanvas3D implements
  DEViseGenericTreeSelectionCallback
{
    // The panel holding the actual Jmol viewer.
    private JmolPanel jmolPanel = null;

    private class JmolState {
        public JmolPanel jmolPanel;
    }

    // The frame holding the Atom Display and Atom Selection trees.
    private JFrame treeFrame;

    private JmolTree structDisplayTree; // select what structure(s) to display
    private final String STRUCTURE_TREE_NAME = "Structure Display";

    private JmolTree atomDisplayTree; // select what to display
    private final String DISPLAY_TREE_NAME = "Atom Display";

    private JmolTree highlightTree; // select what to highlight
    private final String HIGHLIGHT_TREE_NAME = "Atom Selection";

    private static final int DEBUG = 0; // 0 - 3

    private static final String BACKBONE_STR = "backbone";
    private static final String SIDE_CHAINS_STR = "side chains";
    private static final String PROTONS_STR = "side chain protons";

    // Whether the secondary structure (molecule, backbone, side chains,
    // protons) to display has been updated since the last repaint.
    private boolean structUpdated = false;

    // The secondary structure(s) to display.
    private Vector structDisplayNodes = null;

    // Whether the atoms to display have been updated since the last repaint.
    private boolean atomUpdated = false;

    // The atoms to display.
    private Vector atomDisplayNodes = null;

    // Whether the atoms to highlight have bee updated since the last repaint.
    private boolean highlightUpdated = false;

    // The atoms to highlight.
    private Vector highlightNodes = null;

    private JmolViewer viewer;
    private MyCallbackListener myCallbackListener;

    // True if atoms should be highlighted with halos, false if they
    // should be updated by changing color.
    private boolean highlightWithHalos = true;

    // The atoms to display, in the form of DEViseGData.
    private Vector gDatasToDisplay;

    private Hashtable atomId2AtomNum;

    class DistRestraint {
        public int atom1Num;
	public int atom2Num;
	public boolean isViolated;
	public boolean isAmbiguous;
    };

    // Vector of DistRestraint objects.
    private Vector distRestraints;

    // Whether the distance restraints have been updated since the last
    // repaint.
    private boolean distRestUpdated = false;

    // True if drill-down is enabled in Jmol, false otherwise.
    private boolean drillDownEnabled = false;

    // Whether we've set Jmol to wireframe style since the last time
    // we loaded atoms into it.
    private boolean setToWireframe = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Constructor.
     * @param v: the base view for this canvas.
     * @param img: the background image for this canvas.
     */
    // v is base view if there is a pile in this canvas.
    public DEViseCanvas3DJmol (DEViseView v, Image img)
    {
	super(v, img);
	jsc.showJmol(this);
    }

    //-------------------------------------------------------------------
    /**
     * Paint this canvas.
     * @param gc: the graphics context to paint on.
     */
    public void paint(Graphics gc)
    {
	if (structUpdated) {
	    structureSelected(structDisplayNodes);
	    structUpdated = false;
	}

	if (atomUpdated) {
	    displaySelected(atomDisplayNodes);
	    atomUpdated = false;
	}

	if (highlightUpdated) {
	    highlightSelected(highlightNodes);
	    highlightUpdated = false;
	}

	if (distRestUpdated) {
	    drawRestraints();
	    distRestUpdated = false;
	}

        //TEMP? jsc.parentFrame.setCursor(jsc.mouseCursor.defaultCursor);
        if (treeFrame != null) {
	    treeFrame.setCursor(jsc.mouseCursor.defaultCursor);
        }

    	super.paint(gc);
    }

    //-------------------------------------------------------------------
    /**
     * Called when a session is closed -- destroys the tree selection
     * window.
     */
    public void close()
    {
    	Runnable doClose = new Runnable() {
	    public void run() {
	        if (treeFrame != null) treeFrame.dispose();
	        treeFrame = null;
	        jsc.hideJmol();
	    }
	};

	SwingUtilities.invokeLater(doClose);
    }

    //-------------------------------------------------------------------
    /**
     * Actually creates the Jmol viewer and tree selection window.
     * There's some reason this isn't done in the constructor, but I
     * don't remember exactly what it is.  wenger 2007-04-24.
     */
    public void create3DViewer()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseCanvas3DJmol.create3DViewer()");
	}

        if (view.viewGDatas.size() == 0) {
            return;
        }

    	createTrees();
    	createJmol();
    }

    //-------------------------------------------------------------------
    public void clear3DViewer()
    {
    }

    //-------------------------------------------------------------------
    public JmolViewer get3DViewer()
    {
        return viewer;
    }

    //-------------------------------------------------------------------
    // Save the Jmol state for this canvas (part of bug 1005 fix).
    public Object saveJmolState()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.saveJmol()");
	}

	// We need to do this to *reliably* save the orientation and
	// zoom of the molecule.
	//TEMP -- do we need semicolon at end below?
	jmolEvalStringErr(viewer, "save state saved1");

	JmolState jmolState = new JmolState();
	jmolState.jmolPanel = jmolPanel;

	return jmolState;
    }

    //-------------------------------------------------------------------
    // Restore the Jmol state for this canvas (part of bug 1005 fix).
    public void restoreJmolState(Object jmState)
    {
    	if (DEBUG >= 2) {
	    System.out.println(Thread.currentThread() + ":");
	    System.out.println("  DEViseCanvas3DJmol.restoreJmol()");
	}

	if (jmState == null) {
	    return;
	}

	if (!(jmState instanceof JmolState)) {
	    System.err.println("Error: DEViseCanvas3DJmol.restoreJmol() " +
	      "called with an object that is not a JmolState!");
	}

	JmolState jmolState = (JmolState)jmState;
        Runnable doRestore = new DoRestoreJmol(jmolState);

        SwingUtilities.invokeLater(doRestore);
    }

    //-------------------------------------------------------------------
    // This class is used to allow us to actually do the restoring
    // of Jmol state in the event dispatched thread -- fixes problems
    // I ran into with things locking up on Windows.
    private class DoRestoreJmol implements Runnable {
        private JmolState _jmolState;

	DoRestoreJmol(JmolState jmolState) {
	    _jmolState = jmolState;
	}

	public void run() {
	    if (_jmolState.jmolPanel != null) {
	        if (jmolPanel != null) {
	            remove(jmolPanel);
	            jmolPanel = null;
	        }

	        setJmolPanel(_jmolState.jmolPanel);

	        // We need to do this to *reliably* restore the orientation and
	        // zoom of the molecule.
	        //TEMP -- do we need semicolon at end below?
	        jmolEvalStringErr(viewer, "restore state saved1");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Restart Jmol -- this allows the user to reset Jmol's state to
    // the default, in case we get goofed up trying to preserve the Jmol
    // state.
    public void restartJmol()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.restartJmol()");
	}

	if (jmolPanel != null) {
	    remove(jmolPanel);
	    jmolPanel = null;
	}
	createJmol();
	structUpdated = true;
	repaint();
    }

    //-------------------------------------------------------------------
    /**
     * Force the window containing the selection trees to be shown.
     */
    public void showTrees()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.showTrees()");
	}
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseCanvas3DJmol.showTrees()");
	}

	if (treeFrame == null) {
    	    createTrees();
	    updateStructTree();
	}

	treeFrame.setExtendedState(Frame.NORMAL); // de-iconify
	// The line below seems to partially fix bug 957 (de-iconifies
	// it, but doesn't bring it to the front.
	treeFrame.setState(Frame.NORMAL);
	treeFrame.toFront();
    }

    //-------------------------------------------------------------------
    /**
     * Deal with selection in one of the trees controlling this canvas.
     * This method is called whenever the user selects a node in one
     * of the selection trees controlling this canvas.
     * @param name: the name of the tree in which the selection occurred.
     * @param nodes: the list of selected nodes
     */
    public void nodesSelected(String name, Vector nodes)
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.nodesSelected(" +
	      name +  ")");
    	    if (DEBUG >= 3) {
	        System.out.println("  " + nodes);
	    }
            if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	      (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	      !SwingUtilities.isEventDispatchThread())) {
	        System.out.println(Thread.currentThread() +
	        " calls DEViseCanvas3DJmol.nodesSelected()");
	        if (!SwingUtilities.isEventDispatchThread()) {
		    System.out.println("  Warning: not event " +
		      "dispatched thread!");
		}
	    }
	}

	//TEMP? jsc.parentFrame.setCursor(jsc.mouseCursor.waitCursor);
	treeFrame.setCursor(jsc.mouseCursor.waitCursor);

	if (name.equals(HIGHLIGHT_TREE_NAME)) {
	    highlightNodes = nodes;
	    highlightUpdated = true;
	} else if (name.equals(DISPLAY_TREE_NAME)) {
	    atomDisplayNodes = nodes;
	    atomUpdated = true;
	} else if (name.equals(STRUCTURE_TREE_NAME)) {
	    structDisplayNodes = nodes;
	    structUpdated = true;
	} else {
	    System.err.println("Unexpected tree name " + name +
	      " in DEViseCanvas3DJmol.nodesSelected()");
	}

	repaint();
    }

    //-------------------------------------------------------------------
    /**
     * Reset the highlighted atoms to those selected by the data, as
     * opposed to those selected in the highlight tree.
     */
    public void resetSelection()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.resetSelection()");
	}
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseCanvas3DJmol.resetSelection()");
	}

	setHighlightFromData();
    }

    //-------------------------------------------------------------------
    public void front()
    {
    	viewer.rotateFront();
    }

/* No longer supported by newer Jmol version (11.8.21).
    //-------------------------------------------------------------------
    public void top()
    {
    	viewer.rotateToX(90);
    }

    //-------------------------------------------------------------------
    public void bottom()
    {
    	viewer.rotateToX(-90);
    }

    //-------------------------------------------------------------------
    public void right()
    {
	viewer.rotateToY(90);
    }

    //-------------------------------------------------------------------
    public void left()
    {
	viewer.rotateToY(-90);
    }
*/

    //-------------------------------------------------------------------
    public void defineCenter()
    {
	viewer.setCenterSelected();
	viewer.setModeMouse(JmolConstants.MOUSE_ROTATE);
	viewer.setSelectionHalos(false);
    }

    //-------------------------------------------------------------------
    /**
     * Controls whether atoms are highlighted by surrounding them with
     * a halo or by changing their color.
     * @param halosOn: true turns on halos, false turns on highlighting
     * by changing atom color
     */
    public void setHighlightWithHalos(boolean halosOn)
    {
	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.setHighlightWithHalos("
	      + halosOn + ")");
	}

        if (halosOn != highlightWithHalos) {
	    if (halosOn) {
	        viewer.setSelectionHalos(true);
		// Note: we're assuming here that the "normal" color is
		// CPK; that may not always be true, but I don't know
		// what else to do for now.  wenger 2006-06027.
		//TEMP -- do we need semicolon at end below?
                jmolEvalStringErr(viewer, "color atoms CPK");
	    } else {
	        viewer.setSelectionHalos(false);
		//TEMP -- do we need semicolon at end below?
                jmolEvalStringErr(viewer, "color atoms lime");
	    }
	    highlightWithHalos = halosOn;
	}
    }

    //-------------------------------------------------------------------
    /**
     * Evaluate the given string with the given Jmol viewer, and report
     * any error that results.
     * @param viewer: the Jmol viewer
     * @param dataStr: the string to open
     */
    public static void jmolEvalStringErr(JmolViewer viewer, String script)
    {
	if (DEBUG >= 3) {
	    System.out.print(Thread.currentThread() + ": ");
	    System.out.println("DEViseCanvas3DJmol.jmolEvalStringErr(" +
	      script + ")");
	    //jsc.pn("DEViseCanvas3DJmol.jmolEvalStringErr(" + script + ")");
	}
        String errStr = viewer.scriptWait(script);
	if (errStr != null && errStr.indexOf("ERROR") != -1) {
	    System.err.println("ERROR: Jmol error evaluating script <" +
	      script + ">: " + errStr);
	}
    }

    //-------------------------------------------------------------------
    /**
     * Enable drill-down in the Jmol viewer.
     */
    public void enableDrillDown()
    {
    	drillDownEnabled = true;
    }

    //-------------------------------------------------------------------
    /**
     * Disable drill-down in the Jmol viewer.
     */
    public void disableDrillDown()
    {
    	drillDownEnabled = false;
    }

    //-------------------------------------------------------------------
    /**
     * Override DEViseCanvas so we don't get non-Jmol-applicable mouse
     * cursors here (behavior depends on JVM).
     */
    public synchronized void checkMousePos(Point mouseLoc,
      boolean checkDispatcher) throws YError
    {
        if (DEBUG >= 3) {
            System.out.println("DEViseCanvas(" + view.viewName +
	      ").checkMousePos(" + mouseLoc.x + ", " + mouseLoc.y + ")");
        }
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    /**
     * Create the trees for selecting which atoms are displayed, and
     * which atoms are highlighted, in Jmol.
     */
    protected void createTrees()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseCanvas3DJmol.createTrees()");
	}

	//TEMP -- the whole tree window should probably be its own class
        if (treeFrame == null) {
	    //
	    // Create the tree GUI.
	    //
	    JLabel displayLabel = new JLabel(DISPLAY_TREE_NAME);
	    displayLabel.setBackground(jsc.jsValues.uiglobals.bg);
	    displayLabel.setForeground(jsc.jsValues.uiglobals.fg);
	    displayLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

	    JLabel highlightLabel = new JLabel(HIGHLIGHT_TREE_NAME);
	    highlightLabel.setBackground(jsc.jsValues.uiglobals.bg);
	    highlightLabel.setForeground(jsc.jsValues.uiglobals.fg);
	    highlightLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

	    structDisplayTree = new JmolTree(STRUCTURE_TREE_NAME, this);

	    atomDisplayTree = new JmolTree(DISPLAY_TREE_NAME, this);

	    highlightTree = new JmolTree(HIGHLIGHT_TREE_NAME, this);
	    highlightTree.tree.getParentComponent().setAlignmentX(
	      Component.CENTER_ALIGNMENT);

	    Dimension dim = structDisplayTree.tree.getParentComponent().
	      getPreferredSize();
	    dim.height = 90;
	    structDisplayTree.tree.getParentComponent().setPreferredSize(dim);

	    JPanel structDispPanel = new JPanel();
	    structDispPanel.setLayout(new BoxLayout(structDispPanel,
	      BoxLayout.Y_AXIS));
	    structDispPanel.add(structDisplayTree.tree.getParentComponent());

	    JPanel atomDispPanel = new JPanel();
	    atomDispPanel.setLayout(new BoxLayout(atomDispPanel,
	      BoxLayout.Y_AXIS));
	    atomDispPanel.add(atomDisplayTree.tree.getParentComponent());

	    JSplitPane structAtomSplit = new JSplitPane(
	      JSplitPane.VERTICAL_SPLIT);
	    structAtomSplit.setAlignmentX(Component.CENTER_ALIGNMENT);
	    structAtomSplit.setTopComponent(structDispPanel);
	    structAtomSplit.setBottomComponent(atomDispPanel);

	    JPanel displayPanel = new JPanel();
	    displayPanel.setBackground(jsc.jsValues.uiglobals.bg);
	    displayPanel.setForeground(jsc.jsValues.uiglobals.fg);
	    displayPanel.setLayout(new BoxLayout(displayPanel,
	      BoxLayout.Y_AXIS));
	    displayPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    displayPanel.add(displayLabel);
	    displayPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    displayPanel.add(structAtomSplit);
	    displayPanel.add(Box.createRigidArea(new Dimension(0, 5)));

	    //TEMP -- think of a clearer name for this button?
            DEViseButton highlightResetButton =
	      new DEViseButton("Reset", jsc.jsValues);
	    highlightResetButton.setAlignmentX(Component.CENTER_ALIGNMENT);
	    highlightResetButton.addActionListener(new ActionListener()
	        {
		    public void actionPerformed(ActionEvent event)
		    {
			resetSelection();
		    }
		});

	    JPanel highlightPanel = new JPanel();
	    highlightPanel.setLayout(new BoxLayout(highlightPanel,
	      BoxLayout.Y_AXIS));
	    highlightPanel.setBackground(jsc.jsValues.uiglobals.bg);
	    highlightPanel.setForeground(jsc.jsValues.uiglobals.fg);
	    highlightPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    highlightPanel.add(highlightLabel);
	    highlightPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    highlightPanel.add(highlightTree.tree.getParentComponent());
	    highlightPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    highlightPanel.add(highlightResetButton);
	    highlightPanel.add(Box.createRigidArea(new Dimension(0, 5)));

	    JSplitPane dispHighSplit = new JSplitPane(
	      JSplitPane.HORIZONTAL_SPLIT);
	    dispHighSplit.setLeftComponent(displayPanel);
	    dispHighSplit.setRightComponent(highlightPanel);

	    treeFrame = new JFrame("Structure Selection");
	    treeFrame.setBackground(jsc.jsValues.uiglobals.bg);
	    treeFrame.setForeground(jsc.jsValues.uiglobals.fg);
	    treeFrame.setLocation(10, 30);
	    treeFrame.addWindowListener(new WindowAdapter() {
	    	public void windowClosing(WindowEvent we) {
		    treeFrame = null;
		}
	      });
	    treeFrame.getContentPane().add(dispHighSplit);
	    treeFrame.pack();
	    treeFrame.setVisible(true);
	}
    }

    //-------------------------------------------------------------------
    /**
     * Create the Jmol viewer and its enclosing panel.
     */
    protected void createJmol()
    {
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseCanvas3DJmol.createJmol()");
	    if (!SwingUtilities.isEventDispatchThread()) {
	        System.out.println("  Warning: not event " +
		  "dispatched thread!");
	    }
	}

        if (jmolPanel == null) {
	    //
	    // Create the actual Jmol panel.
	    //
	    setJmolPanel(new JmolPanel(jsc));

	    setToWireframe = false;
	}

	// Doing updateStructTree() only when the base view's GData is
	// dirty fixes the problem of moving the data cursor resetting
	// what's displayed, not just what is highlighted.  We're ignoring
	// the dirty flag of the highlight view because we shouldn't even
	// get here if at least that GData isn't dirty.
	if (view._gdataIsDirty) {
	    updateStructTree();
	    view._gdataIsDirty = false;
	} else {
	    setHighlightFromData();
	}
    }

    //-------------------------------------------------------------------
    /**
     * Set the given JmolPanel to be the JmolPanel of this canvas,
     * and set up callbacks accordingly.
     * @param The JmolPanel.
     */
    protected void setJmolPanel(JmolPanel jmPanel)
    {
        jmolPanel = jmPanel;
        add(jmolPanel);
        jmolPanel.setSize(canvasDim.width, canvasDim.height);
        jmolPanel.setVisible(true);

        viewer = jmolPanel.getViewer();
        myCallbackListener = new MyCallbackListener();
	viewer.setJmolCallbackListener(myCallbackListener);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // This class adapts a DEViseGenericTree to the specific requirements
    // of controlling this canvas.
    private class JmolTree
    {
        public final String name;
        public DEViseGenericTree tree;
	public int maxEntityAssemblyID;
	public Hashtable entityAssemblyNodes;
	public DEViseGenericTreeNode topNode;

	public JmolTree(String treeName, DEViseCanvas3DJmol canvas) {
	    name = treeName;
	    topNode = new TreeMoleculeNode();
	    tree = new DEViseGenericTree(name, topNode, canvas);
	    maxEntityAssemblyID = -1;
	}
    }

    //-------------------------------------------------------------------
    // Specializes a DEViseGenericTreeNode to represent a molecule.
    private class TreeMoleculeNode extends DEViseGenericTreeNode
    {
	public TreeMoleculeNode()
	{
	    super("Assembly");
	}
    }

    //-------------------------------------------------------------------
    // Specializes a DEViseGenericTreeNode to represent an entity assembly.
    private class TreeEntityAssemblyNode extends DEViseGenericTreeNode
    {
	public int entityAssemblyID;
        public int maxResidueNum;
	public Hashtable residueNodes;

	public TreeEntityAssemblyNode(int id)
	{
	    super("Entity assembly " + id);

	    if (DEBUG >= 3) {
	        System.out.println(
		  "TreeEntityAssemblyNode.TreeEntityAssemblyNode(" +
		  id + ")");
	    }
	    entityAssemblyID = id;

	    maxResidueNum = -1;
	    residueNodes = new Hashtable();
	}
    }

    //-------------------------------------------------------------------
    // Specializes a DEViseGenericTreeNode to represent a residue.
    private class TreeResidueNode extends DEViseGenericTreeNode
    {
	public int entityAssemblyID; // used to select things properly in Jmol
	public int residueNumber;
	public String residueLabel;
	public Hashtable atomNodes;

	public TreeResidueNode(int number, String label, int entAssemID)
	{
	    super("[" + label + "] " + number);

	    if (DEBUG >= 3) {
	        System.out.println("TreeResidueNode.TreeResidueNode("
		  + number + ", " + label + ")");
	    }
	    residueNumber = number;
	    residueLabel = label;
	    entityAssemblyID = entAssemID;
	    atomNodes = new Hashtable();
	}
    }

    //-------------------------------------------------------------------
    // Specializes a DEViseGenericTreeNode to represent an atom.
    private class TreeAtomNode extends DEViseGenericTreeNode
    {
	public int atomNumber;
	public String atomName;
	public DEViseGData gData;

	public TreeAtomNode(int number, String name, DEViseGData gd)
	{
	    super(name);

	    if (DEBUG >= 3) {
	        System.out.println("TreeAtomNode.TreeAtomNode(" +
		  number + ", " + name + ")");
	    }
	    atomNumber = number;
	    atomName = name;
	    gData = gd;
	}
    }

    //-------------------------------------------------------------------
    /**
     * Open the given string with the given Jmol viewer, and report
     * any error that results.
     * @param viewer: the Jmol viewer
     * @param dataStr: the string to open
     */
    private static void jmolOpenStringErr(JmolViewer viewer, String dataStr)
    {
	if (DEBUG >= 3) {
	    System.out.println("DEViseCanvas3DJmol.jmolOpenStringErr(" +
	      dataStr + ")");
	}
	String errStr = viewer.openStringInline(dataStr);
	if (errStr != null) {
	    System.err.println("ERROR: Jmol error opening data string: " +
	      errStr);
	}
    }

    //-------------------------------------------------------------------
    /**
     * Update what's shown in the Structure Display tree.  (This shows
     * a fixed list of the different secondary structures.)
     */
    void updateStructTree()
    {
	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.updateStructTree()");
	}

	structDisplayTree.topNode.children.removeAllElements();

	Vector selectedNodes = new Vector();

	DEViseGenericTreeNode backboneNode =
	  new DEViseGenericTreeNode(BACKBONE_STR);
	structDisplayTree.topNode.addChild(backboneNode);
	selectedNodes.addElement(backboneNode);

	DEViseGenericTreeNode sideChainNode =
	  new DEViseGenericTreeNode(SIDE_CHAINS_STR);
	structDisplayTree.topNode.addChild(sideChainNode);
	selectedNodes.addElement(sideChainNode);

	DEViseGenericTreeNode protonNode =
	  new DEViseGenericTreeNode(PROTONS_STR);
	structDisplayTree.topNode.addChild(protonNode);
	if (view.viewName.indexOf("wireframe") >= 0) {
	    selectedNodes.addElement(protonNode);
	}

        structDisplayTree.tree.updateTree(structDisplayTree.topNode);
	structDisplayTree.tree.setSelection(selectedNodes);
    }

    //-------------------------------------------------------------------
    /**
     * Update the given JmolTree by replacing its nodes with nodes
     * generated from the given GData.  Note that the input GData
     * is "flat" -- it's simply a list of all atoms, no matter which
     * residue they're in.  We generate a hierarchical structure of
     * residues containing atoms in this method.
     * @param jmTree: the tree to update
     * @param gDatas: the GData to use to generate the new nodes;
     *   each GData record must represent an atom
     */
    void updateTreeData(JmolTree jmTree, Vector gDatas)
    {
	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.updateTreeData(" +
	      jmTree.name + ", " + gDatas.size() + ")");
	}

	jmTree.entityAssemblyNodes = new Hashtable();
	jmTree.topNode.children.removeAllElements();

	int maxResidueNum = -1;

	// A lot of the complexity here is because we're not guaranteed
	// to get atoms in order of ascending entity assembly ID and
	// residue number within an entity assembly.  So we initially
	// stick things into hash tables, and then go back and pull
	// them out of the hash tables in numerical order, and stick
	// them into the tree nodes.
	for (int atomNum = 0; atomNum < gDatas.size(); atomNum++) {
	    DEViseGData gd = (DEViseGData)gDatas.elementAt(atomNum);

	    if (gd.symbolType != DEViseGData._symOval) {
	    	System.err.println("ERROR: illegal symbol type (" +
		  gd.symbolType + ") for Jmol view");
	        continue;
	    }

	    if (gd.entityAssemblyID > jmTree.maxEntityAssemblyID) {
	        jmTree.maxEntityAssemblyID = gd.entityAssemblyID;
	    }

	    // Find or construct the node for the appropriate entity assembly.
	    Integer entityAssemblyID = new Integer(gd.entityAssemblyID);
	    TreeEntityAssemblyNode entityAssemblyNode =
	      (TreeEntityAssemblyNode)jmTree.entityAssemblyNodes.get(
	        entityAssemblyID);
	    if (entityAssemblyNode == null) {
		entityAssemblyNode = new TreeEntityAssemblyNode(
		  gd.entityAssemblyID);
		jmTree.entityAssemblyNodes.put(entityAssemblyID,
		  entityAssemblyNode);
	    }

	    if (gd.residueNum > entityAssemblyNode.maxResidueNum) {
	        entityAssemblyNode.maxResidueNum = gd.residueNum;
	    }

	    // Find or construct the node for the appropriate residue.
	    Integer resNum = new Integer(gd.residueNum);
	    TreeResidueNode residueNode =
	      (TreeResidueNode)entityAssemblyNode.residueNodes.get(resNum);
	    if (residueNode == null) {
		residueNode = new TreeResidueNode(gd.residueNum,
		  gd.residueLabel, gd.entityAssemblyID);
		entityAssemblyNode.residueNodes.put(resNum, residueNode);
	    }

	    // Add this atom to the residue node.
	    //TEMP -- do the atoms need to be in a certain order within a residue?
	    TreeAtomNode atomNode = new TreeAtomNode(gd.atomNum, gd.atomName,
	      gd);
	    residueNode.addChild(atomNode);
	    residueNode.atomNodes.put(gd.atomName, atomNode);
        }

	// Now add the entity assembly and residue nodes to the tree.
	for (int entAssemID = 1; entAssemID <= jmTree.maxEntityAssemblyID;
	  entAssemID++) {
	    Integer entityAssemblyID = new Integer(entAssemID);
	    TreeEntityAssemblyNode entityAssemblyNode =
	      (TreeEntityAssemblyNode)jmTree.entityAssemblyNodes.get(
	        entityAssemblyID);
	    if (entityAssemblyNode != null) {
	        // Add residues to entity assembly node.
	        for (int residueNum = 1;
		  residueNum <= entityAssemblyNode.maxResidueNum;
		  residueNum++) {
	            Integer resNum = new Integer(residueNum);
	            TreeResidueNode residueNode =
	              (TreeResidueNode)entityAssemblyNode.residueNodes.get(
		        resNum);
	            if (residueNode != null) {
	                entityAssemblyNode.addChild(residueNode);
	            }
	        }

		// Add entity assembly node to tree.
		jmTree.topNode.addChild(entityAssemblyNode);
	    }
	}

        jmTree.tree.updateTree(jmTree.topNode);
    }

    //-------------------------------------------------------------------
    /**
     * Visually highlight the selected atoms in the Jmol viewer.
     * (This is called from the paint() method.)
     * @param nodes: a Vector describing what to highlight -- it can be
     *   the entire molecule or a combination of residues and atoms.
     */
    private void highlightSelected(Vector nodes)
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.highlightSelected()");
	}

	String selection = "";
	String selectCmd = "";

	boolean isFirst = true;

	for (int index = 0; index < nodes.size(); index++) {
	    Object node = nodes.elementAt(index);
    	    if (DEBUG >= 3) {
	        System.out.println("  DevNode: " + node);
	    }

	    if (node instanceof TreeMoleculeNode) {
	        selectCmd = "select ;";
		break;

	    } else if (node instanceof TreeEntityAssemblyNode) {
		int entAssemID =
		  ((TreeEntityAssemblyNode)node).entityAssemblyID;
		String chain = DEViseJmolData.entAssem2Chain(entAssemID);
	        if (!isFirst) {
	            selection += ",";
	        }
	        selection += "*:" + chain;

	    } else if (node instanceof TreeResidueNode) {
	        int resNum = ((TreeResidueNode)node).residueNumber;
	        int entAssemID = ((TreeResidueNode)node).entityAssemblyID;
		String chain = DEViseJmolData.entAssem2Chain(entAssemID);
	        if (!isFirst) {
	            selection += ",";
	        }
	        selection += resNum;
		selection += ":" + chain;

	    } else if (node instanceof TreeAtomNode) {
	        int atomNum = ((TreeAtomNode)node).atomNumber;
	        if (!isFirst) {
	            selection += ",";
	        }
	        selection += "atomno=" + atomNum;
	    }

	    isFirst = false;
	}

	if (selectCmd.equals("")) {
	    if (selection.equals("")) {
	        // Note: just plain "select" selects everything!
	        selectCmd = "select 0;";
	    } else {
	        selectCmd = "select " + selection + ";";
	    }
        }

	if (!highlightWithHalos) {
	    // Note: combining everything into one big command here, because
	    // somehow it seems like only one command can be given to the
	    // JmolViewer between repaints or something -- otherwise, only
	    // the last command of any set seems to have any effect.
	    // wenger 2006-06-27.

	    // Unhighlight the currently-highlighted stuff.
	    selectCmd = "color atoms CPK; " + selectCmd;

	    // Now highlight what we are selecting this time.
	    selectCmd += " color atoms lime;";
	}

        jmolEvalStringErr(viewer, selectCmd);

	if (highlightWithHalos) {
	    //TEMP -- do we need semicolon at end below?
       	    jmolEvalStringErr(viewer, "color selectionHalos limegreen");
	    viewer.setSelectionHalos(true);
	}
    }

    //-------------------------------------------------------------------
    private void drawRestraints()
    {
	if (view.viewName.indexOf("wireframe") >= 0 && !setToWireframe) {
	    //TEMP -- do we need semicolon at end below?
            jmolEvalStringErr(viewer, "select; wireframe only; select 0");
	    setToWireframe = true;
	}

	// Delete any restraints we drew previously.
	//TEMP -- do we need semicolon at end below?
        jmolEvalStringErr(viewer, "draw delete");

	if (distRestraints != null) {
	    for (int index = 0; index < distRestraints.size(); index++) {
	        DistRestraint restraint =
		  (DistRestraint)distRestraints.elementAt(index);
		String color = getDistRestColor(restraint);
	    	String cmd = "draw myrestraint" + index;
		cmd += " diameter 0.05 {atomno=" + restraint.atom1Num + "}";
		cmd += " {atomno=" + restraint.atom2Num + "}";
		cmd += " color " + color;
		if (DEBUG >= 3) {
                    System.out.println("Restraint cmd: <" + cmd + ">");
		}
		//TEMP -- do we need semicolon at end below?
	        jmolEvalStringErr(viewer, cmd);
	    }
	}
    }

    //-------------------------------------------------------------------
    /**
     * Display the data selected by the Structure Display and Atom
     * Display trees.  (This method is called from paint() when the
     * atoms to display have changed since the last repaint.)  This method
     * updates both what's shown in the Jmol viewer and what's shown
     * in the Atom Selection tree.
     * @param nodes: a Vector of DEViseGenericTreeNodes that represent
     *   the data to display
     */
    private void displaySelected(Vector nodes)
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.displaySelected()");
	}

	//TEMP -- set busy cursor here?
	// Turn the list of nodes into a list of GData to display.
	gDatasToDisplay = new Vector();

	for (int index = 0; index < nodes.size(); index++) {
	    DEViseGenericTreeNode node =
	      (DEViseGenericTreeNode)nodes.elementAt(index);
	    addNodeGData(node, gDatasToDisplay);
	}

	// Update what's shown in Jmol.
	String jmolData = DEViseJmolData.createJmolData(gDatasToDisplay);
	if (!jmolData.equals("") && viewer != null) {
	    viewer.setSelectionHalos(false);
	    jmolOpenStringErr(viewer, jmolData);
            viewer.setShowHydrogens(true);
            viewer.setShowAxes(true);
	}

	// Update what's shown in the Atom Selection tree.
	updateTreeData(highlightTree, gDatasToDisplay);

	// Set what's selected in the Atom Selection tree according
	// to the highlight data we got from DEVise.
	setHighlightFromData();

	setToWireframe = false;
    }

    //-------------------------------------------------------------------
    /**
     * Deal with a change to the secondary structure(s) to display.  (This
     * method is called from paint() when the secondary structure(s) to
     * display have changed since the last repaint.)  This method updates
     * what's shown in the Atom Display tree according to the secondary
     * structure(s) we want to show.
     * @param nodes: a Vector of DEViseGenericTreeNodes that represent
     *   the secondary structure(s) to show
     */
    private void structureSelected(Vector nodes)
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.structureSelected()");
	}

	boolean showBackbone = false;
	boolean showSideChains = false;
	boolean showProtons = false;

	for (int index = 0; index < nodes.size(); index++) {
	    DEViseGenericTreeNode node =
	      (DEViseGenericTreeNode)nodes.elementAt(index);
	    if (node instanceof TreeMoleculeNode) {
	    	showBackbone = true;
	    	showSideChains = true;
	    	showProtons = true;
		break;
	    } else if (node.name.equals(BACKBONE_STR)) {
	    	showBackbone = true;
	    } else if (node.name.equals(SIDE_CHAINS_STR)) {
	    	showSideChains = true;
	    } else if (node.name.equals(PROTONS_STR)) {
	    	showProtons = true;
	    }
	}

	Vector gDatasToDisplay = new Vector();

	for (int index = 0; index < view.viewGDatas.size(); index++) {
	    DEViseGData gdata = (DEViseGData)view.viewGDatas.elementAt(index);
	    // Note: strings here ("backbone", "side_chains", and
	    // "all_hydrogens") must match the strings in S2DAtomicCoords.java
	    // in the Peptide-CGI source.  wenger 2006-05-18
	    if (showBackbone && gdata.structType.equals("backbone")) {
	        gDatasToDisplay.addElement(gdata);
	    } else if (showSideChains && gdata.structType.equals("side_chains")) {
	        gDatasToDisplay.addElement(gdata);
	    } else if (showProtons && gdata.structType.equals("all_hydrogens")) {
	        gDatasToDisplay.addElement(gdata);
	    }
	}

	// Save atom ID string to atom number mapping.  We will use
	// this to tell Jmol which atoms to connect for distance
	// restraints.
	atomId2AtomNum = new Hashtable();
        for (int index = 0; index < gDatasToDisplay.size(); index++) {
	    DEViseGData gdata = (DEViseGData)gDatasToDisplay.elementAt(index);
	    atomId2AtomNum.put(gdata.atomId, new Integer(gdata.atomNum));
	}

	updateTreeData(atomDisplayTree, gDatasToDisplay);
	atomDisplayTree.tree.selectTop();
    }

    //-------------------------------------------------------------------
    /**
     * Add the data represented by node to the given GData.  Note that
     * if node does not represent an atom, we will recursively call this
     * method until we get down to the atom level.
     * @param node: the tree node representing the data that we want
     *   to add to the GData
     * @param gDatas: the GData to add to
     */
    private void addNodeGData(DEViseGenericTreeNode node, Vector gDatas)
    {
    	if (DEBUG >= 3) {
	    System.out.println("DEViseCanvas3DJmol.addNodeGData(" +
	      node + ")");
	}

	if (node instanceof TreeAtomNode) {
	    TreeAtomNode atomNode = (TreeAtomNode)node;
	    if (atomNode.gData != null &&
	      !gDatas.contains(atomNode.gData)) {
	        gDatas.addElement(atomNode.gData);
	    }
	} else {
	    for (int index = 0; index < node.children.size(); index++) {
	        DEViseGenericTreeNode child =
		  (DEViseGenericTreeNode)node.children.elementAt(index);
	        addNodeGData(child, gDatas);
	    }
	}
    }

    //-------------------------------------------------------------------
    /**
     * Set what is highlighted in Jmol and the highlight tree according
     * to the data sent by DEVise to the views that are piled "behind"
     * the main view in this pile.
     */
    private void setHighlightFromData()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.setHighlightFromData()");
	}
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseCanvas3DJmol.setHighlightFromData()");
	}

	int oldDistRCount =
	  (distRestraints != null) ? distRestraints.size() : 0;
	distRestraints = new Vector(); // throw away old restraints
	distRestUpdated = true;

	// Piled views are used to highlight atoms that were created in
	// the "base" view.
	// Right now the JavaScreen expects the 3D highlight views to
	// contain symbols that are ovals -- that's probably kind of silly,
	// but for now it requires the fewest changes.  wenger 2006-02-28.
        if (view.viewPiledViews.size() > 0) {
	    Vector selectedDevNodes = new Vector();

            for (int i = 0; i < view.viewPiledViews.size(); i++) {
	        DEViseView v = (DEViseView)view.viewPiledViews.elementAt(i);
		for (int j = 0; j < v.viewGDatas.size(); j++) {
		    DEViseGData gdata = (DEViseGData)v.viewGDatas.elementAt(j);

		    if (gdata.symbolType == gdata._symOval) {
			// Highlight an atom.
			highlightOneGData(gdata, selectedDevNodes);
		    } else if (gdata.symbolType == gdata._symSegment) {
			// Draw a distance restraint.
		        DistRestraint restraint = new DistRestraint();
			Integer atom1Num = (Integer)atomId2AtomNum.get(gdata.atom1Id);
			Integer atom2Num = (Integer)atomId2AtomNum.get(gdata.atom2Id);
			if (atom1Num != null && atom2Num != null) {
			    restraint.atom1Num = atom1Num.intValue();
			    restraint.atom2Num = atom2Num.intValue();
			    restraint.isViolated = gdata.isViolated;
			    restraint.isAmbiguous = gdata.isAmbiguous;
			    distRestraints.addElement(restraint);
			}
                    } else {
			jsc.pn("Warning: unexpected symbol type (" +
			  gdata.symbolType + ") in DEViseCanvas3DJmol");
                    }
                }
            }

	    highlightTree.tree.setSelection(selectedDevNodes);
        }

	if (oldDistRCount > 0 || distRestraints.size() > 0) {
	    // Make sure we update the distrance restraints.
	    repaint();
        }
    }

    //-------------------------------------------------------------------
    /**
     * Add to the highlight list according to the given GData record.
     */
    private void highlightOneGData(DEViseGData gdata,
      Vector selectedDevNodes)
    {
        Integer entAssemID = new Integer(gdata.entityAssemblyID);
        TreeEntityAssemblyNode entAssemNode = (TreeEntityAssemblyNode)
	  highlightTree.entityAssemblyNodes.get(entAssemID);

        if (entAssemNode != null) {
            Integer resNum = new Integer(gdata.residueNum);
	    TreeResidueNode residueNode = (TreeResidueNode)
	      entAssemNode.residueNodes.get(resNum);

            if (residueNode != null) {
	        String atomName = gdata.atomName;

		// If no atom name is specified, or the atom name is "X",
		// we want to highlight the entire residue specified by
		// this GData record.  If a valid atom name is given, we
		// we want to just highlight that particular atom.
	        if (atomName == null || atomName.equals("X")) {
	            selectedDevNodes.addElement(residueNode);
	        } else {
		    TreeAtomNode atomNode = (TreeAtomNode)
		      residueNode.atomNodes.get(atomName);

		    if (atomNode != null) {
		        selectedDevNodes.addElement(atomNode);
		    }
	        }
	    }
	}
    }

    //-------------------------------------------------------------------
    /**
     * Get distance restraint color name based on whether the restraint
     * is violated and/or ambiguous.
     * @param restraint: the distance restraint object.
     * @return: the name of the color in which the restraint should be
     *   drawn (e.g., "yellow").
     */
    private static String getDistRestColor(DistRestraint restraint)
    {
        if (restraint.isViolated) {
	    if (restraint.isAmbiguous) {
	        return "magenta";
	    } else {
	        return "orange";
	    }
	} else {
	    if (restraint.isAmbiguous) {
	        return "cyan";
	    } else {
	        return "yellow";
	    }
	}
    }

    //===================================================================
    static class JmolPanel extends JPanel {
        JmolViewer viewer;
        JmolAdapter adapter;

        //---------------------------------------------------------------
        JmolPanel(jsdevisec jsc) {
	    try {
	        String htmlName = "JavaScreen";
	        String options = "-applet";
	        // The "-applet" string seems to be the critical thing here
	        // -- without it, this call fails in an applet.
                viewer = JmolViewer.allocateViewer(this, null, htmlName,
	          jsc.jsValues.connection.documentBase,
	          jsc.jsValues.connection.codeBase, options, null);
	    } catch (Exception ex) {
	        jsc.pn("Exception creating Jmol viewer: " + ex.toString());
		jsc.pnStackTrace(ex);
		viewer = null;
	    }
        }

        //---------------------------------------------------------------
        public JmolViewer getViewer() {
            return viewer;
        }

        final Dimension currentSize = new Dimension();
        final Rectangle rectClip = new Rectangle();

        //---------------------------------------------------------------
        public void paint(Graphics g) {
            getSize(currentSize);
            g.getClipBounds(rectClip);
            viewer.renderScreenImage(g, currentSize, rectClip);
        }
    }

    //===================================================================
    // This class gets notified of various events in the Jmol viewer.
    // We use is for drill down, bringing up the popup menu, etc.
    class MyCallbackListener implements JmolCallbackListener {

	private DoDrillDown _ddd;

        //---------------------------------------------------------------
        // I'm not sure what this does.
        public void setCallbackFunction(String callbackType,
          String callbackFunction) {
        }

        //---------------------------------------------------------------
	// This method is called when the user picks an atom (we use
	// it for drill-down).
        public void notifyCallback(int type, Object[] data) {
	    String strInfo = (data == null || data[1] == null ? null : data[1]
	              .toString());
            switch(type) {
	    case JmolConstants.CALLBACK_PICK:
		if (drillDownEnabled) {
		    if (data == null || data[2] == null) {
		        System.err.println(
			  "ERROR: missing data in CALLBACK_PICK event!");
		    } else {
		        int atomIndex = ((Integer)data[2]).intValue();
		        DEViseGData gData =
		          (DEViseGData)gDatasToDisplay.elementAt(atomIndex);
		        _ddd = new DoDrillDown(view.getCurlyName(), gData.x0,
		          gData.y0, gData.z0);
		    }
		}
	        break;
	    }
        }

        //---------------------------------------------------------------
	// Tell Jmol what events we want to know about.
        public boolean notifyEnabled(int type) {
            switch (type) {
            case JmolConstants.CALLBACK_PICK:
                return true;
            }
            return false;
        }

        //===============================================================
        // This class does drill-down in a Jmol view.  The main reason for
        // this class is to create the thread that allows us to differentiate
        // between single and double clicks (a double-click cancels the
        // drill-down).
        private class DoDrillDown implements Runnable
        {
	    private String _viewName;
	    private float _x;
	    private float _y;
	    private float _z;
	    private Thread _ddThread;
	    private boolean _cancelled;

	    // How fast the second click has to come to consider it a double-
	    // click and cancel the drill-down.
	    private static final int DOUBLE_CLICK_TIME = 500;

            public DoDrillDown(String viewName, float x, float y, float z) {
	        _viewName = viewName;
	        _x = x;
	        _y = y;
	        _z = z;
	        _cancelled = false;

	        _ddThread = new Thread(this);
	        _ddThread.setName("3D Jmol drill down");
	        _ddThread.start();
	    }

	    public void run() {
		/* We don't need this anymore because notifyCallback()
		 * doesn't get called twice for a double-click (with
		 * Jmol 11.8.21).  In fact, there doesn't seem to be
		 * a way to tell a double-click from a single-click,
		 * so if we're in drill-down mode, we'll drill down
		 * on a double-click.
	        try {
	            Thread.sleep(DOUBLE_CLICK_TIME);
	        } catch (InterruptedException e)  {
	        }
		*/

	        if (_cancelled) {
		    if (DEBUG >= 2) {
	                System.out.println(
			  "Drill-down cancelled by double-click");
		    }
	        } else {
		    if (DEBUG >= 2) {
	                System.out.println("Doing 3D drill-down");
		    }
	            String cmd = DEViseCommands.SHOW_RECORDS3D + " " +
	              _viewName + " 1 ";
	            cmd += _x + " " + _y + " " + _z;

	            jscreen.guiAction = true;
	            dispatcher.start(cmd);
	        }
	    }

	    public void stop() {
	    }

	    public void cancel() {
	        _cancelled = true;
	    }
        }
    }
}
