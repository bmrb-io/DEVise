/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class SchemaBrowser.java
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
 * The <CODE>SchemaBrowser</CODE> class
 *
 * @author  Shilpa Lawande
 */

public class SchemaBrowser extends Browser
{
  public SchemaBrowser() {
    super("Schema Browser");
  }
  public void loadList() {
    File f = new File(LMgr.SCHEMA_DIR);
    String schemas[] = f.list();
    for (int i = 0; i < schemas.length; i++) {
      m_list.add(schemas[i]);
    }
  }
  public void onNew() {
    SchemaEditor dlg = new SchemaEditor();
    int choice = dlg.doModal();
    if (choice == 1) {
      return;
    } else {
      m_list.add(dlg.getSchemaName());
    }
  }
  public void onDelete() {
    String schemaToDel = m_selection.getText();
    File f = new File(LMgr.SCHEMA_DIR + schemaToDel);
    if (f != null) {
      f.delete();
    }
    m_list.remove(schemaToDel);
    setSelection("");
  }

  public void onEdit() {
    String schemaToEdit = m_selection.getText();
    if (schemaToEdit.equals("")) {
      return;
    }
    SchemaEditor dlg = new SchemaEditor(schemaToEdit);
    dlg.doModal();
  }
}

class SchemaEditor extends Dialog {
  String m_schemaName;
  TextField m_schemaText;
  TextField m_attribute;
  List m_list;
  boolean m_saved = false;
  int m_choice = 0;
  public SchemaEditor() {
    this(null);
  }
  public SchemaEditor(String schemaName) {
    super(new Frame(), "Schema Editor");
    setSize(300,300);

    m_schemaName = schemaName;

    Panel mainPanel = new Panel(new FlowLayout());

    Panel name = new Panel();
    Label lname = new Label("Name");
    m_schemaText = new TextField();
    m_schemaText.setColumns(20);
    name.add(lname);
    name.add(m_schemaText);

    m_list = new List(10);
    if (schemaName != null) {
      m_schemaText.setEditable(false);
      m_schemaText.setText(schemaName);
      loadSchema(schemaName);
    }

    Panel attribute = new Panel(new FlowLayout());
    Label lattr = new Label("Attribute");
    m_attribute = new TextField("");
    m_attribute.setColumns(20);
    attribute.add(lattr);
    attribute.add(m_attribute);

    Panel buttons = new Panel(new FlowLayout());
    Button bOk = new Button("OK");
    bOk.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        m_choice = 0;
        SchemaEditor.this.saveSchema();
        SchemaEditor.this.dispose();
      }
    });
    Button bCancel = new Button("Cancel");
    bCancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        m_choice = 1;
        SchemaEditor.this.dispose();
      }
    });
    Button bAdd = new Button("Add");
    bAdd.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        int index = m_list.getSelectedIndex();
        m_list.add(m_attribute.getText(), index);
      }
    });
    Button bDelete = new Button("Del");
    bDelete.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        int index = m_list.getSelectedIndex();
        m_list.delItem(index);
      }
    });
    buttons.add(bOk);
    buttons.add(bCancel);
    buttons.add(bAdd);
    buttons.add(bDelete);

    mainPanel.add(name);
    mainPanel.add(m_list);
    mainPanel.add(attribute);
    add(mainPanel, "Center");
    add(buttons, "South");
  }
  public int doModal() { 
    setModal(true);
    show();
    return m_choice; 
  }
  public void loadSchema(String schemaName) {
    DEViseSchema schema = new DEViseSchema(LMgr.SCHEMA_DIR + schemaName);
    String attrList[] = schema.getAttrList();
    for (int i = 0; i < attrList.length; i++) {
      m_list.add(attrList[i]);
    }
  }
  public void saveSchema() {
    m_schemaName = m_schemaText.getText();
    if (m_schemaName == null) {
      LMError.report("No schema name specified");
      return;
    }
    String attrList[] = new String[m_list.getItemCount()];
    for (int i = 0; i < attrList.length; i++) {
      attrList[i] = m_list.getItem(i);
    }
    DEViseSchema schema 
      = new DEViseSchema(m_schemaName, LMgr.SCHEMA_DIR + m_schemaName, attrList);
  }

  public String getSchemaName() {
    return m_schemaText.getText();
  }
}
