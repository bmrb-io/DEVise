package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.lang.*;


class SingleRunInterface extends JFrame {
    // Constant
    public static final boolean DEBUG = true;
    static final String title = "Data Visulization";
    static final Color greenColor = new Color(204, 255, 204);

    // Data members

    // Input file names (index, resources... including directory)
    String sourceFName, indexFName;
    // current directory name
    String dirName;
    // Record DataSets and Metrics elements
    LinkedList allFilesList, allMeasureList;
    // Hierarchy Structure on interface
    DefaultMutableTreeNode srcTreeRoot, indexTreeRoot;
    DefaultMutableTreeNode fileTreeRoot, allFileTreeRoot, focusTreeRoot;
    JTree source_tree, index_tree, file_tree, focus_tree;
    // Metrics List on interface
    //JList measureList;
    // Data Controllers
    IndexTreeController indexTreeController;
    FileTreeController fileTreeController;
    FocusTreeController focusTreeController;
    MetricsController metricsController;
    TreeHandler treeHandler;

    JTextArea messageText;
    JPanel sourceTreePanel;
    JPanel buttonPanel;
    JComponent singleRun;

    //Display mode button
    JButton displayButton;
    JButton hideZeroButton;

    // portNumber to connect with DEVise
    int portNumber = 0;
    String machineName = "chocolate";
    
    // Constructor
    public SingleRunInterface() {
	this(null,null);
    }

    // Constructor
    public SingleRunInterface(int portNumber, String machineName) {
	this(null,null);
	this.portNumber = portNumber;
	this.machineName = machineName;
    }

    // Constructor
    //public SingleRunInterface(JButton display) {
    //this(null, null, display);
    //}

    // Constructor
    public SingleRunInterface(String afileName) {
	this(null, afileName);
    }
    // Constructor
    public SingleRunInterface(String sourceName, String indexName) {
	super(title);
	
	//this.display = display;

        sourceFName = null;
	indexFName = null;
	dirName = null;

	srcTreeRoot = null;
	indexTreeRoot = null;
	fileTreeRoot = null;
	focusTreeRoot = null;
	allFileTreeRoot = null;
	allMeasureList = null;
	allFilesList = null;
	indexTreeController = null;
	fileTreeController = null;
	focusTreeController = null;
	metricsController = null;
	treeHandler = new TreeHandler();

	singleRun = createUI(sourceName, indexName);
	getContentPane().add(singleRun);
    }

