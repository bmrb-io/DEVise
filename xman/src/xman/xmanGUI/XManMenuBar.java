package xman.xmanGUI;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JMenuBar;
import javax.swing.KeyStroke;
import javax.swing.ImageIcon;
import javax.swing.JFileChooser;
import javax.swing.JPanel;

public class XManMenuBar extends JMenuBar 
    implements ItemListener {
    private JFrame parent_;

    public XManMenuBar(JFrame         frame,
		       ActionListener actionListener) {
	super();
	parent_ = frame;
        JMenu menu;
        JMenuItem menuItem;
	Font defaultFont = new Font("Times", Font.BOLD, 12);
	setFont(defaultFont);

        //Build the first menu.
	menu = new JMenu("Experiments");
	menu.setFont(defaultFont);
        menu.setMnemonic(KeyEvent.VK_A);
        menu.getAccessibleContext().setAccessibleDescription("Experiments");
        add(menu);

	menuItem = new JMenuItem("Create a New Experiment Collection");
	menuItem.setFont(defaultFont);
        menuItem.setMnemonic(KeyEvent.VK_C);
        menuItem.getAccessibleContext().setAccessibleDescription("Create");
	menuItem.addActionListener(actionListener);
        menu.add(menuItem);
	

	menuItem = new JMenuItem("Open an Existing Experiment Collection");
	menuItem.setFont(defaultFont);
        menuItem.setMnemonic(KeyEvent.VK_O);
        menuItem.getAccessibleContext().setAccessibleDescription("Open");
	menuItem.setActionCommand("Open");
	menuItem.addActionListener(actionListener);
        menu.add(menuItem);

	menu.addSeparator();
        menuItem = new JMenuItem("Quit");
	menuItem.setFont(defaultFont);
        menuItem.setMnemonic(KeyEvent.VK_Q);
	menuItem.setActionCommand("Quit");
	menuItem.addActionListener(actionListener);
	menu.add(menuItem);

	// Add second menu
        menu = new JMenu("Action");
	menu.setFont(defaultFont);
        menu.setMnemonic(KeyEvent.VK_A);
        menu.getAccessibleContext().setAccessibleDescription("Action");
        add(menu);

	menuItem = new JMenuItem("Browse");
	menuItem.setFont(defaultFont);
        menuItem.setMnemonic(KeyEvent.VK_B);
        menuItem.getAccessibleContext().setAccessibleDescription("Browse");
        menu.add(menuItem);

	menuItem = new JMenuItem("Add New Experiment");
	menuItem.setFont(defaultFont);
        menuItem.setMnemonic(KeyEvent.VK_N);
        menuItem.getAccessibleContext().setAccessibleDescription("Browse");
        menu.add(menuItem);

	menuItem = new JMenuItem("Add New Attributes");
	menuItem.setFont(defaultFont);
        menuItem.setMnemonic(KeyEvent.VK_A);
        menuItem.getAccessibleContext().setAccessibleDescription("Browse");
        menu.add(menuItem);

        //Build second menu in the menu bar.
        menu = new JMenu("Help");
	menu.setFont(defaultFont);
	menu.setMnemonic(KeyEvent.VK_N);
        menu.getAccessibleContext().setAccessibleDescription("Help");
        add(menu);
    }

    public void itemStateChanged(ItemEvent e) {
        JMenuItem source = (JMenuItem)(e.getSource());
	if (source.getText() == "Quit")
	    {
		if (e.getStateChange() == ItemEvent.SELECTED)
		    {
			System.exit(0);
		    }
	    }
    }
}



