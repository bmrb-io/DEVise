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

class FocusTreeController implements ActionListener, MouseListener{

    protected DefaultMutableTreeNode root;
    protected JTree tree;
    protected LinkedList allFilesList;
    protected FileTreeRender focusTreeRender;

    protected TreeHandler treeHandler;
    protected MetricsController metricsController;
    protected IndexTreeController indexTreeController;
    protected FileTreeController fileTreeController;
    protected Cursor defaultCursor;
    protected JButton displayButton;
    protected JButton hideZeroButton;
    protected boolean displayMode;
    protected boolean hideZero;
    protected int currentTreeRowCnt;

    protected JFrame errorFrame;
    protected JLabel errorLabel;

    protected int portNumber; //default # is 12345
    protected String machineName = "chocolate";
    protected int thumbnailRow = 8;
    protected int thumbnailCol = 8;
    protected XVisClient client;

    public static final boolean DEBUG = true; 


    // Constructor
    public FocusTreeController(JTree tree, LinkedList allFilesList, TreeHandler treeHandler, IndexTreeController indexTreeController, int portNumber, String machineName, FileTreeController fileTreeController) {
	this(tree, null, allFilesList, treeHandler, null, indexTreeController, null,null, portNumber, machineName, fileTreeController);
    }

    // Constructor
    public FocusTreeController(JTree tree, LinkedList allFilesList, TreeHandler treeHandler, IndexTreeController indexTreeController, JButton displayButton, JButton hideZeroButton, int portNumber, String machineName, FileTreeController fileTreeController) {
	this(tree, null, allFilesList, treeHandler, null, indexTreeController, displayButton, hideZeroButton, portNumber, machineName, fileTreeController);
    }

