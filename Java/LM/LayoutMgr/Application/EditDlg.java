/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class EditDlg.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;

// Class
/**
 * The <CODE>EditDlg</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class EditDlg extends Dialog
{
  DEViseViewPeer m_view;
  DEViseView m_dview;
  TextField m_x,m_y;
  TextField m_width, m_height;
  TextField m_fg,m_bg;
  TextField m_name;
  boolean m_custom;
  ColorSelectDlg dlg = new ColorSelectDlg();
  
  /* Constructor */
  public EditDlg(DEViseViewPeer view) {
    super(new Frame(), "Edit properties for " + view.getName());
    m_view = view;
    m_dview = view.getDEViseView();
    setSize(300,200);

    Panel buttonPanel = new Panel();
    Button bOk = new Button("OK");
    Button bCancel = new Button("Cancel");
    bOk.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onOK();
      }
    });
    bCancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onCancel();
      }
    });
    buttonPanel.add(bOk);
    buttonPanel.add(bCancel);
    Panel mainPanel = new Panel(new GridBagLayout());
    GridBagConstraints constraints = new GridBagConstraints();
    

    System.out.println("...");
    Label name = new Label("Name");
    constraints.gridx = 0;
    constraints.gridy = 0;
    constraints.weightx = 0.2;
    constraints.weighty = 0.33;
    m_name = new TextField(m_dview.getName());
    mainPanel.add(name, constraints);
    constraints.gridx = GridBagConstraints.RELATIVE;
    constraints.gridy = 0;
    constraints.weightx = 0.5;
    constraints.weighty = 0.33;
    constraints.gridwidth = 1;
    mainPanel.add(m_name, constraints);

    System.out.println("...");
    Label bg = new Label("Background");
    constraints.gridx = 0;
    constraints.gridy = 1;
    constraints.weightx = 0.1;
    constraints.weighty = 0.33;
    constraints.gridwidth = 1;
    constraints.fill = GridBagConstraints.NONE;
    mainPanel.add(bg, constraints);
    m_bg = new TextField("   ");
    m_bg.setEditable(false);
    m_bg.setBackground(m_dview.getBgColor().getColor());
    constraints.gridx = GridBagConstraints.RELATIVE;
    constraints.weightx = 0.16;
    constraints.weighty = 0.33;
    constraints.gridwidth = 2;
    constraints.fill = GridBagConstraints.HORIZONTAL;
    mainPanel.add(m_bg, constraints);
    Button selectBgColor = new Button("Select");
    selectBgColor.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        dlg.setColor(m_dview.getBgColor().getColor());
        dlg.doModal();
        m_bg.setBackground(dlg.getColor());
      }
    });
    mainPanel.add(selectBgColor, constraints);
    
    System.out.println("...");
    Label fg = new Label("Foreground");
    constraints.gridx = 0;
    constraints.gridy = 2;
    constraints.gridwidth = 1;
    constraints.weightx = 0.1;
    constraints.weighty = 0.33;
    constraints.fill = GridBagConstraints.NONE;
    mainPanel.add(fg, constraints);
    m_fg = new TextField("   ");
    m_fg.setEditable(false);
    m_fg.setBackground(m_dview.getFgColor().getColor());
    constraints.gridx = GridBagConstraints.RELATIVE;
    constraints.weightx = 0.16;
    constraints.weighty = 0.33;
    constraints.gridwidth = 2;
    constraints.fill = GridBagConstraints.HORIZONTAL;
    mainPanel.add(m_fg, constraints);
    Button selectFgColor = new Button("Select");
    selectFgColor.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        dlg.setColor(m_dview.getFgColor().getColor());
        dlg.doModal();
        m_fg.setBackground(dlg.getColor());
      }
    });
    mainPanel.add(selectFgColor, constraints);
    
    System.out.println("EditDlg...");
    // check if parent has custom layout
    // otherwise position and dimensions cannot be edited
    DEViseLayout parentLayout 
      = ((DEViseViewPeer)m_view.getParent()).getDEViseLayout();
    if (parentLayout instanceof DEViseCustomLayout) {
      m_custom = true;
	    Label x = new Label("X");
	    constraints.gridx = 0;
	    constraints.gridy = 3;
	    constraints.weightx = 0.1;
	    constraints.weighty = 0.33;
	    constraints.gridwidth = 1;
	    mainPanel.add(x, constraints);
	    m_x = new TextField(String.valueOf(m_dview.getX()));
	    constraints.gridx = GridBagConstraints.RELATIVE;
	    constraints.weightx = 0.16;
	    constraints.weighty = 0.33;
	    constraints.gridwidth = 2;
	    constraints.fill = GridBagConstraints.HORIZONTAL;
	    mainPanel.add(m_x, constraints);
	    
	    Label y = new Label("Y");
	    constraints.gridwidth = 1;
	    constraints.weightx = 0.1;
	    constraints.weighty = 0.33;
	    constraints.fill = GridBagConstraints.NONE;
	    mainPanel.add(y, constraints);
	    m_y = new TextField(String.valueOf(m_dview.getY()));
	    constraints.weightx = 0.16;
	    constraints.weighty = 0.33;
	    constraints.gridwidth = 2;
	    constraints.fill = GridBagConstraints.HORIZONTAL;
	    mainPanel.add(m_y, constraints);
	
	    Label width = new Label("Width");
	    constraints.gridx = 0;
	    constraints.gridy = 4;
	    constraints.weightx = 0.1;
	    constraints.weighty = 0.33;
	    constraints.gridwidth = 1;
	    constraints.fill = GridBagConstraints.NONE;
	    mainPanel.add(width, constraints);
	    m_width = new TextField(String.valueOf(m_dview.getWidth()));
	    constraints.gridx = GridBagConstraints.RELATIVE;
	    constraints.weightx = 0.16;
	    constraints.weighty = 0.33;
	    constraints.gridwidth = 2;
	    constraints.fill = GridBagConstraints.HORIZONTAL;
	    mainPanel.add(m_width, constraints);
	    
	    Label height = new Label("Height");
	    constraints.gridwidth = 1;
	    constraints.weightx = 0.1;
	    constraints.weighty = 0.33;
	    constraints.fill = GridBagConstraints.NONE;
	    mainPanel.add(height, constraints);
	    m_height = new TextField(String.valueOf(m_dview.getHeight()));
	    constraints.weightx = 0.16;
	    constraints.weighty = 0.33;
	    constraints.gridwidth = 2;
	    constraints.fill = GridBagConstraints.HORIZONTAL;
	    mainPanel.add(m_height, constraints);
	  } else {
      m_custom = false;
    }
    add(mainPanel, "Center");
    add(buttonPanel, "South");
  }
  /* Public methods */
  void doModal() {
    setModal(true);
    show();
  }
  void onOK() {
    if (m_custom) {
      m_dview.setX(Double.valueOf(m_x.getText()).doubleValue()); 
      m_dview.setY(Double.valueOf(m_y.getText()).doubleValue());
      m_dview.setWidth(Double.valueOf(m_width.getText()).doubleValue());
      m_dview.setHeight(Double.valueOf(m_height.getText()).doubleValue());
    }
    m_dview.setBgColor(new DEViseColor(m_bg.getBackground()));
    m_dview.setFgColor(new DEViseColor(m_fg.getBackground()));

    String name =  m_name.getText();
    if (!name.equals(m_dview.getName())) {
      m_view.rename(name);
      m_dview.setName(name);
    }
    System.out.println(m_dview);
    dispose();
  }
  void onCancel() {
    dispose();
  }
}
