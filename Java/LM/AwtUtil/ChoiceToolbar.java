/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ChoiceToolbar.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.net.*;
import COM.objectspace.jgl.*;
import gjt.*;

// Class
/**
 * The <CODE>ChoiceToolbar</CODE> class
 *
 * @author  Shilpa Lawande
 */

// special toolbar where each button is a sticky button and 
// only one button can be depressed at any time
public class ChoiceToolbar extends NiceToolbar  implements ActionListener
{  
  public static void main(String args[]) {
    Frame f = new Frame("ToolBarTest");
    f.setSize(1000,1000);
    ChoiceToolbar tb = new ChoiceToolbar("Test", Orientation.HORIZONTAL);

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
    ImageButton flyButton = new ImageButton(f.createImage(ip1).getScaledInstance(20,20, Image.SCALE_AREA_AVERAGING));
    tb.add(flyButton, "Fly");
    ImageButton eagleButton = new ImageButton(f.createImage(ip2).getScaledInstance(20,20,Image.SCALE_AREA_AVERAGING));
    tb.add(eagleButton,"Eagle");
    tb.addSpacer(50);
    ImageButton snailButton = new ImageButton(f.createImage(ip3).getScaledInstance(20,20,Image.SCALE_AREA_AVERAGING));
    tb.add(snailButton, "Snail");
    flyButton.setBubbleHelp("help the fly");
    eagleButton.setBubbleHelp("help the eagle");
    snailButton.setBubbleHelp("help the snail");

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
    snailButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        System.out.println("snail button : fired");
      }
    });

    f.add(tb, "South");
    Panel p = new Panel();
    p.setSize(300,300);
    f.add(p, "Center");
    f.pack();
    f.show();
  }
  private String m_mode = null;
  private DList m_buttonList = new DList();

  public ChoiceToolbar(String name, Orientation orientation) {
    super(name, orientation);
  }
  public ChoiceToolbar(String name, Orientation orientation, 
                       int buttonWidth, int buttonHeight,
                       int thickness) {
    super(name, orientation, buttonHeight, buttonHeight, thickness);
  }
  public void add(ImageButton button, String name) {
    super.add(button, name);
    button.addActionListener(this);
    button.setListener(new StickyImageButtonListener());
    m_buttonList.add(button);
    if (m_mode == null) {
      m_mode = name;
      button.setInset();
      button.setEnabled(false);
    }
  }
  public String getMode()  { return m_mode; }
  public void actionPerformed(ActionEvent e) {
    ImageButton b = (ImageButton)e.getSource();
    String newMode = getButtonPanel().getButtonName(b);
    if (newMode.equals(m_mode)) {
      return;
    }
    m_mode = newMode;
    for (DListIterator d = m_buttonList.begin(); d.hasMoreElements(); ) {
      ImageButton ib = (ImageButton)d.nextElement();
      if (ib != b) {
        ib.setRaised();
        ib.setEnabled(true);
      } else {
        ib.setInset();
        ib.setEnabled(false);
      }
    }
  }
}
