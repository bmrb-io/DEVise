package xman.xnav;

import java.awt.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;

class IndexTreeRender extends DefaultTreeCellRenderer {

    protected LinkedList secondColorList;
    protected Color defaultBackground = Color.white;
    protected JTabbedPane tabbedPane;
 
    // green color
    protected Color coloredBackground = new Color(204,255, 204);

    // Constructor
    public IndexTreeRender() {
	this(null, null);
    }

    // Constructor
    public IndexTreeRender(JTabbedPane tabbedPane) {
	this(null, tabbedPane);
    }

    // Constructor
    public IndexTreeRender(LinkedList secondColorList) {
	this(secondColorList, null);
    }

    public IndexTreeRender(LinkedList secondColorList, JTabbedPane tabbedPane) {
	this.secondColorList = secondColorList;
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

	if (((DefaultMutableTreeNode)value).getUserObject() instanceof MarkedNode) {
	    MarkedNode markednode = (MarkedNode)(((DefaultMutableTreeNode)value).getUserObject());
	    String cellText = markednode.getNodeName();
	    setText(cellText);

	    setForeground(Color.black);
	    setFont(new Font("Helvetica", Font.BOLD, 12));
	    setToolTipText("This node has performance data related");

	    if (leaf)
		setIcon(leafIcon);
	    else
		setIcon(null);
	}
	else if (((DefaultMutableTreeNode)value).getUserObject() instanceof MergingNode) {
	    MergingNode mergingnode = (MergingNode)(((DefaultMutableTreeNode)value).getUserObject());
	    String cellText = mergingnode.getNodeName();
	    String tipText = mergingnode.getListName();
	    if (tipText != "")
		setToolTipText(tipText);
	    else setToolTipText(null);
	    setText(cellText);
	    
	    // set merging icons
	    ImageIcon mergeIcon = (new RoundIcon(mergingnode, tabbedPane)).getMergeIcon();
	    setIcon(mergeIcon);

	    if (mergingnode.getUserObject() instanceof MarkedNode) {
		setForeground(Color.black);
		setFont(new Font("Helvetica", Font.BOLD, 12));
	    }
	    else { 
		setForeground(Color.gray);
		setFont(new Font("Helvetica", Font.PLAIN, 12));
	    }
	}
	else {
	    setToolTipText(null);
	    setForeground(Color.gray);
	    setFont(new Font("Helvetica", Font.PLAIN, 12));
	    if (leaf)
		setIcon(leafIcon);
	    else
		setIcon(null);

	}

	//System.out.println("selectionColor: "+getBackgroundSelectionColor());
	if (secondColorList != null) {
	    if (secondColorList.contains(value))
		setBackgroundNonSelectionColor(coloredBackground);
	    else 
		setBackgroundNonSelectionColor(defaultBackground);
	}
	else setBackgroundNonSelectionColor(defaultBackground);
	

	return this;
    }

    public void setColorList(LinkedList secondColorList) {
	this.secondColorList = secondColorList;
    }

}
