package gjt;

import java.awt.*;
import java.awt.event.*;

/**
 * Similar in fuction to the MessageDialog, YesNoDialog poses 
 * a question, that is answered by selection of either a Yes 
 * button or a No button.<p>
 *
 * YesNoDialog dialog = new YesNoDialog(gjt.Util.getFrame(this),
 *								this, // DialogClient (can be null)
 *								"Title", // dialog title
 *								"want?", // question
 *								image, // picture for dialog (can be null)
 *								true); // modal
 *  dialog.show();
 *
 *  if(dialog.answeredYes())
 *    // ...
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     GJTDialog
 * @see     MessageDialog
 * @see     gjt.test.DialogTest
 */
public class YesNoDialog extends WorkDialog {
    private Button      yesButton;
    private Button      noButton;
    private boolean     answer      = false;
    private ButtonPanel buttonPanel = new ButtonPanel();
	private YesNoPanel  yesNoPanel;

    public YesNoDialog(Frame  frame, DialogClient client, 
                       String title, String       question,
					   Image  image) {
		this(frame, client, title, question, image, false);
	}
    public YesNoDialog(Frame  frame, DialogClient client, 
                       String title, String       question,
					   Image  image, boolean      modal) {
        super(frame, client, title, modal);

		ButtonListener buttonListener = new ButtonListener();

        yesButton = addButton("Yes");
        noButton  = addButton("No");

		yesButton.addActionListener(buttonListener);
		noButton.addActionListener(buttonListener);
		setWorkPanel(yesNoPanel = new YesNoPanel(image,question));

		if(image != null)
			setImage(image);
    }
	/**
	 * @deprecated as of JDK1.1
	 */
    public void layout() {
        yesButton.requestFocus();
        super.layout(); 
    }
    public void doLayout() {
		layout();
	}
	public void setYesButtonLabel(String label) {
		yesButton.setLabel(label);
	}
	public void setNoButtonLabel(String label) {
		noButton.setLabel(label);
	}
    public boolean answeredYes() {
        return answer;
    }
    public void setMessage(String question) {
		yesNoPanel.setMessage(question);
    }
    public void setImage(Image image) {
		yesNoPanel.setImage(image);
    }
	class ButtonListener implements ActionListener {
		public void actionPerformed(ActionEvent event) {
        	if(event.getSource() == yesButton) answer = true;
        	else                               answer = false;

			dispose();
		}
	}
}
class YesNoPanel extends Postcard {
	private Label label;

	public YesNoPanel(String question) {
		this(null, question);
	}
	public YesNoPanel(Image image, String question) {
		super(image, new Panel());
		getPanel().add(label = new Label(question,Label.CENTER));
	}
	public void setMessage(String question) {
		label.setText(question);
	}
}