    // Layout general user interface
    JComponent createUI(String sourceName, String indexName) {

        // create the JTree and scroll pane for performance/index tree.
        JPanel indexTreePanel = new JPanel(new BorderLayout()){
            public Dimension getPreferredSize(){
                Dimension size = SingleRunInterface.this.getSize();
		return new Dimension(size.width * 2/5, size.height);
                }
            public Dimension getMinimumSize(){
                return new Dimension(100, 100);
                }
            };
        indexTreePanel.add(new JScrollPane(index_tree = treeHandler.createJTree()), BorderLayout.CENTER);
        indexTreePanel.setBorder(BorderFactory.createCompoundBorder(
	    BorderFactory.createTitledBorder("Performance Tree View"),
            BorderFactory.createEmptyBorder(4, 4, 4, 4)
            ));

	// create the JTree and scroll pane for datafiles tree.
        JPanel focusTreePanel = new JPanel(new BorderLayout()){
            public Dimension getPreferredSize(){
                Dimension size = SingleRunInterface.this.getSize();
		return new Dimension(size.width * 2/5, size.height);
                }
            public Dimension getMinimumSize(){
                return new Dimension(100, 100);
                }
            };
	file_tree =  treeHandler.createJTree(); // virture tree
        focusTreePanel.add(new JScrollPane(focus_tree = treeHandler.createJTree()) , BorderLayout.CENTER);
	displayButton = new JButton("Display Mode");
	//fileTreePanel.add(displayButton, BorderLayout.SOUTH);

	hideZeroButton = new JButton("Hide Zero Data");
	buttonPanel = new JPanel(new GridLayout(1,2));
	buttonPanel.add(displayButton);
	buttonPanel.add(hideZeroButton);
	focusTreePanel.add(buttonPanel, BorderLayout.SOUTH); 

        focusTreePanel.setBorder(BorderFactory.createCompoundBorder(
	    BorderFactory.createTitledBorder("Foci"),
            BorderFactory.createEmptyBorder(4, 4, 4, 4)
            ));

	// create the JButtons to display classification
	JPanel ListPanel = new JPanel(new BorderLayout()) {
            public Dimension getPreferredSize(){
                Dimension size = SingleRunInterface.this.getSize();
 		return new Dimension(size.width * 11/50, size.height);
	    }
            public Dimension getMinimumSize(){
                return new Dimension(50, 100);
	    }
	    // public Dimension getMaximumSize() {
	    // return new Dimension(200,200);
	    // };
	};
	buttonPanel = new JPanel();

	buttonPanel.setBackground(Color.white);
  	ListPanel.add(new JScrollPane(buttonPanel),
            BorderLayout.CENTER);
        ListPanel.setBorder(BorderFactory.createCompoundBorder(
	    BorderFactory.createTitledBorder("Metrics"),
            BorderFactory.createEmptyBorder(4, 4, 4, 4)
            ));
	// Create the Message Box
        messageText = new JTextArea();
        JPanel messagePanel = new JPanel(new BorderLayout()){
            public Dimension getPreferredSize(){
                Dimension size = SingleRunInterface.this.getSize();
                return new Dimension(size.width/2, size.height *2/5);
	    }
	    
            public Dimension getMinimumSize(){
                return new Dimension(100, 100);
	    }
	    //public Dimension getMaximumSize() {
	    //return new Dimension(100, 100);
	    //}
	};
        messagePanel.add(new JScrollPane(messageText),
            BorderLayout.CENTER);
        messagePanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createTitledBorder("Messages"),
            BorderFactory.createEmptyBorder(4, 4, 4, 4)
            ));

	// For a seperate Use
        // create the JTree and scroll pane for source tree.
        sourceTreePanel = new JPanel(new BorderLayout());
        sourceTreePanel.add(new JScrollPane(source_tree = treeHandler.createJTree()) {
            public Dimension getPreferredSize(){
                Dimension size = SingleRunInterface.this.getSize();
                return new Dimension(size.width / 2, size.height /2);
                }
            public Dimension getMinimumSize(){
                return new Dimension(100, 200);
                }
            },
            BorderLayout.CENTER);
        sourceTreePanel.setBorder(BorderFactory.createCompoundBorder(
	    BorderFactory.createTitledBorder("Resources Tree View"),
            BorderFactory.createEmptyBorder(4, 4, 4, 4)
            ));



	if (indexName != null)
	    refreshUI(indexName);
	if (sourceName != null)
	    refreshUI(sourceName);

	// check if both index and resources file parsed.
	// if not, disable the tree structure of nonparsed file
	if (indexFName == null) {
	    index_tree.setModel(null);
	    focus_tree.setModel(null);
	}
	if (sourceFName == null)
	    source_tree.setModel(null);

	JComponent split1 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
					   true, indexTreePanel, focusTreePanel);
	JComponent split2 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
					   true, ListPanel, split1);
	return split2;	
    }

    void refreshUI(String filename) {
	if (DEBUG)
	    System.out.println("SingleRunInterface: START refreshUI:"+filename);


	// get DirName from filename
	dirName = retrieveDirName(filename);


	// Parse input file to get sourceTree
	ParserToTree parserToTree = new ParserToTree(filename, dirName);
	DefaultMutableTreeNode treeRoot = parserToTree.getTreeRoot();

	if (treeRoot == null)
	    messageText.setText("No data files are found in the directory: "+ filename);

	// resources file parsed
	if (parserToTree.getFileType() == 0) { 
	    if (DEBUG) System.out.println("SingleRunInterface: Start parsing resources file.");
	    srcTreeRoot = treeRoot;
	    if (srcTreeRoot == null) {
		source_tree.setModel(null);
		return; }

	    DefaultTreeModel sourceTreeModel = new DefaultTreeModel(srcTreeRoot);
	    source_tree.setModel(sourceTreeModel);
	    source_tree.updateUI();
	    sourceFName = filename;
	}

	// index file parsed
	else if (parserToTree.getFileType() == 1) { 
	    if (DEBUG) System.out.println("SingleRunInterface: Start parsing index file.");
	    indexFName = filename;

	    indexTreeRoot = treeRoot;
	   
	    indexTreeController = new IndexTreeController(index_tree, indexTreeRoot, treeHandler);
	    indexTreeController.configTree();

	    // Input file is "index" file, get performance data Sets
	    allFilesList = parserToTree.getDataFileList();
	    // Build marked node network
	    indexTreeController.buildNodesRelations(allFilesList);

	    // Build DataSets tree (file_tree)
	    allFileTreeRoot = null;
	    fileTreeController = new FileTreeController(allFilesList, treeHandler, indexTreeController, hideZeroButton);
	     focusTreeController = new FocusTreeController(focus_tree, null, treeHandler, indexTreeController, displayButton, hideZeroButton, portNumber, machineName, fileTreeController);
	    
	    refreshFileTree(allFilesList);
	    // set fileTreeController to indexTreeController
	    indexTreeController.setFileTreeController(fileTreeController);
	    indexTreeController.setFocusTreeController(focusTreeController);

	    
	    // Create Display Metrics/List
	    allMeasureList =(new FilesToTree(allFilesList)).getMeasureList();
	    //System.out.println("buttonPanel: in SingleRunInterface: "+buttonPanel.toString());
	    buttonPanel.removeAll();
	    metricsController = new MetricsController(allMeasureList, buttonPanel, treeHandler, indexTreeController, fileTreeController, focusTreeController);
	    metricsController.setUpData();
	    fileTreeController.setMetricsController(metricsController);
	    focusTreeController.setMetricsController(metricsController);
	    indexTreeController.setMetricsController(metricsController);
	    //messageText.setText(parserToTree.getAllDataFileInfo());
	}
	else { 
	    // get no type of file
	    if (DEBUG) System.out.println("The file type is not correct, expect either index file or resources file");
	   
	    return;
	}
    }
	
    void refreshFileTree(LinkedList filesList){
	//if (DEBUG)
	//  System.out.println("START: RefreshFileTree");

	if (fileTreeController != null) {
	    fileTreeController.refreshFileTree(filesList);
	}

	if (focusTreeController != null)	
	    focusTreeController.refreshFocusTree(null);


	if (allFileTreeRoot == null)
	    allFileTreeRoot = fileTreeRoot;
    }


    // retrieveDirName from a complete filename by seperate the last "/"
    String retrieveDirName(String filename) {
	int i = filename.lastIndexOf("/");
	if (i >= 0) {
	    String dir = filename.substring(0, i+1);
	    return dir;
	}
	else {
	    String dir = "./";
	    return dir;
	}
    }

    /************* public return/set interface ******************/
    public JComponent getSingleRun() { return singleRun;}


    public JPanel getSrcPanel() {
	return sourceTreePanel;
    }

    public String getDirName() {
	return dirName;
    }

    public String getIndexFileName() {
	return indexFName;
    }

    public String getSrcFileName() {
	return sourceFName;
    }

    public LinkedList getAllFilesList() {
	return allFilesList;
    }

    public DefaultMutableTreeNode getSrcTreeRoot() {
	return srcTreeRoot;
    }

    public DefaultMutableTreeNode getIndexTreeRoot() {
	return indexTreeRoot;
    }
    
    public void setSourceTreePanel(JPanel panel) {
	this.sourceTreePanel = panel;
    }

    public void setSourceTree(JTree tree) {
	this.source_tree = tree;
    }

    public void setSourceTreeRoot(DefaultMutableTreeNode root) {
	this.srcTreeRoot = root;
    }

    public void setDirName(String dirName) {
	this.dirName = dirName;
    }

    public IndexTreeController getIndexTreeController() {
	return indexTreeController;
    }

    public MetricsController getMetricsController() {
	return metricsController;
    }

    public FileTreeController getFileTreeController() {
	return fileTreeController;
    }

    public FocusTreeController getFocusTreeController() {
	return focusTreeController;
    }

    /************ For combined/merged run interface ***********/


    // public interface "refreshMergeUI" - equavelent to "refreshUI"
    // for a single original run
    public void refreshMergeUI(LinkedList srcTreeRootList, LinkedList indexTreeRootList, LinkedList allFilesListList, LinkedList treeNameList, JTabbedPane tabbedPane) {
	if (DEBUG)
	    System.out.println("SingleRunInterface: START refreshMergeUI");

	// Call StrucDiffWin to execute merging for resource
	StrucDiffWin srcStrucDiffWin =  new StrucDiffWin(srcTreeRootList, treeNameList);
	DefaultMutableTreeNode treeRoot = srcStrucDiffWin.getMergeTreeRoot();
	srcTreeRoot = treeRoot;
	if (srcTreeRoot == null) 
	    source_tree.setModel(null);
	else {
	    DefaultTreeModel sourceTreeModel = new DefaultTreeModel(srcTreeRoot);
	    source_tree.setModel(sourceTreeModel);
	    source_tree.setCellRenderer(new MergingTreeRender(tabbedPane));
	    source_tree.updateUI();
	}

	// Call PerfStrucDiffWin to execute merging for performance
	PerfStrucDiffWin perfStrucDiffWin = new PerfStrucDiffWin(indexTreeRootList, allFilesListList, treeNameList);
	perfStrucDiffWin.executeMerge();

	// get Merged Index TreeRoot
	treeRoot = perfStrucDiffWin.getMergedIndexTreeRoot();
	if (treeRoot != null) {
	    indexTreeRoot = treeRoot;
	    indexTreeController = new IndexTreeController(index_tree, indexTreeRoot, treeHandler, tabbedPane);
	    indexTreeController.configTree();

	    // get merged performance data Sets
	    allFilesList = perfStrucDiffWin.getMergedAllFilesList();
	    // Build marked node network
	    indexTreeController.buildNodesRelations(allFilesList);

	    // Build DataSets tree (file_tree)
	    allFileTreeRoot = null;
	    fileTreeController = new FileTreeController(allFilesList, treeHandler, indexTreeController, hideZeroButton);
	    focusTreeController = new FocusTreeController(focus_tree, allFilesList, treeHandler, indexTreeController, displayButton, hideZeroButton, portNumber, machineName, fileTreeController);
	    indexTreeController.setFileTreeController(fileTreeController);
	    indexTreeController.setFocusTreeController(focusTreeController);
	    refreshFileTree(allFilesList);


	    // Create Display Metrics/List
	    allMeasureList =(new FilesToTree(allFilesList)).getMeasureList();
	    System.out.println("buttonPanel: in SingleRunInterface: "+buttonPanel.toString());
	    buttonPanel.removeAll();
	    metricsController = new MetricsController(allMeasureList, buttonPanel, treeHandler, indexTreeController, fileTreeController, focusTreeController);
	    metricsController.setUpData();
	    fileTreeController.setMetricsController(metricsController);
	    focusTreeController.setMetricsController(metricsController);
	    indexTreeController.setMetricsController(metricsController);
	}
    }
}
