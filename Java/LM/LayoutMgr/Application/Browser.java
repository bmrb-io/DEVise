/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class Browser.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;
import Util.*;


// Class
/**
 * The <CODE>Browser</CODE> class
 *
 * @author  Shilpa Lawande
 */

public abstract class Browser extends Dialog
{
  List m_list;
  TextField m_selection;

  public Browser(String title) {
    super(new Frame(), title);
    setSize(300,300);
    Panel mainPanel = createMainPanel();
    Panel buttonPanel = createButtonPanel();
  
    add(mainPanel, "Center");
    add(buttonPanel, "South");

    setModal(true);
    show();
  }

  public Panel createMainPanel() {
    Panel mainPanel = new Panel(new BorderLayout());

    Panel selection = new Panel(new FlowLayout());
    Label l = new Label("Selection:");
    m_selection = new TextField();
    m_selection.setColumns(20);
    selection.add(l);
    selection.add(m_selection);

    m_list = new List();
    m_list.addItemListener(new ItemListener() {
      public void itemStateChanged(ItemEvent e) {
        if (e.getStateChange() == ItemEvent.SELECTED) {
          String str = m_list.getSelectedItem();
          setSelection(str);
        }
      }
    });
    m_list.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onEdit();
      }
    });

    loadList();
    mainPanel.add(selection, "South"); 
    mainPanel.add(m_list, "Center");
    return mainPanel;
  }
  public Panel createButtonPanel() {
    Panel buttonPanel = new Panel(new FlowLayout());
    Button bOK = new Button("OK");
    bOK.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onOK();
      }
    });
    Button bNew = new Button("New");
    bNew.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onNew();
      }
    });
    Button bDelete = new Button("Del");
    bDelete.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onDelete();
      }
    });
    Button bEdit = new Button("Edit");
    bEdit.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        onEdit();
      }
    });
    buttonPanel.add(bOK);
    buttonPanel.add(bNew);
    buttonPanel.add(bDelete);
    buttonPanel.add(bEdit);
    return buttonPanel;
  }
  public Insets getInsets() {
    return new Insets(30, 15, 15, 15);
  }
  public abstract void loadList();
  public void onOK() {
    dispose();
  }
  public abstract void onNew();
  public abstract void onDelete();
  public abstract void onEdit();

  public void setSelection(String str) {
    m_selection.setText(str);
  }
  public String getSelection() {
    return m_selection.getText();
  }
}

