package xman.xnav;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.plaf.*;
import java.io.Serializable; 
import javax.swing.plaf.metal.MetalTabbedPaneUI;

public class MyTabPaneUI extends MetalTabbedPaneUI {

    protected int calculateTabHeight(int tabPlacement, int tabIndex, int fontHeight) {
	return 25;
    } 

    protected int calculateMaxTabHeight(int tabPlacement) {
       return 25;
    }

    /*
    protected int calculateTabWidth(int tabPlacement, int tabIndex, FontMetrics metrics) {
       return 105;
    }
    
    protected int calculateMaxTabWidth(int tabPlacement) {
       return 105;
    }
    */

    protected boolean shouldPadTabRun(int tabPlacement, int run) {
        return false;
    }
}
