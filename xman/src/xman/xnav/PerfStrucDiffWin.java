package xman.xnav;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.lang.*;

public class PerfStrucDiffWin {
    // Constant
    public static final boolean DEBUG = true;
    static final String title = "Structual Difference";

    // Data Members

    // before merge
    protected LinkedList indexTreeRootList; // members are treeroot/TreeNode
    protected LinkedList allFilesListList;  // members are linklist of DataFile
    protected LinkedList treeNameList;      // members are String, dirNames or "Merged"

    // after merge
    //protected LinkedList mergedAllMeasureList;   // members are fileTree's first-level Children nodes
    protected DefaultMutableTreeNode mergedIndexTreeRoot;
    //protected DefaultMutableTreeNode mergedFileTreeRoot;
    protected LinkedList mergedAllFilesList;

    // Constructor
    public PerfStrucDiffWin() {
	this(null, null, null);
    }

    // Constructor
    public PerfStrucDiffWin(LinkedList indexTreeRootList, LinkedList allFilesListList, LinkedList treeNameList) {
	this.indexTreeRootList = indexTreeRootList;
	this.allFilesListList = allFilesListList;
	this.treeNameList = treeNameList;

	this.mergedIndexTreeRoot = null;
	//this.mergedFileTreeRoot = null;
	//this.mergedAllMeasureList = null;
	this.mergedAllFilesList = new LinkedList();
    }

    public void executeMerge() {
	buildIndexMergeTree();
	buildMergeFilesList();
    }
    
    // merge two or more index_trees, run performance Structural Diff
    // get mergedIndexTreeRoot
    protected void buildIndexMergeTree() {
	// mergedIndexTree's userObject is MergingNode
	// MergingNode's userObject is MarkedNode with empty nodeList
	// which is the userObject in singleRun indexTree
	if ((indexTreeRootList == null) || (indexTreeRootList.size() <= 1)) {
	    System.out.println("PerfStrucDiffWin: no enough data in the strucDiffList.");
	    return;
	}

	DefaultMutableTreeNode root1 = (DefaultMutableTreeNode)indexTreeRootList.get(0);
	String rootName1 = (String)treeNameList.get(0);
	for (int i= 1; i < indexTreeRootList.size(); i++) {
	    DefaultMutableTreeNode root2 = (DefaultMutableTreeNode)indexTreeRootList.get(i);
	    String rootName2 = (String)treeNameList.get(i);
	    // Call structualDiff class to get merge tree
	    StructuralDiff perfstructuralDiff = new StructuralDiff(root1, root2, rootName1, rootName2);
	    perfstructuralDiff.findDiff();
	    DefaultMutableTreeNode interRoot = perfstructuralDiff.getMergeRoot();
	    root1 = interRoot;
	    rootName1 = null;
	    
	}
	mergedIndexTreeRoot = root1;
    }


    // build allFilesListList - merge & copy allFilesLists
    // update allFilesListList's DataFile.nodeList by
    // looking up the name and getting new pointers

    // this method has to be called after buildMergeIndexTree
    protected void buildMergeFilesList() {
	if (mergedIndexTreeRoot == null) return;
	
	if ((allFilesListList == null) || (allFilesListList.size() == 0))
	    return;
	
	for (int i = 0; i < allFilesListList.size(); i++) {
	    LinkedList filesList = (LinkedList)allFilesListList.get(i);
	    if ((filesList == null) || (filesList.size() == 0))
		continue;
	    // else, traverse and copy singleRun's allFilesList
	    for (int j = 0; j < filesList.size(); j++) {
		DataFile datafile = (DataFile)filesList.get(j);
		DataFile newdatafile = new DataFile();
		newdatafile.copyADataFile(datafile);

		// build new_datafile's nodeList (new pointers)
		// traverse mergedIndexTree to find same nodeName
		// as filesList
		if (datafile.getNodeListSize() == 0) {
		    mergedAllFilesList.add(newdatafile);
		    continue;
		}
		// traverse datafile.nodeList
		LinkedList oldnodeList = datafile.getNodeList();
		for (int k = 0; k < oldnodeList.size(); k++) {
		    DefaultMutableTreeNode oldnode = (DefaultMutableTreeNode)oldnodeList.get(k);
		    DefaultMutableTreeNode newnode = findInMergedIndexTree(oldnode, mergedIndexTreeRoot);
		    if (newnode == null) continue;
		    newdatafile.addTreeNode(newnode);
		}
		// add newdatafile to the updating mergedAllFilesList
		mergedAllFilesList.add(newdatafile);
	    }
	}
    }

    // find newpointer of node in mergedIndexTree giving an old one
    private DefaultMutableTreeNode findInMergedIndexTree(DefaultMutableTreeNode oldnode, DefaultMutableTreeNode subroot) {
	DefaultMutableTreeNode foundNode = null;
	if (subroot != null) {
	    int childCnt = subroot.getChildCount();
	    String oldnodeName = getObjName(oldnode.getUserObject());

	    // compare with root first
	    String newnodeName = getObjName(subroot.getUserObject());
	    if ((newnodeName == null) || (newnodeName.length() == 0)) return null;

	    if (oldnodeName.equals(newnodeName))
		foundNode = subroot;
	    else {
		// compare with root's children
		for (int i = 0; i < childCnt; i++) {
		    DefaultMutableTreeNode subrootChild = (DefaultMutableTreeNode)subroot.getChildAt(i);
		    if (subrootChild == null) continue;
		    
		    if (getObjName(subrootChild.getUserObject()).equals(oldnodeName)) {
			foundNode = subrootChild;
			break;
		    }
		    else {
			foundNode = findInMergedIndexTree(oldnode, subrootChild);
			if (foundNode != null)
			    break;
		    }
		}
	    }
	}
	
	return foundNode;
    }

    protected String getObjName(Object obj) {
	if (obj instanceof String)
	    return (String)obj;
	else if (obj instanceof MarkedNode)
	    return ((MarkedNode)obj).getNodeName();
	else if (obj instanceof DataFile)
	    return ((DataFile)obj).getNodeNameList();
	else if (obj instanceof MergingNode)
	    return ((MergingNode)obj).getNodeName();
	else
	    return null;
    }
	
    // using the updated mergedAllFilesList to build nodes networks
    // for the index_tree



    // using the mergedAllMeasureList to build metrics buttons


    public DefaultMutableTreeNode getMergedIndexTreeRoot() {
	return mergedIndexTreeRoot;
    }

    public LinkedList getMergedAllFilesList() {
	return mergedAllFilesList;
    }


}
