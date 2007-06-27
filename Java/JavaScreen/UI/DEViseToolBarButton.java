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
// Revision 1.1.2.7  2007/06/22 21:49:45  wenger
// Added disabled images for toolbar buttons; 3D buttons now stay
// disabled even when we have a 3D view because they don't actually
// work yet.
//
// Revision 1.1.2.6  2007/06/15 20:46:17  wenger
// Fixed problems with how DEViseJSValues was used in the toolbar code;
// got rid of static members for loading images in jsdevisec, because
// they might cause problems; made some changes to the toolbar constructor
// to move towards actually making it functional.
//
// Revision 1.1.2.5  2007/06/15 16:34:50  wenger
// Got JavaScreen toolbar icon images to load correctly from jar files (and
// significantly cleaned up the image loading in general, getting rid of a
// bunch of duplicate code).
//
// Revision 1.1.2.4  2007/06/15 14:27:12  adayton
// Updated some of the button behaviours and made some improvements to the icons
//
// Revision 1.1.2.3  2007/06/06 14:13:15  wenger
// Got JavaScreen Makefile to work with Andy's latest changes; added
// headers to the UI java files.
//

// ------------------------------------------------------------------------

package JavaScreen.UI;

import javax.swing.*;
import JavaScreen.*;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Toolkit;
import java.io.File;
import java.net.*;

public final class DEViseToolBarButton extends JButton {
	// defaults
	static final String IMAGES_DIR = "resources/toolbar_icons/";
	static final String IMAGE_EXTENSION = "png";
	static final String DEFAULT_STATE = "default";
	static final String PRESSED_STATE = "pressed";
	static final String SELECTED_STATE = "selected";
	static final String DISABLED_STATE = "disabled";
	static final int WIDTH = 22;
	static final int HEIGHT = 22;

	
	// values for icon images
	private String imagePath;
	private String imageExtension;
	private String imageNameBase;

	public DEViseToolBarButton(String tooltip, String nameBase,
	  DEViseJSValues jsValues) {
		this(tooltip, nameBase, IMAGE_EXTENSION, IMAGES_DIR, jsValues);
	}

    public DEViseToolBarButton(String tooltip, String nameBase, String ext,
      String path, DEViseJSValues jsValues) {
		imageExtension = new String(ext);
		imagePath = new String(path);
		imageNameBase = new String(nameBase);
		
		String defaultIconFilePath = constructIconFilePath(DEFAULT_STATE);
		String pressedIconFilePath = constructIconFilePath(PRESSED_STATE);
		String selectedIconFilePath = constructIconFilePath(SELECTED_STATE);
		String disabledIconFilePath = constructIconFilePath(DISABLED_STATE);
		
		// miscellanious attributes
		setToolTipText(tooltip);
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		setBorder(BorderFactory.createLineBorder(jsValues.uiglobals.screenBg));
		
		// set icons
		try { 
		    setIcon(new ImageIcon(jsdevisec.loadImage(defaultIconFilePath,
			  jsValues)));
		    setPressedIcon(new ImageIcon(jsdevisec.loadImage(
			  pressedIconFilePath, jsValues)));
		    setSelectedIcon(new ImageIcon(jsdevisec.loadImage(
			  selectedIconFilePath, jsValues)));
		    setDisabledIcon(new ImageIcon(jsdevisec.loadImage(
			  disabledIconFilePath, jsValues)));
		} catch (Exception ex) {
			System.err.println("Error creating toolbar button icons:" + ex);
		}
	}
	
	// getters / setters
	public void setImagePath(String path) { imagePath = path; }
	public String getImagePath() { return imagePath; }
	public void setImageExtension(String ext) { imageExtension = ext; }
	public String getImageExtension() { return imageExtension; }
	
	// PRIVATE METHODS
	private String constructIconFilePath(String state) {
		return imagePath + imageNameBase + "." + state + "." + imageExtension;
	}
	
	private boolean checkForFile(String path) {
		File testFile = new File(path);
		return testFile.isFile();
	}
	
}
