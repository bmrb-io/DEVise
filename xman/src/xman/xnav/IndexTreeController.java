package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.lang.*;

class IndexTreeController implements MouseListener, MouseMotionListener{

    public static final boolean DEBUG = true;

    protected DefaultMutableTreeNode root;
    protected JTree tree;
    protected IndexTreeRender treeRender;
    protected FileTreeController fileTreeController;
    protected FocusTreeController focusTreeController;
    protected TreeHandler treeHandler;
    protected MetricsController metricsController;
    protected Cursor defaultCursor;
    protected int currentTreeRowCnt;
    protected boolean dragged; // detect mouse drag
    protected JTabbedPane tabbedPane;

    // Constructor
    public IndexTreeController(){
	this(null, null, null, null, null, null, null);
    }

    public IndexTreeController(JTree tree, DefaultMutableTreeNode root,
			       TreeHandler treeHandler) {
	this(tree, root, treeHandler, null, null, null, null);
    }

    // Constructor
    public IndexTreeController(JTree tree, DefaultMutableTreeNode root, 
			       TreeHandler treeHandler,
			       MetricsController metricsController,
			       FileTreeController fileTreeController){
	this(tree, root, treeHandler, metricsController, fileTreeController, null,null);
    }

    // Constructor
    public IndexTreeController(JTree tree, DefaultMutableTreeNode root, 
			       TreeHandler treeHandler,
			       JTabbedPane tabbedPane){
	this(tree, root, treeHandler, null, null, tabbedPane, null);
    }

    // Constructor
    public IndexTreeController(JTree tree, DefaultMutableTreeNode root, 
			       TreeHandler treeHandler,
			       MetricsController metricsController,
			       FileTreeController fileTreeController,
			       JTabbedPane tabbedPane, FocusTreeController focusTreeController) {
	this.tree = tree;
	this.root = root;
	this.treeRender = new IndexTreeRender(tabbedPane);
	this.treeHandler = treeHandler;
	this.metricsController =metricsController;
	this.fileTreeController = fileTreeController;
	this.focusTreeController = focusTreeController;
	this.defaultCursor = Cursor.getDefaultCursor();
	this.currentTreeRowCnt = 0;
	this.dragged = false;
	this.tabbedPane = tabbedPane;
    }

    public void setMetricsController(MetricsController metricsController) {
	this.metricsController = metricsController;
    }

    public void setTreeHandler(TreeHandler treeHandler) {
	this.treeHandler = treeHandler;
    }

    public void setFileTreeController(FileTreeController fileTreeController) {
	this.fileTreeController = fileTreeController;
    }

    public void setFocusTreeController(FocusTreeController focusTreeController) {
	this.focusTreeController = focusTreeController;
    }

    public void setTreeRoot(DefaultMutableTreeNode root){
	this.root = root;
    }

    public void setTreeRenderer(IndexTreeRender treeRender) {
	this.treeRender = treeRender;
    }

    public void setTree(JTree tree) {
	this.tree = tree;
    }

    public JTree getTree() {
	return tree;
    }

    public DefaultMutableTreeNode getTreeRoot() {
	return root;
    }

    public IndexTreeRender getRenderer () {
	return treeRender;
    }

    public void configTree() {
	if (root == null) {
	    tree.setModel(null);
	    return; }

	DefaultTreeModel treeModel = new DefaultTreeModel(root);
	tree.getSelectionModel().setSelectionMode
	    (TreeSelectionModel.DISCONTIGUOUS_TREE_SELECTION);
	
	tree.setModel(treeModel);
	tree.setCellRenderer(treeRender);
	tree.updateUI();
	// Listen for when the selection changes, call nodeSelected(node)
	// Mouse listen of selection to refresh file_tree.
	// avoid multiple register of listeners
	tree.removeMouseListener(this);
	tree.removeMouseMotionListener(this);

	tree.addMouseListener(this);
	tree.addMouseMotionListener(this);
	tree.addTreeSelectionListener(
					new TreeSelectionListener() {
	    public void valueChanged(TreeSelectionEvent e) {
	    }
	} );
    }

