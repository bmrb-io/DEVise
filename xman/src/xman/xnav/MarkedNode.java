package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.tree.*;

class MarkedNode {

    // Data members
    protected String nodeName;   // name of datafile in current directory
    protected boolean marked;    // marked node means there are data related
    protected LinkedList nodeList; // all other nodes having data with this node
                                   // members are DefaultMutableTreeNode
    // corresponding metrics of the nodeList
    // members are LinkedList for multi-metrics of the same node
    protected LinkedList measureList; 

    // Constructor
    public MarkedNode(String nodeName, boolean marked) {
	this.nodeName = nodeName;
	this.marked = marked;
	this.nodeList = new LinkedList();
	this.measureList = new LinkedList();
	
    }

    public String getNodeName() { return nodeName; }
    public boolean getMarked() { return marked; }
    public LinkedList getNodeList() { return nodeList; }
    public LinkedList getMeasureList() { return measureList; }

    public void addToNodeList(Object aNode) {
	nodeList.add(aNode);
    }

    public void addToMeasureList(Object metricsList) {
	measureList.add(metricsList);
    }

    public boolean isInList(Object aNode) {
	return nodeList.contains(aNode);
    }

    public boolean isInNodeMeasureList(String metrics, DefaultMutableTreeNode aNode) {
	boolean isIn = false;
	LinkedList list =  getaNodeMeasureList(aNode);
	for (int i = 0; i < list.size(); i++) {
	    if (((String)list.get(i)).equals(metrics)) {
		isIn = true;
		break;
	    }
	}
	return isIn;
    }

    public boolean isInNodeMeasureList(String metrics, int index) {
	boolean isIn = false;
	LinkedList list =  getaNodeMeasureList(index);
	for (int i = 0; i < list.size(); i++) {
	    if (((String)list.get(i)).equals(metrics)) {
		isIn = true;
		break;
	    }
	}
	return isIn;
    }

    public void addToNodeMeasureList(String metrics, DefaultMutableTreeNode aNode) {
	LinkedList list =  getaNodeMeasureList(aNode);
	if ((list != null) && (!isInNodeMeasureList(metrics, aNode)))
	    list.add(metrics);
    }

    public void addToNodeMeasureList(String metrics, int index) {
	LinkedList list =  getaNodeMeasureList(index);
	if ((list != null) && (!isInNodeMeasureList(metrics, index)))
	    list.add(metrics);
    }

    public LinkedList getaNodeMeasureList(DefaultMutableTreeNode aNode) {
	int index = nodeList.indexOf(aNode);
	if (index < 0) return null;
	
	return (LinkedList)measureList.get(index);
    }

    public LinkedList getaNodeMeasureList(int index) {
	if (measureList.size() <= index)
	    return null;
	return (LinkedList)measureList.get(index);
    }

}
