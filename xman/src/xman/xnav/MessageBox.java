package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import java.util.*;
//import java.beans.*;

class MessageBox extends JDialog implements ActionListener
{

    //protected JDialog messageDialog;
    protected String title = "MessageBox";

    public MessageBox(Frame frame, String messageString)
    {

	super(frame, "MessageBox", true);  //create modal dialog
	//position the dialog  in the parent window
	Rectangle bounds = frame.getBounds();
	setLocation(bounds.x +bounds.width/3, bounds.y + bounds.height/3);
	
	//set up panel to display the message
	JPanel messagePane = new JPanel();
	//Label message = new Label(messageString); //label to hold the message
	JTextArea message = new JTextArea(messageString);
	messagePane.add(message);
	getContentPane().add(messagePane, BorderLayout.CENTER);
	
	//create the dialog buttons
	JPanel buttonPane = new JPanel();
	JButton button = new JButton("OK");
	buttonPane.add(button);
	button.addActionListener(this);
	getContentPane().add(buttonPane, BorderLayout.SOUTH);
	pack();
    }
    
    //Handle button events
    public void actionPerformed(ActionEvent e)
    {
	setVisible(false);        //hide the dialog
    }

    //  If argv[0] is present it is considered to be a filename
    public static void main(String[] argv) throws Exception {

	JFrame frame = new JFrame();

	frame.addWindowListener(new java.awt.event.WindowAdapter() {
	    public void windowClosing(java.awt.event.WindowEvent e) {
		System.exit(0);
	    }
        });
        frame.setSize(300, 200);
	MessageBox messageBox = new MessageBox(frame, "Hello....");
	messageBox.show();

	//frame.pack();
        //frame.setVisible(true);
	
    } 


}
