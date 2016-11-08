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

// Generic tree GUI -- the code that "knows" about the data builds up a
// tree of DEViseGenericTreeNodes, and passes it to an instance of this
// class.  All selection, etc., is done via the DEViseGenericTreeNodes.

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5.14.1  2011/04/20 17:15:07  wenger
// Changed the DEViseGenericTree.setSelection() method and the YLogGUI
// p() and pn() methods to only actually update the GUI in the event
// dispatched thread, to hopefully cure problems with incorrect 3D
// highlight updating in the s2pred visualization, and null pointer
// problems when showing the log window.  (I actually meant to do some
// earlier commits to this branch with more of the debug code in place,
// but I forgot to do that.)
//
// Revision 1.5  2008/09/03 19:15:59  wenger
// Initial changes to JavaScreen client to support entity assembly
// IDs in 3D Jmol visualizations.  (Still needs some cleanup.)
//
// Revision 1.4  2007/04/25 20:43:51  wenger
// Greatly improved the documentation of the DEViseCanvas3DJmol class;
// minor improvements to other classes.
//
// Revision 1.3  2007/04/23 19:41:46  wenger
// Added some extra GUI-thread-related diagnostic output.
//
// Revision 1.2  2006/05/26 16:22:15  wenger
// Merged devise_jmol_br_0 thru devise_jmol_br_1 to the trunk.
//
// Revision 1.1.2.5  2006/05/23 18:17:50  wenger
// Added initial Jmol menu with a menu item to show the tree selection
// window; destroying and re-creating the window currently doesn't
// preserve the existing selection, although I started on provision for
// that.
//
// Revision 1.1.2.4  2006/05/19 17:01:25  wenger
// Geez -- Component doesn't have a setPreferredSize() method in
// Java 1.4 (it does in 1.5).  JScrollPane *does* in 1.4, though,
// so DEViseGenericTree.getParentComponent() now returns a
// JScrollPane so things work in 1.4...
//
// Revision 1.1.2.3  2006/05/12 20:15:39  wenger
// Both structure trees are now in a single window.
//
// Revision 1.1.2.2  2006/05/12 14:50:26  wenger
// Now have two trees for a Jmol view: the first one selects which
// atoms are shown in the Jmol view and in the second tree; the
// second tree selects which atoms are highlighted in the Jmol view.
//
// Revision 1.1.2.1  2006/05/08 20:19:47  wenger
// Made the JavaScreen tree code itself generic (although the Jmol-
// related code that calls it is not).
//

// ========================================================================

// DEViseGenericTree.java

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;

public class DEViseGenericTree
{
    private static final int DEBUG = 0; // 0 - 3

    // The scroll pane containing the JTree.
    private JScrollPane scrollPane;

    private DefaultMutableTreeNode top;
    private JTree tree;
    private DefaultTreeModel treeModel;
    private String treeName;
    private DEViseGenericTreeSelectionCallback selectionCallback;

    //===================================================================
    // PUBLIC METHODS
    
    //-------------------------------------------------------------------
    // Constructor.
    public DEViseGenericTree(String name, DEViseGenericTreeNode topDevNode,
      DEViseGenericTreeSelectionCallback callback)
    {
	treeName = name;
	selectionCallback = callback;

        // Create the tree.
        top = new DefaultMutableTreeNode(topDevNode);
	treeModel = new DefaultTreeModel(top);
        tree = new JTree(treeModel);
	tree.setEditable(false);
	tree.setShowsRootHandles(true);
        tree.getSelectionModel().setSelectionMode
                (TreeSelectionModel.DISCONTIGUOUS_TREE_SELECTION);

        // Listen for when the selection changes.
        tree.addTreeSelectionListener(new MyTreeSelListener());

        // Create the scroll pane and add the tree to it. 
        scrollPane = new JScrollPane(tree);

	scrollPane.setPreferredSize(new Dimension(200, 400));
    }

    //-------------------------------------------------------------------
    public JScrollPane getParentComponent()
    {
        return scrollPane;
    }

    //-------------------------------------------------------------------
    // Update the set of nodes in the tree.
    public void updateTree(DEViseGenericTreeNode topDevNode)
    {
    	if (DEBUG >= 1) {
            System.out.println("DEViseGenericTree(" + treeName +
	      ").updateTree()");
        }
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseGenericTree.updateTree()");
	}

