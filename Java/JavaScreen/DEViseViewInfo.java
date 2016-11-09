// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to display the mouse location in data units
// in the text boxes in the upper right corner of the JavaScreen.

// There is only one instance of this class.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseViewInfo.java

package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.math.*;
import java.text.*;
import javax.swing.*;

public class DEViseViewInfo extends JPanel
{
    private static final int DEBUG = 0;

    private jsdevisec jsc = null;
    Vector images = null;

    JTextField mouseX = new JTextField(12);
    JTextField mouseY = new JTextField(12);
    //DEViseTrafficLight light = null;

    public DEViseViewInfo(jsdevisec what, Vector array)
    {
        jsc = what;
        images = array;

        setBackground(jsc.jsValues.uiglobals.bg);
        setForeground(jsc.jsValues.uiglobals.fg);
        setFont(jsc.jsValues.uiglobals.font);

        mouseX.setBackground(jsc.jsValues.uiglobals.textBg);
        mouseX.setForeground(jsc.jsValues.uiglobals.textFg);
        mouseX.setFont(DEViseFonts.getFont(12, DEViseFonts.MONOSPACED, 0, 0));

        mouseY.setBackground(jsc.jsValues.uiglobals.textBg);
        mouseY.setForeground(jsc.jsValues.uiglobals.textFg);
        mouseY.setFont(DEViseFonts.getFont(12, DEViseFonts.MONOSPACED, 0, 0));

        setLayout(new FlowLayout(FlowLayout.LEFT, 2, 1));

        //if (images != null && images.size() == 11) {
        //    try {
        //        light = new DEViseTrafficLight((Image)images.elementAt(9), (Image)images.elementAt(10), "0");
        //    } catch (YException e) {
        //        light = null;
        //    }
        //}

        //if (light != null)
        //    add(light);

        add(mouseX);
        add(mouseY);
    }

    public void updateInfo(String name, String x, String y)
    {
        /* Don't change the title according to which view we're in.  wenger 2010-06-02.
                if (name == null) {
                    if (jsc.parentFrame != null) {
                        jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle);
                    }
                } else {
                    if (jsc.parentFrame != null) {
                        jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle + "     \"" + name + "\"");
                    }
                }
        */
        if ((x == null) || (x == "")) {
            // This 'if' is added to avoid flickering when no values to display
            if(!mouseX.getText().equals("")) {
                mouseX.setText("");
            }
        } else {
            mouseX.setText(x);
        }

        if ((y == null) || (y == "")) {
            if(!mouseY.getText().equals("")) {
                mouseY.setText("");
            }
        } else {
            mouseY.setText(y);
        }

        // Getting rid of validate here seems to fix the lockup we've
        // been getting with JS 5.8.0.
        //validate();
    }


    public void updateInfo(String x, String y)
    {
        updateInfo(null, x, y);
    }

    public void updateInfo()
    {
        /* Don't change the title according to which view we're in.  wenger 2010-06-02.
                if (jsc.parentFrame != null) {
                    jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle);
                }
        */

        mouseX.setText("");
        mouseY.setText("");

        // Getting rid of validate here seems to fix the lockup we've
        // been getting with JS 5.8.0.
        //validate();
    }

    // status is DEViseTrafficLight.STATUS_*.
    public void updateImage(int status, boolean isOn)
    {
        if (jsc.light != null) {
            Runnable doUpdateImage = new DoUpdateImage(status, isOn);
            SwingUtilities.invokeLater(doUpdateImage);
        }
    }

    private class DoUpdateImage implements Runnable {
        private int _status;
        private boolean _isOn;

        DoUpdateImage(int status, boolean isOn) {
            _status = status;
            _isOn = isOn;
        }

        public void run() {
            jsc.light.updateImage(_status, _isOn);
        }
    }

    public void updateCount(int number)
    {
        if (number < 0 || number > 99 || jsc.light == null)
            return;

        Runnable doUpdateCount = new DoUpdateCount(number);
        SwingUtilities.invokeLater(doUpdateCount);
    }

