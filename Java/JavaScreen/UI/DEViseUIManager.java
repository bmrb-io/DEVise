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

import javax.swing.*;
import JavaScreen.*;

public final class DEViseUIManager {
	
	private JFrame _appFrame;
	
	private DEViseMenuPanel _menuPanel;
	private JPanel _javaScreen;
	private DEViseStatusPanel _statusPanel;

    public DEViseUIManager( 
		JFrame appFrame,
		DEViseMenuPanel menuPanel,
		JPanel javaScreen,
		DEViseStatusPanel statusPanel
		)
	{
        _appFrame = appFrame;
		_menuPanel = menuPanel;
		_javaScreen = javaScreen;
		_statusPanel = statusPanel;
		
		_addMainPanels();
    }

	private void _addMainPanels() {
		_appFrame.getContentPane().add(_menuPanel);
		_appFrame.getContentPane().add(_javaScreen);
		_appFrame.getContentPane().add(_statusPanel);
		
	}
}
