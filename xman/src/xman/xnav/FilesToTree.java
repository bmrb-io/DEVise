package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;

class FilesToTree {

    // Data
    protected LinkedList dataFiles;
    protected DefaultMutableTreeNode treeRoot;
    protected DefaultTreeModel treeModel; 
    protected LinkedList measureList;

    // Constructor
    public FilesToTree(LinkedList dataFiles) {
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
	    treeRoot = new DefaultMutableTreeNode("data sets");
	    treeModel = new DefaultTreeModel(treeRoot);
	    return;
	}
       
	int len = dataFiles.size();
	if (len == 0) {
	    treeRoot = new DefaultMutableTreeNode("data sets");
	    treeModel = new DefaultTreeModel(treeRoot);
	    return;  //empty datafile list
	}

	// Set "data files" as treeRoot node
	// This Root's userObject is String
	treeRoot = new DefaultMutableTreeNode("data sets");
	treeModel = new DefaultTreeModel(treeRoot);
	TreeHandler treeHandler = new TreeHandler(treeRoot, treeModel);	

	for (int i = 0; i < len; i++) {
	    DataFile datafile = (DataFile)dataFiles.get(i);
	    if (datafile == null) continue;

	    String measure = datafile.getMeasure();
	    if ((measure == null) && (measure == "")) continue;

	    DefaultMutableTreeNode subParent = treeRoot;
	    DefaultMutableTreeNode subTree = 
		treeHandler.hitChildren(measure, subParent);
	    // Subtree has existed
	    if (subTree != null)
		subParent = subTree;
	    
	    // Add a new node into the subtree
	    else { 
		subParent = treeHandler.addObject(subParent, measure);
		measureList.add(subParent);
	    }
	    
	    // Leaf Node's userObject is type of DataFile
	    DefaultMutableTreeNode leafNode = treeHandler.addObject(subParent, datafile);
	}

	treeRoot = treeHandler.getTreeRootNode();
	treeModel = treeHandler.getTreeModel();

    }

}
			
		
	    
	    
	    
	    
