package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.JTabbedPane;
import javax.swing.tree.*;

// for combine two runs to make merging hierachy
class MergingNode {

    // Data members
    protected String nodeName;   // name of node
    protected Object userObject;  // MutableTreeNode's userObject, not just String
    protected LinkedList mergingList; // list of the combined runs, 
                                      // Members are runs' directory names

    // Constructor
    public MergingNode(Object obj) {
	// initialize
	this.nodeName = null;
	this.userObject = null;

	if (obj instanceof String) {
	    this.nodeName = (String)obj;
	    this.userObject = obj;
	}
	else {
	    if (obj instanceof MarkedNode) {
		// MarkedNode is userObject of single indexTree
		this.nodeName = ((MarkedNode)obj).getNodeName();
		this.userObject = new MarkedNode(nodeName, true);
	    }
	    else if (obj instanceof DataFile) {
		// DataFile is userObject of single fileTree
		// But this situation did not appear so far
		this.nodeName = ((DataFile)obj).getNodeNameList();
		this.userObject = new DataFile();
		((DataFile)userObject).copyADataFile((DataFile)obj);
	    }
	    else if (obj instanceof MergingNode) {
		this.nodeName = ((MergingNode)obj).getNodeName();
		this.userObject = ((MergingNode)obj).getUserObject();
	    }
	}

	this.mergingList = new LinkedList();

    }

    public String getNodeName() { return nodeName; }

    public LinkedList getMergingList() { return mergingList; }

    public String getListName() {
	String listName = "";
	if (mergingList.size() == 0) return listName;
	
	for (int i = 0; i < mergingList.size(); i++)
	    listName += (String)mergingList.get(i) + "\n";
	return listName;
    }
    
    public int getListSize() {
	return mergingList.size();
    }

    public void addToMergingList(String nodeName) {
	mergingList.add(nodeName);
    }

    public void addToMergingList(LinkedList nodeNameList) {
	for (int i = 0; i < nodeNameList.size(); i++) {
	    mergingList.add(nodeNameList.get(i));
	}
    }

    public boolean isInList(Object aNode) {
	return mergingList.contains(aNode);
    }

    public Object getUserObject() {
	return userObject;
    }
	


}
