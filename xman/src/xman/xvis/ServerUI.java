//------------------------------------------------------------------------------------
// $Id$
//------------------------------------------------------------------------------------
package xman.xvis;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*; 
import javax.swing.BorderFactory; 
import javax.swing.border.Border;
import java.util.Observable;

public class ServerUI extends Observable 
    implements ActionListener, WindowListener
{
    public static int gap = 200;
    
    private MyTextField  hostNameTextField;
    private MyTextField  portNumTextField;
    private JProgressBar progressBar;
    private Timer        timer;
    private int          progVal;
    private JFrame       frame;
    
    public ServerUI () 
    {
	frame = new JFrame ();
	
	//Create a timer.
	timer = new Timer(gap, new ActionListener() 
			  {
			      public void actionPerformed(ActionEvent evt) 
			      {
				  if (progVal > 100) progVal = 0;
				  progressBar.setValue(progVal);
				  progVal += 10;
			      }
			  });
        
	GridBagConstraints con = new GridBagConstraints ();
	
	con.anchor = GridBagConstraints.SOUTHWEST;
	con.weightx = 1; con.fill = GridBagConstraints.HORIZONTAL;
	
        JPanel contentPane = new JPanel();
        contentPane.setLayout(new BorderLayout());
	
	JPanel centerP = new JPanel ();
	centerP.setLayout (new BorderLayout ());
	
	ImageIcon icon = new ImageIcon("images/Host24.gif", "XVisServer");
	
	JToolBar toolBar = new JToolBar();
	addButtons (toolBar);
	
	contentPane.add (toolBar, BorderLayout.NORTH);
	
	MyJPanel p = new MyJPanel (new BorderLayout (), "XVisServer info");
	
	//Layout the labels in a panel
        JPanel labelPane = new JPanel();
        labelPane.setLayout(new GridLayout(0, 1));
        labelPane.add (new JLabel ("Hostname"));
	labelPane.add (new JLabel ("Port"));
	
        //Layout the text fields in a panel
        JPanel fieldPane = new JPanel();
        fieldPane.setLayout(new GridLayout(0, 1));
        fieldPane.add (hostNameTextField = new MyTextField ("", 10, false));
        fieldPane.add (portNumTextField = new MyTextField ("", 10, false));
	
        p.add(labelPane, BorderLayout.WEST);
        p.add(fieldPane, BorderLayout.CENTER);
	
	centerP.add(p, BorderLayout.CENTER);
	
	progressBar = new JProgressBar(0, 100);
	progressBar.setOrientation(JProgressBar.HORIZONTAL);
        progressBar.setValue(0);
        progressBar.setStringPainted(false);
	//	progressBar.setBackground (Color.black);
	//	progressBar.setForeground (Color.red);
	progressBar.setEnabled (false);

	Border b1 = BorderFactory.createEtchedBorder (); //LineBorder (Color.black);
	//	Border b2 = BorderFactory.createEmptyBorder (10, 10, 10, 10); 
	progressBar.setBorder (b1);//BorderFactory.createCompoundBorder(b1, b2));
			       
	JPanel pgP = new JPanel ();
	pgP.setLayout (new BorderLayout ());
	pgP.add (new JLabel ("Status"), BorderLayout.WEST);
	pgP.add (progressBar, BorderLayout.EAST);
	centerP.add (pgP, BorderLayout.SOUTH);
	
	contentPane.add (centerP, BorderLayout.CENTER);
	frame.setContentPane(contentPane);
	
	frame.addWindowListener (this);

	frame.pack();
        frame.setVisible(true);
    }
    
    private void addButtons(JToolBar toolBar) {
	JButton button = null;
	
	button = new JButton(new ImageIcon ("images/Save24.gif"));
	button.setToolTipText ("Save"); button.setActionCommand ("Save");
	button.addActionListener (this); button.setEnabled (false);
	toolBar.add(button);
	
	button = new JButton(new ImageIcon("images/Print24.gif"));
	button.setToolTipText ("Print");  button.setActionCommand ("Print");
	button.addActionListener (this);  button.setEnabled (false);
	toolBar.add(button);
	
	button = new JButton(new ImageIcon("images/Redo24.gif"));
	button.setToolTipText ("Restart");  button.setActionCommand ("Restart");
	button.addActionListener (this);  button.setEnabled (false);
	toolBar.add(button);
	
	button = new JButton(new ImageIcon ("images/Stop24.gif"));
	button.setToolTipText ("Stop");  button.setActionCommand ("Stop");
	button.addActionListener (this);  button.setEnabled (false);
	toolBar.add(button);
	
	button = new JButton(new ImageIcon("images/About24.gif"));
	button.setToolTipText ("About");  button.setActionCommand ("About");
	button.addActionListener (this);  button.setEnabled (false);
	toolBar.add(button);
    }
    
    public void stop () 
    {
	Toolkit.getDefaultToolkit().beep();
	timer.stop ();
	progressBar.setValue(progressBar.getMinimum());
	progressBar.setEnabled (false);
    }
    
    public void start () 
    {
	progressBar.setEnabled (true);
	progVal = 0;
	timer.start ();
    }
    
    public void setHostName (String hname) 
    {
	frame.setTitle ("XVisServer@" + hname);
	hostNameTextField.setText (hname);
    }
    
    public void setPortNumber (int num) 
    {
	portNumTextField.setText ((new Integer (num)).toString ());
    }
    
    public void actionPerformed (ActionEvent evt)
    {
	setChanged ();
	notifyObservers (evt.getActionCommand ());
    }
    
    public void windowClosing(WindowEvent e) {
	setChanged ();
	notifyObservers ("Window destroyed");
    }
    
    public void killFrame () { frame.dispose (); }
    
    public void windowClosed(WindowEvent e) {
	setChanged ();
	notifyObservers ("Window destroyed");
    }
    
    public void windowOpened(WindowEvent e) {}
    
    public void windowIconified(WindowEvent e) {}
    
    public void windowDeiconified(WindowEvent e) {}
    
    public void windowActivated(WindowEvent e) {}
    
    public void windowDeactivated(WindowEvent e) {}
}

