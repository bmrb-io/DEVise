package xman.xnav;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;

public class TreeHandler {
    protected DefaultMutableTreeNode rootNode;
    protected DefaultTreeModel treeModel;

    public TreeHandler() {
	this.rootNode = null;
	this.treeModel = null;
    }

    public TreeHandler(DefaultMutableTreeNode rootNode, DefaultTreeModel treeModel) {
	this.rootNode = rootNode;
        this.treeModel = treeModel;
    }

    // Remove all nodes except the root node 
    public void clear() {
        rootNode.removeAllChildren();
        treeModel.reload();
    }

    // Remove a node
    public void removeANode(DefaultMutableTreeNode currentNode) {

	MutableTreeNode parent = (MutableTreeNode)(currentNode.getParent());
	if (parent != null) {
	    treeModel.removeNodeFromParent(currentNode);
	    return;
	}
    }

    // Add child to a node
    public DefaultMutableTreeNode addObject(Object child) {
        return addObject(rootNode, child);
    }

    public DefaultMutableTreeNode addObject(DefaultMutableTreeNode parent,
                                            Object child) {
        DefaultMutableTreeNode childNode = 
                new DefaultMutableTreeNode(child);

        if (parent == null) {
            parent = rootNode;
        }

        treeModel.insertNodeInto(childNode, parent, parent.getChildCount());
        return childNode;
    }

    // the subtree has the same level root as parent
    public void copySubtree(DefaultMutableTreeNode parent, DefaultMutableTreeNode subtree) {
	if ((subtree == null) || (parent == null)) return;

	// traverse the subtree and add to parent one by one
	int childCnt = subtree.getChildCount();
	for (int i = 0; i < childCnt; i++) {
	    DefaultMutableTreeNode subtreeChild = (DefaultMutableTreeNode)subtree.getChildAt(i);
	    if (subtreeChild == null) continue;
	    Object obj = subtreeChild.getUserObject();
	    DefaultMutableTreeNode child = addObject(parent, obj);
	    copySubtree(child, subtreeChild);
	}
    }

    // Check if a given nodeName is in the first level of the subtree 
    // and retrieve the treeNode if yes
    public DefaultMutableTreeNode hitChildren(String nodeName, 
					      DefaultMutableTreeNode subTreeNode) {
	if ((nodeName == null) || (nodeName == "") || (subTreeNode == null)) {
	    System.out.println("ERROR: Invalid nodeName or subTreeNode given");
	    return null;
	}
	int childrenCnt = subTreeNode.getChildCount();
	DefaultMutableTreeNode hitNode = null;
	for (int i = 0; i < childrenCnt; i++) {
	    DefaultMutableTreeNode children = (DefaultMutableTreeNode)subTreeNode.getChildAt(i);
	    Object obj = children.getUserObject();
	    String compareName;
	    if (obj instanceof MarkedNode)
		compareName = ((MarkedNode)obj).getNodeName();
	    else if (obj instanceof FocusNode)
		compareName = ((FocusNode)obj).getNodeNameList();
	    else
		compareName = obj.toString();

	    if (nodeName.compareTo(compareName) == 0) {
		hitNode = children;
		break;
	    }
	}
	return hitNode;
    }
	    
    // Return root node
    public DefaultMutableTreeNode getTreeRootNode() {
	return rootNode;
    }

    // Return tree model
    public DefaultTreeModel getTreeModel() {
	return treeModel;
    }

    public void collapseTree(JTree tree) {
	int rows = tree.getRowCount();
	for (int i = rows-1; i >= 0; i--) {
	    tree.collapseRow(i);
            }
    }

    public void expandTree(JTree tree) {
        int rows = tree.getRowCount();
	for (int i = 0; i < rows; i++)
	    tree.expandRow(i);
	int exprows = tree.getRowCount();
	if (exprows > rows)
	    expandTree(tree);
    }

    public void expandOneLevel(JTree tree) {
	tree.expandRow(0);
    }

    public JTree createJTree() {
        JTree tree = new JTree();
        if (tree == null)
            return null; // should never happen.

	tree.setAutoscrolls(true);
        tree.setRowHeight(24);

	//Enable tool tips.
        ToolTipManager.sharedInstance().registerComponent(tree);
        return tree;

    }

    // traverse the tree and get all Childrens's userObject into list
    // for file_tree, get DataFiles, 
    public void addChildrenToList(DefaultMutableTreeNode node, LinkedList linkedlist) {
	if (node == null) return;

	int childrenCnt = node.getChildCount();
	for (int i = 0; i < childrenCnt; i++) {
	    DefaultMutableTreeNode child = (DefaultMutableTreeNode)node.getChildAt(i);
	    if (child == null) continue;
	    Object obj = child.getUserObject();
	    if (obj != null)
		linkedlist.add(obj);
	    addChildrenToList(child, linkedlist);
	}
    }

    public DefaultMutableTreeNode getSelectedNode(JTree tree) {
	TreePath currentSelection = tree.getSelectionPath();
	if (currentSelection == null) return null;
	
	DefaultMutableTreeNode currentNode = (DefaultMutableTreeNode)
	    (currentSelection.getLastPathComponent());
	return currentNode;
    }

     public DefaultMutableTreeNode [] getAllSelectedNodes(JTree tree) {
	 try { // some weird class cast exception may happen
	     if (tree.getSelectionPaths() == null){
		 //System.out.println("TreeHandler return NULL - no selection");
		 return null;}
	     
	     TreePath []currentSelection = tree.getSelectionPaths();
	     
	     DefaultMutableTreeNode [] currentNode = new DefaultMutableTreeNode[currentSelection.length];
	     for (int i = 0; i < currentSelection.length; i++) {
		 currentNode[i] = (DefaultMutableTreeNode)
		     (currentSelection[i].getLastPathComponent());
	     }
	     return currentNode;
	 }catch (Exception ex) { return null;}
     }
    

}
