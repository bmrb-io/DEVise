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

// This class is used to display the mouse location in data units
// in the text boxes in the upper right corner of the JavaScreen.

// There is only one instance of this class.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.48  2000/07/20 22:38:27  venkatan
// Mouse Location Format display:
// 1. Both X and Y axis formats are recognised.
// 2. "-" is recognised for "" String.
// 3. %.0f is now recognised.
//
// Revision 1.47  2000/07/20 16:26:07  venkatan
// Mouse Location Display format - is now controlled by printf type
// format strings specified by the VIEW_DATA_AREA command
//
// Revision 1.46  2000/06/12 22:13:57  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.45  2000/04/24 20:22:01  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.44  2000/04/07 22:44:10  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.43  2000/03/23 16:26:15  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.42  1999/10/10 08:49:53  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.41  1999/08/03 05:56:50  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.40  1999/07/27 17:11:19  hongyu
// *** empty log message ***
//
// Revision 1.38  1999/06/23 20:59:18  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseViewInfo.java

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.math.*;

public class DEViseViewInfo extends Panel
{
    private jsdevisec jsc = null;
    Vector images = null;

    TextField viewName = new TextField(16);
    TextField mouseX = new TextField(12), mouseY = new TextField(12);
    //DEViseTrafficLight light = null;

    public DEViseViewInfo(jsdevisec what, Vector array)
    {
        jsc = what;
        images = array;

        setBackground(DEViseUIGlobals.bg);
        setForeground(DEViseUIGlobals.fg);
        setFont(DEViseUIGlobals.font);

        viewName.setBackground(DEViseUIGlobals.textBg);
        viewName.setForeground(DEViseUIGlobals.textFg);
        viewName.setFont(DEViseUIGlobals.textFont);

        mouseX.setBackground(DEViseUIGlobals.textBg);
        mouseX.setForeground(DEViseUIGlobals.textFg);
        mouseX.setFont(DEViseUIGlobals.textFont);

        mouseY.setBackground(DEViseUIGlobals.textBg);
        mouseY.setForeground(DEViseUIGlobals.textFg);
        mouseY.setFont(DEViseUIGlobals.textFont);

        setLayout(new FlowLayout(FlowLayout.LEFT, 2, 12));

        //if (images != null && images.size() == 11) {
        //    try {
        //        light = new DEViseTrafficLight((Image)images.elementAt(9), (Image)images.elementAt(10), "0");
        //    } catch (YException e) {
        //        light = null;
        //    }
        //}

        //if (light != null)
        //    add(light);

        //add(viewName);

        add(mouseX);
        add(mouseY);
    }

    public void updateInfo(String name, String x, String y)
    {
        if (name == null) {
            //viewName.setText("");
            if (jsc.parentFrame != null) {
                jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle);
            }
        } else {
            //viewName.setText(name);
            if (jsc.parentFrame != null) {
                jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle + "     \"" + name + "\"");
            }
        }

        if (x == null) {
            mouseX.setText("");
        } else {
            mouseX.setText(x);
        }

        if (y == null) {
            mouseY.setText("");
        } else {
            mouseY.setText(y);
        }

        validate();
    }

    public void updateInfo(String x, String y)
    {
        updateInfo(null, x, y);
    }

    public void updateInfo()
    {
        //viewName.setText("");
        if (jsc.parentFrame != null) {
            jsc.parentFrame.setTitle(DEViseUIGlobals.javaScreenTitle);
        }

        mouseX.setText("");
        mouseY.setText("");

        validate();
    }

    // status is DEViseTrafficLight.STATUS_*.
    public void updateImage(int status, boolean isOn)
    {
        if (jsc.light != null) {
            jsc.light.updateImage(status, isOn);
            jsc.validate();
        }
    }

    public void updateCount(int number)
    {
        if (number < 0 || number > 99 || jsc.light == null)
            return;

        jsc.light.updateCount("" + number);
        jsc.validate();
    }

    public static String viewParser(float x, String format){
       String result = "" + x;
       if(format  == null) return "" + x;
      try{
       PrintfFormatter pf = new PrintfFormatter(format);
       result = pf.form((double) x);
       return result;
       }
       catch(Exception e){
	  e.printStackTrace();
	}
	return result;
    }

}




    

