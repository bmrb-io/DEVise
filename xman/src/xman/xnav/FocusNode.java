package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.tree.*;
import java.lang.Integer;
import javax.swing.ImageIcon;

class FocusNode {

    // Data members
    // fNames, measure, phases have the same order in the list
    protected LinkedList fNames;   // full names of datafiles w/ same focus
    protected LinkedList measures; // measurement of the focus, e.g. cpu, io ...
    protected LinkedList nodeList; // all nodes in the focus, inherit from DataFile
    protected LinkedList phases;  // phase Data indicator "-1" means global
    protected ImageIcon thumbnailIcon;
    protected int zeroDataCheck; // 0 is ZeroData, 1 is non-ZeroData, -1 is unknown yet


    // Constructor
    public FocusNode(LinkedList nodeList) {
	fNames = new LinkedList(); // member is String
	measures = new LinkedList(); // member is String
	this.nodeList = nodeList; // member is DefaultMutableTreeNode
	phases = new LinkedList(); // member is interger
	thumbnailIcon = null;
	zeroDataCheck = -1;
    }

    public LinkedList getNodeList() { return nodeList; }
    public int getNodeListSize() { return nodeList.size(); }
    public int getPhaseCnt() { return phases.size();}
    public int getMeasureCnt() { return measures.size(); }
    public LinkedList getMeasures() {return measures; }

    public void addMeasure(String measure) {
	measures.add(measure);
    }
    public void addFName(String fname) {
	fNames.add(fname);
    }
    public void addPhase(int phase) {
	phases.add(new Integer(phase));
    }

    public LinkedList getFNameList() {
	return fNames;
    }

    public String getFNames() { 
	// each name separated by ":", for passing to Devise
	String allFNames="";
	int len = fNames.size();
	if (len > 0) {
	    for (int i = 0; i < len-1; i++)
		allFNames += (String)fNames.get(i)+":";
	    allFNames += (String)fNames.get(len-1);
	}
	return allFNames;
    }


    public String getNodeNameList() {
	String nodeNames = "";
	int len = nodeList.size();

	if (len == 0) {
	    nodeNames = "whole program ";
	    return nodeNames;
	}
	
	for (int i = 0; i < len; i++) {
	    DefaultMutableTreeNode aNode = (DefaultMutableTreeNode)nodeList.get(i);
	    nodeNames += getNodeFullName(aNode) + ", ";
	}

	return nodeNames; 
    }

    // nodes' name including the path from parents
    protected String getNodeFullName(DefaultMutableTreeNode aNode) {
	if (aNode == null) return "";

	String fullName;
	Object obj = aNode.getUserObject();
	if (obj instanceof MergingNode)
	    obj = ((MergingNode)obj).getUserObject();

	if (obj instanceof MarkedNode)
		fullName = ((MarkedNode)obj).getNodeName();
	else
	    fullName = obj.toString();

	//String fullName = (String)aNode.getUserObject();
	DefaultMutableTreeNode child = aNode;
	DefaultMutableTreeNode parent = (DefaultMutableTreeNode)child.getParent();
	while (parent.getParent() != null) {
	    obj = parent.getUserObject();
	    if (obj instanceof MergingNode)
		obj = ((MergingNode)obj).getUserObject();

	    if (obj instanceof MarkedNode)
		fullName = ((MarkedNode)obj).getNodeName() + "/" + fullName;
	    else
		fullName = (String)obj + "/" + fullName;
	    child = parent;
	    parent = (DefaultMutableTreeNode)child.getParent();
	}
	fullName = "/"+fullName;

	return fullName;
    }

    public ImageIcon getThumbnailIcon() {
	return thumbnailIcon;
    }

    public void setThumbnailIcon(ImageIcon thumbIcon) {
	thumbnailIcon = thumbIcon;
    }

    public int getZeroDataCheck() {
	return zeroDataCheck;
    }

    public void setZeroDataCheck(int zeroCheck) {
	zeroDataCheck = zeroCheck;
    }
}
