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

import java.awt.*;
import java.awt.event.*;
import java.awt.Font;
import java.awt.FlowLayout.*;
import xman.xnav.*;
//----------------------------------------------------------------------
// Private imports
//----------------------------------------------------------------------
import xman.shared.*;

//----------------------------------------------------------------------
//
// Class Name        : XManButtonsPanel
// Class Description : Buttons for user to push :-)
//
//----------------------------------------------------------------------

public class XManButtonPanel 
    extends JPanel {
	    //    implements ActionListener{

    //protected JButton okayButton   = new JButton(Resources.STR_OKAY);
    protected JButton viewButton   = new JButton("View Selected Data");
    protected JButton selectAllButton = new JButton("Display All Data");
    protected JButton deselectAllButton = new JButton("Unselect All");

    public XManButtonPanel(ActionListener listener) {
	
	//okayButton.addActionListener(listener);
	viewButton.addActionListener(listener);
	selectAllButton.addActionListener(listener);
	deselectAllButton.addActionListener(listener);

	Font defaultFont = new Font("Times", Font.BOLD, 12);
	//okayButton.setFont(defaultFont);
	viewButton.setFont(defaultFont);
	selectAllButton.setFont(defaultFont);
	deselectAllButton.setFont(defaultFont);
	viewButton.setActionCommand("SELECT");
	selectAllButton.setActionCommand("RESET");
        //add(okayButton);
	add(viewButton);
	add(selectAllButton);
	//add(deselectAllButton);
	//add(changeButton);

	//setLayout();
    }

}


