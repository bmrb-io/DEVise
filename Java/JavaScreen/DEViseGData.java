// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2014
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// There is once instance of this class for each GData record the
// JavaScreen is handling.

// An object in this class corresponds to a "decoded" GData record
// -- in other words, the values from the GData record have been pulled
// out and  put in the object according to the object type.

// Note: this is pretty bad design -- there really should be a subclass
// for each type of symbol, instead of this one class with a bunch of
// members that only apply to some symbol types.  However, I don't want
// to deal with that right now.  wenger 2006-02-21.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseGData.java

package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.net.*;

public class DEViseGData
{
    public jsdevisec jsc = null; //TEMP -- do we really need this for *every* GData record???
    public DEViseView parentView = null;

    public Rectangle GDataLoc = null;

    //TEMP -- why do we need x, y, etc, *and* GDataLoc
    // x, y, z are x0, y0, z0 converted from data units to pixels.
    public int x = 0, y = 0, z = 0, width = 0, height = 0;
    // x0, y0, z0 are the values in the GData (data units).
    public float x0, y0, z0, x1, y1, z1;

    //public String[] data = null;
    public Component symbol = null;
    public boolean isJavaSymbol = false;
    public String _buttonLabel;
    public String _buttonCmd;
    public String _menuType;
    public String _menuName;
    public int symbolType = 0;

    // Note: I should probably make subclasses of DEViseGData...

    // Values for symbolType.
    public static final int _symOval = 4;
    public static final int _symSegment = 7;
    public static final int _symText = 12;
    public static final int _symEmbeddedTk = 15;
    public static final int _symFixedText = 16;

    public String string = null;
    public Color color = null;
    public Font font = null;
    public int outline = 0;

    // Values for Jmol atoms
    public int atomNum;
    public String atomName;
    public String residueLabel; // amino acid
    public int entityAssemblyID;
    public int residueNum;
    public String atomType;
    public String structType;
    public String atomId;

    // Distance restraint values for Jmol
    public String atom1Id;
    public String atom2Id;
    public boolean isViolated;
    public boolean isAmbiguous;

    private static final boolean _debug = false;

    // GData format: <x> <y> <z> <color> <size> <pattern> <orientation>
    // <symbol type> <shape attr 0> ... <shape attr 14>
    // xm is x multiplier (GData to pixel conversion)
    // xo is x offset (GData to pixel conversion)
    // ym is y multiplier (GData to pixel conversion)
    // yo is y offset (GData to pixel conversion)
    public DEViseGData(jsdevisec panel, DEViseView view, String gdata, float xm,
                       float xo, float ym, float yo) throws YException
    {
        jsc = panel;

        jsc.jsValues.gdata._gdCount++;
        if (_debug) {
            System.out.println("DEViseGData constructor " +
                               jsc.jsValues.gdata._gdCount + "(" + gdata + ")");
            System.out.println("Free memory: " +
                               Runtime.getRuntime().freeMemory() + "/" +
                               Runtime.getRuntime().totalMemory());
        }

        parentView = view;
        if (parentView == null) {
            throw new YException("Invalid parent view for GData!");
        }

        String[] data = DEViseGlobals.parseString(gdata, '{', '}', false);
        if (_debug) {
            System.out.println("GData string = <" + gdata + ">");
            System.out.print("Parsed GData: ");
            for (int index = 0; index < data.length; index++) {
                System.out.print("<" + data[index] + "> ");
            }
            System.out.println("");
        }
        if (data == null || data.length != 23) {
            throw new YException("Invalid GData: {" + gdata + "}");
        }
        gdata = null;

        float size = 0;
        try {
            // ADD COMMENT about conversion here
            x0 = (Float.valueOf(data[0])).floatValue();
            x = (int)(x0 * xm + xo);
            y0 = (Float.valueOf(data[1])).floatValue();
            y = (int)(y0 * ym + yo);
            z0 = (Float.valueOf(data[2])).floatValue();
            z = (int)z0;
            size = (Float.valueOf(data[4])).floatValue();
            symbolType = Integer.parseInt(data[7]);
        } catch (NumberFormatException e) {
            throw new YException("Invalid GData: (" + gdata + ")");
        }

        //TEMP -- perhaps this should be done when the symbol is
        // drawn, instead of now
        if (symbolType == _symEmbeddedTk) { // check symbol type
            EmbeddedTk(data, size, xm, ym);

        } else if (symbolType == _symText) {
            TextLabel(data, size, xm, ym);

        } else if (symbolType == _symFixedText) {
            FixedTextLabel(data, size);

        } else if (symbolType == _symOval) {
            if (view.getUseJmol() && view.viewDimension == 3) {
                JmolOval(data, size, xm, ym);
            } else {
                Oval(data, size, xm, ym);
            }

        } else if (symbolType == _symSegment) {
            if (view.getUseJmol() && view.viewDimension == 3) {
                JmolSegment(data, size, xm, ym);
            } else {
                Segment(data, size, xm, ym);
            }

        } else {
            DefaultSymbol(data, size, xm, ym);
        }

        GDataLoc = new Rectangle(x, y, width, height);
    }

