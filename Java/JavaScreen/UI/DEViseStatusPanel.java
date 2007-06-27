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

// $Id$

// $Log$
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

public class DEViseStatusPanel extends JPanel {
	
	private static final int DEFAULT_HGAP = 0;
	private static final int DEFAULT_VGAP = 0;
	
	private JPanel _statusWest; // for left-aligned widgets
	private JPanel _statusEast; // right-aligned widgets
	
	private DEViseTrafficLight _lights;
	private DEViseStatusMessage _statusMessage;
	private DEViseViewInfo _viewInfo;
	
	public DEViseStatusPanel(
		DEViseTrafficLight lights,
		DEViseStatusMessage statusMessage,
		DEViseViewInfo viewInfo	
		)
	{
		this(lights, statusMessage, viewInfo, DEFAULT_HGAP, DEFAULT_VGAP);
	}

    public DEViseStatusPanel(
		DEViseTrafficLight lights,
		DEViseStatusMessage statusMessage,
		DEViseViewInfo viewInfo,
		int hgap,
		int vgap
		)
	{
		super(new BorderLayout());
		
		_statusWest = new JPanel(new FlowLayout(FlowLayout.LEFT));
		_statusEast = new JPanel(new FlowLayout(FlowLayout.RIGHT));

		this.add(_statusWest, BorderLayout.WEST);
		this.add(_statusEast, BorderLayout.EAST);
		
		_lights = lights;
		_statusMessage = statusMessage;
		_viewInfo = viewInfo;
		
		// add to west/left side of status panel
		if(_lights != null ) { _statusWest.add(_lights); }
		// _statusWest.add(_statusMessage);
		
		// add to east/right side of status panel
		_statusEast.add(_viewInfo);
  }

	// Automatically sets _statusWest and _statusEast to statusPanel's bg
	// color. Hopefully this won't be neccessary once Look & Feel is implemented
	public void inheritBackground() {
		_statusEast.setBackground(this.getBackground());
		_statusWest.setBackground(this.getBackground());
	}
}
