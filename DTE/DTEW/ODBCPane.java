// ODBCPane.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

// The pane where user can exeucte ODBC related commands.
public class ODBCPane extends UpperComponent
{
  // The main panel.
  protected Panel mainPanel = new Panel ();

  // Component constants.
  private static final String sCreateDSN = "Create ODBC DSN:  ";
  private static final String sCreateTable = "Create ODBC Table:";
  private static final String sShowDSN = "Show ODBC DSNs:   ";
  private static final String sShowTable = "Show ODBC Tables: ";
  private static final String sDeleteDSN = "Delete ODBC DSN:  ";
  private static final String sDeleteTable = "Delete ODBC Table:";
  
  private static final String sDSNName = "DSN Name:";
  private static final String sConnect = "Connect String:";
  private static final String sDTETable = "DTE Table Name:";
  private static final String sTable = "ODBC Table Name:";
  private static final String sUnder = "Under DSN:";

  private static final String comCreate = "Create";
  private static final String comShow = "Show";
  private static final String comDelete = "Delete";
  
  // The different command panels.
  private TitledInputPanel pCreateDSN;
  private TitledInputPanel pCreateTable;
  private TitledInputPanel pShowDSN;
  private TitledInputPanel pShowTable;
  private TitledInputPanel pDeleteDSN;
  private TitledInputPanel pDeleteTable;

  // Constructor.
  public ODBCPane ()
    {
      // Create command panels and add them into the main panel.
      pCreateDSN = new TitledInputPanel (sCreateDSN, comCreate, this, sDSNName, sConnect);
      pCreateTable = new TitledInputPanel (sCreateTable, comCreate, this, sDTETable, sDSNName, sTable);
      pShowDSN = new TitledInputPanel (sShowDSN, comShow, this);
      pShowTable = new TitledInputPanel (sShowTable, comShow, this, sUnder);
      pDeleteDSN = new TitledInputPanel (sDeleteDSN, comDelete, this, sDSNName);
      pDeleteTable = new TitledInputPanel (sDeleteTable, comDelete, this, sTable);

      mainPanel.setLayout (new GridLayout (0, 1));
      mainPanel.add (pCreateDSN);
      mainPanel.add (pCreateTable);
      mainPanel.add (pShowDSN);
      mainPanel.add (pShowTable);
      mainPanel.add (pDeleteDSN);
      mainPanel.add (pDeleteTable);
      
      add (mainPanel);
      setScrollPosition (0, 0);
    }
  
  // Action event handler.
  public final void actionPerformed (ActionEvent e)
    {
      Object o = e.getSource ();

      if (o == pCreateDSN)
	createDSN ();
      else if (o == pCreateTable)
	createTable ();
      else if (o == pShowDSN)
	showDSN ();
      else if (o == pShowTable)
	showTable ();
      else if (o == pDeleteDSN)
	deleteDSN ();
      else if (o == pDeleteTable)
	deleteTable ();
    }
  
  // Create a DSN.
  private final void createDSN ()
    {
      String name = pCreateDSN.getText (0).trim ();
      String connect = pCreateDSN.getText (1).trim ();
	  
      String q = "Create ODBCDSN " + name + " , '" + connect + "' ;";
      getEnvironment ().runQuery (q, this);
   }
  
  // Create an ODBC Table.
  private final void createTable ()
    {
      String dte = pCreateTable.getText (0).trim ();
      String dsn = pCreateTable.getText (1).trim ();
      String table = pCreateTable.getText (2).trim ();
	  
      String q = "Create ODBCTable " + dte + " , " + dsn + " , " + table + " ;";
      getEnvironment ().runQuery (q, this);
      updateHome ();
    }
  
  // Show all DSN entries.
  private final void showDSN ()
    {
      getEnvironment ().runQuery ("DSNEntries;", this);
    }

  // Show all tables under a DSN entry.
  private final void showTable ()
    {
      String name = pShowTable.getText (0).trim ();
	  
      String q = "ODBCTables " + name + " ; ";
      getEnvironment ().runQuery (q, this);
    }
  
  // Delete a DSN.
  private final void deleteDSN ()
    {
      String name = pDeleteDSN.getText (0).trim ();
	  
      String q = "Delete ODBCDSN " + name + " ; ";
      getEnvironment ().runQuery (q, this);    
    }
  
  // Delete an ODBC table.
  private final void deleteTable ()
    {
      String name = pDeleteTable.getText (0).trim ();
	  
      String q = "Delete ODBCTable " + name + " ; ";
      getEnvironment ().runQuery (q, this);
      updateHome ();
    }
}