    public void buildNodesRelations(LinkedList allFilesList) {
	DataNetWork network = new DataNetWork(allFilesList);
	network.buildNetWork();
    }
    
    public LinkedList nodeSel_DataSetList(LinkedList allFilesList, DefaultMutableTreeNode[] node) {
	
	int len = allFilesList.size();
	if (len == 0) //No data files associated with performance data
	    return null;

	// Update file_tree
	LinkedList selFileList = new LinkedList();
	for (int i = 0; i < len; i++) {
	    DataFile datafile = (DataFile)allFilesList.get(i);
	    if (datafile == null) continue;
	    boolean foundAllnode = true;
	    for (int j = 0; j < node.length; j++)
		foundAllnode = foundAllnode && datafile.findANode(node[j]);
	    
	    if (foundAllnode) {
		selFileList.add(datafile);
		//System.out.println("selFileList: "+ datafile.getNodeNameList());
	    }
	}

	return selFileList;	
    }

    public LinkedList nodeSel_MarkIndexTree(DefaultMutableTreeNode[] node) {
	//set selection color as blue
	//treeRender.setBackgroundSelectionColor(new Color(204, 204, 255));

	// Mark index_tree
	tree.clearSelection();
	tree.updateUI();
	LinkedList allMarkedList = new LinkedList();
	for (int j=0; j<node.length; j++) {
	    // Mark selected nodes selection
	    TreePath treePath = new TreePath(node[j].getPath());
	    tree.addSelectionPath(treePath);

	    LinkedList processList = new LinkedList();
	    Object obj = node[j].getUserObject();
	    if (obj instanceof MergingNode)
		obj = ((MergingNode)obj).getUserObject();

	    if (obj instanceof MarkedNode) {
		LinkedList markedList = ((MarkedNode)obj).getNodeList();
		for (int i = 0; i < markedList.size(); i++) {
		    DefaultMutableTreeNode marknode = 
			(DefaultMutableTreeNode)markedList.get(i);
		    if (marknode != null) {
			treePath = new TreePath(marknode.getPath());
			// add then remove is just for UI since no otherway
			// though it looks weird
			tree.addSelectionPath(treePath);
			if (!selected(marknode, node))
			    tree.removeSelectionPath(treePath);
			// add the marknode to list
			if (j == 0)
			    processList.add(marknode);
			else {
			    // only mark the common nodes within selected nodes
			    if (selected(marknode, allMarkedList))
				processList.add(marknode);
			}
		    }
		}
	    }
	    allMarkedList = processList;
	}
	//treeRender.setColorList(allMarkedList);
	return allMarkedList;
    }


    public LinkedList nodeSel_MarkIndexTree(DefaultMutableTreeNode[] node, String [] metricsNames) {
	//set selection color as blue
	//treeRender.setBackgroundSelectionColor(new Color(204, 204, 255));

	// Mark index_tree
	tree.clearSelection();
	tree.updateUI();
	LinkedList allMarkedList = new LinkedList();
	for (int j=0; j<node.length; j++) {
	    // Mark selected nodes selection
	    TreePath treePath = new TreePath(node[j].getPath());
	    tree.addSelectionPath(treePath);

	    LinkedList processList = new LinkedList();
	    Object obj = node[j].getUserObject();
	    if (obj instanceof MergingNode)
		obj = ((MergingNode)obj).getUserObject();

	    if (obj instanceof MarkedNode) {
		LinkedList markedList = ((MarkedNode)obj).getNodeList();
		LinkedList measureList = ((MarkedNode)obj).getMeasureList();

		for (int i = 0; i < markedList.size(); i++) {
		    DefaultMutableTreeNode marknode = 
			(DefaultMutableTreeNode)markedList.get(i);
		    // check if the marknode has selected metrics related to 
		    // the selected node
		    boolean cont = false;
		    for (int k = 0; k< metricsNames.length; k++) {
			if (((MarkedNode)obj).isInNodeMeasureList(metricsNames[k], marknode)) {
			    cont = true;
			    break;
			}
		    }
		    // if not match metrics, get another marknode    
		    if (!cont) continue;

		    if (marknode != null) {
			treePath = new TreePath(marknode.getPath());
			// add then remove is just for UI since no otherway
			// though it looks weird
			tree.addSelectionPath(treePath);
			if (!selected(marknode, node))
			    tree.removeSelectionPath(treePath);
			// add the marknode to list
			if (j == 0)
			    processList.add(marknode);
			else {
			    if (selected(marknode, allMarkedList))
				processList.add(marknode);
			}
		    }
		}
	    }
	    allMarkedList = processList;
	}
	//treeRender.setColorList(allMarkedList);
	return allMarkedList;
    }


