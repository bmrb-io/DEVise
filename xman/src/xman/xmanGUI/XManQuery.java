//----------------------------------------------------------------------
// 
// File Name:    XManQuery.java
// 
// Description:  Experiment Management Query Window
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.AbstractButton;
import javax.swing.JButton;
import javax.swing.*;
import javax.swing.border.*;
import java.util.*;
import java.text.*;
import java.io.File;

import java.awt.*;
import java.awt.event.*;
import java.awt.Font;
import java.awt.Dimension;
//----------------------------------------------------------------------
// Private imports
//----------------------------------------------------------------------
import xman.shared.*;
import xman.importers.*;

public class XManQuery extends JPanel
    implements ActionListener {

    private JButton searchButton_, clearButton_;
    private XManImportBase importer_;
    private XManQueryPanel queryPanel_;
    final static String XMAN_COMMAND_SEARCH = "Search";
    final static String XMAN_COMMAND_CLEAR  = "Clear";
    final static String STR_HELP_TEXT ="Please enter the value for the corresponding attribute and click on the search button.";
    public XManQuery(XManImportBase importer) {
	super();

	importer_ = importer;

        setLayout(new GridBagLayout());
	GridBagConstraints gbc = new GridBagConstraints();
	gbc.anchor = GridBagConstraints.WEST;
	gbc.fill = GridBagConstraints.HORIZONTAL;
	gbc.insets = new Insets(2, 2, 2, 2);

	queryPanel_ = new XManQueryPanel(importer);

	JScrollPane scrollPane = new JScrollPane(queryPanel_);

	//JTextArea label = new JTextArea(STR_HELP_TEXT, 3, 20);
	//label.setFont(Resources.defaultFont);

	gbc.gridx = 0;
	gbc.gridy = 1;
	//add(label, gbc);

	add(scrollPane, gbc);

	// Add buttons
	JPanel panel = new JPanel();

	clearButton_ = new JButton("Clear All");
	searchButton_ = new JButton("Search");
	clearButton_.setFont(Resources.defaultFont);
	clearButton_.setActionCommand(XMAN_COMMAND_CLEAR);
	searchButton_.setFont(Resources.defaultFont);
	searchButton_.setActionCommand(XMAN_COMMAND_SEARCH);
	searchButton_.addActionListener(this);
	clearButton_.addActionListener(this);
	panel.add(searchButton_);
	panel.add(clearButton_);
	
	gbc.gridy = gbc.gridy + 4;
	gbc.gridx = 0;
	add(panel, gbc);

	setMinimumSize(getMinimumSize());
    } // constructor

    public void actionPerformed(ActionEvent e) {
	String action = e.getActionCommand();
	if (action == XMAN_COMMAND_SEARCH) {
	    System.out.println("Searching....");
	    if (importer_.search(queryPanel_.queryData())) {
		System.out.println("Good");
	    }
	}
	else if (action == XMAN_COMMAND_CLEAR) {
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
}





