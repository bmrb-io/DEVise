package xman.xnav;

import java.awt.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;

class MergingTreeRender extends DefaultTreeCellRenderer {
    protected JTabbedPane tabbedPane;

    public MergingTreeRender(JTabbedPane tabbedPane) {
	this.tabbedPane = tabbedPane;
    }

    public Component getTreeCellRendererComponent(
						  JTree tree,
						  Object value,
						  boolean sel,
						  boolean expanded,
						  boolean leaf,
						  int row,
						  boolean hasFocus) {

	
	super.getTreeCellRendererComponent(
					   tree, value, sel,
					   expanded, leaf, row,
					   hasFocus);
	
	// get defaultIcons
	Icon nonLeafIcon = getDefaultOpenIcon();
	Icon leafIcon = getDefaultLeafIcon();

	if (((DefaultMutableTreeNode)value).getUserObject() instanceof MergingNode) {
	    MergingNode mergingNode = (MergingNode)(((DefaultMutableTreeNode)value).getUserObject());
	    String cellText = mergingNode.getNodeName();
	    String tipText = mergingNode.getListName();
	    if (tipText != "")
		setToolTipText(tipText);
	    else setToolTipText(null);
	    setText(cellText);
	    ImageIcon mergeIcon = (new RoundIcon(mergingNode, tabbedPane)).getMergeIcon();
	    setIcon(mergeIcon);
	}
	else {
	    Object obj = ((DefaultMutableTreeNode)value).getUserObject();
	    if (obj instanceof MarkedNode) {
		String cellText = ((MarkedNode)obj).getNodeName();
		setText(cellText);
	    }
	
	    if (leaf)
		setIcon(leafIcon);
	    else
		setIcon(nonLeafIcon);
	    setToolTipText(null);
	}
	return this;
    }
}