/**
 * A class for formatting numbers that follows printf conventions.
**/

class PrintfFormatter { 
    
    /* cFormats the number following printf conventions.  */
    
    private int width;
    private int precision;
    private String pre;
    private String post;
    private boolean leading_zeroes;
    private boolean show_plus;
    private boolean alternate;
    private boolean show_space;
    private boolean left_align;
    private char fmt; 
    
    public  PrintfFormatter(String s)
    {  
	width = 0;
	precision = -1;
	pre = "";
	post = "";
	leading_zeroes = false;
	show_plus = false;
	alternate = false;
	show_space = false;
	left_align = false;
	fmt = ' ';  
	
	int state = 0;
	int length = s.length();
	int parse_state = 0;
	
	// 0 = prefix, 1 = flags, 2 = width, 3 = precision,
	// 4 = format, 5 = end
	
	
	int i = 0;
	
	
	// Analysing for the presence of %
	while (parse_state == 0)
	    {  if (i >= length) parse_state = 5;
	    else if (s.charAt(i) == '%')
		{  
		    if (i < length - 1)
			{  if (s.charAt(i + 1) == '%')
			    {  pre = pre + '%';
			    i++;
			    }
			else{ 
			    parse_state = 1;
			}
			}
		    else {
			throw new java.lang.IllegalArgumentException();
		    }
		}
	    else{
		pre = pre + s.charAt(i);
	    }
	    i++;
	    
	    }
	// Test for the presence of  space, plus, zeroes, alternate
	while (parse_state == 1)
	    {  
		if (i >= length) parse_state = 5;
		else if (s.charAt(i) == ' '){show_space = true; i++;}
		else if (s.charAt(i) == '-'){left_align = true; i++;}
		else if (s.charAt(i) == '+'){show_plus = true;i++;}
		else if (s.charAt(i) == '0'){leading_zeroes = true; i++;}
		else if (s.charAt(i) == '#'){alternate = true; i++;}
		
		parse_state = 2;
	    }
	
	// calculation of width or test for precision
	
	while (parse_state == 2)
	    {  if (i >= length) parse_state = 5;
	    else if ('0' <= s.charAt(i) && s.charAt(i) <= '9')
		{  width = width * 10 + s.charAt(i) - '0';
		i++;
		}
	    else if (s.charAt(i) == '.')
		{  parse_state = 3;
		precision = 0;
		i++;
		} else{
		    parse_state = 4;
		}
	    
	    }
	
	// calculation of precision 
	
	while (parse_state == 3)
	    {  if (i >= length) parse_state = 5;
	    else if ('0' <= s.charAt(i) && s.charAt(i) <= '9')
		{  precision = precision * 10 + s.charAt(i) - '0';
		parse_state = 4;
		i++;
		}
	    else
		parse_state = 4;
	    }
	
	// determination of fmt - format
	
	if (parse_state == 4)
	    {  if (i >= length) parse_state = 5;
	    else fmt = s.charAt(i);
	    i++;
	    }
	
	if(fmt  =='g' || fmt =='G'){
	    
	    width = precision;
	}
	
	// determination of post fix.
	
	if (i < length)
	    post = s.substring(i, length);
	
    }
    
    /**
     * prints a formatted number following printf conventions
     */

    public static void print(java.io.PrintStream s, String fmt, double x)
    {  s.print(new PrintfFormatter(fmt).form(x));
    }
    
    /**
     * prints a formatted number following printf conventions
     */
    public static void print(java.io.PrintStream s, String fmt, long x)
    { 
	s.print(new PrintfFormatter(fmt).form(x));
    }
    
    /**
     * prints a formatted number following printf conventions
     */
    
    public static void print(java.io.PrintStream s, String fmt, char x)
    { 
	s.print(new PrintfFormatter(fmt).form(x));
    }
    
