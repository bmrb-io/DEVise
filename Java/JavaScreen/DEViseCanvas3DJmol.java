//TEMPTEMP -- question -- should structDisplayTree feed into atomDisplayTree and then Jmol (like it does now), or should structDisplayTree and atomDisplayTree be in parallel?
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

// Jmol 3D implementation-specific aspects of DEViseCanvas.
// We use Jmol (http://jmol.sourceforge.net/) to do the 3D drawing,
// etc. for us.

// Note that this version of the 3D stuff is somewhat molecular-
// biology specific, because that's what Jmol is oriented towards.

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

// $Id$

// $Log$
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

// Import the Jmol stuff we need.
import org.jmol.adapter.smarter.SmarterJmolAdapter;
import org.jmol.api.JmolAdapter;
import org.jmol.api.JmolViewer;
//import org.jmol.app.Jmol;

public class DEViseCanvas3DJmol extends DEViseCanvas3D implements
  DEViseGenericTreeSelectionCallback
{
    private JmolPanel jmolPanel = null;

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

    private boolean structUpdated = false;
    private Vector structDisplayNodes = null;
    private boolean atomUpdated = false;
    private Vector atomDisplayNodes = null;
    private boolean highlightUpdated = false;
    private Vector highlightNodes = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // v is base view if there is a pile in this canvas.
    public DEViseCanvas3DJmol (DEViseView v, Image img)
    {
	super(v, img);
	jsc.showJmol(this);
    }

    //-------------------------------------------------------------------
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

        //TEMPTEMP? jsc.parentFrame.setCursor(DEViseUIGlobals.defaultCursor);
        if (treeFrame != null)
	  treeFrame.setCursor(DEViseUIGlobals.defaultCursor);

    	super.paint(gc);
    }

    //-------------------------------------------------------------------
    // Called when a session is closed.
    public void close()
    {
	treeFrame.dispose();
	treeFrame = null;
	jsc.hideJmol();
    }

    //-------------------------------------------------------------------
    public void create3DViewer()
    {
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
    public void showTrees()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.showTrees()");
	}

	if (treeFrame == null) {
    	    createTrees();
	    updateStructTree();
	}

	treeFrame.setExtendedState(Frame.NORMAL); // de-iconify
	treeFrame.toFront();
    }

    //-------------------------------------------------------------------
    public void nodesSelected(String name, Vector nodes)
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.nodesSelected(" +
	      name +  ")");
    	    if (DEBUG >= 3) {
	        System.out.println("  " + nodes);
	    }
	}

	//TEMPTEMP? jsc.parentFrame.setCursor(DEViseUIGlobals.waitCursor);
	treeFrame.setCursor(DEViseUIGlobals.waitCursor);

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
    public void resetSelection()
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.resetSelection()");
	}

	setHighlightFromData();
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    //TEMP -- document this
    protected void createTrees()
    {
	//TEMP -- the whole tree window should probably be its own class
        if (treeFrame == null) {
	    //
	    // Create the tree GUI.
	    //
	    JLabel displayLabel = new JLabel(DISPLAY_TREE_NAME);
	    displayLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

	    JLabel highlightLabel = new JLabel(HIGHLIGHT_TREE_NAME);
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
	    displayPanel.setLayout(new BoxLayout(displayPanel,
	      BoxLayout.Y_AXIS));
	    displayPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    displayPanel.add(displayLabel);
	    displayPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	    displayPanel.add(structAtomSplit);
	    displayPanel.add(Box.createRigidArea(new Dimension(0, 5)));

	    //TEMP -- think of a clearer name for this button?
	    JButton highlightResetButton = new JButton("Reset");
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
    //TEMP -- document this
    protected void createJmol()
    {
        if (jmolPanel == null) {
	    //
	    // Create the actual Jmol panel.
	    //
            jmolPanel = new JmolPanel();
    	        // TEMP Jmol seems like it may have the menus
            //JmolPanel jmolPanel = new Jmol();//TEMP?
            add(jmolPanel);
            jmolPanel.setSize(canvasDim.width, canvasDim.height);
            jmolPanel.setVisible(true);
        }

	updateStructTree();
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
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
    private class TreeMoleculeNode extends DEViseGenericTreeNode
    {
	public TreeMoleculeNode()
	{
	    super("Molecule");
	}
    }

    //-------------------------------------------------------------------
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
    private static void jmolEvalStringErr(JmolViewer viewer, String script)
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

        JmolViewer viewer = jmolPanel.getViewer();
        jmolEvalStringErr(viewer, selectCmd);

	viewer.setColorSelection(Color.GREEN);
	viewer.setSelectionHaloEnabled(true);
    }

    //-------------------------------------------------------------------
    private void displaySelected(Vector nodes)
    {
    	if (DEBUG >= 2) {
	    System.out.println("DEViseCanvas3DJmol.displaySelected()");
	}

//TEMPTEMP -- set busy cursor here?
	Vector gDatasToDisplay = new Vector();

	for (int index = 0; index < nodes.size(); index++) {
	    DEViseGenericTreeNode node =
	      (DEViseGenericTreeNode)nodes.elementAt(index);
	    addNodeGData(node, gDatasToDisplay);
	}

	String jmolData = DEViseJmolData.createJmolData(gDatasToDisplay);
	if (!jmolData.equals("")) {
            JmolViewer viewer = jmolPanel.getViewer();
	    viewer.setSelectionHaloEnabled(false);
	    jmolOpenStringErr(viewer, jmolData);
            viewer.setShowHydrogens(true);
            viewer.setShowAxes(true);
	}

	updateTreeData(highlightTree, gDatasToDisplay);

	setHighlightFromData();
    }

    //-------------------------------------------------------------------
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
    // Set what is highlighted in Jmol and the highlight tree according
    // to the data sent by DEVise to the views that are piled "behind"
    // the main view in this pile.
    private void setHighlightFromData()
    {
    	if (DEBUG >= 3) {
	    System.out.println("DEViseCanvas3DJmol.setHighlightFromData()");
	}

//TEMP -- if 3D highlight view is not a DEVise highlight view, this stuff should probably within the if above
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
}
