package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;

import gjt.*;

/**
 * Test for MessageDialog.
 *
 * @version 1.0, Feb, 1997
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.MessageDialog
 */
public class MessageDialogTest extends UnitTest {
    public String title() { 
        return "MessageDialog Test"; 
    }
    public Panel centerPanel() { 
        return new MessageDialogLauncher(this);
    }
}
class MessageDialogLauncher extends Panel 
							implements 
							DialogClient, ActionListener {
	private Applet         applet;
    private Button         messageDialogButton;
    private MessageDialog  messageDialog;
	private Image          image = null;
	private Checkbox       modal = new Checkbox("modal");

    public MessageDialogLauncher(Applet applet) {
		this.applet = applet;
		add(modal);
        add(messageDialogButton  = 
					new Button("Launch Message Dialog"));

		messageDialogButton.addActionListener (this);
    }
	public void actionPerformed(ActionEvent event) {
		Image image = applet.getImage(applet.getCodeBase(),
			                          "gifs/information.gif");
		if(messageDialog == null) {
        	messageDialog = new MessageDialog(
                		Util.getFrame(this), this,
                		"Example Message Dialog", 
                		"This is an example of a message dialog.",
						image, modal.getState());
		}
		else {
			if(modal.getState()) messageDialog.setModal(true);
			else                 messageDialog.setModal(false);
		}
        messageDialog.setVisible(true);
    }
    public void dialogDismissed(Dialog d) {
    	applet.showStatus("MessageDialog Dismissed");
    }
	public void dialogCancelled(Dialog d) {
		applet.showStatus("Message Dialog Cancelled");
	}
}