    protected boolean selected(DefaultMutableTreeNode checknode, DefaultMutableTreeNode[] nodeArray) {
	boolean select = false;
	for (int i=0; i< nodeArray.length; i++) {
	    if (checknode == nodeArray[i]){
		select = true;
		break;
	    }
	}

	return select;
    }

    protected boolean selected(DefaultMutableTreeNode checknode, LinkedList list) {
	boolean select = false;
	for (int i=0; i< list.size(); i++) {
	    if (checknode == (DefaultMutableTreeNode)list.get(i)) {
		select = true;
		break;
	    }
	}

	return select;
    }
	    
    public void nodeSel_MarkMetrics(DefaultMutableTreeNode fileTreeRoot, JPanel buttonPanel) {
	
	// Mark List/Metrics
	int cnt = fileTreeRoot.getChildCount();
	for (int i = 0; i< cnt; i++) {
	    TreeNode colornode = fileTreeRoot.getChildAt(i);
	    selectButton(colornode, buttonPanel);
	}
    }

    protected void selectButton(Object obj, JPanel buttonPanel) {
	Component [] buttons = buttonPanel.getComponents();
	if (buttons == null) return;
	for (int i = 0; i < buttons.length; i++) {
	    JButton button = (JButton)buttons[i];
	    if (button == null) continue;
	    button.setSelected(false);
	    //System.out.println("Buttons["+i+"] is :"+buttons[i]);
	    if (button.getText().equals(obj.toString()))
		markButton(button);
	}
    }

    public void markButton(JButton button) {
	//button.setSelected(true);
	// Green color
	button.setBackground(new Color(204, 255, 204));
    }


    public void mouseDragged(MouseEvent event) {
	//System.out.println("ACTION: Mouse dragged - "+event.paramString());
	dragged = true;
    }

    public void mouseMoved(MouseEvent event) {}

