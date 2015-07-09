// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2007-2015
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
// Revision 1.1.30.3  2015/07/09 19:00:24  wenger
// Final cleanup of Aditya's changes.
//
// Revision 1.1.30.2  2015/07/08 21:48:11  wenger
// Merged aditya1_br_8 thru aditya1_br_9 to aditya_merge_br.
//
// Revision 1.1.30.1  2015/07/08 20:57:17  wenger
// Merged aditya1_br_7 thru aditya1_br_8 to aditya_merge_br.
//
// Revision 1.1.28.4  2015/02/11 20:09:46  kancherla
// Fixed help icons bug - by changing path to absolute
//
// Revision 1.1.28.3  2015/02/04 20:07:56  kancherla
// Fxed the icons not showing in the help window bug - 1025
//
// Revision 1.1.28.2  2014/12/29 18:57:35  kancherla
// Added hyperlinks
//
// Revision 1.1.28.1  2014/12/19 21:05:33  kancherla
// fixed help icons bug  - 1025
//
// Revision 1.1  2008/02/08 21:03:19  wenger
// Changed version from 5.8.3 to 5.9.0 because the toolbar and associated
// stuff is such a big change; added JavaScreen help dialog (although the
// text is not yet complete, and some is commented out until I get html
// links to work in that dialog); added (currently disabled) menu item to
// show JS version history.
//

// ------------------------------------------------------------------------

package JavaScreen.UI;

import java.util.*;
import java.io.*;
import java.awt.*;

import javax.activation.MimeType;
import javax.swing.*;
import JavaScreen.DEViseJSValues;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLEditorKit;

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
	//JEditorPane editorPane = new JEditorPane("text/html", htmlText);
	try {
		final JEditorPane editorPane = new JEditorPane("text/html", "");
		/*File file = new File("html/javascreen_help.html");
		editorPane.setPage(file.toURI().toURL());
		editorPane.setEditable(false);*/
		
		editorPane.setPage(getClass().getResource("/JavaScreen/html/javascreen_help.html"));
		editorPane.setEditable(false);
		
		editorPane.addHyperlinkListener(new HyperlinkListener() {
		    public void hyperlinkUpdate(HyperlinkEvent e) {
		        if(e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
		        	String desc = e.getDescription();
		        	try {
		        		if (desc.startsWith("#")) {
	                	editorPane.setPage(e.getURL());
		        		} else {
		        			Desktop desktop = Desktop.getDesktop();
	                        desktop.browse(e.getURL().toURI());
		        		}
		        	} catch (Exception ex) {
                        ex.printStackTrace();
                    }
		        }
		    }
		});
	
		JScrollPane scrollPane = new JScrollPane(editorPane);
		add(scrollPane);
	} catch(Exception e) {
		e.printStackTrace();
	}
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
