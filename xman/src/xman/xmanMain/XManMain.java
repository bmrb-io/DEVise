//----------------------------------------------------------------------
// 
// File Name:    XManMain.java
// 
// Description:  Experiment Management Main Program
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.XManMain;

import javax.swing.JOptionPane;
import javax.swing.JFrame;
import java.awt.*;
import java.awt.event.*;

import xman.shared.*;
import xman.xmanGUI.*;

class XManMain {

    private static JFrame frame;

    public static void main(String[] args) 
	throws java.io.IOException{
	XManDialog xmanDialog = new XManDialog();
	xmanDialog.pack();
	xmanDialog.show();

	//try {
	    frame = new XManFrame(Resources.STR_APP_NAME, 
				  xmanDialog.workingDir());
	    frame.addWindowListener(new WindowAdapter() {
		public void windowClosing(WindowEvent e) {System.exit(0);}
	    });
	    frame.setSize(780, 400);
	    frame.setVisible(true);
	    //}
	    //catch(Exception exception) {
	    //JOptionPane.showMessageDialog(frame, exception);
	    //}
    }
}



