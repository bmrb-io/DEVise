package xman.xnav;

import java.awt.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.lang.*;


class StrucDiffWin {
    // Constant
    public static final boolean DEBUG = true;
    static final String title = "Structual Difference";

    // lists of TreeRootNodes and their names for SourceStrucDiff
    protected LinkedList strucDiffList;
    protected LinkedList strucNameList;
    protected DefaultMutableTreeNode mergeTreeRoot;
    protected TreeHandler treeHandler;

    // Constructor
    public StrucDiffWin() {
	this(null, null);
    }

    // Constructor
    public StrucDiffWin(LinkedList strucDiffList, LinkedList strucNameList) {
	this.strucDiffList = strucDiffList;
	this.strucNameList = strucNameList;

	treeHandler = new TreeHandler();
	mergeTreeRoot = null;

	mergeTreeRoot = mergeSrcDiff();
    }

    // build merge tree
    DefaultMutableTreeNode mergeSrcDiff() {

	if ((strucDiffList == null) || (strucDiffList.size() <= 1)) {
	    System.out.println("StrucDiffWin: no enough data in the strucDiffList.");
	    return null;
	}

	DefaultMutableTreeNode root1 = (DefaultMutableTreeNode)strucDiffList.get(0);
	String rootName1 = (String)strucNameList.get(0);
	for (int i= 1; i < strucDiffList.size(); i++) {
	    DefaultMutableTreeNode root2 = (DefaultMutableTreeNode)strucDiffList.get(i);
	    String rootName2 = (String)strucNameList.get(i);
	    // Call structualDiff class to get merge tree
	    StructuralDiff structuralDiff = new StructuralDiff(root1, root2, rootName1, rootName2);
	    structuralDiff.findDiff();
	    DefaultMutableTreeNode interRoot = structuralDiff.getMergeRoot();
	    root1 = interRoot;
	    rootName1 = null;
	    
	}
	//mergeTreeRoot = root1;
	return root1;	
    }

    public DefaultMutableTreeNode getMergeTreeRoot() {
	return mergeTreeRoot;
    }
}
