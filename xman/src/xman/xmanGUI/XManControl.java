//----------------------------------------------------------------------
// 
// File Name:    x-contrl.java
// 
// Description:  Experiment Management Main Control Window
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.JTabbedPane;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.AbstractButton;
import javax.swing.JButton;
import javax.swing.tree.*;
import javax.swing.JTree;

import java.awt.*;
import java.awt.event.*;
import java.awt.Font;
//----------------------------------------------------------------------
// Private imports
//----------------------------------------------------------------------
import xman.shared.*;
import xman.importers.*;

//----------------------------------------------------------------------
//
// Class Name        : XManMainControl
// Class Description : Main control window for experiment management
//
//----------------------------------------------------------------------
public class XManControl 
    extends JPanel {

    private XManImportBase dataServer_;

    public XManControl(XManImportBase manager) {

	dataServer_ = manager;

        JTabbedPane tabbedPane = new JTabbedPane();

	tabbedPane.setFont(Resources.defaultFont);
        //Component browsePanel = new XManBrowser();
	//browsePanel.setFont(Resources.defaultFont);
        //tabbedPane.addTab(Resources.STR_BROWSE_TABLET, browsePanel);

        Component dataPanel = new XManAddData(dataServer_);
	dataPanel.setFont(Resources.defaultFont);
        tabbedPane.addTab(Resources.STR_DATA_TABLET, dataPanel);

        Component attributePanel = new XManAttributeEditor(dataServer_);

	attributePanel.setFont(Resources.defaultFont);
        tabbedPane.addTab(Resources.STR_ATTRIBUTE_TABLET, attributePanel);

        Component queryPanel = new XManQuery(dataServer_);
	queryPanel.setFont(Resources.defaultFont);
        tabbedPane.addTab(Resources.STR_QUERY_TABLET, queryPanel);

        tabbedPane.setSelectedIndex(0);

        //Add the tabbed pane to this panel.
        setLayout(new GridLayout(1, 1)); 
        add(tabbedPane);
	repaint();
	setMinimumSize(getMinimumSize());
    } // constructor

    public Dimension getPreferredSize()
    {
	return new Dimension(400, 400);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(400, 400);
    }
}


