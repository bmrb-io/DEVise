//----------------------------------------------------------------------
// 
// File Name:    XManFrame.java
// 
// Description:  Experiment Management Frame Window
//
// Author:       Hao Wang
// 
// D.O.B.        02/27/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.Dimension;
import java.util.Observer;
import java.util.Observable;

import xman.shared.*;
import xman.importers.*;
import xman.xnav.*;

public class XManFrame extends JFrame
    implements ActionListener {

    // Some data
    protected DefaultTreeModel treeModel;

    private JSplitPane splitPane_;
    private XManDataPanel dataTree_;
    private XManControl xmanControl_;
    private XManImportBase dataManager_;
    private Toolkit toolkit = Toolkit.getDefaultToolkit();
    private XManButtonPanel buttons;
    private ThreadInterface showFrame_;
    private SelectedData    selectedData_ = new SelectedData();
    private String workingDir_;

    public XManFrame(String title, String workingDir)
	throws java.io.IOException {
	super(title);      // Set the title for window

	workingDir_ = workingDir;

	setJMenuBar(new XManMenuBar(this, this));
	
	// This should be passed in
	dataManager_ = new XManImportParadyn(workingDir_);

	dataTree_ = new XManDataPanel(dataManager_);

	JScrollPane scrollPane = 
	    new JScrollPane(dataTree_, 
			    JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
			    JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);

	JPanel panel_ = new JPanel();
	panel_.setLayout(new BoxLayout(panel_, BoxLayout.Y_AXIS));

	panel_.add(scrollPane);
	buttons = new XManButtonPanel(this);
	panel_.add(buttons);

	xmanControl_ = new XManControl(dataManager_);

	splitPane_ = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, 
				    xmanControl_,
   				    panel_);

	splitPane_.setDividerLocation(400);
	splitPane_.setAutoscrolls(true);

	getContentPane().add(splitPane_, BorderLayout.CENTER);
    }

    public void actionPerformed(ActionEvent e) {
	if (e.getActionCommand() == "Quit") {
	    try {
		System.out.println("Flushing meta file");
		dataManager_.flushMetaFile();
		System.exit(0);
	    }
	    catch(Exception exception) {
	    }
	}
	else if (e.getActionCommand() == "Browse")  {
	}
	else if (e.getActionCommand() == "Open")  {
	    System.out.print("Open\n");
	}
	else if (e.getActionCommand() == "SELECT")   {
	    int index = dataManager_.indexOf(XManImportBase.STR_SOURCE);
	    String dirName[] = dataTree_.getSelectedData(index);
	    
	    if (dirName.length > 0) {
		if (showFrame_ == null) {
		    showFrame_ = new ThreadInterface();
		    showFrame_.setSize(750, 650);
		    showFrame_.setVisible(true);
		    selectedData_.addObserver(showFrame_);
		    showFrame_.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
			    //showFrame_.dispose();
			    showFrame_.setVisible(false);
			}
		    });
		}
		else if (!showFrame_.isVisible()) {
		    showFrame_.setVisible(true);
		}
		if (showFrame_ != null) {
		    selectedData_.notify(dirName);
		}
	    }
	}
	else   {
	}
    }

    class SelectedData extends Observable {
	public void SelectedData() {
	}

	public void notify(String[] dirName) {
	    setChanged();
	    notifyObservers(dirName);
	}
    }
}