    /**
     * prints a formatted number following printf conventions
     */
    
    public static void print(java.io.PrintStream s, String fmt, String x)
    { 
	s.print(new PrintfFormatter(fmt).form(x));
    }
    
    /**
     * Converts a string of digits (decimal, octal or hex) to an integer
     */
    
    public static int atoi(String s)
    { 
	return (int)atol(s);
    }
    
    /**
     * Converts a string of digits (decimal, octal or hex) to a long integer
     */
    
    public static long atol(String s)
    {  
	int i = 0;
	
	while (i < s.length() && Character.isWhitespace(s.charAt(i))) i++;
	if (i < s.length() && s.charAt(i) == '0')
	    {  if (i + 1 < s.length() && (s.charAt(i + 1) == 'x' || s.charAt(i + 1) == 'X'))
		return parseLong(s.substring(i + 2), 16);
	    else return parseLong(s, 8);
	    }
	else return parseLong(s, 10);
    }
    
    private static long parseLong(String s, int base)
    {
	int i = 0;
	int sign = 1;
	long r = 0;
	
	while (i < s.length() && Character.isWhitespace(s.charAt(i))) i++;
	if (i < s.length() && s.charAt(i) == '-') { sign = -1; i++; }
	else if (i < s.length() && s.charAt(i) == '+') { i++; }
	while (i < s.length())
	    {  char ch = s.charAt(i);
	    if ('0' <= ch && ch < '0' + base)
		r = r * base + ch - '0';
	    else if ('A' <= ch && ch < 'A' + base - 10)
		r = r * base + ch - 'A' + 10 ;
	    else if ('a' <= ch && ch < 'a' + base - 10)
		r = r * base + ch - 'a' + 10 ;
	    else
		return r * sign;
	    i++;
	    }
	return r * sign;
    }
    
    /**
     * Converts a string of digits to an double
     */
    
    public static double atof(String s)
    { 
	int i = 0;
	int sign = 1;
	double r = 0; // integer part
	double f = 0; // fractional part
	double p = 1; // exponent of fractional part
	int state = 0; // 0 = int part, 1 = frac part
	
	while (i < s.length() && Character.isWhitespace(s.charAt(i))) i++;
	if (i < s.length() && s.charAt(i) == '-') { sign = -1; i++; }
	else if (i < s.length() && s.charAt(i) == '+') { i++; }
	while (i < s.length())
	    {  char ch = s.charAt(i);
	    if ('0' <= ch && ch <= '9')
		{  if (state == 0)
		    r = r * 10 + ch - '0';
		else if (state == 1)
		    {  p = p / 10;
		    r = r + p * (ch - '0');
		    }
		}
	    else if (ch == '.')
		{  if (state == 0) state = 1;
		else return sign * r;
		}
	    else if (ch == 'e' || ch == 'E')
		{  long e = (int)parseLong(s.substring(i + 1), 10);
		return sign * r * Math.pow(10, e);
		}
	    else return sign * r;
	    i++;
	    }
	return sign * r;
    }
    
    /**
     * cFormats a double into a string (like sprintf in C)
     */
    
    public String form(double x)
    { 
	String r;
	if (precision < 0) precision = 6;
        if(fmt =='g' || fmt =='G'){
	    GFormat gf = new GFormat(precision);
	    return gf.format(x);
        }
	
	int s = 1;
	if (x < 0) { x = -x; s = -1; }
	if (fmt == 'f')
	    r = fixed_format(x);
	else if (fmt == 'e' || fmt == 'E' || fmt == 'g' || fmt == 'G')
	    r = exp_format(x);
	else {
	    throw new java.lang.IllegalArgumentException();
	}
	
	return pad(sign(s, r));
    }
    
    /**
     * cFormats a long integer into a string (like sprintf in C)
     */
    
