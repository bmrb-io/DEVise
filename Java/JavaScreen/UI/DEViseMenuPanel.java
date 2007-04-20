// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2007
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
		DEViseComponentPanel menu
		) 
	{
		this(throbber, menu, false, DEFAULT_HGAP, DEFAULT_HGAP);
	}
	
  public DEViseMenuPanel(
		DEViseAnimPanel throbber,
		DEViseComponentPanel menu,
		boolean menuDisabled
		) 
	{
		this(throbber, menu, menuDisabled, DEFAULT_HGAP, DEFAULT_HGAP);
	}

	public DEViseMenuPanel(
		DEViseAnimPanel throbber,
		DEViseComponentPanel menu,
		boolean menuDisabled,
		int hgap,
		int vgap	
		)
	{
		super(new FlowLayout(FlowLayout.LEFT, hgap, vgap));
		
		_throbber = throbber;
		_menu = menu;
		_menuDisabled = menuDisabled;
		
		this.add(_throbber);
		
		if(!_menuDisabled){ this.add(_menu); }
	}
	
	// Sets children to statusPanel's bg color.
	// Hopefully this won't be neccessary once Look & Feel is implemented
	public void inheritBackground() {
		_throbber.setBackground(this.getBackground());
		_menu.setBackground(this.getBackground());
	}
}
