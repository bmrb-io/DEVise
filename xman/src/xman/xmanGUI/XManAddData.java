//----------------------------------------------------------------------
// 
// File Name:    XManAddData.java
// 
// Description:  Experiment Management Add Data Window
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
import java.io.IOException;
import java.io.FileNotFoundException;
import javax.swing.JScrollPane;
import javax.swing.JOptionPane;
import java.awt.*;
import java.awt.event.*;
import java.awt.Font;
import java.awt.Dimension;
//----------------------------------------------------------------------
// Private imports
//----------------------------------------------------------------------
import xman.shared.*;
import xman.importers.*;

public class XManAddData extends JPanel {

    private String current, currentArch_, finString_;
    private JTextField importDir_, exportDir_, experimentName_, cpu_;
    private JPanel resultPanel, patternPanel;
    private JButton importButton_, clearButton_;
    private JLabel  date;
    private XManImportBase importer_;
    private XManAddTable   attrTable_;

    public XManAddData(XManImportBase importer) {
	final JLabel result, experimentLabel, dateLabel,cpuLabel;

	importer_ = importer;

	final JFileChooser fimport = new JFileChooser();
	fimport.setFont(Resources.defaultFont);
	fimport.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	fimport.setApproveButtonText("Select");

        current = Resources.importFilter[0];

	setLayout(new GridBagLayout());
	GridBagConstraints gbc = new GridBagConstraints();
	gbc.anchor = GridBagConstraints.WEST;
	gbc.insets = new Insets(2, 2, 2, 2);

        // Set up the UI for selecting a pattern.
        JLabel patternLabel = new JLabel("Select trace format");
	patternLabel.setFont(Resources.defaultFont);

        JComboBox patternList = new JComboBox(Resources.importFilter);
	patternList.setFont(Resources.defaultFont);

        patternList.setAlignmentX(Component.LEFT_ALIGNMENT);
        patternList.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox)e.getSource();
                String newSelection = (String)cb.getSelectedItem();
                current = newSelection;
            }
        });

	gbc.gridy = 0;
	gbc.gridx = 0;
	add(patternLabel, gbc);
	gbc.gridx = 1;
	add(patternList, gbc);

        // Create the UI for displaying result
	//      JLabel fileLabel = new JLabel("Import from ...", JLabel.LEFT);
	//	fileLabel.setFont(Resources.defaultFont);
	importDir_ = new JTextField(32);
	importDir_.setEditable(true);
	importDir_.setFont(Resources.defaultFont);
	//fileLabel.setLabelFor(importDir_);

        //Create the open button
        JButton openButton = new JButton("Import from ...");
	openButton.setFont(Resources.defaultFont);
        openButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int returnVal = fimport.showOpenDialog(XManAddData.this);

                if (returnVal == JFileChooser.APPROVE_OPTION) {
                    File file = fimport.getSelectedFile();
		    importDir_.setText(file.toString());
                } 
		else {
                }
            }
        });

	++gbc.gridy;
	gbc.gridx = 0;
	gbc.gridwidth = 2;
	add(importDir_, gbc);
	gbc.gridwidth = 1;
	gbc.gridx = 2;
	add(openButton, gbc);

	attrTable_ = new XManAddTable(importer);
	JScrollPane scrollPane = new JScrollPane(attrTable_);
	gbc.gridy = gbc.gridy + 1;
	gbc.gridx = 0;
	gbc.gridwidth  = 3;
	//gbc.gridheight = 3;
	gbc.fill = GridBagConstraints.BOTH;
	add(scrollPane, gbc);

	clearButton_ = new JButton("Clear All");
	importButton_ = new JButton("Import Data");

	importButton_.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {

		finString_  = new String(importDir_.getText());
		
                if (finString_.length() > 0) {
		    try {
			importer_.addNewExperiment(attrTable_.getData(), 
						   finString_);
		    }
		    catch(Exception exception) {
			JOptionPane.showMessageDialog(XManAddData.this, 
						      exception.getMessage(), 
						      "alert",
						      JOptionPane.ERROR_MESSAGE);
		    }
                } 
		else {
                }
            }
        });

	clearButton_.setFont(Resources.defaultFont);
	importButton_.setFont(Resources.defaultFont);
	gbc.gridy = gbc.gridy + 6;
	gbc.gridx = 1;
	gbc.fill = GridBagConstraints.NONE;
	JPanel panel = new JPanel();
	panel.add(clearButton_);
	panel.add(importButton_);
	gbc.gridwidth = 1;
	add(panel, gbc);
	setMinimumSize(getMinimumSize());
    } // constructor

    public Dimension getPreferredSize()
    {
	return new Dimension(350, 450);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(350, 450);
    }
}





