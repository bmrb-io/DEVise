package xman.xnav;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;

public class StructuralDiff {
    // Compare two resource hierarchies in a top-down manner
    // And build the derived hierarchy
    protected DefaultMutableTreeNode rootNode1;
    protected DefaultMutableTreeNode rootNode2;
    protected DefaultMutableTreeNode rootMerge;
    protected String runName1; // should be run's directory name
    protected String runName2;

    protected TreeHandler treeHandler;
    protected DefaultTreeModel mergeTreeModel;

    // Constructor
    public StructuralDiff() {
	rootNode1 = null;
	rootNode2 = null;
	rootMerge = null;
	runName1 = null;
	runName2 = null;
    }

    // Constructor
    public StructuralDiff(DefaultMutableTreeNode rootNode1, DefaultMutableTreeNode rootNode2) {
	this.rootNode1 = rootNode1;
	this.rootNode2 = rootNode2;
	this.runName1 = null;
	this.runName2 = null;
	rootMerge = null;
    }

    // Constructor
    public StructuralDiff(DefaultMutableTreeNode rootNode1, DefaultMutableTreeNode rootNode2, String runName) {
	this.rootNode1 = rootNode1;
	this.rootNode2 = rootNode2;
	this.runName1 = runName;
	this.runName2 = runName;
	rootMerge = null;
    }

    // Constructor
    public StructuralDiff(DefaultMutableTreeNode rootNode1, DefaultMutableTreeNode rootNode2, String runName1, String runName2) {
	this.rootNode1 = rootNode1;
	this.rootNode2 = rootNode2;
	this.runName1 = runName1;
	this.runName2 = runName2;
	rootMerge = null;
    }

    public DefaultMutableTreeNode getMergeRoot() { return rootMerge; }

    public void findDiff() {
	//System.out.println("StructuralDiff - start Merge!");

	// if not null, set root "whole program" for rootMerge
	if ((rootNode1 == null) || (rootNode2 == null))
	    return;

	// get rootNodes' UserObjects, to decide type of merge
	Object userObj1 = rootNode1.getUserObject();
	Object userObj2 = rootNode2.getUserObject();

	/**************************************************/
	// two single runs merge

	System.out.println("StructuralDiff - Both are singleRun Tree");
	// detect if the two runs are the same by their name
	if ((runName1 != null) && (runName2 != null))
	    if (runName1.equals(runName2)) {
		System.out.println("No need to Merge, they are the same!");
		rootMerge = rootNode1;
		return;
	    }
		
	// make the rootMerge's userObject as MergingNode
	if ((!(userObj1 instanceof MergingNode)) && (!(userObj2 instanceof MergingNode))) {
	    MergingNode mergingRoot = new MergingNode("whole program");
	    if (runName1 != null)
		mergingRoot.addToMergingList(runName1);
	    if (runName2 != null)
		mergingRoot.addToMergingList(runName2);

	    rootMerge = new DefaultMutableTreeNode(mergingRoot);	     
	    mergeTreeModel = new DefaultTreeModel(rootMerge);
	    treeHandler = new TreeHandler(rootMerge, mergeTreeModel);

	    findSameLevelDiff(rootNode1, rootNode2, rootMerge);
	    return;
	}
	/***************************************************/

	/***************************************************/
	// Both of runs are from mergingTree merged before
	System.out.println("StructuralDiff - Both are merged Tree");

	if ((userObj1 instanceof MergingNode) && (userObj2 instanceof MergingNode)) {
	    
	    // get runNames from their mergingList
	    runName1 = ((MergingNode)userObj1).getListName();
	    runName2 = ((MergingNode)userObj2).getListName();

	    // Make rootNode for the new mergingTree
	    MergingNode mergingRoot = new MergingNode("whole program");
	    mergingRoot.addToMergingList(runName1);
	    mergingRoot.addToMergingList(runName2);

	    rootMerge = new DefaultMutableTreeNode(mergingRoot);	     
	    mergeTreeModel = new DefaultTreeModel(rootMerge);
	    treeHandler = new TreeHandler(rootMerge, mergeTreeModel);

	    findSameLevelDiff(rootNode1, rootNode2, rootMerge);
	    return;
	}
	/***************************************************/

	/***************************************************/
	// One of two runs is single original run, the other is from merging
	// single run should have the runName

	System.out.println("StructrualDiff - one is merged, the other is single.");

	// Make rootNode for the new mergingTree
	MergingNode mergingRoot = new MergingNode("whole program");

	if (userObj1 instanceof MergingNode) {
	    // update runName1
	    runName1 = ((MergingNode)userObj1).getListName();
	    mergingRoot.addToMergingList(((MergingNode)userObj1).getMergingList());
	}
	else
	    mergingRoot.addToMergingList(runName1);

	if (userObj2 instanceof MergingNode) {
	    //  update runName2;
	    runName2 = ((MergingNode)userObj2).getListName();
	    mergingRoot.addToMergingList(((MergingNode)userObj2).getMergingList());
	}
	else
	    mergingRoot.addToMergingList(runName2);

	rootMerge = new DefaultMutableTreeNode(mergingRoot);	     
	mergeTreeModel = new DefaultTreeModel(rootMerge);
	treeHandler = new TreeHandler(rootMerge, mergeTreeModel);
	
	findSameLevelDiff(rootNode1, rootNode2, rootMerge);
	return;

	/****************************************************/
    }
    

