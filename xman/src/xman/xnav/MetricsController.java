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

class MetricsController implements MouseListener, MouseMotionListener {

    public static final boolean DEBUG = true;
    protected LinkedList allMeasureList; // Members are MutableTreeNodes of fileTree
    protected IndexTreeController indexTreeController;
    protected TreeHandler treeHandler;
    protected FileTreeController fileTreeController;
    protected FocusTreeController focusTreeController;
    protected JPanel buttonPanel;  
    protected Cursor defaultCursor;
    protected boolean dragged; // detect mouse drag

    // Constructor
    public MetricsController(LinkedList allMeasureList, JPanel buttonPanel,
			     TreeHandler treeHandler,
			     IndexTreeController indexTreeController,
			     FileTreeController fileTreeController, 
			     FocusTreeController focusTreeController) {
	this.allMeasureList = allMeasureList;
	this.buttonPanel = buttonPanel;

	this.treeHandler = treeHandler;
	this.indexTreeController = indexTreeController;
	this.fileTreeController = fileTreeController;
	this.focusTreeController = focusTreeController;
	this.defaultCursor = Cursor.getDefaultCursor();
	this.dragged = false;
    }
    
    public void setUpData() {
	GridBagLayout gridbaglayout = new GridBagLayout();
	buttonPanel.setLayout(gridbaglayout);
	GridBagConstraints c = new GridBagConstraints();
	c.fill = GridBagConstraints.HORIZONTAL;
	c.anchor = GridBagConstraints.NORTHWEST;

	//Layout vertical extra space only in the bottom
	int col = 1;  // How many cols in a row
	int len = allMeasureList.size();
	if (len <= col)
	    c.weighty = 1.0;

	for (int i = 0; i < len; i++) {
	    JButton button = new JButton(allMeasureList.get(i).toString());
	    button.addMouseListener(this);
	    button.addMouseMotionListener(this);
	    button.setRolloverEnabled(true);
	    button.setFocusPainted(false);
	    //System.out.print("button color: "+button.getBackground());
	    if (((i+1) % col) != 0) {
		c.weightx = 1.0;
		c.gridwidth = 1;
		gridbaglayout.setConstraints(button, c);
		buttonPanel.add(button);
	    }
	    else {
		c.weightx = 1.0;
		c.gridwidth = GridBagConstraints.REMAINDER; //end row
		gridbaglayout.setConstraints(button, c);
		if ((len / col) <= ((i+1)/col+1))
		    c.weighty = 1.0;
		buttonPanel.add(button);
		//System.out.println("Button: "+button);
	    }
	}
	buttonPanel.updateUI();
    }

    // When metrics selected, get corresponding DataFiles List
    public LinkedList getSelectedDataFileList() {
	Object [] selNodes = getSelectedNodes();
	if (selNodes == null) return null;
	
	LinkedList datafilelist = new LinkedList();
	TreeHandler treeHandler = new TreeHandler();

	for (int i=0; i< selNodes.length; i++) {
	    if (selNodes[i] != null)
		treeHandler.addChildrenToList((DefaultMutableTreeNode)selNodes[i], datafilelist);
	}

	return datafilelist;
    }

    protected void markSingleFocus(LinkedList secondColor) {
	// blue mark index_tree if only one focus selected
	String filenames = fileTreeController.getSelectedFileNames();
	if ((filenames != null) && (filenames.indexOf(";") < 0)) {
	    for (int i = 0; i < secondColor.size(); i++) {
		    DefaultMutableTreeNode marknode = 
			(DefaultMutableTreeNode)secondColor.get(i);
		    if (marknode != null) {
			TreePath treePath = new TreePath(marknode.getPath());
			indexTreeController.getTree().addSelectionPath(treePath);
		    }
	    }
	}
    }

    public void listSel_DataSets(DefaultMutableTreeNode fileTreeRoot) {
	// selNodes are selected buttons's objects here
	Object [] selNodes = getSelectedNodes();
	
	if (selNodes == null) return;
	if (fileTreeRoot == null) {
	    System.out.println("MetricsController: fileTreeRoot passed is NULL.");
	    return;
	}

	// update fileTree
	LinkedList datafileList = getSelectedDataFileList();
	LinkedList selMetricsList = null;
	if (fileTreeController == null) return;

	fileTreeController.refreshFileTree(datafileList);
	focusTreeController.refreshFocusTree(datafileList);
	selMetricsList = fileTreeController.getFirstLevelChildrenList();
	

	// update indexTree , set selection color as green
	// indexTreeController.getRenderer().setBackgroundSelectionColor(new Color(204, 255, 204));
	indexTreeController.getTree().clearSelection();
	treeHandler.collapseTree(indexTreeController.getTree());
	treeHandler.expandOneLevel(indexTreeController.getTree());
	indexTreeController.getRenderer().setColorList(null);

	LinkedList secondColor = new LinkedList();
	for (int i=0; i< selMetricsList.size(); i++) {
	    //fileNodeSelected(node[i]);
	    DefaultMutableTreeNode thenode = (DefaultMutableTreeNode)selMetricsList.get(i);
	    if (thenode != null) {
		fileTreeController.nodeSel_MarkIndexTree(thenode, indexTreeController.getTree(), secondColor);
	    }
	}
	if (secondColor.size() > 0) 
	    indexTreeController.getRenderer().setColorList(secondColor);

	// Double check and mark selection if there is only one focus
	markSingleFocus(secondColor);
    }

