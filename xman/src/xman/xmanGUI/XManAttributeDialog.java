//----------------------------------------------------------------------
// 
// File Name:    XManAttibuteDialog.java
// 
// Description:  A dialog for adding/modidying attributes
//
// Author:       Hao Wang
// 
// D.O.B.        Today
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JDialog;
import javax.swing.JCheckBox;
import javax.swing.JTextField;
import javax.swing.JLabel;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.BorderLayout;
import java.awt.event.*;
import javax.swing.JPanel;
import javax.swing.BoxLayout;
import javax.swing.BorderFactory;
import java.awt.Insets;

import xman.importers.*;
import xman.shared.*;

public class XManAttributeDialog 
 extends JDialog {
    
    private JButton okayButton         = new JButton("Okay"); 
    private JButton cancelButton       = new JButton("Cancel");;
    private JTextField attributeName_;
    private JTextField attributeValue_;
    private JCheckBox  editable_, displayable_;
    private boolean deleteable_;
    private XManAttribute attribute_;

    final static String STR_EDITABLE = "To make this attribute changeable";
    final static String STR_DISPLAYABLE = "To make this attribute appear in search panel";
    //------------------------------------------
    // Use this method for adding a new one
    //------------------------------------------
    public XManAttributeDialog() {
	super();

	setTitle("Adding a new attribute");
	
	attribute_ = new XManAttribute(new String(), new String(), true, true, true);
	initialize();
    }

    //------------------------------------------
    // Use this method for modifying an old one
    //------------------------------------------
    public XManAttributeDialog(XManAttribute attribute) {
	super();

	setTitle("Modifying an existing attribute");

	attribute_ = attribute;

	initialize();
    }

    public XManAttribute newAttribute() {
	return attribute_;
    }

    private void initialize() {
	
	setModal(true);

	getContentPane().setLayout(new GridBagLayout());   // Set layout

	GridBagConstraints gbc = new GridBagConstraints();
	gbc.anchor = GridBagConstraints.WEST;
	gbc.insets = new Insets(1, 1, 1, 1);

	gbc.gridy = 0;
	addTextField(gbc);

	//final JSeparator sep = new JSeparator();
	//final MetalSeparatorUI sepUI = new MetalSeparatorUI();
	//sep.setUI(sepUI);

	gbc.gridy = 3;
	addCheckButtons(gbc);

	// Now, the entry field
	gbc.gridy = 5;
	addButtons(gbc);

	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) { dispose();}
	});
    }

    private void addTextField(GridBagConstraints gbc) {
	attributeName_ = new JTextField(attribute_.attributeName(),32);
	attributeValue_ = new JTextField(attribute_.defaultValue(), 32);
	final JLabel entryLabel = new JLabel("Attribute name: ");
	final JLabel defaultLabel = new JLabel("Default value: ");
	entryLabel.setFont(Resources.defaultFont);
	defaultLabel.setFont(Resources.defaultFont);
	attributeName_.setFont(Resources.defaultFont);
	attributeValue_.setFont(Resources.defaultFont);

	if (attributeName_.getText().length() == 0) {
	    attributeName_.setEditable(true);
	}
	else {
	    attributeName_.setEditable(false);
	}
	gbc.gridx = 0;
	getContentPane().add(entryLabel, gbc);
	gbc.gridx = 1;
	getContentPane().add(attributeName_, gbc);

	gbc.gridy++;

	gbc.gridx = 0;
	getContentPane().add(defaultLabel, gbc);
	gbc.gridx = 1;
	getContentPane().add(attributeValue_, gbc);
    }

    private void addCheckButtons(GridBagConstraints gbc) {

	editable_ = new JCheckBox(STR_EDITABLE, 
				       attribute_.isEditable().booleanValue());
	displayable_ = new JCheckBox(STR_DISPLAYABLE,
				     attribute_.isDisplayable().booleanValue());    

	editable_.setFont(Resources.defaultFont);
	displayable_.setFont(Resources.defaultFont);
	
	gbc.gridx = 1;
	getContentPane().add(editable_, gbc);
	gbc.gridy += 1;
	getContentPane().add(displayable_, gbc);
    }

    private void addButtons(GridBagConstraints gbc) {
	final JPanel panel = new JPanel();

	okayButton.setFont(Resources.defaultFont);
	cancelButton.setFont(Resources.defaultFont);

	okayButton.setActionCommand("Okay");
	cancelButton.setActionCommand("Cancel");
	panel.setLayout(new BorderLayout());
	
	panel.add(okayButton, BorderLayout.WEST);
	panel.add(cancelButton, BorderLayout.EAST);

	gbc.anchor = GridBagConstraints.EAST;
	getContentPane().add(panel, gbc);

	class ButtonListener implements ActionListener {
	   public void actionPerformed(ActionEvent e) {
		String action = e.getActionCommand();
		if (action == "Okay") {
		    if (attributeName_.getText().length() == 0) {
			String message = new String("Attribute field is required");
			JOptionPane.showMessageDialog(XManAttributeDialog.this, 
						      message, 
						      "alert",
						      JOptionPane.ERROR_MESSAGE
						      );
		    }
		    else {
			attribute_.setDefaultValue(attributeValue_.getText());
			attribute_.setAttributeName(attributeName_.getText());
			attribute_.setEditable(editable_.isSelected());
			attribute_.setDisplayable(displayable_.isSelected());
			setVisible(false);
			dispose();
		    }
		}
		else {
		    setVisible(false);
		    dispose();
		}
	   }
	}

	ButtonListener buttonListener = new ButtonListener();

	okayButton.addActionListener(buttonListener);
	cancelButton.addActionListener(buttonListener);
    }
}





