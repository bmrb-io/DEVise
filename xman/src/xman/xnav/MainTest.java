package xman.xnav;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import java.lang.*;

class MainTest {

 //  If argv[0] is present it is considered to be a filename
    public static void main(String[] argv) throws Exception {

        int portNumber;
	String machineName;
	JFrame frame;

	if (argv.length == 1) {
            portNumber = Integer.parseInt(argv[0]);
	    frame = new ThreadInterface(portNumber);
	}
	else if (argv.length == 2) {
            portNumber = Integer.parseInt(argv[0]);
	    machineName = argv[1];
	    frame = new ThreadInterface(portNumber, machineName);
	}
	else
	    frame = new ThreadInterface();

	frame.addWindowListener(new java.awt.event.WindowAdapter() {
	    public void windowClosing(java.awt.event.WindowEvent e) {
		System.exit(0);
	    }
        });
        frame.setSize(840, 740);
	//frame.pack();
        frame.setVisible(true);
	
    } 
}
