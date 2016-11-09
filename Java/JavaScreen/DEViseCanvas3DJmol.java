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

// ========================================================================

// DEViseCanvas3DJmol.java

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
