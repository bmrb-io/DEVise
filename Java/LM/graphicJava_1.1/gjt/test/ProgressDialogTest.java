package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;

import gjt.*;

/**
 * Tests the gjt.ProgressDialog.
 *
 * @version 1.0, Feb, 1997
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DialogClient
 * @see     gjt.ProgressDialog;
 */
public class ProgressDialogTest extends UnitTest {
    public String title() { 
        return "Progress Dialog Test"; 
    }
    public Panel centerPanel() { 
        return new ProgressDialogLauncher(this);
    }
}
class ProgressDialogLauncher extends Panel 
							 implements DialogClient,
							 ActionListener {
    private ProgressDialog progressDialog;
	private ProgressDialogThread thread;
	private Checkbox       modal = new Checkbox("modal");
	private Applet         applet;

    private Button progressDialogButton;

    public ProgressDialogLauncher(Applet applet) {
		this.applet = applet;
		add(modal);
        add(progressDialogButton = 
				new Button("Launch Progress Dialog"));

		progressDialogButton.addActionListener(this);
    }
	public void actionPerformed(ActionEvent event) {
		if(progressDialog == null) {
    		progressDialog = new ProgressDialog(
                    			Util.getFrame(this), this,
                    			"Example Progress Dialog",
                    			Color.blue, modal.getState());
		}
		else {
			progressDialog.reset();

			if(modal.getState()) progressDialog.setModal(true);
			else 				 progressDialog.setModal(false);
		}
        thread = new ProgressDialogThread(progressDialog);
        thread.start();
        progressDialog.setVisible(true);
    }
    public void dialogDismissed(Dialog d) {
		applet.showStatus("Progress Dialog Dismissed");
    }
	public void dialogCancelled(Dialog d) {
		applet.showStatus("Dialog Cancelled");
		thread.stop();
	}
}
class ProgressDialogThread extends Thread {
    private ProgressDialog dialog;
    private double         percentComplete = 0;

    public ProgressDialogThread(ProgressDialog dialog) {
        this.dialog = dialog;
    }
    public void run() {
		while(percentComplete <= 100) {
            try { Thread.currentThread().sleep(250); }
            catch(InterruptedException e) { }

			if(dialog.isShowing()) {
            	dialog.setPercentComplete(percentComplete);
            	percentComplete += 10;
			}
        }
    }
}
