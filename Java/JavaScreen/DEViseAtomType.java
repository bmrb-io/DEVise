// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class holds information for each atom "type" (element).
// There is an image that is used to redraw the atoms by scaling
// the image, unless the difference in size between the image and
// what we need is too great.

// There is one instance of this class for each "type" of atom (e.g., element).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9  2000/04/07 22:43:13  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.8  2000/04/05 06:25:36  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.7  2000/03/23 16:26:12  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.6  2000/01/12 14:37:48  hongyu
// *** empty log message ***
//
// Revision 1.5  1999/12/10 15:37:00  wenger
// Added standard headers to source files.
//
// ========================================================================


import java.awt.*;
import java.awt.image.*;

public class DEViseAtomType {
    // The name for the atom, it is an unique ID for this type of atom
    public String name = new String("UnknownAtom");

    // the radius in data unit
    public float radius = 1.0f;

    // The radius, diameter in pixels
    public int R, D;

    // The color for the atom
    public Color color = null, selectedColor = Color.cyan;

    // The image for this atom after apply drawScheme
    private Image image = null, selectedImage = null;

    // The minimum display size for the atom in pixels.
    private static final int minD = 10;

    // This is something like the difference between the size of the largest
    // and smallest atoms, in pixels.
    private int rangeD;

    public int drawSize; // size we're drawing an atom, in pixels

    public DEViseAtomType(String n)
    {
        if (n != null) {
            name = n;
        }

        // use default value for all other data
        color = new Color(0, 0, 0);
    }

    public DEViseAtomType(String n, Color c)
    {
        if (n != null) {
            name = n;
        }

        // use default value for all other data
        if (c != null) {
            color = c;
        } else {
            color = new Color(DEViseUIGlobals.COLOR_MIN,
	      DEViseUIGlobals.COLOR_MIN, DEViseUIGlobals.COLOR_MIN);
        }
    }

    public DEViseAtomType(String n, float rr, int r, int g, int b)
    {
        if (n != null) {
            name = n;
        }

        if (rr > 0.0) {
            radius = rr;
        }

	int red = Math.min(DEViseUIGlobals.COLOR_MAX,
	  Math.max(DEViseUIGlobals.COLOR_MIN, r));
	int green = Math.min(DEViseUIGlobals.COLOR_MAX,
	  Math.max(DEViseUIGlobals.COLOR_MIN, g));
	int blue = Math.min(DEViseUIGlobals.COLOR_MAX,
	  Math.max(DEViseUIGlobals.COLOR_MIN, b));

        color = new Color(red, green, blue);
    }

    public void setColor(Color c)
    {
        if (c != null) {
            color = c;
            image = null;
        }
    }

    public void setSelectColor(Color c)
    {
        if (c != null) {
            selectedColor = c;
            selectedImage = null;
        }
    }

/* Not used.  RKW 2000-05-23.
    // r is pixels?
    public void setRadius(int r)
    {
        int oldR = R;

        R = r;
        if (R < 5) {
            R = 5;
        }
        if (R > 50) {
            R = 50;
        }

        D = 2 * R;
        rangeD = D - minD;

        radius = radius * (float)R / oldR;

        image = null;
    }
*/

/* Not used.  RKW 2000-05-23.
    // r is data units?
    public void setRadius(float r)
    {
        float oldradius = radius;

        if (r > 0) {
            radius = r;
        } else {
            return;
        }

        R = (int)(R * radius / oldradius);
        if (R < 5) {
            R = 5;
        }

        if (R > 50) {
            R = 50;
        }

        D = 2 * R;
        rangeD = D - minD;

        image = null;
    }
*/

    // Some kind of restting values to defaults.
    public void reset(float pixelToUnit)
    {
        if (pixelToUnit <= 0.0f) {
            pixelToUnit = 10;
        }

        R = (int)(radius * pixelToUnit + 0.5f);
        if (R < 5) {
            R = 5;
        }

        if (R > 50) {
            R = 50;
        }

        D = 2 * R;
        rangeD = D - minD;

        image = null;
        selectedImage = null;
    }

