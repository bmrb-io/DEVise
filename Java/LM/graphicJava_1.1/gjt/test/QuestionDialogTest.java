package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;

import gjt.*;

/**
 * Tests QuestionDialog.
 *
 * @version 1.0, Feb, 1997
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DialogClient
 * @see     gjt.QuestionDialog;
 */
public class QuestionDialogTest extends UnitTest {
    public String title() { 
        return "QuestionDialog Test"; 
    }
    public Panel centerPanel() { 
        return new QuestionDialogLauncher(this);
    }
}
class QuestionDialogLauncher extends Panel 
							 implements DialogClient, 
							 ActionListener {
	private Applet         applet;
    private QuestionDialog questionDialog;
	private Image          image = null;
	private Checkbox       modal = new Checkbox("modal");

    private Button questionDialogButton;

    public QuestionDialogLauncher(Applet applet) {
		this.applet = applet;

		add(modal);
        add(questionDialogButton = 
				new Button("Launch Question Dialog"));

		questionDialogButton.addActionListener(this);
    }
	public void actionPerformed(ActionEvent event) {
		Image image = applet.getImage(applet.getCodeBase(),
			                          "gifs/book.gif");
		if(questionDialog == null) {
        	questionDialog = 
                new QuestionDialog(Util.getFrame(this), this,
                                   "Example Question Dialog", 
                                   "Book Of The Month:  ", 
								   "Shogun", 
								   45, image, modal.getState());
		}
		if(modal.getState()) questionDialog.setModal(true);
		else                 questionDialog.setModal(false);

        questionDialog.setVisible(true);
    }
    public void dialogDismissed(Dialog d) {
    	if(questionDialog.wasCancelled())
        	applet.showStatus("CANCELLED"); 
        else 
            applet.showStatus("Book Of The Month:  " + 
                questionDialog.getTextField().getText());
	}
	public void dialogCancelled(Dialog d) {
		applet.showStatus("Dialog Cancelled");
	}
}
