package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.tree.*;
import java.lang.Integer;
import javax.swing.ImageIcon;

class DataFile {

    // Data members
    protected String allInfo; // complete infomation of this datafile
    protected String fName;   // name of datafile in current directory
    protected String measure; // measurement of data, e.g. cpu, io ...
    protected LinkedList nodeList; // all nodes related to this dataset
    protected String dirName; // files directory location
    protected int phase;  // phase Data indicator "-1" means global
    protected ImageIcon thumbnailIcon;
    protected int zeroDataCheck; // 0 is ZeroData, 1 is non-ZeroData, -1 is unknown yet


    // Constructor
    public DataFile() {
	allInfo = null;
	fName = null;
	measure = null;
	//empty linklist, elements of the list should be "DefaultMutableTreeNode"
	nodeList = new LinkedList();
	dirName = null;
	phase = -100; // invalid phase
	zeroDataCheck = -1;
	thumbnailIcon = null;
    }

    public String getAllInfo() { return allInfo; }
    public String getFileName() { return fName; }
    public String getMeasure() { return measure; }
    public LinkedList getNodeList() { return nodeList; }
    public int getNodeListSize() { return nodeList.size(); }
    public int getPhase() { return phase;}

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

    public String getDirName() {
	return dirName;
    }

    public String getFullFileName() {
	return dirName+fName;
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

    public String getShortDirName() {
	if (dirName.length() <= 6)
	    return dirName;

	String shortName = dirName;
	StringTokenizer token = new StringTokenizer(dirName, "/");
	int tokenCnt = token.countTokens();
	if (tokenCnt <= 2)
	    return dirName;

	String str[] = new String[tokenCnt];
        int i = 0;
        while (token.hasMoreTokens()) {
            str[i]= token.nextToken().trim();
            if (str[i].length() > 0)
		i++; }
          
        tokenCnt = i;      // total number of str
	//shortName = ".../";
	shortName = "/";
	for (i = tokenCnt-1; i< tokenCnt; i++)
	    shortName+=str[i]+"/";
	
	return shortName;
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

    public void setAllInfo(String allInfo) {
	this.allInfo = allInfo;
    }

    public void setFileName(String fName) {
	this.fName = fName;
    }

    public void setDirName(String dirName) {
	this.dirName = dirName;
    }

    public void setMeasure(String measure) {
	this.measure = measure;
    }

    public void setPhase(String phase) {
	this.phase = (new Integer(phase)).intValue();
    }

    // copy a DataFile except the nodeList
    public void copyADataFile(DataFile datafile) {
	this.allInfo = datafile.getAllInfo();
	this.fName = datafile.getFileName();
	this.dirName = datafile.getDirName();
	this.measure = datafile.getMeasure();
    }

    public void addTreeNode(DefaultMutableTreeNode aNode) {
	nodeList.add(aNode);
    }

    public boolean findANode(DefaultMutableTreeNode checkNode) {
	boolean found = false;
	// consider whole program and first child as no focus
	if (checkNode.getLevel() <= 1) {
	    //either whole program or first child
	    if (nodeList.size() == 0) 
		found = true;
	    return found;
	}
	

	found = nodeList.contains(checkNode); 

	/* found node and its children too
	if (checkNode.isLeaf() || found) return found;

	int childenCnt;	
	childenCnt = checkNode.getChildCount();
	for (int i = 0; i < childenCnt; i++) {
	    DefaultMutableTreeNode child = (DefaultMutableTreeNode)checkNode.getChildAt(i);
	    found = findANode(child);
	    if (found) return found;
	}
	*/

	return found; 
    }
		


}


