package gjt;

import java.awt.Dialog;

/**
 * DialogClients are notified when the Dialog with which they 
 * are associated is dismissed or cancelled.  A reference to the 
 * dismissed dialog is passed as a parameter of dialogDismissed() 
 * in case a DialogClient is a client of more than one Dialog.<p>
 *
 * @version 1.0, Apr 1 1996
 * @version 1.1, Feb 1997
 *
 * 	Added dialogCancelled() which is invoked when the dialog
 *  is closed via the close box or system menu.
 *
 * @author  David Geary
 * @see     GJTDialog
 * @see     MessageDialog
 * @see     QuestionDialog
 * @see     YesNoDialog
 * @see     gjt.test.DialogTest
 */
public interface DialogClient {
	abstract public void dialogDismissed(Dialog d); 
	abstract public void dialogCancelled(Dialog d); 
}
