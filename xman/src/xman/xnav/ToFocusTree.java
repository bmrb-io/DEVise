package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;

class ToFocusTree {

    // Data
    protected LinkedList dataFiles;
    protected DefaultMutableTreeNode treeRoot;
    protected DefaultTreeModel treeModel; 
    protected LinkedList measureList;

    // Constructor
    public ToFocusTree(LinkedList dataFiles) {
	this.dataFiles = dataFiles;
	this.treeRoot = null;
	this.treeModel = null;
	this.measureList = new LinkedList(); // Members are MutableTreeNodes

	buildTree();
    }

    public DefaultMutableTreeNode getTreeRoot() {
	return treeRoot;
    }

    public DefaultTreeModel getTreeModel() {
	return treeModel;
    }

    public LinkedList getMeasureList() {
	return measureList;
    }

    protected void buildTree() {

	if (dataFiles == null) {
	    treeRoot = new DefaultMutableTreeNode("Foci");
	    treeModel = new DefaultTreeModel(treeRoot);
	    return;
	}
       
	int len = dataFiles.size();
	if (len == 0) {
	    treeRoot = new DefaultMutableTreeNode("Foci");
	    treeModel = new DefaultTreeModel(treeRoot);
	    return;  //empty datafile list
	}

	// Set "data files" as treeRoot node
	// This Root's userObject is String
	treeRoot = new DefaultMutableTreeNode("Foci");
	treeModel = new DefaultTreeModel(treeRoot);
	TreeHandler treeHandler = new TreeHandler(treeRoot, treeModel);	

	for (int i = 0; i < len; i++) {
	    DataFile datafile = (DataFile)dataFiles.get(i);
	    if (datafile == null) continue;

	    //String measure = datafile.getMeasure();
	    //if ((measure == null) && (measure == "")) continue;
	    String focusName = datafile.getNodeNameList(); 

	    DefaultMutableTreeNode subParent = treeRoot;
	    DefaultMutableTreeNode subTree = 
		treeHandler.hitChildren(focusName, subParent);
	    // Subtree has existed, the focus existed
	    // modify the focusNode
	    if (subTree != null) {
		//System.out.println("found same focus");
		FocusNode focusNode = (FocusNode)subTree.getUserObject();
		focusNode.addMeasure(datafile.getMeasure());
		focusNode.addPhase(datafile.getPhase());
		focusNode.addFName(datafile.getFullFileName());

		String measure = datafile.getMeasure();
		subParent = subTree;
		//subTree = treeHandler.hitChildre(measure, subParent);
		int childrenCnt = subTree.getChildCount();
		for (int j = 0; j < childrenCnt; j++) {
		    DefaultMutableTreeNode children = (DefaultMutableTreeNode)subTree.getChildAt(j);
		    FocusNode obj = (FocusNode)children.getUserObject();
		    
		    if (measure.equals((String)obj.getMeasures().get(0))) {
			subTree = children;
			break;
		    }
		}
	      
		// if no hit Child, add focusNode under metrics
		if (subTree == subParent) {
		    //System.out.println("no same metrics found");
		    focusNode = new FocusNode(datafile.getNodeList());
		    focusNode.addMeasure(datafile.getMeasure());
		    focusNode.addPhase(datafile.getPhase());
		    focusNode.addFName(datafile.getFullFileName());
		    subParent = treeHandler.addObject(subParent, focusNode);
		    //subParent.setNodeName(measure);
		}
		else { // update subnode under metrics
		    //System.out.println("same metrics found");
		    focusNode = (FocusNode)subTree.getUserObject();
		    focusNode.addMeasure(datafile.getMeasure());
		    focusNode.addPhase(datafile.getPhase());
		    focusNode.addFName(datafile.getFullFileName());

		}
	    }
	    // Add a new node (FocusNode) into the subtree
	    else {
		//System.out.println("no same focus found");
		FocusNode focusNode = new FocusNode(datafile.getNodeList());
		focusNode.addMeasure(datafile.getMeasure());
		focusNode.addPhase(datafile.getPhase());
		focusNode.addFName(datafile.getFullFileName());
		// first level - all the data in same focus
		subParent = treeHandler.addObject(subParent, focusNode);
		// second level - metrics (make a copy of focusNode)
		focusNode = new FocusNode(datafile.getNodeList());
		focusNode.addMeasure(datafile.getMeasure());
		focusNode.addPhase(datafile.getPhase());
		focusNode.addFName(datafile.getFullFileName());
		subParent = treeHandler.addObject(subParent, focusNode);
		//subParent.setNodeName(datafile.getMeasure());
		//measureList.add(subParent);
	    }
	    
	    // Leaf Node's userObject is type of DataFile
	    //DefaultMutableTreeNode leafNode = treeHandler.addObject(subParent, datafile);
	}

	treeRoot = treeHandler.getTreeRootNode();
	treeModel = treeHandler.getTreeModel();

    }

}
			
		
	    
	    
	    
	    