	top.removeAllChildren();
	createNodes(top, topDevNode);
	treeModel.reload();

	// Make all of the entity assemblies visible.
	tree.expandRow(0);
    }

    //-------------------------------------------------------------------
    // Set which nodes in the tree are selected.
    public void setSelection(Vector selectedDevNodes)
    {
        if (SwingUtilities.isEventDispatchThread()) {
	    doSetSelection(selectedDevNodes);
	} else {
	    Runnable runSet = new RunSetSelection(selectedDevNodes);
	    SwingUtilities.invokeLater(runSet);
	}
    }

    //-------------------------------------------------------------------
    // Select the top node in the tree.
    public void selectTop()
    {
       tree.setSelectionRow(0);
    }

    //===================================================================
    // PROTECTED METHODS

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // This class is used to allow us to actually set the selection
    // in the event dispatched thread -- fixes a problem with selection
    // not getting properly propagated to the trees and 3D view in
    // some cases.
    private class RunSetSelection implements Runnable {
        private Vector _selectedDevNodes;

	RunSetSelection(Vector selectedDevNodes) {
	    _selectedDevNodes = selectedDevNodes;
	}

	public void run() {
	    doSetSelection(_selectedDevNodes);
	}
    }

    //-------------------------------------------------------------------
    // "Really" set which nodes in the tree are selected.
    private void doSetSelection(Vector selectedDevNodes)
    {
    	if (DEBUG >= 1) {
            System.out.println("DEViseGenericTree(" + treeName +
	      ").doSetSelection()");
    	    if (DEBUG >= 3) {
	        System.out.println("  " + selectedDevNodes);
	    }
        }
        if (DEViseGlobals.DEBUG_GUI_THREADS >= 2 ||
	  (DEViseGlobals.DEBUG_GUI_THREADS >= 1 &&
	  !SwingUtilities.isEventDispatchThread())) {
	    System.out.println(Thread.currentThread() +
	    " calls DEViseGenericTree.doSetSelection()");
	}

	tree.clearSelection();

	Vector treePaths = new Vector();

        for (int index = 0; index < selectedDevNodes.size(); index++) {
	    DEViseGenericTreeNode devNode =
	      (DEViseGenericTreeNode)selectedDevNodes.elementAt(index);
	    DefaultMutableTreeNode node =
	      (DefaultMutableTreeNode)devNode.treeInfo;
	    TreePath path = new TreePath(node.getPath());
	    treePaths.addElement(path);
	}

	TreePath[] pathArray = new TreePath[treePaths.size()];
	for (int index = 0; index < treePaths.size(); index++) {
	    pathArray[index] = (TreePath)treePaths.elementAt(index);
	}

	tree.setSelectionPaths(pathArray);
    }
    //-------------------------------------------------------------------
    // Create nodes in the tree corresponding to devNode and its children,
    // and add them below node.
    private void createNodes(DefaultMutableTreeNode node,
      DEViseGenericTreeNode devNode)
    {
        //TEMP -- should this remove the old top node and stick new one in?

	devNode.treeInfo = node;

	for (int index = 0; index < devNode.children.size(); index++) {
	    DEViseGenericTreeNode devChild =
	      (DEViseGenericTreeNode)devNode.children.elementAt(index);
	    DefaultMutableTreeNode childNode =
	      new DefaultMutableTreeNode(devChild);
	    node.add(childNode);
	    createNodes(childNode, devChild);
	}
    }

    //-------------------------------------------------------------------
    private class MyTreeSelListener implements TreeSelectionListener {

        //---------------------------------------------------------------
        public void valueChanged(TreeSelectionEvent e) {
	    if (DEBUG >= 2) {
                System.out.println("MyTreeSelListener.valueChanged()");
	    }

	    Vector selectedNodes = new Vector();

            TreePath[] paths = tree.getSelectionModel().getSelectionPaths();
	    if (paths != null) {
                for (int pindex = 0; pindex < paths.length; pindex++) {
		    DefaultMutableTreeNode node =
		      (DefaultMutableTreeNode)
		      paths[pindex].getLastPathComponent();

                    Object nodeInfo = node.getUserObject();
	            if (DEBUG >= 2) {
		        System.out.println("  " + nodeInfo.toString() +
			  " selected");
		    }

		    selectedNodes.addElement(nodeInfo);
                }
            }

	    selectionCallback.nodesSelected(treeName, selectedNodes);
        }
    }
}