    private void createImage(Component component, boolean isSelected)
    {
        if (component == null) {
            return;
        }

        byte[] newdata = new byte[D * D];
        Image newimage = null;
        Color newcolor = null;
        if (isSelected) {
            newcolor = selectedColor;
        } else {
            newcolor = color;
        }

	// ADD COMMENT -- what are hx, hy?
        int maxR = 0, hx = (int)(0.2f * D + 0.5f), hy = (int)(0.2f * D + 0.5f);
        int pos, x0, xx, yy, r;

        for (int y = 0; y < D; y++) {
            // the effect Y range is the range that line X = x intersect
	    // with the circle
            x0 = (int)(Math.sqrt(R * R - (y - R) * (y - R)) + 0.5f);

            // the starting position of this range in the data array
            pos = y * D + R - x0;

            for (int x = -x0; x < x0; x++) {
                // the x and y position shifted according to highlight
		// point (hx, hy)
                xx = x + hx;
                yy = y - R + hy;

                // the distance from (x, y) to (hx, hy)
                r = (int)(Math.sqrt(xx * xx + yy * yy) + 0.5f);

                // recording the maximum r
                if (r > maxR) {
                    maxR = r;
                }

                // recording the position realR into data array, later on,
		// we will use an index color model to dim or intensify the
		// color according to the magnitude of this position
                newdata[pos++] = (r <= 0 ? (byte)1 : (byte)r);
            }
        }

	//
	// Shade the atom.  Make part of it lighter, and part of it
	// darker.
	//
        byte[] icmR = new byte[maxR+1], icmG = new byte[maxR+1],
	  icmB = new byte[maxR+1];
        int red = newcolor.getRed();
        int green = newcolor.getGreen();
        int blue = newcolor.getBlue();
        int endLighter = maxR / 2;

	for (int i = 0; i <= endLighter; i++) {
            float factor = (float)i / endLighter;
            icmR[i] = (byte)blend(red, DEViseUIGlobals.COLOR_MAX, factor);
            icmG[i] = (byte)blend(green, DEViseUIGlobals.COLOR_MAX, factor);
            icmB[i] = (byte)blend(blue, DEViseUIGlobals.COLOR_MAX, factor);
	}

	for (int i = endLighter + 1; i <= maxR; i++) {
            float factor = (float)i / maxR;
	    factor = 1.0F - factor;
	    factor += 0.5; // avoid making things too dark...
            factor = Math.min(factor, 1.0F);
            icmR[i] = (byte)blend(red, DEViseUIGlobals.COLOR_MIN, factor);
            icmG[i] = (byte)blend(green, DEViseUIGlobals.COLOR_MIN, factor);
            icmB[i] = (byte)blend(blue, DEViseUIGlobals.COLOR_MIN, factor);
        }

        IndexColorModel model = new IndexColorModel(8, maxR + 1, icmR,
	  icmG, icmB, 0);
        newimage = component.createImage(new MemoryImageSource(D, D,
	  model, newdata, 0, D));

        if (isSelected) {
            selectedImage = newimage;
        } else {
            image = newimage;
        }
    }

    // Factor = 0 -> returns bg; factor = 1 -> returns fg.
    private static int blend(int fg, int bg, float fgfactor)
    {
        return (int)(bg + (fg - bg) * fgfactor);
    }

/* Not used.  RKW 2000-05-22.
    public static int xorcolor(int rgb)
    {
        return ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ 0xff0000) |
	  ((rgb & 0xff00) ^ 0xff00) | ((rgb & 0xff) ^ 0xff));
    }
*/

    // Values for drawStyle:
    public static final int DRAW_PLAIN_SELECTED = 3;
    public static final int DRAW_PLAIN_UNSELECTED = 2;
    public static final int DRAW_SHADED_SELECTED = 1;
    public static final int DRAW_SHADED_UNSELECTED = 0;

    public void paint(Component component, Graphics gc, int x, int y,
      float zfactor, int drawStyle) throws YError
    {
        if (component == null || gc == null) {
            return;
        }

        if (zfactor < 0.0f) {
            drawSize = minD;
        } else {
            drawSize = minD + (int)(zfactor * rangeD + 0.5f);
        }

        if (drawStyle == DRAW_PLAIN_UNSELECTED) {
            Color oldcolor = gc.getColor();
            gc.setColor(color);
            gc.fillOval(x - drawSize / 2, y - drawSize / 2, drawSize, drawSize);
            gc.setColor(oldcolor);
        } else if (drawStyle == DRAW_PLAIN_SELECTED) {
            Color oldcolor = gc.getColor();
            gc.setColor(selectedColor);
            gc.fillOval(x - drawSize / 2, y - drawSize / 2, drawSize, drawSize);
            gc.setColor(oldcolor);
        } else if (drawStyle == DRAW_SHADED_SELECTED) {
            if (selectedImage == null) {
                createImage(component, true);
                if (selectedImage == null) {
                    return;
                }
            }
            gc.drawImage(selectedImage, x - drawSize / 2, y - drawSize / 2,
	      drawSize, drawSize, component);
        } else if (drawStyle == DRAW_SHADED_UNSELECTED) {
            if (image == null) {
                createImage(component, false);
                if (image == null) {
                    return;
                }
            }
            gc.drawImage(image, x - drawSize / 2, y - drawSize / 2, drawSize,
	      drawSize, component);
        } else {
	    throw new YError("Illegal draw style: " + drawStyle);
	}
    }
}
