/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ToolBarTest.java
****************************************************************************/

//Packages 

//package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.net.*;

import gjt.*;

// Class
/**
 * The <CODE>ToolBarTest</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ToolBarTest 
{
  int SINGLE_MODE = 0;
  int MULTI_MODE = 1;
  int NORMAL = 2;
  private int m_mode = NORMAL;
  public static void main(String[] args) {
    Frame f = new Frame("ToolBarTest");
    f.setSize(1000,1000);
    ImageButtonPanel tb= new ImageButtonPanel(Orientation.HORIZONTAL);
    URL fly,eagle, snail;
    try {
      fly = new URL("file:/u/s/s/ssl/Work/LM/AwtUtil/test.gif");
      eagle = new URL("file:/u/s/s/ssl/Work/LM/AwtUtil/eagle.gif");
      snail = new URL("file:/u/s/s/ssl/Work/LM/AwtUtil/snail.gif");
    } catch (Exception e) {
      System.out.println("Malformed url");
      return;
    }
    ImageProducer ip1, ip2, ip3;
    try {
      ip1 = (ImageProducer)fly.getContent();
      ip2 = (ImageProducer)eagle.getContent();
      ip3 = (ImageProducer)snail.getContent();
    } catch (Exception e) {
      System.out.println("Io Exception");
      return;
    }
    ImageButton flyButton = tb.add(f.createImage(ip1).getScaledInstance(20,20, Image.SCALE_AREA_AVERAGING), "Fly");
    ImageButton eagleButton = tb.add(f.createImage(ip2).getScaledInstance(20,20,Image.SCALE_AREA_AVERAGING), "Eagle");
    tb.addSpacer(50);
    DualModeStickyImageButton snailButton
      = new DualModeStickyImageButton(f.createImage(ip3).getScaledInstance(20,20,Image.SCALE_AREA_AVERAGING), 2, new DualModeStickyImageButtonListener());
    tb.add(snailButton);

    flyButton.setBubbleHelp("help the fly");
    eagleButton.setBubbleHelp("help the eagle");
    snailButton.setBubbleHelp("help the snail");
    flyButton.setListener(new SpringyImageButtonListener());
    eagleButton.setListener(new StickyImageButtonListener());
    //snailButton.setListener(new DualModeStickyImageButtonListener());
    flyButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        System.out.println("flyButton: fired");
      }
    });
    eagleButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        System.out.println("eagleButton: fired");
      }
    });
    snailButton.addActionListener(new DualModeActionListener() {
      public void doSingleModeActivated() {
        System.out.println("Single Mode");
      }
      public void doMultiModeActivated() {
        System.out.println("multi Mode");
      }
      public void doRaised() {
        System.out.println("raised");
      }
    });
    f.add(tb, "South");
    Panel p = new Panel();
    p.setSize(300,300);
    f.add(p, "Center");
    p.addMouseListener(new MyMouseAdapter(snailButton));
    f.pack();
    f.show();
  }  
}
  class MyMouseAdapter extends MouseAdapter {
    private DualModeStickyImageButton m_but;
    public MyMouseAdapter(DualModeStickyImageButton but) {
      m_but = but;
    }
    public void mouseClicked(MouseEvent me) {
      System.out.println("MouseClick : Raise the snail button...."); 
      m_but.raise();
    }
  }