    private class DoUpdateCount implements Runnable {
        private int _number;

        DoUpdateCount(int number) {
            _number = number;

        }

        public void run() {
            jsc.light.updateCount("" + _number);
        }
    }

    public static String viewParser(float x, String format)
    {
        String result = String.valueOf(x);

        if (format  != null) {
            try {
                PrintfFormatter pf = PrintfFormatter.get(format);
                result = pf.form((double) x);
            } catch(Exception e) {
                e.printStackTrace();
            }
        }

        return result;
    }

    // This should be called whenever we close a session, just to make
    // sure we keep the number of format objects to a reasonable level.
    public static void clearFormatters()
    {
        PrintfFormatter.clear();
        GFormat.clear();
        EFormat.clear();
        FFormat.clear();
    }
}


// ========================================================================
/**
 * A class for formatting numbers that follows printf conventions.
**/

class PrintfFormatter {
    private static final int DEBUG = 0;

    /* cFormats the number following printf conventions.  */

    private int width = 0;
    private int precision = -1;
    private String pre = "";
    private String post = "";
    private boolean leading_zeroes = false;
    private boolean show_plus = false;
    private boolean alternate = false;
    private boolean show_space = false;
    private boolean left_align = false;
    private char fmt = ' ';

    private MyFormat _formatter = null;

    private static Hashtable _pffList = new Hashtable();

    // --------------------------------------------------------------------
    // Keep a list of PrintfFormatter objects, indexed by the format
    // string, so we don't keep creating duplicates of ones we already
    // have.
    public static PrintfFormatter get(String format)
    {
        PrintfFormatter pff = (PrintfFormatter)_pffList.get(format);
        if (pff == null) {
            pff = new PrintfFormatter(format);
            _pffList.put(format, pff);
        }

        return pff;
    }

    // --------------------------------------------------------------------
    // Clear the list of PrintfFormatter objects.
    public static void clear()
    {
        _pffList.clear();
    }

    // --------------------------------------------------------------------
    public  PrintfFormatter(String format)
    {
        if (DEBUG >= 1) {
            System.out.println("PrintfFormatter constructor(" + format + ")");
        }

        int state = 0;
        int length = format.length();

        // 0 = prefix, 1 = flags, 2 = width, 3 = precision,
        // 4 = format, 5 = end
        int parse_state = 0;

        int i = 0;

        //TEMP -- I'm sure this parsing code can use cleaning up, but
        // I am leaving it for now.  RKW 2002-08-01.

        // Analysing for the presence of %
        while (parse_state == 0) {
            if (i >= length) {
                parse_state = 5;
            } else if (format.charAt(i) == '%') {
                if (i < length - 1) {
                    if (format.charAt(i + 1) == '%') {
                        pre = pre + '%';
                        i++;
                    } else {
                        parse_state = 1;
                    }
                } else {
                    throw new java.lang.IllegalArgumentException();
                }
            } else {
                pre = pre + format.charAt(i);
            }

            i++;
        }

        // Test for the presence of  space, plus, zeroes, alternate
        while (parse_state == 1) {
            if (i >= length) {
                parse_state = 5;
            } else if (format.charAt(i) == ' ') {
                show_space = true;
                i++;
            } else if (format.charAt(i) == '-') {
                left_align = true;
                i++;
            } else if (format.charAt(i) == '+') {
                show_plus = true;
                i++;
            } else if (format.charAt(i) == '0') {
                leading_zeroes = true;
                i++;
            } else if (format.charAt(i) == '#') {
                alternate = true;
                i++;
            }

            parse_state = 2;
        }

        // calculation of width or test for precision

        while (parse_state == 2) {
            if (i >= length) {
                parse_state = 5;
            } else if ('0' <= format.charAt(i) && format.charAt(i) <= '9') {
                width = width * 10 + format.charAt(i) - '0';
                i++;
            } else if (format.charAt(i) == '.') {
                parse_state = 3;
                precision = 0;
                i++;
            } else {
                parse_state = 4;
            }

        }

        // calculation of precision

        while (parse_state == 3) {
            if (i >= length) {
                parse_state = 5;
            } else if ('0' <= format.charAt(i) && format.charAt(i) <= '9') {
                precision = precision * 10 + format.charAt(i) - '0';
                parse_state = 4;
                i++;
            } else {
                parse_state = 4;
            }
        }

        // determination of fmt - format

        if (parse_state == 4) {
            if (i >= length) {
                parse_state = 5;
            } else {
                fmt = format.charAt(i);
            }

            i++;
        }

        if (fmt  =='g' || fmt =='G') {
            width = precision;
        }

        // determination of post fix.

        if (i < length) {
            post = format.substring(i, length);
        }


        if (precision < 0) precision = 6;

        if (fmt =='g' || fmt =='G') {
            _formatter = GFormat.get(precision);
        } else if (fmt == 'f' || fmt == 'F') {
            _formatter = FFormat.get(precision);
        } else if (fmt == 'e' || fmt == 'E') {
            _formatter = EFormat.get(precision);
        } else {
            throw new java.lang.IllegalArgumentException();
        }
    }