    public String form(long x)
    { 
	String r;
	int s = 0;
	if (fmt == 'd' || fmt == 'i')
	    {  s = 1;
	    if (x < 0) { x = -x; s = -1; }
	    r = "" + x;
	    }
	else if (fmt == 'o')
	    r = convert(x, 3, 7, "01234567");
	else if (fmt == 'x')
	    r = convert(x, 4, 15, "0123456789abcdef");
	else if (fmt == 'X')
	    r = convert(x, 4, 15, "0123456789ABCDEF");
	else {
	    throw new java.lang.IllegalArgumentException();
	}
	
	return pad(sign(s, r));
    }
    
    /**
     * cFormats a character into a string (like sprintf in C)
     */
    
    public String form(char c)
    {  
	if (fmt != 'c'){
	    throw new java.lang.IllegalArgumentException();
	}
	
	String r = "" + c;
	return pad(r);
    }
    
    /**
     * cFormats a string into a larger string (like sprintf in C)
     */
    
    public String form(String s)
    { 
	if (fmt != 's'){
	    throw new java.lang.IllegalArgumentException();}
	if (precision >= 0) s = s.substring(0, precision);
	return pad(s);
    }
    
    
    private static String repeat(char c, int n)
    {  
	if (n <= 0) return "";
	StringBuffer s = new StringBuffer(n);
	for (int i = 0; i < n; i++) s.append(c);
	return s.toString();
    }
    
    private static String convert(long x, int n, int m, String d)
    { 
	if (x == 0) return "0";
	String r = "";
	while (x != 0)
	    {  r = d.charAt((int)(x & m)) + r;
	    x = x >>> n;
	    }
	return r;
    }
    
    private String pad(String r)
    {  
	String p = repeat(' ', width - r.length());
	if (left_align) return pre + r + p + post;
	else return pre + p + r + post;
    }
    
    private String sign(int s, String r)
    { 
	String p = "";
	if (s < 0) p = "-";
	else if (s > 0)
	    {  if (show_plus) p = "+";
	    else if (show_space) p = " ";
	    }
	else
	    {  if (fmt == 'o' && alternate && r.length() > 0 && r.charAt(0) != '0') p
											= "0";
	    else if (fmt == 'x' && alternate) p = "0x";
	    else if (fmt == 'X' && alternate) p = "0X";
	    }
	int w = 0;
	if (leading_zeroes)
	    w = width;
	else if ((fmt == 'd' || fmt == 'i' || fmt == 'x' || fmt == 'X' || fmt == 'o')
		 && precision > 0) w = precision;
	
	return p + repeat('0', w - p.length() - r.length()) + r;
    }
    
    private String fixed_format(double d)
    { 
	String f = "";
	
	//  if (d > 0x7FFFFFFFFFFFFFFFL) return exp_format(d);
	
	long l = (long)(precision == 0 ? d + 0.5 : d);
	f = f + l;
	
	double fr = d - l; // fractional part
	// if (fr >= 1 || fr < 0) return exp_format(d);
	
	return f + frac_part(fr);
    }
    
    private String frac_part(double fr)
    // precondition: 0 <= fr < 1
    {  
	String z = "";
	if (precision > 0)
	    {  double factor = 1;
	    String leading_zeroes = "";
	    for (int i = 1; i <= precision && factor <= 0x7FFFFFFFFFFFFFFFL; i++)
		{  factor *= 10;
		leading_zeroes = leading_zeroes + "0";
		}
	    long l = (long) (factor * fr + 0.5);
	    
	    z = leading_zeroes + l;
	    z = z.substring(z.length() - precision, z.length());
	    }
	
	if (precision > 0 || alternate) z = "." + z;
	if ((fmt == 'G' || fmt == 'g') && !alternate)
	    // remove trailing zeroes and decimal point
	    {  int t = z.length() - 1;
	    z = z.substring(0, t + 1);
	    }
	return z;
    }
    
