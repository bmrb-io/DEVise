// YAtom.java
// last updated on 09/20/99


import java.awt.*;
import java.awt.image.*;

public class YAtom {
    // The name for the atom, it is an unique ID for this type of atom
    public String atomName = null;
    // The radius, diameter in pixel
    public int R, D;
    // The color for the atom
    public int red, green, blue;

    private int rangeD;
    // The index color array for the atom
    private byte[] data;
    // How to draw this atom, default is 0
    private int drawScheme;
    // The image for this atom after apply drawScheme
    private Image atomImage = null;

    // The minimum display size for the atom in pixel
    public static int minD;

    static {
        minD = 10;
    }

    public YAtom(String name, int radius, int r, int g, int b)
    {
        if (name != null) {
            atomName = name;
        } else {
            atomName = new String("UnknownAtom");
        }

        // The radius of an atom will be at least 5 pixel and at most 50 pixels
        // to ensure minimal visual effects
        if (radius < 5) {
            R = 5;
        } else if (radius > 50) {
            R = 50;
        } else {
            R = radius;
        }

        D = 2 * R;
        rangeD = D - minD;

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
    }

    public void reset()
    {
        atomImage = null;
    }

    private void setup(Component component)
    {
        if (component == null) {
            return;
        }

        data = new byte[D * D];

        if (drawScheme == 0) {
            int maxR = 0, hx = (int)(0.2f * D + 0.5f), hy = (int)(0.2f * D + 0.5f);
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
                    data[pos++] = (r <= 0 ? (byte)1 : (byte)r);
                }
            }

            byte[] icmR = new byte[256], icmG = new byte[256], icmB = new byte[256];
            icmR[0] = (byte)255;
            icmG[0] = (byte)255;
            icmB[0] = (byte)255;
            for (int i = 1; i <= maxR; i++) {
                float factor = (float)i / maxR;
                icmR[i] = (byte)blend(red, 255, factor);
                icmG[i] = (byte)blend(green, 255, factor);
                icmB[i] = (byte)blend(blue, 255, factor);
            }

            IndexColorModel model = new IndexColorModel(8, maxR + 1, icmR, icmG, icmB, 0);
            atomImage = component.createImage(new MemoryImageSource(D, D, model, data, 0, D));
        }
    }

    private int blend(int fg, int bg, float fgfactor)
    {
        return (int)(bg + (fg - bg) * fgfactor);
    }

    public void paint(Component component, Graphics gc, int x, int y, float zfactor)
    {
        if (component == null || gc == null) {
            return;
        }

        if (atomImage == null) {
            setup(component);
            if (atomImage == null) {
                return;
            }
        }

        int zoomSize;
        if (zfactor < 0) {
            zoomSize = minD;
        } else {
            zoomSize = minD + (int)(zfactor * rangeD + 0.5f);
        }

        gc.drawImage(atomImage, x - zoomSize / 2, y - zoomSize / 2, zoomSize, zoomSize, component);
    }
}
