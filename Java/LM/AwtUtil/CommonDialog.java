/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class CommonDialog.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;

// Class
/**
 * The <CODE>CommonDialog</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class CommonDialog extends Dialog implements ActionListener
{
  private Panel m_buttonPanel;
  private String[] m_buttons;
  private int m_choice;

  public CommonDialog(String title, String [] buttons) {
    super(new Frame(), title); 
    m_buttons = buttons;
    m_buttonPanel = new Panel();
    for (int i = 0; i < buttons.length; i++) {
      Button b = new Button(buttons[i]);
      m_buttonPanel.add(b);
      b.setActionCommand(buttons[i]);
      b.addActionListener(this);
    }
    add(m_buttonPanel, "South");
  }
  public void actionPerformed(ActionEvent e) {
    for (int i = 0; i < m_buttons.length; i++) {
      if (e.getActionCommand() == m_buttons[i]) {
        m_choice = i;
        dispose();
        break;
      }
    }
  }
  public int doModal() {
    setModal(true); 
    show();
    return m_choice;
  }
}