    private String exp_format(double d)
    {  
	
	/*  token = new StringTokenizer(d.toString(), "Ee");
	    if(token.countTokens <=1){
	    
	    token = new StringTokenizer(d.toString(), ".");
	    throw new IllegalArgumentException("Invalid floating point format");
	    }
	    else{
	    boolean addZero = false;
	    mantissa = token.nextToken();
	    int exponent = Integer.parseInt(token.nextToken()); 
	    exponent+= checkDotPos(mantissa);
	    if(!addZero && mantissa.length < precision){
	    mantissa = mantissa + ".";
	    }
	    while(addZero && mantissa.length() <= precision ){
	    mantissa = mantissa + "0";
	    }
	    token = new StringTokenizer(d.toString(), "Ee");
	*/
	
	
	
	
	
	
	String f = "";
	int e = 0;
	double dd = d;
	double factor = 1;
	while (dd > 10) { e++; factor /= 10; dd = dd / 10; }
	if ((fmt == 'g' || fmt == 'G') && e >= -4 && e < precision)
	    return fixed_format(d);

	d = d * factor;
	f = f + fixed_format(d);
	
	if (fmt == 'e' || fmt == 'g')
	    f = f + "e";
	else
	    f = f + "E";
	
	String p = "000";
	if (e >= 0)
	    {  f = f + "+";
	    p = p + e;
	    }
	else
	    {  f = f + "-";
	    p = p + (-e);
	    }
	
	return f + p.substring(p.length() - 3, p.length());
    }
    
}

                                         
				 



/** 
    Class that formats in g type - string like "%.4g"
**/

class GFormat{
    
    int width ;
    double lowbound;
    double highbound;
    String sign;
    
    public GFormat(int a){
	
	width = a;
	lowbound = Math.pow(10, -(a-1));
	highbound = Math.pow(10, (a-1));
	
    }
    
    public String format(double d){
	
	String sign = "";
	String result = null;
	
	if(d < 0){
	    sign = "-"; 
	    d = -1*d;
	} 
	
	StringTokenizer dtokens = new StringTokenizer(new Double(d).toString(), "eE");
	if( dtokens.countTokens() == 2){
	    result = expFormat( dtokens.nextToken(), dtokens.nextToken()); 
    }
	else{ 
	    dtokens = new StringTokenizer(new Double(d).toString(), ".");
	    if(dtokens.countTokens()==2){
		result = decFormat(d);
		// Double x = new Double(result);
		// System.out.println(x.toString());
	    }
	    
	    
	} 
	return sign + result;
	
}
    
    
    
    private String decFormat(double d){
	String result = "";
	boolean isExp = false;
	boolean isCloseToTen = false;
	int intpart = 0; int fracpart = 0; int exp = 0;
	String dstring = new Double(d).toString();
	
	BigDecimal bd, bd1; 
	bd = new BigDecimal(d);
	bd1 =  bd.setScale(0,BigDecimal.ROUND_HALF_EVEN); 
	double dx = bd1.doubleValue();
	
	if( dx == 10.0){
	    isCloseToTen = true;
	}
  
	if(!isCloseToTen &&  d > 0 &&  (d <lowbound || d > highbound)){
	    isExp = true;
	    
	    while( d <= 9.0 && d > 0){ 
		exp = exp-1;
		d = d * 10;
	    }
	    while( d > 10) {
		exp = exp+1;
		d = d / 10;
	    }
	    
	}
	
	if(isExp){ 
	    
	    // Using the BigDecimal rounding method
	    bd = new BigDecimal(d);
	    bd1 = bd.setScale(width-1, BigDecimal.ROUND_HALF_EVEN);

	    // - Using the NumberFormat methods
	    // NumberFormat nf = NumberFormat.getInstance();
	    // nf.setMaximumFractionDigits(width-1);
	    // dstring = nf.format(d);
	    
	    dstring = bd1.toString() +"E"+ new Integer(exp).toString();
	    
	    // System.out.println("this is string"+ dstring);
	    
	    
	    // - did not work this one --
	    // dstring = new Double(dstring).toString();
	    //System.out.println(dstring);
	    // StringTokenizer dtokens = new StringTokenizer(new Double(d).toString(), "eE");
	    // result = expFormat(dtokens.nextToken(), dtokens.nextToken());
	    
	    
	    result = dstring;
	    
	}
	else{ 
	    int ifZeroExtra = 0;
	    bd1 = bd.setScale(1, BigDecimal.ROUND_HALF_EVEN);
	    StringTokenizer st = new StringTokenizer(bd1.toString(), ".");
	    if(st.countTokens() != 2){
		System.out.println(bd.toString()); 
		throw new NumberFormatException();
	    }
	    String temp  = st.nextToken(); 
	    if( temp.equals("0")){
	        ifZeroExtra = 1;
            }

	    int digits =  width - temp.length() + ifZeroExtra;
	    if( digits < 0 ){ digits = 0;}
	    bd1 =  bd.setScale(digits, BigDecimal.ROUND_HALF_EVEN); 
	    result =  bd1.toString(); 
	    
	}
	return result;  
	
    } 
    
    
    

