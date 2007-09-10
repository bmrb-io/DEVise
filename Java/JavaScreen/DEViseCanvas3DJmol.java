// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2007
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
import javax.swing.JButton;
import javax.swing.BoxLayout;
import javax.swing.Box;
import javax.swing.SwingUtilities;

// Import the Jmol stuff we need.
import org.jmol.api.*;
import org.jmol.adapter.smarter.SmarterJmolAdapter;
import org.jmol.api.JmolAdapter;
import org.jmol.api.JmolViewer;
import org.openscience.jmol.ui.JmolPopup;
import org.jmol.viewer.JmolConstants;
import org.openscience.jmol.app.Jmol;

public class DEViseCanvas3DJmol extends DEViseCanvas3D implements
  DEViseGenericTreeSelectionCallback
{
    // The panel holding the actual Jmol viewer.
    private JmolPanel jmolPanel = null;

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
    private static final String PROTONS_STR = "protons";

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
    private MyStatusListener myStatusListener;
    private JmolPopup jmolPopup;

    // True if atoms should be highlighted with halos, false if they
    // should be updated by changing color.
    private boolean highlightWithHalos = true;

    // The atoms to display, in the form of DEViseGData.
    private Vector gDatasToDisplay;

    // True if drill-down is enabled in Jmol, false otherwise.
    private boolean drillDownEnabled = false;

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

    //-------------------------------------------------------------------
    public void defineCenter()
    {
	viewer.setCenterSelected();
	viewer.setModeMouse(JmolConstants.MOUSE_ROTATE);
	viewer.setSelectionHaloEnabled(false);
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
	        viewer.setSelectionHaloEnabled(true);
		// Note: we're assuming here that the "normal" color is
		// CPK; that may not always be true, but I don't know
		// what else to do for now.  wenger 2006-06027.
                jmolEvalStringErr(viewer, "color atoms CPK");
	    } else {
	    	viewer.setSelectionHaloEnabled(false);
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
	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.jmolEvalStringErr(" +
	      script + ")");
	}
        String errStr = viewer.evalString(script);
	if (errStr != null) {
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
	    JButton highlightResetButton = new JButton("Reset");
	    highlightResetButton.setBackground(jsc.jsValues.uiglobals.bg);
	    highlightResetButton.setForeground(jsc.jsValues.uiglobals.fg);
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
	}

        if (jmolPanel == null) {
	    //
	    // Create the actual Jmol panel.
	    //
            jmolPanel = new JmolPanel();
            add(jmolPanel);
            jmolPanel.setSize(canvasDim.width, canvasDim.height);
            jmolPanel.setVisible(true);

            viewer = jmolPanel.getViewer();
	    myStatusListener = new MyStatusListener();
	    viewer.setJmolStatusListener(myStatusListener);
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

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // This class adapts a DEViseGenericTree to the specific requirements
    // of controlling this canvas.
    private class JmolTree
    {
        public final String name;
        public DEViseGenericTree tree;
        public Hashtable residueNodes;
	public DEViseGenericTreeNode topNode;

	public JmolTree(String treeName, DEViseCanvas3DJmol canvas) {
	    name = treeName;
	    topNode = new TreeMoleculeNode();
	    tree = new DEViseGenericTree(name, topNode, canvas);
	}
    }

    //-------------------------------------------------------------------
    // Specializes a DEViseGenericTreeNode to represent a molecule.
    private class TreeMoleculeNode extends DEViseGenericTreeNode
    {
	public TreeMoleculeNode()
	{
	    super("Molecule");
	}
    }

    //-------------------------------------------------------------------
    // Specializes a DEViseGenericTreeNode to represent a residue.
    private class TreeResidueNode extends DEViseGenericTreeNode
    {
	public int residueNumber;
	public String residueLabel;

	public TreeResidueNode(int number, String label)
	{
	    super("[" + label + "] " + number);

	    if (DEBUG >= 3) {
	        System.out.println("TreeResidueNode.TreeResidueNode("
		  + number + ", " + label + ")");
	    }
	    residueNumber = number;
	    residueLabel = label;
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
	viewer.openStringInline(dataStr);
        // Note: nothing shows up if you don't call getOpenFileError()
	// after openFile()
    	String errStr = viewer.getOpenFileError();
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

	jmTree.residueNodes = new Hashtable();
	jmTree.topNode.children.removeAllElements();

	int maxResidueNum = -1;

	//
	// We put the residue nodes into a Hashtable, and then pull
	// them out of the hash table and add them to the top-level
	// node, because the atoms are not always strictly in order
	// of increasing residue number.
	//
	for (int atomNum = 0; atomNum < gDatas.size(); atomNum++) {
	    DEViseGData gd = (DEViseGData)gDatas.elementAt(atomNum);

	    if (gd.symbolType != DEViseGData._symOval) {
	    	System.err.println("ERROR: illegal symbol type (" +
		  gd.symbolType + ") for Jmol view");
	        continue;
	    }

	    if (gd.residueNum > maxResidueNum) maxResidueNum = gd.residueNum;

	    // Find or construct the node for the appropriate residue.
	    Integer resNum = new Integer(gd.residueNum);
	    TreeResidueNode residueNode =
	      (TreeResidueNode)jmTree.residueNodes.get(resNum);
	    if (residueNode == null) {
		residueNode = new TreeResidueNode(gd.residueNum,
		  gd.residueLabel);
		jmTree.residueNodes.put(resNum, residueNode);
	    }

	    // Add this atom to the residue node.
	    //TEMP -- do the atoms need to be in a certain order within a residue?
	    TreeAtomNode atomNode = new TreeAtomNode(gd.atomNum, gd.atomName,
	      gd);
	    residueNode.addChild(atomNode);
	}

	// Now add the residue nodes to the tree.
	for (int residueNum = 1; residueNum <= maxResidueNum; residueNum++) {
	    Integer resNum = new Integer(residueNum);
	    TreeResidueNode residueNode =
	      (TreeResidueNode)jmTree.residueNodes.get(resNum);
	    if (residueNode != null) {
	        jmTree.topNode.addChild(residueNode);
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
	        selectCmd = "select;";
		break;
	    } else if (node instanceof TreeResidueNode) {
	        int resNum = ((TreeResidueNode)node).residueNumber;
	        if (!isFirst) {
	            selection += ",";
	        }
	        selection += resNum;
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
	    viewer.setColorSelection(Color.GREEN);
	    viewer.setSelectionHaloEnabled(true);
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
	if (!jmolData.equals("")) {
	    viewer.setSelectionHaloEnabled(false);
	    jmolOpenStringErr(viewer, jmolData);
            viewer.setShowHydrogens(true);
            viewer.setShowAxes(true);
	}

	// Update what's shown in the Atom Selection tree.
	updateTreeData(highlightTree, gDatasToDisplay);

	// Set what's selected in the Atom Selection tree according
	// to the highlight data we got from DEVise.
	setHighlightFromData();

	// Load this here so it knows how many atoms and bonds there are.
	loadPopupMenuAsBackgroundTask();
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
			Integer resNum = new Integer(gdata.residueNum);
		        DEViseGenericTreeNode devNode =
			  (DEViseGenericTreeNode)highlightTree.residueNodes.get(resNum);
			if (devNode != null) {
			    selectedDevNodes.addElement(devNode);
			}
                    } else {
		    	//TEMP -- error?
                    }
                }
            }

	    highlightTree.tree.setSelection(selectedDevNodes);
        }
    }

    //-------------------------------------------------------------------
    /**
     * Load the Jmol popup menu.  (This code is copied from the Jmol
     * source.)
     */
    void loadPopupMenuAsBackgroundTask() {
    	// no popup on MacOS 9 NetScape
	if (viewer.getOperatingSystemName().equals("Mac OS") && 
	  viewer.getJavaVersion().equals("1.1.5")) {
	    return; 
	}
	new Thread(new LoadPopupThread()).start();
    }

    //===================================================================

    //-------------------------------------------------------------------
    static class JmolPanel extends JPanel {
        JmolViewer viewer;
        JmolAdapter adapter;

        //---------------------------------------------------------------
        JmolPanel() {
            adapter = new SmarterJmolAdapter(null);
            viewer = JmolViewer.allocateViewer(this, adapter);
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

    //-------------------------------------------------------------------
    // This class exists just to load the Jmol popup menu.
  class LoadPopupThread implements Runnable {

    public void run() {
      Thread.currentThread().setPriority(Thread.MIN_PRIORITY);
      // long beginTime = System.currentTimeMillis();
      // System.out.println("LoadPopupThread starting ");
      // this is a background task
      JmolPopup popup;
      try {
        popup = JmolPopup.newJmolPopup(viewer);
      } catch (Exception e) {
        System.out.println("JmolPopup not loaded");
        return;
      }
      if (viewer.haveFrame()) {
        popup.updateComputedMenus();
      }
      jmolPopup = popup;
      // long runTime = System.currentTimeMillis() - beginTime;
      // System.out.println("LoadPopupThread finished " + runTime + " ms");
    }
  }

    //===================================================================
    // This class gets notified of various events in the Jmol viewer.
    // We use is for drill down, bringing up the popup menu, etc.
  class MyStatusListener implements JmolStatusListener {
    private DoDrillDown _ddd;

    public void notifyFileLoaded(String fullPathName, String fileName,
                                 String modelName, Object clientFile,
                                 String errorMsg) {
    }

    public void setStatusMessage(String statusMessage) {
    }

    public void scriptEcho(String strEcho) {
    }

    public void scriptStatus(String strStatus) {
    }

    public void notifyScriptTermination(String errorMessage, int msWalltime) {
    }

    public void handlePopupMenu(int x, int y) {
      if (jmolPopup != null) {
        jmolPopup.show(x, y);
      }
    }

    public void measureSelection(int atomIndex) {
    }

    public void notifyMeasurementsChanged() {
	// Cancel drill-down (notifyAtomPicked gets called on the first
	// click of a double-click).
    	if (_ddd != null) _ddd.cancel();
    }

    public void notifyFrameChanged(int frameNo) {
    }

    public void notifyAtomPicked(int atomIndex, String strInfo) {
    	if (DEBUG >= 1) {
	    System.out.println(
	      "DEViseCanvas3DJmol.MyStatusListener.notifyAtomPicked(" +
	      atomIndex + ", " + strInfo + ")");
	}

	if (drillDownEnabled) {
	    DEViseGData gData = (DEViseGData)gDatasToDisplay.elementAt(
	      atomIndex);

	    _ddd = new DoDrillDown(view.getCurlyName(), gData.x0,
	      gData.y0, gData.z0);
        }
    }

    public void showUrl(String urlString) {
    }

    public void showConsole(boolean showConsole) {
    }

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
	    try {
	        Thread.sleep(DOUBLE_CLICK_TIME);
	    } catch (InterruptedException e)  {
	    }

	    if (_cancelled) {
		if (DEBUG >= 2) {
	            System.out.println("Drill-down cancelled by double-click");
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