    // --------------------------------------------------------------------
    /**
     * cFormats a double into a string (like sprintf in C)
     */
    public String form(double x)
    {
        return _formatter.format(x);
    }

    // --------------------------------------------------------------------
    // Return a String consisting of n cs.
    public static String repeat(char c, int n)
    {
        if (n <= 0) return "";

        StringBuffer s = new StringBuffer(n);

        for (int i = 0; i < n; i++) {
            s.append(c);
        }

        return s.toString();
    }
}

// ------------------------------------------------------------------------
interface MyFormat {
    public String format(double value);
}

// ========================================================================
// Format a number in printf %g format.
class GFormat implements MyFormat {
    private static final int DEBUG = 0;

    int _precision ;

    EFormat _ef;
    FFormat _ff;

    private static Hashtable _gfList = new Hashtable();

    // --------------------------------------------------------------------
    // Keep a list of GFormat objects, indexed by the precision, so we
    // don't keep creating duplicates of ones we already have.
    public static GFormat get(int precision)
    {
        GFormat gf = (GFormat)_gfList.get(new Integer(precision));
        if (gf == null) {
            gf = new GFormat(precision);
            _gfList.put(new Integer(precision), gf);
        }

        return gf;
    }

    // --------------------------------------------------------------------
    // Clear the list of GFormat objects.
    public static void clear()
    {
        _gfList.clear();
    }

    // --------------------------------------------------------------------
    private GFormat(int precision) {
        if (DEBUG >= 1) {
            System.out.println("GFormat constructor(" + precision + ")");
        }

        _precision = precision;

        _ef = EFormat.get(precision);
        _ff = FFormat.get(precision);
    }

    // --------------------------------------------------------------------
    public String format(double value) {
        if (DEBUG >= 2) {
            System.out.println("GFormat.format(" + value + ")");
        }

        String result = null;

        if (value == 0.0d) {
            result = _ff.format(value);
        } else {
            int exponent = (int)Math.floor(Math.log(Math.abs(value)) /
                                           Math.log(10.0));
            if (DEBUG >= 2) {
                System.out.println("  exponent = " + exponent);
            }

            if (exponent < -4 || Math.abs(exponent) > _precision) {
                result = _ef.format(value);
            } else {
                result = _ff.format(value);
            }
        }

        if (DEBUG >= 2) {
            System.out.println("  format result: " + result);
        }

        return result;
    }
}

// ------------------------------------------------------------------------
// Format a number in printf %f format.
class FFormat implements MyFormat {
    private static final int DEBUG = 0;

    double _roundoffStep;

    NumberFormat _nf;

    private static Hashtable _ffList = new Hashtable();

