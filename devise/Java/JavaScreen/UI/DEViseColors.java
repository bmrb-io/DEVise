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
// Revision 1.1.2.3  2007/06/06 14:13:13  wenger
// Got JavaScreen Makefile to work with Andy's latest changes; added
// headers to the UI java files.
//

// ------------------------------------------------------------------------

package JavaScreen.UI;

import java.awt.Color;
import java.util.*;

public final class DEViseColors {
  private static final Color LIGHT_TAN = new Color(247, 246, 220);
	private static final Color DARK_TAN = new Color(229, 227, 207);
	private static final Color DARK_GREY = new Color(91, 90, 84);
	private static final Color LIGHT_BLUE = new Color(161, 185, 219);
	private static final Color DARK_BLUE = new Color(106, 156, 216);

	private static Hashtable DEFAULT_COLORS = new Hashtable();
/*	DEFAULT_COLORS.put('MAIN_BACKGROUND', LIGHT_TAN);*/
/*	DEFAULT_COLORS.put('MAIN_FOREGROUND', DARK_GREY);*/
	
	private Hashtable colors;
	
	public DEViseColors() { DEViseColors(DEFAULT_COLORS); }

	public DEViseColors( File xmlSettings ) {
		// not implemented yet, will create a Hashtable from xml data
		// and send to the next constructor
	}
	public DEViseColors( Hashtable colors_hash ) {
		// need to check to make sure all necessary color
		// definitions are present, and replace missing 
		// color definitions with the default value
		
		colors = colors_hash;
	}
	
	public Color getColor(String colorName) {
		return colors.get(colorName);
	}
}
