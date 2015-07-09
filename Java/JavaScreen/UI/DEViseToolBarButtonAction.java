// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Need to write description.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.38.2  2015/07/09 19:00:24  wenger
// Final cleanup of Aditya's changes.
//

// ------------------------------------------------------------------------


package JavaScreen.UI;
import java.awt.event.*;
import javax.swing.*;

import JavaScreen.jsdevisec;

class DEViseToolBarButtonAction extends Object implements ActionListener {
	
	private DEViseToolBar toolbar;
	private jsdevisec jsdvsc;
	private DEViseStatusMessage dvscStatusMessage;
	
	public DEViseToolBarButtonAction(DEViseToolBar tb) {
		toolbar = tb;
	}
	public void actionPerformed(ActionEvent e) {
		DEViseToolBarButton button = (DEViseToolBarButton)e.getSource();
		toolbar.setSelectedButton(button);
		//Updating label in status panel
		jsdvsc = (jsdevisec)toolbar.getParent().getParent();
		dvscStatusMessage = jsdvsc.getStatusPanel().get_statusMessage();
		dvscStatusMessage.setText(button.getToolTipText());
	}
}