    // When click index_tree
    public void mouseReleased(MouseEvent event) {
	
	System.out.println("ACTION: "+event.paramString());

	try {
	    Object obj = event.getSource();
	    if ((obj == null) || (!(obj instanceof JTree))) {
		// set dragged back to false if it turned to be true
		dragged = false;
		return;
	    }

	    int treeRowCnt = tree.getRowCount();
	    // when expand tree, no need to change
	    // but collapse tree cause selection change,
	    // so need to continue
	    if (treeRowCnt > currentTreeRowCnt) {
		currentTreeRowCnt = treeRowCnt;
		dragged = false;
		return;
	    }

	    // set waiting cursor while process
	    getTree().setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

	    // No implementation of drag information,
	    // because it doesn't look very useful here,
	    // moveover, click in tree couldn't tell exactly which
	    // treeNode has been clicked, getSource and getComponent
	    // can only get JTree

	    int clickmode; // 0 is single, 1 with Shift/Control
	    // check if Ctrl- or Shift- Select with Metrics
	    if (event.isControlDown() || event.isShiftDown()) {
		int selectMetrics = metricsController.getSelectedNumber();
		
		//Object [] selNodesInMetrics = metricsController.getSelectedNodes();
		if (selectMetrics == 0) { //only select indexTree
		    clickmode = 0;
		}
		else { // selected both
		    clickmode = 1;
		}
	    }
	    else {
		clickmode = 0;
	    }

	    DefaultMutableTreeNode []currentNodeIndexTree = treeHandler.getAllSelectedNodes(getTree());
	    if (currentNodeIndexTree == null) {
		if (clickmode == 1) {
		    // only metrics has been selected
		    metricsController.metricsSelectionAction();
		}
		else {
		    // no any selection , could be disselect
		    System.out.println("No indexTree selections found.");
		    metricsController.clearAllSelections();
		    getRenderer().setColorList(null);
		    getTree().updateUI();
		    fileTreeController.refreshFileTree(fileTreeController.getAllFilesList());
		    focusTreeController.refreshFocusTree(null);
		    // set back default cursor
		    getTree().setCursor(defaultCursor);
		    dragged = false;
		    return; }
	    }
	    else {
		getRenderer().setColorList(null);
		indexNodeSelected(currentNodeIndexTree, clickmode); 
	    }
	    
	 
	}catch (Exception ex){dragged = false;}

	// set dragged back to false if it turned to be true
	dragged = false;


	// Display Devise if in Display mode
	if (focusTreeController != null)
	    if (focusTreeController.isInDisplayMode())
		focusTreeController.displayModeSelection();

	// set back default cursor
	getTree().setCursor(defaultCursor);

    }
    public void mousePressed(MouseEvent event) {}
    public void mouseClicked(MouseEvent event) {}
    public void mouseEntered(MouseEvent event) {
	currentTreeRowCnt = tree.getRowCount();
    }
    public void mouseExited(MouseEvent event) {}

 
    // When a node of index_tree is selected,
    // update file_tree to show all the performance files
    // associated with the node
    void indexNodeSelected(DefaultMutableTreeNode[] node, int clickmode) {

	// Update file_tree
	LinkedList selFileList; 
	if (clickmode == 0) { // single click
	    selFileList = nodeSel_DataSetList(fileTreeController.getAllFilesList(), node); }
	else
	    selFileList = nodeSel_DataSetList(metricsController.getSelectedDataFileList(), node);

	if (selFileList.size() == 0) {
	    fileTreeController.refreshFileTree(null);
	    focusTreeController.refreshFocusTree(null);
	}
	else {
	    fileTreeController.refreshFileTree(selFileList);
	    focusTreeController.refreshFocusTree(selFileList);
	}

	LinkedList secondColor;
	// Mark index_tree
	if (clickmode == 0) {
	    secondColor = nodeSel_MarkIndexTree(node);
	    getRenderer().setColorList(secondColor);
	}
	else {
	    String metricsNames [] = metricsController.getSelectedMetricsNames();
	    secondColor = nodeSel_MarkIndexTree(node, metricsNames);
	    getRenderer().setColorList(secondColor);
	}

	// blue mark index_tree if only one focus selected
	String filenames = fileTreeController.getSelectedFileNames();
	if ((filenames != null) && (filenames.indexOf(";") < 0)) {
	    for (int i = 0; i < secondColor.size(); i++) {
		    DefaultMutableTreeNode marknode = 
			(DefaultMutableTreeNode)secondColor.get(i);
		    if (marknode != null) {
			TreePath treePath = new TreePath(marknode.getPath());
			tree.addSelectionPath(treePath);
		    }
	    }
	}
	
	// RefreshMetrics
	if (fileTreeController.getTreeRoot() != null) {
	    if (clickmode == 0) {
		metricsController.clearAllSelections();
		nodeSel_MarkMetrics(fileTreeController.getTreeRoot(), metricsController.getButtonPanel());
	    }
	    // metricsController.getRenderer().setColorList(colorList);
	}
	else {
	    metricsController.clearAllSelections();
	    if (DEBUG)
		System.out.println("file_tree doesn't have any data.");
	}
	// metricsController.getMetrics().updateUI();
    }
    
}
