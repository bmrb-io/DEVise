//----------------------------------------------------------------------
// 
// File Name:    XManAttibuteEditor.java
// 
// Description:  Attributes editor
//
// Author:       Hao Wang
// 
// D.O.B.        Today
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JButton;
import javax.swing.BoxLayout;
import java.awt.Insets;
import java.awt.event.*;
import java.awt.Font;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

//----------------------------------------------------------------------
// Private imports
//----------------------------------------------------------------------
import xman.shared.*;
import xman.importers.*;

public class XManAttributeEditor 
    extends JPanel 
    implements ActionListener{

    private JButton commitButton_, resetButton_, deleteButton_, addButton_, changeButton_;
    private XManImportBase importer_;
    private XManAttributeTable attributeTable_;

    final static String XMAN_COMMAND_COMMIT = "COMMIT";
    final static String XMAN_COMMAND_RESET  = "RESET";
    final static String XMAN_COMMAND_ADD    = "ADD";
    final static String XMAN_COMMAND_DELETE = "DELETE";
    final static String XMAN_COMMAND_CHANGE = "CHANGE";

    public XManAttributeEditor(XManImportBase importer) {
	super();

	importer_ = importer;

        setLayout(new GridBagLayout());
	GridBagConstraints gbc = new GridBagConstraints();
	gbc.anchor = GridBagConstraints.WEST;
	gbc.insets = new Insets(2, 2, 2, 2);

	attributeTable_ = new XManAttributeTable(importer);

	JScrollPane scrollPane = new JScrollPane(attributeTable_);

	gbc.gridx = 0;
	gbc.gridy = 1;
	gbc.fill = GridBagConstraints.BOTH;
	//gbc.gridwidth = 3;
	add(scrollPane, gbc);

	// Add buttons
	JPanel panel = new JPanel();

	commitButton_ = new JButton("Commit Changes");
	resetButton_  = new JButton("Reset");
	addButton_    = new JButton("Add");
	deleteButton_ = new JButton("Delete");
	changeButton_ = new JButton("Change");
	commitButton_.setFont(Resources.defaultFont);
	resetButton_.setFont(Resources.defaultFont);
	addButton_.setFont(Resources.defaultFont);
	deleteButton_.setFont(Resources.defaultFont);
	changeButton_.setFont(Resources.defaultFont);

	addButton_.addActionListener(this);
	commitButton_.addActionListener(this);
	//changeButton_.addActionListener(this);

	commitButton_.setActionCommand(XMAN_COMMAND_COMMIT);
	resetButton_.setActionCommand(XMAN_COMMAND_RESET);
	addButton_.setActionCommand(XMAN_COMMAND_ADD);
	deleteButton_.setActionCommand(XMAN_COMMAND_DELETE);
	changeButton_.setActionCommand(XMAN_COMMAND_CHANGE);
	deleteButton_.setEnabled(false);

	//panel.setLayout(new GridBagLayout());
	//GridBagConstraints buttonGBC = new GridBagConstraints();
	//buttonGBC.anchor = GridBagConstraints.WEST;
	//buttonGBC.fill = GridBagConstraints.HORIZONTAL;
	//buttonGBC.insets = new Insets(2, 2, 2, 2);


	panel.add(addButton_);
	panel.add(changeButton_);
	panel.add(deleteButton_);
	panel.add(commitButton_);
	//panel.add(resetButton_);
	
	gbc.gridx = 0;
	gbc.gridy += 5;
	add(panel, gbc);

	setMinimumSize(getMinimumSize());
    } // constructor

    public void actionPerformed(ActionEvent e) {
	String action = e.getActionCommand();
	if (action == XMAN_COMMAND_COMMIT) {
	    importer_.forceRefresh();
	}
	else if (action == XMAN_COMMAND_RESET) {
	}
	else if (action == XMAN_COMMAND_ADD) {
	    addNewData();
	}
	else if (action == XMAN_COMMAND_CHANGE) {
	    //changeData();
	}
	else if (action == XMAN_COMMAND_DELETE) {
	}
	else {
	}
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(350, 400);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(350, 400);
    }

    private void addNewData() {
	XManAttributeDialog newAttribute = new XManAttributeDialog();

	newAttribute.pack();
	newAttribute.show();

	XManAttribute attribute = newAttribute.newAttribute();

	if (attribute.attributeName().length() != 0) {
	    importer_.addAttribute(attribute);
	}
    }

    private void changeData() {
	

    }
}

