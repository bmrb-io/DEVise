/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class MessageDialog.java
****************************************************************************/

//Packages 

//package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;

// Class
/**
 * The <CODE>MessageDialog</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class MessageDialog 
{
  public static int YESNOCANCEL = 0;
  public static int OKCANCEL = 1;
  public static int YESNO = 2;
  public static int OK = 3;
  public static int CUSTOM = 4;
 
  Dialog m_dialog;
  Button m_buttons[];
  Panel m_textPanel;
  Panel m_buttonPanel;
  int m_dflt;
  int m_choice;
  
  /* test code */
  public static void main(String args[]) {
    MessageDialog msg = new MessageDialog("MessageDialog", 
                                          "This is a very very long................................\n.................................................\n...................................test", 
                                          YESNOCANCEL, 1);
    int c = msg.doModal();
    System.out.println("Choice =" + c);
    System.exit(1);
  }
  public MessageDialog(String title, String contents,
                       String [] buttons, int dflt) {
    this(new Frame(), title, contents, buttons, dflt);
  }
  public MessageDialog(String title, String contents,
                       int type, int dflt){
    this(new Frame(), title, contents, type, dflt);
  } 
  public MessageDialog(String title, String contents,
                       int type, String[]buttons, int dflt) {
    this(new Frame(), title, contents, type, buttons, dflt);
  }
  public MessageDialog(Frame frame, String title, String contents,
                       String [] buttons, int dflt) {
    this(frame, title, contents, CUSTOM, buttons, dflt);
  }
  public MessageDialog(Frame frame, String title, String contents,
                       int type, int dflt){
    this(frame, title, contents, type, null, dflt);
  } 
  public MessageDialog(Frame frame, String title, String contents,
                       int type, String[]buttons, int dflt) {
    m_dialog = new Dialog(frame, title);
    m_dialog.setSize(400,150);
    m_textPanel = new Panel();
    TextArea txt = new TextArea(contents,
                                5, 50,
                                TextArea.SCROLLBARS_VERTICAL_ONLY);
    txt.setEditable(false);
    m_textPanel.add(txt);
    m_buttonPanel = new Panel();
    m_dialog.add(m_textPanel, "Center");
    m_dialog.add(m_buttonPanel, "South");
    String butLabels[];
    if (type == YESNOCANCEL) {
      butLabels = new String[] { "YES", "NO", "CANCEL"};
    } else if (type == OKCANCEL) {
      butLabels = new String[] { "OK", "CANCEL"};
    } else if (type == YESNO) {
      butLabels = new String[] { "YES", "NO"};
    } else if (type == YESNOCANCEL) {
      butLabels = new String[] { "YES", "NO", "CANCEL"};
    } else if (type == OK) {
      butLabels = new String[] { "OK" };
    } else if (type == CUSTOM) {
      butLabels = buttons; 
    } else {
      return;
    }
    m_buttons = new Button[butLabels.length];
    if (dflt >= butLabels.length) {
      m_dflt = 0;
    } else {
      m_dflt = dflt;
    }
    for (int i = 0; i < butLabels.length; i++) {
      System.out.println("Button("  + i + ")");
      String label = butLabels[i];
      if (i == m_dflt) {
         label = label + "*";
      }
      m_buttons[i] = new Button(label);
      m_buttonPanel.add(m_buttons[i]);
      m_buttons[i].addActionListener(new ButtonHandler(i));
    }
    // doesn't work...??
    m_dialog.addKeyListener(new KeyAdapter() {
      public void keyPressed(KeyEvent e) {
        System.out.println("Key...");
        if (e.getKeyCode() == KeyEvent.VK_ENTER) {
          m_choice = m_dflt;
          m_dialog.dispose();
        } 
      }
    });
    m_choice = -1;
  }
  public int doModal() {
    m_dialog.setModal(true);
    m_dialog.show();
    return m_choice;
  }
  private class ButtonHandler implements ActionListener {
    private int m_whichBut;
    public ButtonHandler(int choice) {
      super();
      m_whichBut = choice;
    }
    public void actionPerformed(ActionEvent e) {
      m_choice = m_whichBut; 
      m_dialog.dispose();
    }
  }
}