    protected void findSameLevelDiff(DefaultMutableTreeNode subRoot1, DefaultMutableTreeNode subRoot2, DefaultMutableTreeNode subMerge) {

	if ((subRoot1 == null) || (subRoot2 == null)) return; 

	LinkedList leftNodes_sub2 = new LinkedList(); // member is in type of DefaultMutableTreeNode

	// get second hierarchy nodes
	int root2childCnt = subRoot2.getChildCount();
	for (int i = 0; i < root2childCnt; i++)
	    leftNodes_sub2.add(subRoot2.getChildAt(i));

	// traverse first hierarchy in a top-down manner
	int root1childCnt = subRoot1.getChildCount();
	
	for (int i = 0; i < root1childCnt; i++) {
	    boolean match = false;
	    DefaultMutableTreeNode node1 = (DefaultMutableTreeNode)subRoot1.getChildAt(i);
	    Object node1UserObj = node1.getUserObject();

	    String node1Name = "";
	    LinkedList node1RunName = null; // indicate which run belonged

	    // get node1Name for comparison
	    if (!(node1UserObj instanceof MergingNode))
		node1Name = getObjName(node1UserObj);
	    else {
		node1Name = ((MergingNode)node1UserObj).getNodeName();
		node1RunName = ((MergingNode)node1UserObj).getMergingList();
	    }

	    // find if there exists a node in the second hierary with the same name
	    DefaultMutableTreeNode node2 = null;
	    int sub2Index = 0;
	    if (leftNodes_sub2.size() > 0)
		node2 = (DefaultMutableTreeNode)leftNodes_sub2.get(sub2Index);
	    
	    while (node2 != null) {
		Object node2UserObj = node2.getUserObject();

		String node2Name = "";
		LinkedList node2RunName = null;
		// get node2Name for comparison
		if (node2UserObj instanceof MergingNode) {
		    node2Name = ((MergingNode)node2UserObj).getNodeName();
		    node2RunName = ((MergingNode)node2UserObj).getMergingList();
		}
		else {
		    node2Name = getObjName(node2UserObj);
		}
		
		if (node2Name.equals(node1Name)) {
		    // match - they are common node, add to the merge tree
		    // make MergingNode
		    MergingNode mergingNode = new MergingNode(node1UserObj);
		    if (node1RunName != null)
			mergingNode.addToMergingList(node1RunName); // add a NameList
		    else
			mergingNode.addToMergingList(runName1); // add a String Name
		    if (node2RunName != null)
			mergingNode.addToMergingList(node2RunName); // add a NameList
		    else
			mergingNode.addToMergingList(runName2); // add a String Name

		    DefaultMutableTreeNode addMerge = treeHandler.addObject(subMerge, mergingNode);
		    // delete the match nodes from second hierarchy list
		    leftNodes_sub2.remove(node2);
		    sub2Index --;

		    findSameLevelDiff(node1, node2, addMerge);
		    match = true;
		    break;
		}
		else {
		    // get node2's sibling in the list to compare again
		    sub2Index ++;
		    if (sub2Index < leftNodes_sub2.size())
			node2 = node2.getNextSibling();
		    else node2 = null;
		}
	    } // end of "while"

	    if (!match) {
		// add node1 into the merge tree, also its subtree;
		MergingNode mergingNode = new MergingNode(node1UserObj);
		if (node1RunName != null)
		    mergingNode.addToMergingList(node1RunName);
		else
		    mergingNode.addToMergingList(runName1);
		
		DefaultMutableTreeNode addNode1 = treeHandler.addObject(subMerge, mergingNode);
		treeHandler.copySubtree(addNode1, node1);
	    }
	} // end of "for" - finish traverse subRoot1's first level

	// add subRoot2's non-matched/left nodes to mergeTree
	int root2size = leftNodes_sub2.size();

	for (int i = 0; i < root2size; i++) {
	    DefaultMutableTreeNode node2 = (DefaultMutableTreeNode)leftNodes_sub2.get(i);
	    // add node and runName info to mergeTree
	    Object node2UserObj = node2.getUserObject();
	    String node2Name = "";
	    LinkedList node2RunName = null;
	    // get node2Name for comparison
	    if (node2UserObj instanceof MergingNode) {
		node2Name = ((MergingNode)node2UserObj).getNodeName();
		node2RunName = ((MergingNode)node2UserObj).getMergingList(); }
	    else
		node2Name = getObjName(node2UserObj);

	    MergingNode mergingNode = new MergingNode(node2UserObj);
	    if (node2RunName != null)
		mergingNode.addToMergingList(node2RunName);
	    else
		mergingNode.addToMergingList(runName2);

	    DefaultMutableTreeNode addNode2 = treeHandler.addObject(subMerge, mergingNode);
	    treeHandler.copySubtree(addNode2, node2);
	
	} // end of "for" - finish traverse leftNodes_sub2
		    
    }

    protected String getObjName(Object obj) {
	if (obj instanceof String)
	    return (String)obj;
	else if (obj instanceof MarkedNode)
	    return ((MarkedNode)obj).getNodeName();
	else if (obj instanceof DataFile)
	    return ((DataFile)obj).getNodeNameList();
	else
	    return null;
    }
	


} //end of class

    