    public Component[] getButtons() {
	return buttonPanel.getComponents();
    }

    public void clearAllSelections() {
	Component[] buttons = getButtons();
	if (buttons == null) return;

	for (int i=0; i< buttons.length; i++) {
	    JButton button = (JButton)buttons[i];
	    unMarkButton(button);
	}
    }

    public JPanel getButtonPanel() {
	return buttonPanel;
    }

    // for continous selections
    protected void update_Index_Button(String command) { 
	Component [] buttons = getButtons(); 
	if (buttons == null) return; 

	int firstSelection = -1; 
	int thisSelection = -1;

	for (int i = 0; i < buttons.length; i++) {
	    JButton button = (JButton)buttons[i];
	    // Make sure there is no green color left
	    // find the first selection before selection this button
	    if (!button.isSelected())
		unMarkButton(button);
	    if (button.getText().equals(command)) 
		thisSelection = i;
	    else if ((firstSelection == -1) && (button.isSelected()))
		firstSelection = i;
	}

	if (firstSelection == -1)
	    markButton((JButton)buttons[thisSelection]);
	else {
	    int small;
	    int large;
	    if (firstSelection < thisSelection){
		small = firstSelection;
		large = thisSelection;
	    }
	    else {
		small = thisSelection;
		large = firstSelection;
	    }
	    for (int i = small; i <= large; i++) {
		JButton button = (JButton)buttons[i];
		if (!button.isSelected())
		    markButton(button);
	    }
	}  
    }
    
    public void markButton(JButton button) {
	button.setSelected(true);
	// light blue color
	button.setBackground(new Color(204, 204, 255));
    }

    public void unMarkButton(JButton button) {
	button.setSelected(false);
	// Light gray color
	button.setBackground(new Color(204,204,204));
    }
	

    public Object[] getSelectedNodes() {
	Component [] buttons = getButtons();
	if (buttons == null) {
	    System.out.println("MetricsController: buttons no selection");
	    return null;
	}
	Object [] selNodes = new Object[buttons.length];

	int index = 0;
	for (int i = 0; i < buttons.length; i++) {
	    if (((JButton)buttons[i]).isSelected()) {
		selNodes[index] = allMeasureList.get(i);
		index++;
	    }
	}
	return selNodes;
    }

    public String[] getSelectedMetricsNames() {
	int number = getSelectedNumber();

	if (number <= 0) return null;
	String [] names = new String[number];

	Component [] buttons = getButtons();

	int index = 0;
	for (int i = 0; i < buttons.length; i++) {
	    if (((JButton)buttons[i]).isSelected()) {
		names[index] = ((JButton)buttons[i]).getText();
		index ++;
	    }
	}

	return names;
    }

    public int getSelectedNumber() {
	Component [] buttons = getButtons();
	if (buttons == null) return 0;

	Object [] selNodes = new Object[buttons.length];

	int index = 0;
	for (int i = 0; i < buttons.length; i++) {
	    if (((JButton)buttons[i]).isSelected()) {
		selNodes[index] = allMeasureList.get(i);
		index++;
	    }
	}
	return index;
    }

    protected void metricsSelectionAction() {
	indexTreeController.getRenderer().setColorList(null);
	if (getSelectedNumber() == 0){
	    indexTreeController.getTree().updateUI();
	    fileTreeController.refreshFileTree(fileTreeController.getAllFilesList());
	    focusTreeController.refreshFocusTree(null);
	    return;
	}
	
	treeHandler.collapseTree(indexTreeController.getTree());
	treeHandler.expandOneLevel(indexTreeController.getTree());
	listSel_DataSets(fileTreeController.getTreeRoot());
    }

