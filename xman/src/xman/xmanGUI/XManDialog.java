package xman.xmanGUI;

import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JButton;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.BoxLayout;
import javax.swing.BorderFactory;
import javax.swing.JRadioButton;
import javax.swing.JComboBox;
import javax.swing.JTextField;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.JSeparator;
import javax.swing.plaf.metal.MetalSeparatorUI;
import javax.swing.JFileChooser;
import java.io.File;
import java.awt.*;
import java.awt.event.*;
import javax.swing.border.Border;

import xman.shared.*;

public class XManDialog extends JDialog {

    private JButton okayButton         = null; 
    private JButton cancelButton       = null;

    private JRadioButton newCollection = null;
    private JRadioButton oldCollection = null;
    private ButtonGroup  group;
	
    private JComboBox patternList;
    private String    traceFormat_;

    private JTextField workingDir_;

    final static String STRING_NEW = "NEW";
    final static String STRING_OLD = "OLD";
    final static String STR_META_NOT_EXIST = 
	"This directory does not contain the meta file";

    public XManDialog() {
	super();

	setModal(true);

	setTitle("Starting up...");

	// Add some borders
	Border etched = BorderFactory.createEtchedBorder();
	getLayeredPane().setBorder(etched);

	getContentPane().setLayout(new GridBagLayout());   // Set layout

	GridBagConstraints gbc = new GridBagConstraints();
	gbc.anchor = GridBagConstraints.WEST;
	gbc.insets = new Insets(1, 1, 1, 1);

	gbc.gridy = 0;
	addSelection(gbc);

	final JSeparator sep = new JSeparator();
	final MetalSeparatorUI sepUI = new MetalSeparatorUI();
	
	sep.setUI(sepUI);

	gbc.gridy = 1;
	getContentPane().add(sep, gbc);

	gbc.gridy = 2;
	addPatternList(gbc);

	gbc.gridy = 3;
	getContentPane().add(sep, gbc);

	// Now, the entry field
	gbc.gridy = 4;
	addEntryField(gbc);

	gbc.gridy = 5;
	getContentPane().add(sep, gbc);

	gbc.gridy = 6;
	addButtons(gbc);

	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {System.exit(0);}
	});
    }	

    public String workingDir() {
	return workingDir_.getText();
    }

    private void addSelection(GridBagConstraints gbc) {
	final JLabel selectionLabel = new JLabel("Please select:");
	final JPanel panel = new JPanel();

	selectionLabel.setFont(Resources.defaultFont);

	// Radio button group let user pick new collection or old collection
	newCollection = 
	    new JRadioButton("Creating a new experiment collection", true);
	oldCollection = 
	    new JRadioButton("Working with an existing collection", false); 
	newCollection.setActionCommand(STRING_NEW);
	oldCollection.setActionCommand(STRING_OLD);
	newCollection.setFont(Resources.defaultFont);
	oldCollection.setFont(Resources.defaultFont);

	group = new ButtonGroup();
	group.add(newCollection);
	group.add(oldCollection);

	// Add it to the pane
	panel.setLayout(new GridLayout(2,1));
	panel.add(newCollection);
	panel.add(oldCollection);

	gbc.gridx = 0;
	getContentPane().add(selectionLabel, gbc);

	gbc.gridx = 1;
	getContentPane().add(panel, gbc);

	class SelectionListener implements ActionListener {
	    public void actionPerformed(ActionEvent e) {
		String choice = group.getSelection().getActionCommand();
		if (choice == STRING_NEW) {
		    patternList.setEnabled(true);
		}
		else {
		    patternList.setEnabled(false);
		}
	    }
	}

	SelectionListener selection = new SelectionListener();
	newCollection.addActionListener(selection);
	oldCollection.addActionListener(selection);
    }

    private void addPatternList(GridBagConstraints gbc) {
	final JLabel patternLabel = new JLabel("Please select trace format");

	patternLabel.setFont(Resources.defaultFont);

	// Now the pattern list
	patternList = new JComboBox(Resources.importFilter);
	patternList.setFont(Resources.defaultFont);

        patternList.setAlignmentX(Component.LEFT_ALIGNMENT);
        patternList.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JComboBox cb = (JComboBox)e.getSource();
                String newSelection = (String)cb.getSelectedItem();
                traceFormat_ = newSelection;
            }
        });
	
	gbc.gridx = 0;
	getContentPane().add(patternLabel, gbc);
	gbc.gridx = 1;
	getContentPane().add(patternList, gbc);

    }

    private void addEntryField(GridBagConstraints gbc) {
	final JLabel entryLabel = new JLabel("Working directory");
	final JPanel panel      = new JPanel();

	entryLabel.setFont(Resources.defaultFont);
	panel.setLayout(new BorderLayout());

	workingDir_ = new JTextField(32);
	workingDir_.setEditable(true);

	entryLabel.setLabelFor(workingDir_);
	
	panel.add(workingDir_, BorderLayout.CENTER);
	
	//Create the open button
        JButton openButton = new JButton("Pick a directory");
	final JFileChooser fimport = new JFileChooser();
	fimport.setFont(Resources.defaultFont);
	fimport.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	fimport.setApproveButtonText("Select directory");

	openButton.setFont(Resources.defaultFont);
        openButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int returnVal = fimport.showOpenDialog(XManDialog.this);

                if (returnVal == JFileChooser.APPROVE_OPTION) {
                    File file = fimport.getSelectedFile();
		    workingDir_.setText(file.toString());
                } 
            }
        });

	panel.add(openButton, BorderLayout.EAST);

	gbc.gridx = 0;
	getContentPane().add(entryLabel, gbc);
	gbc.gridx = 1;
	getContentPane().add(panel, gbc);
    }

    private void addButtons(GridBagConstraints gbc) {
	final JPanel panel = new JPanel();

	okayButton = new JButton("Okay");
	cancelButton = new JButton("Cancel");
	
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
		    String choice = group.getSelection().getActionCommand();
		    if (choice == STRING_OLD) {
			// Double check to make sure the directory 
			// contains an xman meta file
			File file = new File(workingDir_.getText() + 
					     File.separator +
					     Resources.STR_XMAN_META_FILE);
			if (file.exists()) {
			    setVisible(false);
			    dispose();
			}
			else {
			    String message = new String(STR_META_NOT_EXIST);

			    JOptionPane.showMessageDialog(XManDialog.this, 
							  message, 
							  "alert",
							  JOptionPane.ERROR_MESSAGE);
			}
		    }
		    else {
			setVisible(false);
			dispose();
		    }
		}
		else {
		    System.exit(0);
		}
	   }
	}

	ButtonListener buttonListener = new ButtonListener();

	okayButton.addActionListener(buttonListener);
	cancelButton.addActionListener(buttonListener);
    }
}






