package gjt.test;

import java.awt.*;
import java.awt.event.*;
import java.applet.Applet;

import gjt.*;

/**
 * Tests 4 gjt custom dialogs:
 * <dl>
 * <dd> MessageDialog  (a dialog which displays a message)
 * <dd> QuestionDialog (a dialog which asks a question)
 * <dd> YesNoDialog    (a dialog with yes/no buttons)
 * <dd> ProgressDialog (a dialog which records progress of task)
 * </dl>
 * @version 1.0, Apr 25, 1996
 * @version 1.1, Nov  8, 1996
 *
 *    Changed println's to Applet.showStatus()
 *
 * @author  David Geary
 * @see     gjt.test.UnitTest
 * @see     gjt.DialogClient
 * @see     gjt.MessageDialog
 * @see     gjt.ProgressDialog;
 * @see     gjt.QuestionDialog;
 * @see     gjt.YesNoDialog;
 */
public class DialogTest extends UnitTest {
    public String title() { 
        return "Graphic Java Toolkit Dialog Test"; 
    }
    public Panel centerPanel() { 
        return new DialogLauncher(this);
    }
}
class DialogLauncher extends Panel implements DialogClient,
                                              ActionListener {
	private Applet         applet;
    private MessageDialog  messageDialog;
    private QuestionDialog questionDialog;
    private YesNoDialog    yesNoDialog;
    private ProgressDialog progressDialog;
	private int            cupCnt = 0, coffeeLimit = 3;
	private Image          image = null;
	private Checkbox       modal = new Checkbox("modal");

    private Button messageDialogButton, questionDialogButton,
                   yesNoDialogButton,   progressDialogButton;

    public DialogLauncher(Applet applet) {
		this.applet = applet;
        setLayout(new GridLayout(0,1));

		add(modal);

        add(messageDialogButton  = 
            new Button("Message Dialog"));

        add(questionDialogButton = 
            new Button("Question Dialog"));

        add(yesNoDialogButton    = 
            new Button("YesNo Dialog"));

        add(progressDialogButton = 
                new Button("Progress Dialog"));

		messageDialogButton.addActionListener (this);
		questionDialogButton.addActionListener(this);
		yesNoDialogButton.addActionListener   (this);
		progressDialogButton.addActionListener(this);
    }
	public void actionPerformed(ActionEvent event) {
        if(event.getSource() == messageDialogButton) {
			Image image = applet.getImage(applet.getCodeBase(),
			                              "gifs/information.gif");

            messageDialog = new MessageDialog(
                Util.getFrame(this), this,
                "Example Message Dialog", 
                "This is an example of a message dialog.",
				image, modal.getState());

            messageDialog.setVisible(true);
        }
        else if(event.getSource() == questionDialogButton) {
			Image image = applet.getImage(applet.getCodeBase(),
			                              "gifs/book.gif");
            questionDialog = 
                new QuestionDialog(Util.getFrame(this), this,
                                   "Example Question Dialog", 
                                   "Book Of The Month:  ", 
								   "Shogun", 
								   45, image, modal.getState());

            questionDialog.setVisible(true);
        }
        else if(event.getSource() == yesNoDialogButton) {
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
            yesNoDialog = new YesNoDialog(
								Util.getFrame(this), 
                   				this, "Example YesNo Dialog",
                   				question, image, modal.getState());
            yesNoDialog.setVisible(true);
        }
        else if(event.getSource() == progressDialogButton) {
            progressDialog = new ProgressDialog(
                    Util.getFrame(this), this,
                    "Example Progress Dialog",
                    Color.blue, modal.getState());

            ProgressThread thread = 
							new ProgressThread(progressDialog);
            thread.start();
            progressDialog.setVisible(true);
        }
    }
    public void dialogDismissed(Dialog d) {
		Util.getFrame(this).toFront();

        if(d == messageDialog)  {
            applet.showStatus("MessageDialog Down");
        }
        else if(d == questionDialog) {
            if(questionDialog.wasCancelled())
                applet.showStatus("CANCELLED"); 
            else 
            applet.showStatus(
                "Book Of The Month:  " + 
                questionDialog.getTextField().getText());
        }
        else if(d == yesNoDialog) {
            if(yesNoDialog.answeredYes()) {
				++cupCnt;

				if(cupCnt <= coffeeLimit)
            		applet.showStatus("Cups Of Coffee:  " + 
					                   cupCnt);
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
		else if(d == progressDialog) {
			applet.showStatus("Progress Dialog Dismissed");
		}
    }
	public void dialogCancelled(Dialog d) {
		applet.showStatus("Dialog Cancelled");
	}
}
class ProgressThread extends Thread {
    private ProgressDialog dialog;
    private double         percentComplete = 0;

    public ProgressThread(ProgressDialog dialog) {
        this.dialog = dialog;
    }
    public void run() {
		while(percentComplete <= 100 && dialog.isShowing()) {
            try { Thread.currentThread().sleep(500); }
            catch(InterruptedException e) { }

			if(dialog.isShowing())
            dialog.setPercentComplete(percentComplete);
            percentComplete += 10;
        }
    }
}