    // Constructor
    public FocusTreeController(JTree tree, DefaultMutableTreeNode root, LinkedList allFilesList, TreeHandler treeHandler, MetricsController metricsController,  IndexTreeController indexTreeController, JButton displayButton, JButton hideZeroButton, int portNumber, String machineName, FileTreeController fileTreeController) {
	this.tree = tree;
	this.root = root;
	this.allFilesList = allFilesList;
	this.focusTreeRender = new FileTreeRender();

	this.treeHandler = treeHandler;
	this.metricsController = metricsController;
	this.indexTreeController = indexTreeController;
	this.fileTreeController = fileTreeController;
	this.defaultCursor = Cursor.getDefaultCursor();
	this.displayButton = displayButton;
	this.hideZeroButton = hideZeroButton;
	if (displayButton != null) {
	    displayButton.addActionListener(this);
	    displayButton.setFocusPainted(false);
	}

	if (hideZeroButton != null) {
	    hideZeroButton.addActionListener(this);
	    hideZeroButton.setFocusPainted(false);
	}

	this.displayMode = false;
	this.hideZero = false;
	this.portNumber = portNumber;
	this.machineName = machineName;

	currentTreeRowCnt = 0;

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

	client = null;
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

    public void setTree(JTree tree) {
	this.tree = tree;
    }

    public FileTreeRender getRenderer () {
	return focusTreeRender;
    }

    public JTree getTree() {
	return tree;
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

    public void configTree() {
	if (root == null) {
	    tree.setModel(null);
	    return; }

	DefaultTreeModel treeModel = new DefaultTreeModel(root);
	tree.setModel(treeModel);
	tree.setCellRenderer(focusTreeRender);
	tree.getSelectionModel().setSelectionMode
            (TreeSelectionModel.DISCONTIGUOUS_TREE_SELECTION);
	tree.addTreeSelectionListener(
				       new TreeSelectionListener() {
	    public void valueChanged(TreeSelectionEvent e) {
	    }
	} );
	tree.removeMouseListener(this);
	tree.addMouseListener(this);
    }

    public void displayModeSelection() {
	errorFrame.setVisible(false);
	// Display data - call Devise
	String filename = getSelectedFileNames();
	System.out.println("selectedfileNames to Devise is: "+filename);
	if (filename != null){

		if (client == null) {
		    client = new XVisClient(machineName, portNumber);
		    if (!client.start()) {
			System.out.println("Error: client didn't start.");
			client = null;
			return;
		    }
		} 
	
		client.showFiles(filename);

	}
	else {
	    //JOptionPane.showMessageDialog(new JFrame(), "No data meet your requirement. \n Please try again." );
	    errorLabel.setText("No data meet your requirement. Please try again.");
	    errorFrame.setVisible(true);
	    }
    }

    // When a leaf node of file_tree is selected,
    // mark all the source_tree nodes associated with the leaf node
    public void nodeSel_MarkIndexTree(DefaultMutableTreeNode node, JTree indexTree, LinkedList markNodes) {
	//if (node == root) return;

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

	FocusNode focusnode = (FocusNode)node.getUserObject();
	LinkedList nodesList = focusnode.getNodeList();
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
	    //if (parent.getParent() == null) continue;
	    // first_level focus
	    if (parent.getParent() == null) {
		for (int j = 0; j < parent.getChildCount(); j++) {
		    DefaultMutableTreeNode child = (DefaultMutableTreeNode)parent.getChildAt(j);
		    selectChildButton(child, buttonPanel);
		}
	    }
	    else selectChildButton(parent, buttonPanel);
	}
    }

    protected void selectChildButton(DefaultMutableTreeNode parent, JPanel buttonPanel) {
	if (parent.getParent().getParent() == null) 
	    for (int j = 0; j < parent.getChildCount(); j++) {
		DefaultMutableTreeNode child = (DefaultMutableTreeNode) parent.getChildAt(j);
		selectButton(child, buttonPanel);
	    }
	else
	    selectButton(parent, buttonPanel);
    }



    protected void selectButton(DefaultMutableTreeNode node, JPanel buttonPanel) {
	Component [] buttons = buttonPanel.getComponents();
	if (buttons == null) return;
	String metricsStr = (String)((FocusNode)node.getUserObject()).getMeasures().get(0);
	for (int i = 0; i < buttons.length; i++) {
	    JButton button = (JButton)buttons[i];
	    if (button == null) continue;
	    // clear selections
	    button.setSelected(false);
	    //System.out.println("Buttons["+i+"] is :"+buttons[i]);
	   
	    if (button.getText().equals(metricsStr))
		markButton(button);
	}
    }
	
    public void markButton(JButton button) {
	//button.setSelected(true);
	// Green color
	button.setBackground(new Color(204,255,204));
    }

   public void refreshFocusTree(LinkedList filesList){
	if (DEBUG)
	    System.out.println("START: RefreshFocusTree  in FocusTreeController");

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
		    // thumbnails = OmerData(fullFName, thumbnailCol, thumbnailRow);
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


	ToFocusTree toFocusTree = new ToFocusTree(filesList);
	DefaultMutableTreeNode focusTreeRoot = toFocusTree.getTreeRoot();
	if (focusTreeRoot == null) return;

	setTreeRoot(focusTreeRoot);
	configTree();

	//if (focusTreeController.getTree().getRowCount() <= 3)
        treeHandler.expandTree(getTree());
	getTree().updateUI();
	//treeHandler.expandTree(getTree());
    }

    // get all selected file Names
    public String getSelectedFileNames() {
	DefaultMutableTreeNode []node = treeHandler.getAllSelectedNodes(tree);
	//sort selected nodes from top to bottom in tree order
	if (node != null) {
	    Vector v = new Vector (node.length);
	    for (int i = 0; i < node.length; i++)
		v.addElement(node[i]);
	    Collections.sort(v, new Comparator() {
		public int compare(Object o1, Object o2) {
		    int level1 = tree.getRowForPath(new TreePath(((DefaultMutableTreeNode)o1).getPath()));
		    int level2 = tree.getRowForPath(new TreePath(((DefaultMutableTreeNode)o2).getPath()));
		    if (level1 < level2) return -1;
		    else if (level1 > level2) return 1;
		    else return 0;
		}
	    });
	
	    //replace the order into node[i]
	    for (int i= 0; i < node.length; i++)
		node[i]= (DefaultMutableTreeNode)v.elementAt(i);
	}
		
	StringBuffer filenames = new StringBuffer("");

	if (node != null) {
	    for (int i = 0; i < node.length; i++) {
		if (node[i] == null) continue;
		traverseFileLeafName(node[i], filenames);
	    }
	}
	else {
	    if (root != null)
		traverseFileLeafName(root, filenames);
	    else 
		return null;
	}

	if (filenames.length() == 0)
	    return null;
	else
	    return filenames.toString();
    }


    protected void traverseFileLeafName(DefaultMutableTreeNode node, StringBuffer fileNames) {
	//if (node.isLeaf() && !(node.getUserObject() instanceof FocusNode)) return;

	if (node.isLeaf() & node.getParent() == null)
	    return; // only root "Foci"

	if (node.isLeaf()) {
	    String leafFNames = "";
	    FocusNode focusnode = (FocusNode)node.getUserObject();
	    leafFNames = focusnode.getFNames();

	    // check if the focus has been included
	    if (fileNames.toString().indexOf(leafFNames) < 0){
		TreeNode parent = node.getParent();
		int subindex = parent.getIndex(node);
		int index = parent.getParent().getIndex(parent);
		String atFocus = "("  + (index+1);
		if (parent.getChildCount() > 1)
		    atFocus += "." + (subindex+1) + ")";
		else
		    atFocus += ")";
		fileNames.append(leafFNames + "@"+atFocus+ ";");
	    }
	    //System.out.println("Travers Procedure - fileNames to Devise: "+fileNames.toString());
	}
	else { // traverse the subtree
	    int childenCnt = node.getChildCount();
	    for (int i = 0; i < childenCnt; i++) {
		DefaultMutableTreeNode child = (DefaultMutableTreeNode)node.getChildAt(i);
		traverseFileLeafName(child, fileNames);
	    }
	}
    }
    
    public void actionPerformed(java.awt.event.ActionEvent e) {
	tree.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	if (DEBUG) System.err.println("ACTION: "+e.getActionCommand()+", "+e.paramString());

	String command = e.getActionCommand();

	if (command.equals("Display Mode")) {
	    displayMode = !displayMode;
	    if (displayMode) {
		displayButton.setSelected(true);
		displayButton.setBackground(new Color(204, 204, 255));
		displayModeSelection();
	    }
	    else {
		displayButton.setSelected(false);
		displayButton.setBackground(new Color(204,204,204)); }
	}

	if (command.equals("Hide Zero Data")) {
	    hideZero = !hideZero;
	    if (hideZero) {
		hideZeroButton.setSelected(true);
		hideZeroButton.setBackground(new Color(204, 204, 255));
	    }
	    else {
		hideZeroButton.setSelected(false);
		hideZeroButton.setBackground(new Color(204,204,204)); }
	}

	tree.setCursor(defaultCursor);
    }

    
    // When click file_tree
    public void mouseClicked(MouseEvent event) {
	System.out.println("Mouse cliked in file_tree.");
	try {
	    
	    Object obj = event.getSource();
	    if ((obj == null) || (!(obj instanceof JTree))) 
		return;

	    // set waiting cursor while process
	    tree.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

	    DefaultMutableTreeNode []node = treeHandler.getAllSelectedNodes(tree);

	    if (node == null) {
		tree.setCursor(defaultCursor);
		return;
	    }

	    indexTreeController.getTree().clearSelection();
	    treeHandler.collapseTree(indexTreeController.getTree());
	    treeHandler.expandOneLevel(indexTreeController.getTree());
	    indexTreeController.getRenderer().setColorList(null);
	    
	    LinkedList markNodes = new LinkedList();
	    for (int i=0; i< node.length; i++) 
		nodeSel_MarkIndexTree(node[i], indexTreeController.getTree(), markNodes);
	    if (markNodes.size() > 0) 
		indexTreeController.getRenderer().setColorList(markNodes);
	    // Update measure_List
	    metricsController.clearAllSelections();
	    nodeSel_MarkMetrics(node, metricsController.getButtonPanel());
	    
	    
	} catch (Exception ex) {}

	// check if in DisplayMode
	if (displayMode) {
	    int treeRowCnt = tree.getRowCount();
	    if (treeRowCnt == currentTreeRowCnt)
		displayModeSelection();
	    //else selection not change, but expand/collapse tree
	    else
		currentTreeRowCnt = treeRowCnt;
	}
	//set back default cursor
	tree.setCursor(defaultCursor);
    }
    public boolean isInDisplayMode() {
	return displayMode;
    }
    public boolean isHideZeroData() {
	return hideZero;
    }

    public void mousePressed(MouseEvent event) {}
    public void mouseReleased(MouseEvent event) {}
    public void mouseEntered(MouseEvent event) {
	currentTreeRowCnt = tree.getRowCount();
	//System.out.println("FocusTreeRowCount is :"+currentTreeRowCnt);
}
    public void mouseExited(MouseEvent event) {}

}
