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

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
    public double radius = 1.0;
    // The radius, diameter in pixel
    public int R, D;
    // The color for the atom
    public Color color = null, XORcolor = null, selectedColor = Color.cyan;
    public int red, green, blue;

    // The index color array for the atom
    private byte[] data, selectedData;
    // How to draw this atom, default is 0
    private int scheme = 1;
    // The image for this atom after apply drawScheme
    private Image image = null, selectedImage = null;

    // The minimum display size for the atom in pixel
    public int minD = 10;
    public int rangeD;
    public int drawSize;
    public int lastDrawStyle;

    public DEViseAtomType(String n)
    {
        if (n != null) {
            name = n;
        }

        // use default value for all other data
        color = new Color(red, green, blue);
        XORcolor = new Color(xorcolor(color.getRGB()));
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
            color = new Color(red, green, blue);
        }

        XORcolor = new Color(xorcolor(color.getRGB()));
    }

    public DEViseAtomType(String n, double rr, int r, int g, int b)
    {
        if (n != null) {
            name = n;
        }

        if (rr > 0.0) {
            radius = rr;
        }

        if (r < 0) {
            red = 0;
        } else if (r > 255) {
            red = 255;
        } else {
            red = r;
        }

        if (g < 0) {
            green = 0;
        } else if (g > 255) {
            green = 255;
        } else {
            green = g;
        }

        if (b < 0) {
            blue = 0;
        } else if (b > 255) {
            blue = 255;
        } else {
            blue = b;
        }

        color = new Color(red, green, blue);
        XORcolor = new Color(xorcolor(color.getRGB()));
    }

    public void setColor(Color c)
    {
        if (c != null) {
            color = c;
            XORcolor = new Color(xorcolor(color.getRGB()));
            image = null;
            data = null;
        }
    }

    public void setSelectColor(Color c)
    {
        if (c != null) {
            selectedColor = c;
            selectedImage = null;
            selectedData = null;
        }
    }

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

        radius = radius * (double)R / oldR;

        data = null;
        image = null;
    }

    public void setRadius(double r)
    {
        double oldradius = radius;

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

        data = null;
        image = null;
    }

    public void reset(double pixelToUnit)
    {
        if (pixelToUnit <= 0.0) {
            pixelToUnit = 10;
        }

        R = (int)(radius * pixelToUnit + 0.5);
        if (R < 5) {
            R = 5;
        }

        if (R > 50) {
            R = 50;
        }

        D = 2 * R;
        rangeD = D - minD;

        data = null;
        image = null;
        selectedData = null;
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

        if (scheme == 1) {
            int maxR = 0, hx = (int)(0.2 * D + 0.5), hy = (int)(0.2 * D + 0.5);
            int pos, x0, xx, yy, r;

            for (int y = 0; y < D; y++) {
                // the effect Y range is the range that line X = x intersect with the circle
                x0 = (int)(Math.sqrt(R * R - (y - R) * (y - R)) + 0.5);
                // the starting position of this range in the data array
                pos = y * D + R - x0;
                for (int x = -x0; x < x0; x++) {
                    // the x and y position shifted according to highlight point (hx, hy)
                    xx = x + hx;
                    yy = y - R + hy;

                    // the distance from (x, y) to (hx, hy)
                    r = (int)(Math.sqrt(xx * xx + yy * yy) + 0.5);

                    // recording the maximum r
                    if (r > maxR) {
                        maxR = r;
                    }

                    // recording the position realR into data array, later on, we will use an index color
                    // model to dim or intensify the color according to the magnitude of this position
                    newdata[pos++] = (r <= 0 ? (byte)1 : (byte)r);
                }
            }

            byte[] icmR = new byte[256], icmG = new byte[256], icmB = new byte[256];
            icmR[0] = (byte)255;
            icmG[0] = (byte)255;
            icmB[0] = (byte)255;
            red = newcolor.getRed();
            green = newcolor.getGreen();
            blue = newcolor.getBlue();
            for (int i = 1; i <= maxR; i++) {
                float factor = (float)i / maxR;
                icmR[i] = (byte)blend(red, 255, factor);
                icmG[i] = (byte)blend(green, 255, factor);
                icmB[i] = (byte)blend(blue, 255, factor);
            }

            IndexColorModel model = new IndexColorModel(8, maxR + 1, icmR, icmG, icmB, 0);
            newimage = component.createImage(new MemoryImageSource(D, D, model, newdata, 0, D));

            if (isSelected) {
                selectedImage = newimage;
            } else {
                image = newimage;
            }
        }
    }

    private int blend(int fg, int bg, float fgfactor)
    {
        return (int)(bg + (fg - bg) * fgfactor);
    }

    public static int xorcolor(int rgb)
    {
        return ((rgb & 0xff000000) | ((rgb & 0xff0000) ^ 0xff0000) | ((rgb & 0xff00) ^ 0xff00) | ((rgb & 0xff) ^ 0xff));
    }

    public void paint(Component component, Graphics gc, int x, int y, double zfactor, int isSelected)
    {
        if (zfactor < 0.0) {
            drawSize = minD;
        } else {
            drawSize = minD + (int)(zfactor * rangeD + 0.5);
        }

        paint(component, gc, x, y, drawSize, isSelected);
    }

    public void paint(Component component, Graphics gc, int x, int y, int size, int isSelected)
    {
        drawSize = size;
        lastDrawStyle = isSelected;

        paint(component, gc, x, y, size);
    }

    public void paint(Component component, Graphics gc, int x, int y, int size)
    {
        if (component == null || gc == null) {
            return;
        }

        if (lastDrawStyle == 2) {
            Color oldcolor = gc.getColor();
            gc.setColor(color);
            gc.fillOval(x - drawSize / 2, y - drawSize / 2, drawSize, drawSize);
            gc.setColor(oldcolor);
        } else if (lastDrawStyle == 3) {
            Color oldcolor = gc.getColor();
            gc.setColor(selectedColor);
            gc.fillOval(x - drawSize / 2, y - drawSize / 2, drawSize, drawSize);
            gc.setColor(oldcolor);
        } else if (lastDrawStyle == 1) {
            if (selectedImage == null) {
                createImage(component, true);
                if (selectedImage == null) {
                    return;
                }
            }
            gc.drawImage(selectedImage, x - drawSize / 2, y - drawSize / 2, drawSize, drawSize, component);
        } else {
            if (image == null) {
                createImage(component, false);
                if (image == null) {
                    return;
                }
            }
            gc.drawImage(image, x - drawSize / 2, y - drawSize / 2, drawSize, drawSize, component);
        }
    }
}
