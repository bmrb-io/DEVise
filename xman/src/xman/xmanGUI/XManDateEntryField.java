//----------------------------------------------------------------------
// 
// File Name:    XManQuery.java
// 
// Description:  Experiment Management Query Window
//
// Author:       Hao Wang
// 
// D.O.B.        02/07/2000
//
// $id:$
//----------------------------------------------------------------------
package xman.xmanGUI;

import javax.swing.*; 
import javax.swing.text.*; 

import java.util.Date;
import java.awt.Toolkit;
import java.text.*;

import xman.shared.*;

public class XManDateEntryField extends JTextField {

    public XManDateEntryField(Date date) {
        super(12);  // Fixed for now
        //setDocument(new XManFormattedDoc(f));
        setValue(date);
    }

    public String getValue() {
	return Resources.formatter.format(getText());
    }

    public void setValue(Date date) {
        setText(Resources.formatter.format(date));
    }
}






