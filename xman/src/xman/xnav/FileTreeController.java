package xman.xnav;
import xman.xvis.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.lang.*;

class FileTreeController implements ActionListener {

    protected DefaultMutableTreeNode root;
    protected LinkedList allFilesList;
    protected TreeHandler treeHandler;
    protected MetricsController metricsController;
    protected IndexTreeController indexTreeController;
    protected JButton hideZeroButton;
    protected boolean hideZero;

    protected JFrame errorFrame;
    protected JLabel errorLabel;

    protected int thumbnailRow = 8;
    protected int thumbnailCol = 8;

    public static final boolean DEBUG = true; 


    // Constructor
    public FileTreeController(LinkedList allFilesList, TreeHandler treeHandler, IndexTreeController indexTreeController, JButton hideZeroButton) {
	this(null, allFilesList, treeHandler, null, indexTreeController,hideZeroButton);
    }
    
    // Constructor
    public FileTreeController(DefaultMutableTreeNode root, LinkedList allFilesList, TreeHandler treeHandler, MetricsController metricsController,  IndexTreeController indexTreeController, JButton hideZeroButton) {

	this.root = root;
	this.allFilesList = allFilesList;
	this.treeHandler = treeHandler;
	this.metricsController = metricsController;
	this.indexTreeController = indexTreeController;
	this.hideZeroButton = hideZeroButton;

	if (hideZeroButton != null) {
	hideZeroButton.addActionListener(this);
        hideZeroButton.setFocusPainted(false);
	}

	this.hideZero = false;

	errorFrame = new JFrame("Error Message");
	errorFrame.addWindowListener(new java.awt.event.WindowAdapter() {
	    public void windowClosing(java.awt.event.WindowEvent e) {
		errorFrame.dispose();
	    }
        });
        errorFrame.setSize(500, 80);
	errorLabel = new JLabel();
	errorFrame.getContentPane().add(errorLabel);
	errorFrame.setVisible(false);

    }

    public void setTreeHandler(TreeHandler treeHandler) {
	this.treeHandler = treeHandler;
    }

    public void setMetricsController(MetricsController metricsController) {
	this.metricsController = metricsController;
    }

    public void setIndexTreeController(IndexTreeController indexTreeController) {
	this.indexTreeController = indexTreeController;
    }

    public void setTreeRoot(DefaultMutableTreeNode root){
	this.root = root;
    }


    public DefaultMutableTreeNode getTreeRoot() {
	return root;
    }

    public LinkedList getFirstLevelChildrenList() {
	LinkedList theList = new LinkedList();
	if (root != null) {
	    int childenCnt = root.getChildCount();
	    for (int i = 0; i < childenCnt; i++) {
		DefaultMutableTreeNode child = (DefaultMutableTreeNode)root.getChildAt(i);
		theList.add(child);
	    }
	}
	return theList;
    }


    public LinkedList getAllFilesList() {
	return allFilesList;
    }


    // When a leaf node of file_tree is selected,
    // mark all the source_tree nodes associated with the leaf node
    public void nodeSel_MarkIndexTree(DefaultMutableTreeNode node, JTree indexTree, LinkedList markNodes) {
	if (node == root) return;

	traverseFileLeaf(node, indexTree, markNodes);
    }
    
    protected void traverseFileLeaf(DefaultMutableTreeNode node, JTree indexTree, LinkedList markNodes) {

	if (node.isLeaf())
	    fileLeafSelected(node, indexTree, markNodes);
	else { // traverse the subtree
	    int childenCnt = node.getChildCount();
	    for (int i = 0; i < childenCnt; i++) {
		DefaultMutableTreeNode child = (DefaultMutableTreeNode)node.getChildAt(i);
		traverseFileLeaf(child, indexTree, markNodes);
	    }
	}
    }

    protected void fileLeafSelected(DefaultMutableTreeNode node, JTree indexTree, LinkedList markNodes) {
	if (node == null) return;

	DataFile dataFile = (DataFile)node.getUserObject();
	LinkedList nodesList = dataFile.getNodeList();
	//System.out.println("nodeList is: " + dataFile.getNodeNameList());
	int len = nodesList.size();

	if (len == 0) { //whole program	
	    DefaultMutableTreeNode selectnode = node;
	    while (selectnode.getParent() != null)
		selectnode = (DefaultMutableTreeNode)selectnode.getParent();
	    // now selectnode is root
	    TreePath treePath = new TreePath(selectnode.getPath());	
	    indexTree.addSelectionPath(treePath);
	    markNodes.add(selectnode);
	}
	else {
	    for (int i = 0; i < len; i++) {
		DefaultMutableTreeNode selectnode = 
		    (DefaultMutableTreeNode)nodesList.get(i);
	    
		if (selectnode != null) {
		    TreePath treePath = new TreePath(selectnode.getPath());	
		    indexTree.addSelectionPath(treePath);
		    markNodes.add(selectnode);
		} 
	    }
	}
	// not select indexTree Nodes, but just mark them
	indexTree.clearSelection();
    }
   
    
    public void nodeSel_MarkMetrics(DefaultMutableTreeNode[] node, JPanel buttonPanel) {
	//LinkedList colorList = new LinkedList();
	for (int i = 0; i< node.length; i++) {
	    DefaultMutableTreeNode parent;
	    parent = node[i];
	    if (parent.getParent() == null) continue;
	    while (parent.getParent().getParent() != null)
		parent = (DefaultMutableTreeNode) parent.getParent();
		    
	    // Now the parent is the measure node
	    selectButton(parent, buttonPanel);
	}

    }
    
