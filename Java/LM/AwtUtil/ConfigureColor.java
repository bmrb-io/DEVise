/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ConfigureColor.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;

// Class
/**
 * The <CODE>ConfigureColor</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class ConfigureColor extends Dialog
{
  TextField m_color[];
  ColorSelectDlg dlg = new ColorSelectDlg();
  
  /* Constructor */
  public ConfigureColor(String names[], Color clrs[]) {
    super(new Frame(), "Configure colors"); 
    setSize(300,200);

    Panel mainPanel = new Panel(new GridBagLayout());
    GridBagConstraints constraints = new GridBagConstraints();
    m_color = new TextField[names.length];
    for (int i = 0; i < names.length; i++) {
      Label label =  new Label(names[i]);
      m_color[i] = new TextField("      ");
      m_color[i].setEditable(false);
      m_color[i].setBackground(clrs[i]);
      constraints.gridx = 0;
      constraints.gridy = i;
      constraints.weightx = 0.1;
      constraints.weighty = 0.33;
      constraints.gridwidth = 1;
      
      mainPanel.add(label, constraints);

      constraints.gridx = GridBagConstraints.RELATIVE;
      constraints.weightx = 0.16;
      constraints.weighty = 0.33;
      constraints.gridwidth = 2;
      constraints.fill = GridBagConstraints.HORIZONTAL;
      mainPanel.add(m_color[i], constraints);

      Button selectButton = new Button("Select");
      selectButton.addActionListener(new SelectActionHandler(i));
      mainPanel.add(selectButton, constraints);
    }
    Button bCancel = new Button("Cancel");
    bCancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        ConfigureColor.this.dispose();
      }
    });
    Panel buttonPanel = new Panel();
    buttonPanel.add(bCancel);
     
    add(mainPanel, "Center");
    add(buttonPanel, "South");
  }
  private class SelectActionHandler implements ActionListener {
    private int m_index;
    public SelectActionHandler(int i) {
      m_index = i;
    }
    public void actionPerformed(ActionEvent e) {
      dlg.setColor(m_color[m_index].getBackground());
      dlg.doModal();
      m_color[m_index].setBackground(dlg.getColor());
    }
  }
  /* Public methods */
  public void doModal() {
    setModal(true);
    show();
  }
  public Color getColor(int index) {
    return m_color[index].getBackground();
  }
}
