//----------------------------------------------------------------------
// 
// File Name:    XManBrowser.java
// 
// Description:  Experiment Management Browser Window
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import java.awt.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.Dimension;

public class XManBrowser extends JPanel {
    private JTextArea infoArea;

    //------------------------------------------
    // Constructor
    //------------------------------------------
    public XManBrowser() {

	infoArea = new JTextArea("Experiment Info: \n  Number of Experiments: \n  Last Modified");
	infoArea.setEditable(false);

        setLayout(new BorderLayout());
	add(infoArea, BorderLayout.CENTER);
	//add(splitPane_, BorderLayout.CENTER);
	//setMinimumSize(new Dimension(200, 200));
    }

    public Dimension getPreferredSize()
    {
	return new Dimension(200, 200);
    }

    public Dimension getMinimumSize()
    {
	return new Dimension(200, 200);
    }
}







