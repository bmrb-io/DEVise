package xman.xnav;

import java.awt.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;

class SrcTreeRender extends DefaultTreeCellRenderer {


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

	if (!sel) {
	    tree.collapsePath(new TreePath(((DefaultMutableTreeNode)value).getPath()));
	}
	
	return this;
    }
}
