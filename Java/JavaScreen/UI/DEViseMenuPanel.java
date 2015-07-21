// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2015
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
// Revision 1.3.42.1  2015/07/20 21:18:31  wenger
// Moved Feedback button to the right side of the JavaScreen.
//
// Revision 1.3  2007/06/27 17:48:10  wenger
// Merged andyd_gui_br_5 thru andyd_gui_br_6 to the trunk (this includes
// the toolbar stuff, but not the fixes for the "obscured tooltips"
// problem, which are still in progress).
//
// Revision 1.1.2.3  2007/06/06 14:13:14  wenger
// Got JavaScreen Makefile to work with Andy's latest changes; added
// headers to the UI java files.
//

// ------------------------------------------------------------------------

package JavaScreen.UI;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import JavaScreen.*;
import JavaScreen.UI.*;

public class DEViseMenuPanel extends JPanel {
	
	private static final int DEFAULT_HGAP = 1;
	private static final int DEFAULT_VGAP = 1;
	
	private DEViseAnimPanel _throbber;
	private DEViseComponentPanel _menu;
	private boolean _menuDisabled;
	
  public DEViseMenuPanel(
		DEViseAnimPanel throbber,
		DEViseComponentPanel menu,
		Component feedbackButton,
		boolean menuDisabled
		) 
	{
		this(throbber, menu, feedbackButton, menuDisabled, DEFAULT_HGAP, DEFAULT_HGAP);
	}

	public DEViseMenuPanel(
		DEViseAnimPanel throbber,
		DEViseComponentPanel menu,
		Component feedbackButton,
		boolean menuDisabled,
		int hgap,
		int vgap	
		)
	{
		super();
		this.setLayout(new BoxLayout(this, BoxLayout.X_AXIS));
		
		_throbber = throbber;
		_menu = menu;
		_menuDisabled = menuDisabled;
		
		this.add(_throbber);
		
		if(!_menuDisabled){ this.add(_menu); }
		this.add(Box.createHorizontalGlue());
		this.add(feedbackButton);
	}
	
	// Sets children to statusPanel's bg color.
	// Hopefully this won't be neccessary once Look & Feel is implemented
	public void inheritBackground() {
		_throbber.setBackground(this.getBackground());
		_menu.setBackground(this.getBackground());
	}
}
