/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class ColorSelectDlg.java
****************************************************************************/

//Packages 

package AwtUtil;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;

// Class
/**
 * The <CODE>ColorSelectDlg</CODE> class
 *
 n* @author  Shilpa Lawande
 */

public class ColorSelectDlg extends Dialog
{
  public static void main(String args[]) {
    ColorSelectDlg dlg = new ColorSelectDlg();
    dlg.doModal();
    System.out.println("color = " + dlg.getColor());
  }

  private int m_red;
  private int m_green;
  private int m_blue;
  private TextField m_color;

  /* Constructor */
  public ColorSelectDlg(Color clr) {
    this(clr.getRed(), clr.getGreen(), clr.getBlue());
  }
  public ColorSelectDlg() {
    this(0,0,0);
  }
  public ColorSelectDlg(int red, int green, int blue) {
    super(new Frame(), "Select Color");
    setSize(300,150);

    Panel buttonPanel = new Panel();
    Button bOk = new Button("OK");
    bOk.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onOK();
      }
    });
    buttonPanel.add(bOk);
    
    GridBagLayout layout = new GridBagLayout();
    Panel mainPanel = new Panel(layout);

    Label redL = new Label("Red");
    Scrollbar redSlider = new Scrollbar(Scrollbar.HORIZONTAL,red, 1, 0,255);
    redSlider.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent e) {
        Scrollbar sb = (Scrollbar)e.getAdjustable();
        m_red = sb.getValue();
        setColor();
      }
    });
    Label greenL = new Label("Green");
    Scrollbar greenSlider = new Scrollbar(Scrollbar.HORIZONTAL,green, 1, 0,255);
    greenSlider.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent e) {
        Scrollbar sb = (Scrollbar)e.getAdjustable();
        m_green = sb.getValue();
        setColor();
      }
    });
    Label blueL = new Label("Blue");
    Scrollbar blueSlider = new Scrollbar(Scrollbar.HORIZONTAL,blue, 1, 0,255);
    blueSlider.addAdjustmentListener(new AdjustmentListener() {
      public void adjustmentValueChanged(AdjustmentEvent e) {
        Scrollbar sb = (Scrollbar)e.getAdjustable();
        m_blue = sb.getValue();
        setColor();
      }
    });

    m_color = new TextField("   ");
    m_color.setEditable(false);
    
    GridBagConstraints constraints = new GridBagConstraints();
    constraints.gridx = 0;
    constraints.gridy = 0;
    constraints.gridwidth = 1;
    constraints.anchor = GridBagConstraints.WEST;
    //constraints.fill = GridBagConstraints.NONE;
    mainPanel.add(redL, constraints);
    constraints.gridy = 1;
    mainPanel.add(greenL, constraints);
    constraints.gridy = 2;
    mainPanel.add(blueL, constraints);
   
    constraints.gridx = 1;
    constraints.gridy = 0;
    constraints.gridwidth = GridBagConstraints.REMAINDER;
    constraints.fill = GridBagConstraints.HORIZONTAL;
    constraints.weightx = 3;

    mainPanel.add(redSlider, constraints);
    constraints.gridy = 1;
    mainPanel.add(greenSlider, constraints);
    constraints.gridy = 2;
    mainPanel.add(blueSlider, constraints);
    
    constraints.gridx = 1;
    constraints.gridy = 3;
    constraints.gridwidth = 1;
    mainPanel.add(m_color, constraints);

    add(buttonPanel, "South");
    add(mainPanel, "Center");
  
    setColor(red, green, blue);
  }
   
  /* Public methods */
  public Color getColor() {
    return new Color(m_red, m_green, m_blue);
  }
  public void setColor(Color clr) {
    setColor(clr.getRed(), clr.getGreen(), clr.getBlue());
  }
  public void setColor(int red, int green, int blue) {
    m_red = red;
    m_green = green;
    m_blue = blue;
    setColor();
  }
  public void setColor() {
    m_color.setBackground(new Color(m_red, m_green, m_blue));
  }
  public void doModal() {
    setModal(true);
    show();
  }

  void onOK() {
    dispose();
  }
}
