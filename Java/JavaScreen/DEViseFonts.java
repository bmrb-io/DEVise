// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1  2000/05/22 17:52:49  wenger
// JavaScreen handles fonts much more efficiently to avoid the problems with
// GData text being drawn very slowly on Intel platforms.
//

// This class contains methods for dealing with fonts efficiently.

// ------------------------------------------------------------------------

import java.util.*;
import java.awt.*;

public final class DEViseFonts
{
    public static final int MONOSPACED = 0;
    public static final int SERIF = 1;
    public static final int SANS_SERIF = 2;

    // ---------------------------------------------------------------------
    // fSize = size in points
    // fFam = font family (0 = "Monospaced", 1 = "Serif", 2 = "SanSerif")
    // fWeight = font weight (0 = plain, 1 = bold)
    // fSlant = font slant (0 = plain, 1 = italic)
    public static Font getFont(int fSize, int fFam, int fWeight, int fSlant)
    {
	if (_debug >= 2) {
	    System.out.println("DEViseFonts.getFont(" + fSize + ", " + fFam +
	      ", " + fWeight + ", " + fSlant);
	}

	// TEMP -- should have defined constants for font family
        if (fFam < 0 || fFam >= _families.length) {
            fFam = 1;
        }

        Font font = FindExisting(fSize, fFam, fWeight, fSlant);
	if (font == null) {
	    if (fSize >= _minSize) {

                int fontstyle = ((fWeight == 0) ? Font.PLAIN : Font.BOLD) +
	          ((fSlant == 0) ? Font.PLAIN : Font.ITALIC);

                if (_debug >= 1) {
                    System.out.println("Creating new font: " + fSize + ", " +
		      fFam + ", " + fWeight + ", " + fSlant);
                }
                font = new Font(_families[fFam], fontstyle, fSize);
		_fonts.addElement(font);
	    }
	}

	return font;
    }

    // ---------------------------------------------------------------------
    public static void ClearFonts()
    {
        _fonts.removeAllElements();
    }

    // ---------------------------------------------------------------------
    private static Font FindExisting(int fSize, int fFam, int fWeight,
      int fSlant)
    {
	Font font = null;

	String family = _families[fFam];
	int style = ((fWeight == 0) ? Font.PLAIN : Font.BOLD) +
	  ((fSlant == 0) ? Font.PLAIN : Font.ITALIC);

	int index = 0;
	while (index < _fonts.size() && font == null) {
	    Font tmpFont = (Font)_fonts.elementAt(index);
	    if (tmpFont.getName().equals(family) &&
	      tmpFont.getSize() == fSize && tmpFont.getStyle() == style) {
	        font = tmpFont;
	    }

	    index++;
	}

        return font;
    }

    // DEVise font name
    private static final String[] _families = {"Monospaced", "Serif",
      "SanSerif"};

    private static final int _minSize = 1;

    private static Vector _fonts = new Vector();

    private static final int _debug = 0;
}
