package xman.xnav;

import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;

// Build data network - a node links other nodes with whom it has common 
// performance data

class DataNetWork {

    // Data
    protected LinkedList dataSetList; // Members are DataFile

    // Constructor
    public DataNetWork(LinkedList dataSetList) {
	this.dataSetList = dataSetList;
    }

    public void buildNetWork() {
	int len = dataSetList.size();
	if (len == 0) return; // empty

	// Get dataSet one by one and build network
	for (int i= 0; i< len; i++) {
	    DataFile datafile = (DataFile)dataSetList.get(i);
	    if (datafile == null) continue;

	    LinkedList nodeList = datafile.getNodeList();
	    if (nodeList == null) continue;

	    int size = nodeList.size();
	    if (size == 0) continue;

	    String metrics = datafile.getMeasure();
	    // Build local full network within one dataSet
	    for (int thisnode = 0; thisnode < size; thisnode ++) {
		DefaultMutableTreeNode thisTreeNode = (DefaultMutableTreeNode)nodeList.get(thisnode);
		Object obj = thisTreeNode.getUserObject();
		if (obj instanceof MergingNode)
		    obj = ((MergingNode)obj).getUserObject();
		if (!(obj instanceof MarkedNode)) continue;
		MarkedNode markednode = (MarkedNode)obj;

		// get othernodes now
		for (int othernode = 0; othernode < size; othernode ++) {
		    DefaultMutableTreeNode thatTreeNode = (DefaultMutableTreeNode)nodeList.get(othernode);
		    // get ride of itself
		    if (thisTreeNode == thatTreeNode) continue; 
		    if (!markednode.isInList(thatTreeNode)) {
			markednode.addToNodeList(thatTreeNode);
			LinkedList metricsList = new LinkedList();
			metricsList.add(metrics);
			markednode.addToMeasureList(metricsList);
		    }
		    else {
			markednode.addToNodeMeasureList(metrics, thatTreeNode);
		    }
		}
	    }
	}
    }

}