    protected void metricsIndexSelectionAction(DefaultMutableTreeNode []currentNodeIndexTree) {
	// update fileTree & focusTree
	LinkedList selFileList = indexTreeController.nodeSel_DataSetList(getSelectedDataFileList(), currentNodeIndexTree);
	

	// make sure there is no green left in Metrics
	Component [] buttons = getButtons(); 
	for (int i = 0; i < buttons.length; i++) {
	    JButton button = (JButton)buttons[i];
	    if (!button.isSelected())
		unMarkButton(button);
	}

	if (fileTreeController != null) {
	    fileTreeController.refreshFileTree(selFileList);
	}

	if (focusTreeController != null) {
	    focusTreeController.refreshFocusTree(selFileList);
	}

	// Mark indexTree
	String metricsNames [] = getSelectedMetricsNames();
	LinkedList secondColor = indexTreeController.nodeSel_MarkIndexTree(currentNodeIndexTree, metricsNames);
	indexTreeController.getRenderer().setColorList(secondColor);

	// double check and mark selection if there is only one focus
	markSingleFocus(secondColor);
    }
	
    public void mouseDragged(MouseEvent event) {
	//System.out.println("ACTION: Mouse dragged - "+event.paramString());
	dragged = true;
    }

    public void mouseMoved(MouseEvent event) {}
    public void mouseClicked(MouseEvent event) {}
    public void mousePressed(MouseEvent event) {}
    public void mouseEntered(MouseEvent event) {}
    public void mouseExited(MouseEvent event) {}
    public void mouseReleased(MouseEvent event) {
	Component obj = event.getComponent();

	if ((obj == null) || (!(obj instanceof JButton))) {
	    dragged = false;
	    return;
	}
	// set waiting cursor while process
	buttonPanel.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));

	JButton button = (JButton)event.getComponent();
	if (DEBUG)
	    System.out.println("ACTION: "+button.getText()+", "+event.paramString());
	String command = button.getText();

	// check if ctrl- or shirt- key down where action happen
	if (event.isControlDown()) {
	    
	    if (dragged) { 
		// indexTree keep selection, clear old metrics selection
		clearAllSelections();  
		markButton(button);  
	    }  
	    else { 
	    // discontinous selection or selection with indexTree
		if (!button.isSelected())
		    markButton(button);
		else 
		    unMarkButton(button);
	    } 

	    DefaultMutableTreeNode []currentNodeIndexTree = treeHandler.getAllSelectedNodes(indexTreeController.getTree());

	    // only metrics selected
	    if (currentNodeIndexTree == null) 
		metricsSelectionAction();
	    else { // both metrics and index_tree selected
		if (getSelectedNumber() == 0) {
		    // only indexTree Selected
		    indexTreeController.getRenderer().setColorList(null);
		    indexTreeController.indexNodeSelected(currentNodeIndexTree, 0);
		}
		else
		    metricsIndexSelectionAction(currentNodeIndexTree);
	    }
	}

	else if (event.isShiftDown()) {
	    // continuous selection or selection with indexTree
	    update_Index_Button(command);

	    DefaultMutableTreeNode []currentNodeIndexTree = treeHandler.getAllSelectedNodes(indexTreeController.getTree());

	    // only metrics selected
	    if (currentNodeIndexTree == null)
		metricsSelectionAction();
	    else { // both button and index_tree selection
		if (getSelectedNumber() == 0) {
		    // only indexTree Selected
		    indexTreeController.getRenderer().setColorList(null);
		    indexTreeController.indexNodeSelected(currentNodeIndexTree, 0);
		}
		else
		    metricsIndexSelectionAction(currentNodeIndexTree);
	    }
	}

	else if (event.isAltDown()) {
	    // So far AltDown doesn't work, AltDown couldn't be heard
	    // keep indexTreeSelection, but clear metrics selection
	    // System.out.println("Alt is down!!");
	    clearAllSelections();
	    markButton(button);

	    DefaultMutableTreeNode []currentNodeIndexTree = treeHandler.getAllSelectedNodes(indexTreeController.getTree());

	    // only metrics selected
	    if (currentNodeIndexTree == null)
		metricsSelectionAction();
	    else
		metricsIndexSelectionAction(currentNodeIndexTree);
	}
	else { 
	    //System.out.println("Nothing special is down!!");
	    // simple Select and  Disselect other button
	    clearAllSelections();
	    markButton(button);
	    
	    metricsSelectionAction();
	}

	// set dragged back to false if it turned to be true
	dragged = false;

	// Display Devise if in Display mode
	if (focusTreeController.isInDisplayMode())
	    focusTreeController.displayModeSelection();

	// set back default cursor
	buttonPanel.setCursor(defaultCursor);


    }
}
