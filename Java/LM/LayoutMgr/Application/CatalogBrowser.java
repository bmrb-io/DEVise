/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class CatalogBrowser.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import COM.objectspace.jgl.*;
import AwtUtil.*;
import LayoutMgr.MetaData.*;
import Util.*;


// Class
/**
 * The <CODE>CatalogBrowser</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class CatalogBrowser extends Browser
{
  DummyCatalog m_catalog;
  public CatalogBrowser() {
    super("Catalog Browser");
  }
  public void finalize() {
    try {
      m_catalog.saveCatalog();
    } catch(Exception e) {
      LMError.report("Save: " + e);
    }
  }
  public void loadList() {
    m_catalog = DummyCatalog.getCatalog();
    for (Enumeration e = m_catalog.list(); e.hasMoreElements(); ) {
      String tableName = (String)e.nextElement();
      m_list.add(tableName);
    }
  }
  public void onNew() {
    CatalogEditor dlg = new CatalogEditor();
    int choice = dlg.doModal();
    if (choice == 1) {
      return;
    } else {
      try {
        m_catalog.addTable(dlg.getTableName(), dlg.getDataFileName(), 
                          dlg.getSchemaName());
      } catch (Exception e) {
        LMError.report(e.getMessage());
      }
      m_list.add(dlg.getTableName());
    }
  }
  public void onEdit() {
    String tableToEdit = m_selection.getText();
    if (tableToEdit.equals("")) {
      return;
    }
    try {
      DummyCatalog.DummyCatalogEntry e = m_catalog.findTable(tableToEdit);
      CatalogEditor dlg = new CatalogEditor(tableToEdit, 
                                          e.getDataFileName(),
                                          e.getSchemaName());
      int choice = dlg.doModal();
      if (choice == 1) {
        return;
      } else {
        m_catalog.updateTable(tableToEdit, dlg.getDataFileName(), 
                              dlg.getSchemaName());
      }
    } catch (Exception e) {
      LMError.report("Edit: " + e.getMessage());
    }
  }

  public void onDelete() {
    String tableToDel = m_selection.getText();
    try {
      m_catalog.removeTable(tableToDel);
    } catch (Exception e) {
      LMError.report(e.getMessage());
    }
    m_list.remove(tableToDel);
  }
}


class CatalogEditor extends Dialog {
  
  private TextField m_tableNameText;
  private TextField m_dataFileText;
  private TextField m_schemaText;
  private int m_choice = 0;

  public CatalogEditor() {
    this(null, null, null);
  }

  public CatalogEditor(String tableName, String dataFile, String schemaName) {
    super(new Frame(), "Edit table information");
    setSize(200,500);
    
    Panel mainPanel = new Panel(new FlowLayout());

    Panel tableNamePanel = new Panel();
    Label tableNameLabel = new Label("Table Name");
    m_tableNameText = new TextField();
    m_tableNameText.setColumns(20);
    tableNamePanel.add(tableNameLabel);
    tableNamePanel.add(m_tableNameText);
    if (tableName != null) {
      m_tableNameText.setText(tableName);
    }
    Panel dataFilePanel = new Panel();
    Label dataFileLabel = new Label("Data File");
    m_dataFileText = new TextField();
    m_dataFileText.setColumns(30);
    if (dataFile != null) {
      m_dataFileText.setText(dataFile);
    }
    Button dataFileSelect = new Button("Browse");
    dataFileSelect.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        FileDialog dlg = new FileDialog(new Frame(), "Choose data file");
        dlg.setDirectory(".");
        dlg.setModal(true);
        dlg.show();
        m_dataFileText.setText(dlg.getDirectory() + dlg.getFile());
      }
    });
    dataFilePanel.add(dataFileLabel);
    dataFilePanel.add(m_dataFileText);
    dataFilePanel.add(dataFileSelect);

    Panel schemaPanel = new Panel();
    Label schemaLabel = new Label("Data File");
    m_schemaText = new TextField();
    m_schemaText.setColumns(30);
    if (schemaName != null) {
      m_schemaText.setText(schemaName);
    }
    Button schemaSelect = new Button("Browse");
    schemaSelect.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        SchemaBrowser dlg = new SchemaBrowser();
        m_schemaText.setText(dlg.getSelection());
      }
    });
    schemaPanel.add(schemaLabel);
    schemaPanel.add(m_schemaText);
    schemaPanel.add(schemaSelect);

    mainPanel.add(tableNamePanel);
    mainPanel.add(dataFilePanel);
    mainPanel.add(schemaPanel);
    
    Panel buttonPanel = new Panel();
    Button bOk = new Button("OK");
    bOk.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e){
        m_choice = 0;
        CatalogEditor.this.dispose();
      }
    });
    Button bCancel = new Button("Cancel");
    bCancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        m_choice = 1;
        CatalogEditor.this.dispose();
      }
    });
    buttonPanel.add(bOk);
    buttonPanel.add(bCancel);

    add(mainPanel, "Center");
    add(buttonPanel, "South");
  }
  public String getSchemaName() { return m_schemaText.getText(); }
  public String getTableName() { return m_tableNameText.getText(); }
  public String getDataFileName() { return m_dataFileText.getText(); }
  public int doModal() {
    setModal(true);
    show();
    return m_choice;
  }
}