    // for exp Format
    
    
    private String expFormat( String mantissa, String exponent){
	int digits = width -1;
	int mantFrac = 0, mantInt = 0, exp = 0;
	// validity of exponent as number
	
	try { 
	    
	    exp = Integer.parseInt(exponent);
	    
	    mantFrac =  getFracPart(mantissa);

	    mantInt  =  getIntPart(mantissa);
	}   
	catch(NumberFormatException e){
	    
	    System.out.println("Invalid number format in method : expFormat");
	    
	}
	
	String intpart = new Integer(mantInt).toString();
	String fracpart = new Integer(mantFrac).toString();
	
	if(intpart.charAt(0) == '-'){ 
	    sign = "-";
	    intpart = intpart.substring(1, intpart.length());
	}
	
	int count = intpart.length() - 1;
	if(count > 0){
	    fracpart = intpart.substring(1, intpart.length()) + fracpart;
	    exp = exp + count;
	}

	exponent = new Integer(exp).toString(); 
	
	while(fracpart.length() < digits){
	    fracpart = fracpart +"0";
	}
	if(digits > 1){ 
	    
	    mantissa = intpart + "." + fracpart.substring(0, digits);
	}
	else{ 
	    mantissa = intpart; 
	}
	
	return  mantissa +"E"+ exponent;
	
    }
    
    /*  - Test main method - 
	public static void main(String[] args){
	
	gFormat a = new gFormat(5);
	
	double x = -1124.84;
	double y = 1234.3455;
	double r =  10.0;
	double xr =  10.02;
	double rr =  1.0234;
	double ri =  10234.123;
	double rj =  0.123;
	double rk =  0.00000123;
	double rl =  0.0000123e-5;
	
	System.out.println( "x = " + x + "; formatted x  " + a.format(x));
	System.out.println( "y = " + y + "; formatted y  " + a.format(y));
	System.out.println( "r = " + r + "; formatted r  " + a.format(r));
	System.out.println( "rr = " + rr + "; formatted rr  " + a.format(rr));
	System.out.println( "xr = " + xr + "; formatted xr  " + a.format(xr));
	System.out.println( "ri = " + ri + "; formatted ri  " + a.format(ri));
	System.out.println( "rj = " + rj + "; formatted rj  " + a.format(rj));
	System.out.println( "rk = " + rk + "; formatted rk  " + a.format(rk));
	System.out.println( "rl = " + rl + "; formatted rl  " + a.format(rl));
	
	}
    */
    
    
    private static int getFracPart( String mantissa){
	
	Double db = new Double(mantissa);
	String result = null;
	StringTokenizer st = new StringTokenizer(db.toString(), ".");
	
	if(st.countTokens() == 2){
	    
	    st.nextToken(); 
	    result = st.nextToken(); 
	    
	    return Integer.parseInt(result);
	    
	    
	}else{
	    
	    return 0;
	    
	}
	
    }
    
    private static int getIntPart( String mantissa){
	
	Double db = new Double(mantissa);
	String result = null;
	StringTokenizer st = new StringTokenizer(db.toString(), ".");
	
	if(st.countTokens() == 2){
	    
	    result = st.nextToken(); 
	    
	    return Integer.parseInt(result);
	    
	    
	}else{
	    
	    return Integer.parseInt(mantissa);

	}
    }
    
    
}
      







 
