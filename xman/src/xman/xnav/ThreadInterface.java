package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.tree.*;
import java.util.*;
import javax.swing.plaf.basic.*;


public class ThreadInterface extends JFrame 
    implements ActionListener, Observer {
    
    public static final int initialTabCnt = 1;
    public static final boolean DEBUG = true;
    public String title = "Data Multiple Page View";
    public static final int defaultPort = 12345;
    public static final String defaultMachine = "chocolate";

    protected LinkedList singleRuns; // each member is ThreadRun.class
    protected JTabbedPane tabbedPane;
    protected JPanel tabPaneHolder;
    protected int threadGroupCnt;
    // For Structural Diff, list of selected SingleRuns candidates
    // member is DefaultMutableTreeNode 
    // indicating the root node of a structural hierary
    protected LinkedList srcTreeRootList; 
    protected LinkedList indexTreeRootList;
    // list of selected SingleRuns' allFilesList
    protected LinkedList allFilesListList;
    // list of selected SingleRuns' dirNames
    protected LinkedList dirNameList;
    //private XMan x;
    private Cursor defaultCursor;
    private JButton display;

    protected int portNumber;
    protected String machineName;

    // Constructor 
    public ThreadInterface() {
	this(defaultPort, defaultMachine);
    }
    
    // Constructor 
    public ThreadInterface(int portNumber) {
	this(portNumber, defaultMachine);
    }

    // Constructor
    public ThreadInterface(int portNumber, String machineName) {
	setTitle(title);
	this.portNumber = portNumber;
	this.machineName = machineName;

	// set Menu to the frame
	JMenuBar jmb = new JMenuBar();

	// File Menu
        JMenu fileMenu = new JMenu("File");
	//winMenu = new JMenu("Window");
        JMenuItem item;
	item = new JMenuItem("New Page");
	fileMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("New Window");
	fileMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("Open");
	fileMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("Close Page");
	fileMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("Close Window");
	fileMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("Exit");
	fileMenu.add(item);
	item.addActionListener(this);
	jmb.add(fileMenu);

	// Option Menu
	JMenu optMenu = new JMenu("Option");
	item = new JMenuItem("View Resources");
	optMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("View Performance");
	optMenu.add(item);
	item.addActionListener(this);
	jmb.add(optMenu);

	// StructuralDiff Menu
	JMenu diffMenu = new JMenu("Struc_Diff");
	item = new JMenuItem("SelectForStruc_Diff");
	diffMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("UnSelect");
	diffMenu.add(item);
	item.addActionListener(this);
	item = new JMenuItem("Execute Struc_Diff");
	diffMenu.add(item);
	item.addActionListener(this);
	jmb.add(diffMenu);

	// Call Devise Menu
	/*
	JMenu visiMenu = new JMenu("Visi"); 
	item = new JMenuItem("Show Visi"); 
	visiMenu.add(item); 
	item.addActionListener(this); 
	item = new JMenuItem("Quit Visi"); 
	visiMenu.add(item); 
	item.addActionListener(this); 
	jmb.add(visiMenu); */

	this.setJMenuBar(jmb);

	tabPaneHolder = new JPanel();
	tabPaneHolder.setLayout(new GridLayout(1, 1)); 

	singleRuns = new LinkedList();
	
	// This two list should always have same number of members 
	// and they are orderly corresponding
	srcTreeRootList = new LinkedList();
	dirNameList = new LinkedList();
	indexTreeRootList = new LinkedList();
	allFilesListList = new LinkedList();
	//x = null;
	defaultCursor = Cursor.getDefaultCursor();
	//display = new JButton("Display");
	//display.addActionListener(this);

	threadGroupCnt = 0;
	tabbedPane = createTabPane(initialTabCnt);

	this.getContentPane().add(tabPaneHolder);
    }

    protected JTabbedPane createTabPane(int tabCount) {
	if (tabCount != 0)
	    threadGroupCnt++;
	// New a tabPane
	JTabbedPane tabPane = new JTabbedPane();
	tabPane.setTabPlacement(SwingConstants.BOTTOM);
        tabPane.setUI(new MyTabPaneUI());
	// Set up threads for the tabPane (tabs)
	String subgroupName = "subgroup" + threadGroupCnt;
	ThreadGroup subgroup = new ThreadGroup(subgroupName);
	addTabs(tabCount, tabPane, subgroup, "blank");

	// update tabPane holder UI
	tabPaneHolder.removeAll();
	tabPaneHolder.add(new JScrollPane(tabPane));

	//System.out.println("TabPane layout: "+tabPane.getLayout().toString());

	return tabPane;
    }

    protected void addTabs(int tabCount, JTabbedPane tabPane, ThreadGroup subgroup, String pageName) {
	// to be called by both initial tabPane creation and "New a tab"
	// tabCnt is global variable to memorize how many tabs have been 
	// created in this window.
	int startTab, endTab;
	int tabCnt =  tabPane.getTabCount();
	if ((tabCount == 1) && (tabCnt != 0)) {
	    startTab = tabPane.getTabCount();
	    endTab = tabPane.getTabCount()+1;
	}
	else {
	    startTab = 0;
	    endTab = tabCount;
	}

	for (int i = startTab; i < endTab; i++) {
	    String threadName = subgroup.getName()+".thread"+i;
	    System.out.println("ThreadName is: "+threadName);

	    ThreadRun threadrun = new ThreadRun(subgroup, threadName, portNumber, machineName);
	    singleRuns.add(threadrun);
	    int currentCnt = tabPane.getTabCount();

	    if (tabPane == null) {
		System.out.println("ERROR (ThreadInterface): TabPane is NULL!");
		return; }
	    tabPane.addTab(pageName, threadrun.getSingleRun().getContentPane());
	    tabPane.setSelectedIndex(startTab);
	    //System.out.println("getTabRunCount: "+tabPane.getUI().getTabRunCount(tabPane));

	}

	// update tabIcons
	updateTabIcons(tabPane);
    }

    protected int getNewNumber(String lastPageName){
	String newNumber = "";
	StringTokenizer token = new StringTokenizer(lastPageName, " ");
	while (token.hasMoreTokens())
	    newNumber = token.nextToken().trim();
	Integer value = new Integer(newNumber);
	int pageNo = value.intValue();
	pageNo++;
	return pageNo;
    }

    protected String shortDirName(String dirName) {
	if (dirName.length() <= 2)
	    return dirName;

	String shortName = dirName;
	StringTokenizer token = new StringTokenizer(dirName, "/");
	int tokenCnt = token.countTokens();
	if (tokenCnt <= 2)
	    return dirName;

	String str[] = new String[tokenCnt];
        int i = 0;
        while (token.hasMoreTokens()) {
            str[i]= token.nextToken().trim();
            if (str[i].length() > 0)
		i++; }
          
        tokenCnt = i;      // total number of str
	//shortName = ".../";
	shortName = "/";
	for (i = tokenCnt-1; i< tokenCnt; i++)
	    shortName+=str[i]+"/";
	
	return shortName;
    }

    protected void removeATab() {
	if (DEBUG)
	    System.out.println("Remove the tab (tabCnt = "+tabbedPane.getTabCount()+" )");
	int index = tabbedPane.getSelectedIndex();
	ThreadRun threadrun = (ThreadRun)singleRuns.get(index);	
	// if tabbedPanel setComponent as performance view
	tabbedPane.remove(threadrun.getSingleRun().getContentPane());
	// if tabbedPanel setComponent as resource view
	tabbedPane.remove(threadrun.getSingleRun().getSrcPanel());
	singleRuns.remove(index);
	threadrun.stop();
	
	// update tabIcons
	updateTabIcons(tabbedPane);
    }

    protected void updateTabIcons(JTabbedPane tabPane) {
	if (tabPane  == null) return;

	for (int i = 0; i < tabPane.getTabCount(); i++) {
	    String tabName = tabPane.getTitleAt(i);
	    if (!tabName.equals("blank")) {
		ImageIcon tabIcon = (new RoundIcon(tabPane)).updateTabIcon(tabName, tabPane.getBackgroundAt(i));
		tabPane.setIconAt(i, tabIcon);
	    }
	}
    }

    public void openAnExpt() {
	//JFileChooser fileChooser = new JFileChooser (new File ((System.getProperties()).getProperty("user.dir")));
	JFileChooser fileChooser = new JFileChooser (new File ("/p/paradyn/development/zhou/data"));

	fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
	fileChooser.resetChoosableFileFilters();
	//fileChooser.setVisible(true);
	int retval = fileChooser.showOpenDialog(this);
	//System.out.println("fileChooser: " + retval);
	if (retval == JFileChooser.CANCEL_OPTION) {
	    setCursor(defaultCursor);
	    return;
	}
	
	String fileName;
	File selfile = fileChooser.getCurrentDirectory();
	if (selfile == null) {
	    setCursor(defaultCursor);
	    return;
	}
	String fileDir = selfile.getPath()+"/";
	
	if (DEBUG)
	    System.out.println("(ThreadInterface:) the directory of the local file is: "+fileDir);	 
	
	if (tabbedPane == null){
	    System.out.println("tabPane is NULL!");
	    setCursor(defaultCursor);
	    return;
	}
	if (tabbedPane.getTitleAt(0) != "blank") {
	    addTabs(1, tabbedPane,((ThreadRun)singleRuns.get(0)).getThreadGroup(),"blank");
	    setTitle(title);
	}
	int index = tabbedPane.getSelectedIndex();
	
	// check if overwritten fileDir in the Struc_Diff select list 
	// if yes, remove
	ThreadRun threadrun = (ThreadRun)singleRuns.get(index);
	DefaultMutableTreeNode node = threadrun.getSingleRun().getSrcTreeRoot();
	if (srcTreeRootList.contains(node)) {
	    int sameIndex = srcTreeRootList.indexOf(node);
	    srcTreeRootList.remove(sameIndex);
	    dirNameList.remove(sameIndex);
	    indexTreeRootList.remove(sameIndex);
	    allFilesListList.remove(sameIndex);
	}
	
	// open the new fileDir
	setTitle(fileDir);
	if (DEBUG)
	    System.out.println("(ThreadInterface:) the selected Tab is: "+index);
	
	fileName = fileDir + "index";
	try {
	    FileReader fr = new FileReader(fileName);
	}catch(FileNotFoundException ex) {
	    System.out.println("File NOT Found!");
	    setCursor(defaultCursor);
	    return;
	}
	
	threadrun.getSingleRun().refreshUI(fileName);
	fileName = fileDir + "resources";
	
	threadrun.getSingleRun().refreshUI(fileName); 
	
	// set tab name as directory name
	String tabName = shortDirName(fileDir);
	tabbedPane.setTitleAt(index, tabName);
	
	// update tabIcons
	updateTabIcons(tabbedPane);
    }
    
    public void actionPerformed(java.awt.event.ActionEvent e) {
        if (DEBUG) System.err.println("ACTION: "+e.getActionCommand()+", "+e.paramString());
	String command = e.getActionCommand();
	setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

	// Open Menu
	if (command.equals("Open")) {
	    openAnExpt();
	   
	} // End of "Open"

	// Exit Menu
	else if (command.equals("Exit")) {
	    this.dispose();
	    /*
	    if (x != null) {
		try {
		    x.quit();
		}catch (YException ex) {
		    System.out.println("Omer couldn't quit!");
		}
	    }
	    */
	    System.exit(0);
	}

	// New Page Menu
	else if (command.equals("New Page")) {
	    if (tabbedPane == null)
		System.out.println("ERROR (ThreadInterface- New): TabPane is NULL!");

	    addTabs(1, tabbedPane,((ThreadRun)singleRuns.get(0)).getThreadGroup(), "blank");
	    setTitle(title);
	}

	// New Window Menu
	else if (command.equals("New Window")) {
	    JFrame newWin =  new ThreadInterface();
	    newWin.addWindowListener(new java.awt.event.WindowAdapter() {
	    public void windowClosing(java.awt.event.WindowEvent e) {
		//System.exit(0);
	    }
        });
        newWin.setSize(840, 740);
        newWin.setVisible(true);
	    
	}

	// Close a Tab Menu
	else if (command.equals("Close Page")) {
	    if (tabbedPane == null)
		System.out.println("ERROR (ThreadInterface- Close): TabPane is NULL!");
	    int count = tabbedPane.getTabCount();
	    ThreadGroup group = ((ThreadRun)singleRuns.get(0)).getThreadGroup();

	    if (count >= 1) {
		removeATab();
	    }
	    if (count != 1) {
		tabbedPane.setSelectedIndex(0);
		//tabbedPane.updateUI();
	    }
	    
	    else {
		if (DEBUG)
		    System.out.println("add a new tab, (ThreadInterface: close )");
		addTabs(1, tabbedPane, group,"blank");
	    }
	}  // end of "Close Page"

	// Close a Window
	else if (command.equals("Close Window")) {
	    dispose();
	}

	// In Option Menu, view resources
	else if (command.equals("View Resources")) {
	    int index = tabbedPane.getSelectedIndex();
	    ThreadRun threadrun = (ThreadRun)singleRuns.get(index);
	    String srcName = threadrun.getSingleRun().getSrcFileName();
	    if (srcName != null) {
		srcName= srcName.substring(0, srcName.length()-9);
		srcName = shortDirName(srcName)+"resources";
	    }

	    tabbedPane.setComponentAt(index, threadrun.getSingleRun().getSrcPanel());
	    tabbedPane.updateUI();
	    tabbedPane.setUI(new MyTabPaneUI());
	}

	// View Performance
	else if (command.equals("View Performance")) {

	    int index = tabbedPane.getSelectedIndex();
	    ThreadRun threadrun = (ThreadRun)singleRuns.get(index);

	    tabbedPane.setComponentAt(index, threadrun.getSingleRun().getContentPane());
	    tabbedPane.updateUI();
	    tabbedPane.setUI(new MyTabPaneUI());
	}

	// ActionPerform for Struc_Diff
	else if (command.equals("SelectForStruc_Diff")) {
	    int index = tabbedPane.getSelectedIndex();
	    ThreadRun threadrun = (ThreadRun)singleRuns.get(index);
	    DefaultMutableTreeNode srcnode = threadrun.getSingleRun().getSrcTreeRoot();
	    DefaultMutableTreeNode indexnode = threadrun.getSingleRun().getIndexTreeRoot();
	    LinkedList allFilesList = threadrun.getSingleRun().getAllFilesList();

	    // Mark the tabTitle to indicate selection
	    String tabName = tabbedPane.getTitleAt(index);
	    if (tabName.equals("blank")) {
		setCursor(defaultCursor);
		return; // don't select blank page
	    }
	    if (!srcTreeRootList.contains(srcnode)) {
		srcTreeRootList.add(srcnode);
		dirNameList.add(tabName);
		indexTreeRootList.add(indexnode);
		allFilesListList.add(allFilesList);
	    }

	    if (!tabName.endsWith("*")) {
		tabName += "*";
		tabbedPane.setTitleAt(index, tabName);
	    }
	}

	else if (command.equals("UnSelect")) {
	    int index = tabbedPane.getSelectedIndex();
	    ThreadRun threadrun = (ThreadRun)singleRuns.get(index);
	    DefaultMutableTreeNode node = threadrun.getSingleRun().getSrcTreeRoot();
	    // Unmark the tabTitle 
	    String tabName = tabbedPane.getTitleAt(index);

	    if (srcTreeRootList.contains(node)) {
		int sameIndex = srcTreeRootList.indexOf(node);
		srcTreeRootList.remove(sameIndex);
		dirNameList.remove(sameIndex);
		indexTreeRootList.remove(sameIndex);
		allFilesListList.remove(sameIndex);
	    }

	    if (tabName.endsWith("*")) {
		tabName = tabName.substring(0, tabName.length()-1);
		tabbedPane.setTitleAt(index, tabName);
	    }
	}

	else if (command.equals("Execute Struc_Diff")) {
	    if (srcTreeRootList.size() > 1) {
		if (tabbedPane == null)
		    System.out.println("ERROR (ThreadInterface- New): TabPane is NULL!");

		addTabs(1, tabbedPane,((ThreadRun)singleRuns.get(0)).getThreadGroup(), "blank");
		setTitle(title);
		int index = tabbedPane.getSelectedIndex();
		SingleRunInterface singleInterface = ((ThreadRun)singleRuns.get(index)).getSingleRun();
		singleInterface.refreshMergeUI(srcTreeRootList, indexTreeRootList, allFilesListList, dirNameList, tabbedPane);

		String tabMergeTitle="";
		for (int i = 0; i < dirNameList.size()-1; i++)
		    tabMergeTitle += (String)dirNameList.get(i) + "+";
		tabMergeTitle += (String)dirNameList.get(dirNameList.size()-1);
		//tabbedPane.setFont(new Font(tabMergeTitle, Font.PLAIN, 5));
		tabbedPane.setTitleAt(index, tabMergeTitle);
		//tabbedPane.setFont(defaultFont);
		updateTabIcons(tabbedPane);
	    }

	    else
		System.out.println("No Need to Excute Structual Difference.");
	}

	setCursor(defaultCursor);
    } // end of "ActionPerform"
     public void update(Observable observed, Object arg) {
	String[] dirName = (String []) arg;

	for (int i=0; i < dirName.length; i++) {
	    String tabName = shortDirName(dirName[i]);
	    int index = tabbedPane.indexOfTab(tabName);

	    if (index != -1) {
		updateExistingTab(index, dirName[i]);
	    }
	    else {
		insertNewTab(dirName[i]);
	    }
	}
    }
    
    private void updateExistingTab(int index, String dirName) {
	tabbedPane.setSelectedIndex(index);
	ThreadRun threadrun = (ThreadRun)singleRuns.get(index);
	setTitle(dirName);
	if (DEBUG)
	    System.out.println("(ThreadInterface:) the selected Tab is: "+index);
	// set tab name as directory name
	String tabName = shortDirName(dirName);
	String fileName;
	tabbedPane.setTitleAt(index, tabName);
	fileName = dirName + File.separator + "index";
	threadrun.getSingleRun().refreshUI(fileName);
	fileName = dirName + File.separator + "resources";
	threadrun.getSingleRun().refreshUI(fileName); 
    }

    private void insertNewTab(String dirName) {

	String tabName = shortDirName(dirName);

	addTabs(1, tabbedPane, ((ThreadRun)singleRuns.get(0)).getThreadGroup(), tabName);
	setTitle(dirName);
	int index = tabbedPane.indexOfTab(tabName);
	tabbedPane.setSelectedIndex(index);

	ThreadRun threadrun = (ThreadRun)singleRuns.get(index);
	// set tab name as directory name
	String fileName;
	fileName = dirName + File.separator + "index";
	threadrun.getSingleRun().refreshUI(fileName);
	fileName = dirName + File.separator + "resources";
	threadrun.getSingleRun().refreshUI(fileName); 
    }
}

	   
