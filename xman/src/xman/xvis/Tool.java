// $Id$
package xman.xvis;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

public class Tool extends JFrame implements ActionListener
{
    XVisServer x = null;
    XVisClient y = null;
    
    static public void main (String [] argv)
    {
	try {
	    new Tool (); 
	} catch (YException e) { System.err.println (e.getMsg ()); }
    }
    
    public Tool () throws YException
    {
	x = new XVisServer (64950);//"demo");
	x.start ();
	
	y = new XVisClient ("localhost", 64950);
	y.start ();
	setup ();
    }
    
    private void setup ()
    {
	setTitle ("Pd");
	
	Container contentPane = getContentPane();
	contentPane.setLayout(new GridLayout (0, 1, 10, 10));
	
	JButton button;
	contentPane.add (button = new JButton ("Session 1"));
	button.addActionListener (this);
	
	contentPane.add (button = new JButton ("Session 2"));
	button.addActionListener (this);
	
	contentPane.add (button = new JButton ("Session 3"));
	button.addActionListener (this);
	
	contentPane.add (button = new JButton ("Session 4"));
	button.addActionListener (this);
	
	contentPane.add (button = new JButton ("Session 5"));
	button.addActionListener (this);
	
	addWindowListener (new WindowAdapter () {
	    public void windowClosing (WindowEvent e) {dispose (); 
	    if (x != null) x.quit (); System.exit (0);
	    }
	});
	
	pack ();
	setSize (getMinimumSize ());
	setVisible (true);
    }
    
    public void actionPerformed (ActionEvent evt)
    {
	String command = evt.getActionCommand ();
	
	try {
	    if (command.equals ("Session 1"))
		x.openSession ("/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			       "/data/hist_10:" + 
			       "/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			       "/data/hist_11:" +
			       "/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" +
			       "//data/hist_12:" +
			       "/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" +
			       "/data/hist_13@23;" +
			       "/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			       "/data/hist_14@24;" +
			       "/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			       "/data/hist_15@100;"+
			       "/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" +
			       "/data/hist_16;"
			       );
	    
	    else if (command.equals ("Session 2"))
		x.openSession("/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			      "/data/hist_22@232");
	    
	    else if (command.equals ("Session 3")) 
		x.openSession ("/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			       "/data/hist_20");
	    
	    else if (command.equals ("Session 4"))
		y.showFiles ("/afs/cs.wisc.edu/p/devise/ext9/ozaki/misc" + 
			     "/data/hist_10");
	    
	    else if (command.equals ("Session 5")) {
		System.out.println ("Now I will test the caching policies\n");
		/*
		String s = "/p/paradyn/development/zhou/draco/SUN/most"; 
		File dir = new File (s);
		
		File [] f = dir.listFiles ();
		
		int [] a = new int [8];
		
		System.out.println ("Selected directory => " + dir + " has " + 
				    f.length + " files.");
		
		for (int i = 0; i < f.length; i++) {
		    
		    if (((f [i].getName ()).toString ()).startsWith ("hist_")) {
			
			//System.out.println ("Caching -->" + f [i]);
			
			y.lookUp (f [i], 8, 8, a);
			
			System.out.print ("File: " + f [i] + " ----> ");
			
			for (int j = 0; j < a.length; j++)
			    System.out.print (a [j] + " ");
			System.out.println (" ");
		    }
		}
		
		//		inq.ccnt = 0;
		int fcnt = 0;
		
		a = new int [4];
		
		for (int i = 0; i < f.length; i++) {
		    
		    if (((f [i].getName ()).toString ()).startsWith ("hist_")) {
			fcnt ++;
			//System.out.println ("Caching -->" + f [i]);
			
			y.lookUp (f [i], 4, 8, a);
			
			System.out.print ("File: " + f [i] + " ----> ");
			
			for (int j = 0; j < a.length; j++)
			    System.out.print (a [j] + " ");
			System.out.println (" ");
		    }
		    }
		
		System.out.println ("# of files = " + fcnt);
		*/
	    }
	} catch (YException e) { 
	    e.printStackTrace ();
	    System.err.println ("Error message: " + e.getMsg());
	}
    }
}