    protected void selectButton(Object obj, JPanel buttonPanel) {
	Component [] buttons = buttonPanel.getComponents();
	if (buttons == null) return;
	for (int i = 0; i < buttons.length; i++) {
	    JButton button = (JButton)buttons[i];
	    if (button == null) continue;
	    // clear selections
	    button.setSelected(false);
	    //System.out.println("Buttons["+i+"] is :"+buttons[i]);
	    if (button.getText().equals(obj.toString()))
		markButton(button);
	}
    }
	
    public void markButton(JButton button) {
	//button.setSelected(true);
	// Green color
	button.setBackground(new Color(204,255,204));
    }

   public void refreshFileTree(LinkedList filesList){
	if (DEBUG)
	    System.out.println("START: RefreshFileTree  in FileTreeController");

	indexTreeController.getRenderer().setColorList(null);

	if (hideZero) {
	    // get ride of zero data and update the filesList
	    System.out.println("Zero file should be hide");
	    LinkedList tmpList = new LinkedList();
	    for (int i = 0; i < filesList.size(); i++) {
		DataFile datafile = (DataFile)filesList.get(i);
		if (datafile.getZeroDataCheck() == -1) {
		    // set thumbnail icon for data
		    String fullFName = datafile.getFullFileName();
		    int [] thumbnails = new int [thumbnailCol];
		    // Call Omer's class to get data
		    Inquirer inq = new Inquirer();
		    
		    try {
			inq.lookUp(new File(fullFName), thumbnails, thumbnailCol, thumbnailRow);
		    } catch (YException e) {}
		    ThumbNailIcon createIcon = new ThumbNailIcon(thumbnails, thumbnailRow);
		    ImageIcon thumbIcon = createIcon.getThumbNailIcon();
		    datafile.setThumbnailIcon(thumbIcon);
		    datafile.setZeroDataCheck(createIcon.getZeroCheck());
		}

		if (datafile.getZeroDataCheck() == 1) {
		    // add the data to tmpList
		    tmpList.add(datafile);
		}
	    }
	    filesList = tmpList;
	}


	FilesToTree filesToTree = new FilesToTree(filesList);
	DefaultMutableTreeNode fileTreeRoot = filesToTree.getTreeRoot();
	if (fileTreeRoot == null) return;

	setTreeRoot(fileTreeRoot);

    }

    // get all selected file Names
    public String getSelectedFileNames() {
	LinkedList fileNames = new LinkedList();
	LinkedList focusNames = new LinkedList();
	String filenames = "";

	if (root != null)
	    traverseFileLeafName(root, fileNames, focusNames);
	else 
	    return null;
	

	if (fileNames.size() == 0) return null;
	
	int len = fileNames.size();
	for (int i = 0; i < len; i++){
	    filenames += (String)fileNames.get(i);
	    if (i < len - 1) {
		String thisFocusName = (String)focusNames.get(i);
		String nextFocusName = (String)focusNames.get(i+1);
		if (thisFocusName.equals(nextFocusName)) 
		    filenames += ":";
		else
		    filenames += ";";
	    }
	}

	return filenames;
    }


    protected void traverseFileLeafName(DefaultMutableTreeNode node, LinkedList fileNames, LinkedList focusNames) {
	if (node.isLeaf() && !(node.getUserObject() instanceof DataFile)) return;

	if (node.isLeaf()) {
	    fileNames.add(((DataFile)node.getUserObject()).getFullFileName());
	    focusNames.add(((DataFile)node.getUserObject()).getNodeNameList() + ((DataFile)node.getUserObject()).getMeasure());
	}
	else { // traverse the subtree
	    int childenCnt = node.getChildCount();
	    for (int i = 0; i < childenCnt; i++) {
		DefaultMutableTreeNode child = (DefaultMutableTreeNode)node.getChildAt(i);
		traverseFileLeafName(child, fileNames, focusNames);
	    }
	}
    }
    
    
    public void actionPerformed(java.awt.event.ActionEvent e) {
	//tree.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	if (DEBUG) System.err.println("ACTION: "+e.getActionCommand()+", "+e.paramString());

	String command = e.getActionCommand();

	if (command.equals("Hide Zero Data")) 
	    hideZero = !hideZero;
    } 

    public boolean isHideZeroData() {
	return hideZero;
    }

}
    
