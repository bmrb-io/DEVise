package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;

import gjt.*;

/**
 * Tests the YesNoDialog.
 * 
 * @version 1.0, Feb, 1997
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DialogClient
 * @see     gjt.YesNoDialog;
 */
public class YesNoDialogTest extends UnitTest {
    public String title() { 
        return "YesNoDialog Test"; 
    }
    public Panel centerPanel() { 
        return new YesNoDialogLauncher(this);
    }
}
class YesNoDialogLauncher extends Panel 
						  implements DialogClient, ActionListener {
	private Applet         applet;
    private YesNoDialog    yesNoDialog;
	private int            cupCnt = 0, coffeeLimit = 3;
	private Image          image = null;
	private Checkbox       modal = new Checkbox("modal");

    private Button yesNoDialogButton;

    public YesNoDialogLauncher(Applet applet) {
		this.applet = applet;

		add(modal);
        add(yesNoDialogButton = new Button("Launch YesNo Dialog"));

		yesNoDialogButton.addActionListener(this);
    }
	public void actionPerformed(ActionEvent event) {
		String question = "How about a cup of Java?"; 
		Image image = applet.getImage(applet.getCodeBase(),
			                          "gifs/question.gif");

		if(cupCnt >= 0 && cupCnt < coffeeLimit) {
			question += "  You've had " + cupCnt;
			if(cupCnt == 1) question += " cup already.";
			else            question += " cups already.";
		}
		else {
			question = 
			"Are you sick and tired of coffee analogies?";
		}
		if(cupCnt >= 0 && cupCnt < coffeeLimit) {
			image = applet.getImage(applet.getCodeBase(),
				                    "gifs/questionMark.gif");
		}
		else {
			image = applet.getImage(applet.getCodeBase(),
				                    "gifs/punch.gif");
		}
		if(yesNoDialog == null) {
        	yesNoDialog = new YesNoDialog(
								Util.getFrame(this), 
                   				this, "Example YesNo Dialog",
                   				question, image, modal.getState());
		}
		else {
			if(modal.getState()) yesNoDialog.setModal(true);
			else     			 yesNoDialog.setModal(false);

			yesNoDialog.setImage(image);
			yesNoDialog.setMessage(question);
		}
        yesNoDialog.setVisible(true);
    }
    public void dialogDismissed(Dialog d) {
    	if(yesNoDialog.answeredYes()) {
			++cupCnt;

			if(cupCnt <= coffeeLimit)
            	applet.showStatus("Cups Of Coffee:  " + cupCnt);
			else
				applet.showStatus("Me too");
		}
		else {
			if(cupCnt == 0)
				applet.showStatus("No coffee yet.");
			else if(cupCnt >= coffeeLimit)
				applet.showStatus("Me too");
        }
    }
	public void dialogCancelled(Dialog d) {
		applet.showStatus("Yes No Dialog Cancelled");
	}
}