    protected void finalize() {
        jsc.jsValues.gdata._gdCount--;
        if (_debug) {
            System.out.println("DEViseGData.finalize() " + jsc.jsValues.gdata._gdCount);
            System.out.println("Free memory: " +
                               Runtime.getRuntime().freeMemory() + "/" +
                               Runtime.getRuntime().totalMemory());
        }
    }

    public Rectangle getLocInScreen()
    {
        Rectangle loc = parentView.getLocInScreen();

        Rectangle r = new Rectangle(x + loc.x, y + loc.y, width, height);
        if (r.x < loc.x) {
            r.width = r.width + r.x - loc.x;
            r.x = loc.x;
        } else if (r.x > loc.x + loc.width - r.width) {
            r.width = r.width + r.x - loc.x - loc.width + r.width;
            r.x = loc.x + loc.width - r.width;
        }
        if (r.y < loc.y) {
            r.height = r.height + r.y - loc.y;
            r.y = loc.y;
        } else if (r.y > loc.y + loc.height - r.height) {
            r.height = r.height + r.y - loc.y - loc.height + r.height;
            r.y = loc.y + loc.height - r.height;
        }

        return r;
    }

    protected void TextLabel(String[] data, float size, float xm, float ym)
    throws YException
    {
        isJavaSymbol = false;

        string = data[8];

        float w = 0.0f, h = 0.0f;
        int align, ff, fw, fs;

        // Note: format is ignored for now.  RKW 1999-11-03.

        // default font is courier, regular, nonitalic
        try {
            w = (Float.valueOf(data[10])).floatValue();
            h = (Float.valueOf(data[11])).floatValue();

            if (data[12].equals("")) {
                outline = 0;
            } else {
                outline = Integer.parseInt(data[12]);
            }

            if (data[13].equals("")) {
                align = 0;
            } else {
                align = Integer.parseInt(data[13]);
                if (align < -4 || align > 4) {
                    align = 0;
                }
            }

            if (data[14].equals("")) {
                ff = 0;
            } else {
                ff = Integer.parseInt(data[14]);
            }

            if (data[15].equals("")) {
                fw = 0;
            } else {
                fw = Integer.parseInt(data[15]);
            }

            if (data[16].equals("")) {
                fs = 0;
            } else {
                fs = Integer.parseInt(data[16]);
            }
        } catch (NumberFormatException e) {
            throw new YException("Invalid Gdata!");
        }

        width = (int)(w * size * xm);
        height = (int)(h * size * ym);
        if (width < 0) {
            width = -width;
        }
        if (height < 0) {
            height = -height;
        }

        color = DEViseUIGlobals.convertColor(data[3]);

        // Note: not using the default font here is probably less
        // efficient, but the previous version of this code failed
        // if we needed different fonts for different symbols.
        // RKW 1999-11-02.
        if (w < 0.0f) {
            // Note: Z value in GData (if > 1) is max point size of font.
            font = DEViseUIGlobals.getFont(string, height, ff, fw, fs, (int)z0);
        } else {
            // Note: Z value in GData (if > 1) is max point size of font.
            font = DEViseUIGlobals.getFont(string, width, height, ff, fw, fs, (int)z0);
        }

        if (color == null || font == null) {
            string = null;
            return;
        }

        AlignText(align);

        x = parentView.viewLocInCanvas.x + x;
        y = parentView.viewLocInCanvas.y + y;
    }