    // --------------------------------------------------------------------
    // Keep a list of FFormat objects, indexed by the precision, so we
    // don't keep creating duplicates of ones we already have.
    public static FFormat get(int precision)
    {
        FFormat ff = (FFormat)_ffList.get(new Integer(precision));
        if (ff == null) {
            ff = new FFormat(precision);
            _ffList.put(new Integer(precision), ff);
        }

        return ff;
    }

    // --------------------------------------------------------------------
    // Clear the list of FFormat objects.
    public static void clear()
    {
        _ffList.clear();
    }

    // --------------------------------------------------------------------
    private FFormat(int precision) {
        if (DEBUG >= 1) {
            System.out.println("FFormat.FFormat(" + precision + ")");
        }

        _roundoffStep = Math.pow(10.0d, -(double)precision);

        _nf = NumberFormat.getInstance();
        if (!(_nf instanceof DecimalFormat)) {
            // Calling constructor directly is not preferred.
            _nf = new DecimalFormat();
        }

        String pattern = PrintfFormatter.repeat('#', precision - 1);
        pattern += "0";
        if (precision > 0) {
            pattern += ".";
            pattern += PrintfFormatter.repeat('#', precision);
        }

        if (DEBUG >= 2) {
            System.out.println("  pattern: <" + pattern + ">");
        }

        ((DecimalFormat)_nf).applyPattern(pattern);
    }

    // --------------------------------------------------------------------
    public String format(double value) {
        if (DEBUG >= 2) {
            System.out.println("FFormat.format(" + value + ")");
        }

        // Stupid !@#$% Netscape 4.x doesn't properly round the numbers
        // before formatting, either.  RKW 2002-08-02.
        value = DEViseView.round2Step(value, _roundoffStep, false);

        String result = _nf.format(value);

        if (DEBUG >= 2) {
            System.out.println("  format result: " + result);
        }
        return result;
    }
}

// ========================================================================
// Format a number in printf %e format.
class EFormat implements MyFormat {
    private static final int DEBUG = 0;

    // We use an FFormat here because 'E' DecimalFormat patterns don't
    // work in !@#$% Netscape 4.x, so we have to figure out the mantissa
    // and exponent values ourselves.
    FFormat _ff;

    private static Hashtable _efList = new Hashtable();

    // --------------------------------------------------------------------
    // Keep a list of GFormat objects, indexed by the precision, so we
    // don't keep creating duplicates of ones we already have.
    public static EFormat get(int precision)
    {
        EFormat ef = (EFormat)_efList.get(new Integer(precision));
        if (ef == null) {
            ef = new EFormat(precision);
            _efList.put(new Integer(precision), ef);
        }

        return ef;
    }

    // --------------------------------------------------------------------
    // Clear the list of EFormat objects.
    public static void clear()
    {
        _efList.clear();
    }

    // --------------------------------------------------------------------
    private EFormat(int precision) {
        if (DEBUG >= 1) {
            System.out.println("EFormat.EFormat(" + precision + ")");
        }

        _ff = FFormat.get(precision);
    }

    // --------------------------------------------------------------------
    public String format(double value) {
        if (DEBUG >= 2) {
            System.out.println("EFormat.format(" + value + ")");
        }

        // Note: patterns with 'E' don't work right in Netscape (at
        // least Netscape 4.x), so we have to do some more work
        // ourselves.
        int exponent = 0;
        if (value != 0.0d) {
            exponent = (int)Math.floor(Math.log(Math.abs(value)) /
                                       Math.log(10.0));
        }
        if (DEBUG >= 2) {
            System.out.println("  exponent = " + exponent);
        }

        double powTen = Math.pow(10.0d, (double)exponent);
        double mantissa = value / powTen;

        String result = _ff.format(mantissa);
        result += "E" + exponent;

        if (DEBUG >= 2) {
            System.out.println("  format result: " + result);
        }
        return result;
    }
}
