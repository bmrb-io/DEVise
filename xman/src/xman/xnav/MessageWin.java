package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import java.util.*;
//import java.beans.*;

class MessageWin extends JFrame implements ActionListener{

    public String title = "Message Window";
    public String multipleSelection = "Multiple Selection";

    public MessageWin(String messageString) {

	JDialog messageDialog= new JDialog (this, title, true);

	//position the dialog  in the parent window
	Rectangle bounds = this.getBounds();
	messageDialog.setLocation(bounds.x +bounds.width/3, bounds.y + bounds.height/3);
	//set up panel to display the message
	JPanel messagePane = new JPanel();
	JTextArea message = new JTextArea(messageString);
	messagePane.add(message);
	messageDialog.getContentPane().add(messagePane, BorderLayout.CENTER);
	//create the dialog buttons
	JPanel buttonPane = new JPanel();
	JButton button = new JButton("OK");
	buttonPane.add(button);
	button.addActionListener(this);
	messageDialog.getContentPane().add(buttonPane, BorderLayout.SOUTH);
	//pack();

	messageDialog.show();
    }

    //Handle button events
    public void actionPerformed(ActionEvent e)
    {
	JOptionPane.showMessageDialog(this, "You clicked OK");
	dispose();        //hide the dialog
    }



    //  If argv[0] is present it is considered to be a filename
    public static void main(String[] argv) throws Exception {

	JFrame frame = new MessageWin("Multiple Selection");

	frame.addWindowListener(new java.awt.event.WindowAdapter() {
	    public void windowClosing(java.awt.event.WindowEvent e) {
		System.exit(0);
	    }
        });
        frame.setSize(200, 350);
	//frame.pack();
        //frame.setVisible(true);
	
    } 
}