    protected void FixedTextLabel(String[] data, float size) throws YException
    {
        isJavaSymbol = false;

        string = data[8];

        float w = 0.0f, h = 0.0f;
        int align, ff, fw, fs;

        // default font is courier, regular, nonitalic
        try {
            if (data[10].equals("")) {
                align = 0;
            } else {
                align = Integer.parseInt(data[10]);
                if (align < -4 || align > 4) {
                    align = 0;
                }
            }

            if (data[11].equals("")) {
                ff = 0;
            } else {
                ff = Integer.parseInt(data[11]);
            }

            if (data[12].equals("")) {
                fw = 0;
            } else {
                fw = Integer.parseInt(data[12]);
            }

            if (data[13].equals("")) {
                fs = 0;
            } else {
                fs = Integer.parseInt(data[13]);
            }
        } catch (NumberFormatException e) {
            throw new YException("Invalid Gdata!");
        }

        width = 0;
        height = 0;

        color = DEViseUIGlobals.convertColor(data[3]);

        // Note: not using the default font here is probably less
        // efficient, but the previous version of this code failed
        // if we needed different fonts for different symbols.
        // RKW 1999-11-02.
        int fsize;
        if (size > 1.0f) {
            fsize = (int)(size + 0.25f);
        } else {
            fsize = (int)(size * jsc.jsValues.uiglobals.screenSize.height + 0.25f);
        }
        font = DEViseUIGlobals.getFont(fsize, ff, fw, fs);

        if (color == null || font == null) {
            string = null;
            return;
        }

        AlignText(align);

        x = parentView.viewLocInCanvas.x + x;
        y = parentView.viewLocInCanvas.y + y;
    }

    protected void EmbeddedTk(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = true;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0) {
            width = -width;
        }
        if (height < 0) {
            height = -height;
        }

        x = x - width / 2;
        y = y - height / 2;
        /* Don't force the entire rectangle to be within the view.
         * RKW 1999-10-28.
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        */

        _buttonLabel = data[11];
        _buttonCmd = data[10];
        if (data[12].equals("bmrb_dynamics_movie") && !data[13].equals("")) {
            _buttonCmd = _buttonCmd.replace("<res>", data[13]);
        }
        _menuType = data[21];
        _menuName = data[22];
    }

    protected void Oval(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0)
            width = -width;
        if (height < 0)
            height = -height;

        color = DEViseUIGlobals.convertColor(data[3]);

        // ADD COMMENT -- is this the name of the element??
        string = data[10];
    }

    protected void JmolOval(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0)
            width = -width;
        if (height < 0)
            height = -height;

        color = DEViseUIGlobals.convertColor(data[3]);

        atomNum = (Integer.valueOf(data[10])).intValue();
        atomName = data[11];
        residueLabel = data[14];
        residueNum = (Integer.valueOf(data[13])).intValue();
        atomType = data[15];
        structType = data[16];
        // Get Entity_assembly_ID (pre 11.4.0 versions of Peptide-CGI
        // don't send that).
        String entityAssemblyIDTmp = data[21];
        if (entityAssemblyIDTmp.equals("")) {
            entityAssemblyID = 1;
        } else {
            entityAssemblyID =
                (Integer.valueOf(entityAssemblyIDTmp)).intValue();
        }
        atomId = data[22];
    }

    // This is a distance restraint...
    protected void JmolSegment(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        atom1Id = data[17].trim();
        atom2Id = data[18].trim();
        isViolated = data[19].trim().equals("Violated");
        isAmbiguous = data[22].trim().equals("Ambiguous");
    }

    protected void Segment(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0)
            width = -width;
        if (height < 0)
            height = -height;

        color = DEViseUIGlobals.convertColor(data[3]);

        x1 = x0 + size * (Float.valueOf(data[8])).floatValue();
        y1 = y0 + size * (Float.valueOf(data[9])).floatValue();
        z1 = z0 + size * (Float.valueOf(data[10])).floatValue();
        //z1 = (int)(z0 + size * ym * z1 );
    }

    protected void DefaultSymbol(String[] data, float size, float xm, float ym)
    {
        isJavaSymbol = false;

        width = (int)(size * xm);
        height = (int)(size * ym);
        if (width < 0) {
            width = -width;
        }
        if (height < 0) {
            height = -height;
        }

        x = x - width / 2;
        y = y - height / 2;
        /* Don't force the entire symbol to be within the view.
         * RKW 1999-10-28.
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        */
    }

    protected void AlignText(int align)
    {
        Toolkit tk = Toolkit.getDefaultToolkit();
        FontMetrics fm = tk.getFontMetrics(font);
        int ac = fm.getAscent(), dc = fm.getDescent();
        int sw = fm.stringWidth(string);

        switch (align) {
        case -4: // northwest
            y = y + ac;
            break;
        case -3: // west
            y = y + (ac - dc) / 2;
            break;
        case -2: // southwest
            y = y - dc;
            break;
        case -1: // north
            x = x - sw / 2;
            y = y + ac;
            break;
        case 0: // center
            x = x - sw / 2;
            y = y + (ac - dc) / 2;
            break;
        case 1: // south
            x = x - sw / 2;
            y = y - dc;
            break;
        case 2: // northeast
            x = x - sw;
            y = y + ac;
            break;
        case 3: // east
            x = x - sw;
            y = y + (ac - dc) / 2;
            break;
        case 4: // southeast
            x = x - sw;
            y = y - dc;
            break;
        }
    }
}
