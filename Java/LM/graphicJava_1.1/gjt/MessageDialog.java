package gjt;

import java.awt.*;
import java.awt.event.*;

/**
 * A dialog that displays a message and comes equipped with an 
 * Ok button with which the dialog is dismissed.<p>
 *
 * Note that there is only one MessageDialog, that gets
 * reused.  Clients must call getMessageDialog() in order to
 * access the one and only MessageDialog.<p>
 *
 * <em>Note:  The 1.0.2 version of the AWT seems to have
 * introduced a bug that causes pack() to work incorrectly
 * under Win95.</em>
 *
 * @version 1.0, Apr 1 1996
 * @author  David Geary
 * @see     GJTDialog
 * @see     gjt.test.MessageDialogTest
 * @see     gjt.test.DialogTest
 */
public class MessageDialog extends WorkDialog 
                           implements ActionListener{
    private Button       okButton;
	private MessagePanel messagePanel;

    public MessageDialog(Frame  frame, DialogClient client, 
                         String title, String       message,
						 Image  image) {
		this(frame, client, title, message, image, false);
	}
    public MessageDialog(Frame  frame, DialogClient client, 
                         String title, String       message,
						 Image  image, boolean      modal) {
        super(frame, client, title, modal);
		messagePanel = new MessagePanel(image, message);
		okButton     = addButton("Ok");

		okButton.addActionListener(this);
        setWorkPanel(messagePanel);
	}
	/**
	 * @deprecated as of JDK1.1
	 */
	public void layout() {
        okButton.requestFocus();
		// compiler will issue deprecation warning, but we can't call
		// super.doLayout()!
        super.layout();
    }
    public void doLayout() {
		layout();
	}
	public void actionPerformed(ActionEvent event) {
        dispose();
    }
    private void setMessage(String message) {
        messagePanel.setMessage(message);
    }
	private void setImage(Image image) {
		messagePanel.setImage(image);
	}
}
class MessagePanel extends Postcard {
	private Label label;

    public MessagePanel(String message) {
		this(null, message);
    }
	public MessagePanel(Image image, String message) {
		super(image, new Panel());
		getPanel().add(label = new Label(message,Label.CENTER));
	}
	public void setMessage(String message) {
		label.setText(message);
	}
}
