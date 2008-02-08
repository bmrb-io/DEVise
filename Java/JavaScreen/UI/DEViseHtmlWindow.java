// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// A window to display an HTML page.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ------------------------------------------------------------------------

package JavaScreen.UI;

import java.util.*;
import java.io.*;
import java.awt.*;
import javax.swing.*;
import JavaScreen.DEViseJSValues;

public class DEViseHtmlWindow extends JFrame {

    static final int DEBUG = 0;

    // -------------------------------------------------------------------
    //TEMPTEMP -- document
    public DEViseHtmlWindow(String title, String htmlText) {
	if (DEBUG >= 1) {
    	    System.out.println("DEViseHtmlWindow(" + title + ")");
	}

	setTitle(title);
	setSize(800, 600);
	setLocation(10, 30);
	setVisible(true);

	//TEMPTEMP -- links don't work here...
	JEditorPane editorPane = new JEditorPane("text/html", htmlText);
	editorPane.setEditable(false);

	JScrollPane scrollPane = new JScrollPane(editorPane);

	add(scrollPane);
    }

    // -------------------------------------------------------------------
    //TEMPTEMP -- document -- note that it works in an applet if file is in the jar file
    public static String getTextFromFile(DEViseJSValues jsValues,
      String fileName) {
	if (DEBUG >= 1) {
    	    System.out.println("DEViseHtmlWindowgetTextFromUrl(" +
	      fileName + ")");
	}

        String text = "";

	try {
	    InputStream is =
	      jsValues._imageLoadComp.getClass().getResourceAsStream(fileName);

	    // Note: data is corrupted in applet if we don't use a
	    // BufferedInputStream.  wenger 2007-10-18.
	    BufferedInputStream bis = new BufferedInputStream(is);

            boolean done = false;
	    while (!done) {
	        int size = bis.available();
	        if (DEBUG >= 1) {
    	            System.out.println("size = " + size );
	        }

		// There doesn't seem to be any way to make sure we've
		// gotten to EOF without trying to read off the end.
		// available() returning 0 doesn't seem like it is
		// guaranteed to mean we hit EOF.
		if (size == 0) size = 1;

	        byte[] data = new byte[size];

		int readResult = bis.read(data);
		if (readResult != -1) {
	            text += new String(data);
		} else {
		    done = true;
		}
	    }

	} catch(Exception ex) {
	    System.err.println("Error getting text: " + ex);
	    text = "Error: unable to get requested text";
	}

        return text;
    }
}
