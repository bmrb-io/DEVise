package xman.xnav;
import xman.xvis.*;
import java.awt.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;

class FileTreeRender extends DefaultTreeCellRenderer {

    protected int thumbnailCol = 8;
    protected int thumbnailRow = 8;

    protected Inquirer inq;

    public FileTreeRender() {
	inq = new Inquirer();
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
	if (!leaf)
	    setIcon(null);

	//System.out.println("Class: " + ((DefaultMutableTreeNode)value).getUserObject().getClass().getName());

	if (((DefaultMutableTreeNode)value).getUserObject() instanceof FocusNode) {
	    FocusNode focusnode = (FocusNode)(((DefaultMutableTreeNode)value).getUserObject());
	    String cellText = focusnode.getNodeNameList();

	    if (!leaf) {
		int index = ((DefaultMutableTreeNode)value).getParent().getIndex((DefaultMutableTreeNode)value);
		setText("Focus (" + (index+1) + ")" );
		setToolTipText(cellText); 
	    }
	    else {
		TreeNode parent = ((DefaultMutableTreeNode)value).getParent();
		if (parent.getChildCount() > 1) {
		    int subindex = parent.getIndex((DefaultMutableTreeNode)value);
		    int index = parent.getParent().getIndex(parent);
		    setText((String)(focusnode.getMeasures().get(0))+" (" + (index+1) +"." + (subindex+1) + ")");
		}
		else  setText((String)(focusnode.getMeasures().get(0)));
		setToolTipText(focusnode.getFNames());    
	    }

	    // only leave has thumbnail
	    if (leaf) {
		if (focusnode.getThumbnailIcon() == null) {
		    // set thumbnail icon for data
		    String fullFName = focusnode.getFNames();
		    int [] thumbnails = new int [thumbnailCol];
		    // Call Omer's class to get data
		    // thumbnails = OmerData(fullFName, thumbnailCol, thumbnailRow);
		
		    try {
			inq.lookUp(fullFName, thumbnails, thumbnailCol, thumbnailRow);
		    } catch (YException e) {}
		    ThumbNailIcon createIcon = new ThumbNailIcon(thumbnails, thumbnailRow);
		    ImageIcon thumbIcon = createIcon.getThumbNailIcon();
		    setIcon(thumbIcon);

		    focusnode.setThumbnailIcon(thumbIcon);
		    focusnode.setZeroDataCheck(createIcon.getZeroCheck());
		
		}
		else {
		    setIcon(focusnode.getThumbnailIcon());
		}
	    }
       
	}
	else {
	    DefaultMutableTreeNode node = (DefaultMutableTreeNode)value;
	    if (node.getParent() != null) {
		String cellText = (String)node.getUserObject() + " ("+ node.getChildCount()+")";
		setText(cellText);
	    }
	    setToolTipText(null);
	}
	return this;
    }
}
