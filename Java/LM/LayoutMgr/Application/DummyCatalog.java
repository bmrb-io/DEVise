/****************************************************************************
Source Code for DEVise Layout Manager
Copyright(c) 1997 DEVise Group, University of Wisconsin-Madison.
Author : Shilpa Lawande
class DummyCatalog.java
****************************************************************************/

//Packages 

package LayoutMgr.Application;

import java.lang.*;
import java.io.*;
import java.util.*;
import COM.objectspace.jgl.*;
import Util.*;

// Class
/**
 * The <CODE>DummyCatalog</CODE> class
 *
 * @author  Shilpa Lawande
 */

// A dummy catalog to replace the dte server
public class DummyCatalog implements AsciiSerializable
{
  private Hashtable m_catalog; // list of tables
  private String m_catFile; // catalog file on disk
  private static DummyCatalog theCatalog = null;

  public static DummyCatalog getCatalog() {
    if (theCatalog == null) {
      try {
        theCatalog = new DummyCatalog(LMgr.CATALOG_FILE);
        theCatalog.loadCatalog();
      } catch (DummyCatalogException e) {
        LMError.report(e.getMessage());
      }
    } 
    return theCatalog;
  }

  /* Constructor */
  public DummyCatalog(String catFile) {
    m_catFile = catFile;
  }
  /* Public methods */
  public void addTable(String tableName, String dataFileName, String schema) 
              throws DummyCatalogException {
    if (m_catalog == null) {
      loadCatalog();
    }
    DummyCatalogEntry entry 
      = new DummyCatalogEntry(tableName, dataFileName, schema);
    if (m_catalog.get(tableName) == null) {
      m_catalog.put(tableName, entry);
    } else {
      throw new DummyCatalogException("Attempt to add duplicate entry");
    }
  }
  public void removeTable(String tableName) 
              throws DummyCatalogException {
    if (m_catalog == null) {
      loadCatalog();
    }
    if (m_catalog.remove(tableName) == null) {
      throw new DummyCatalogException("Attempt to delete non-existent table"); 
    }
  }
  public DummyCatalogEntry findTable(String tableName) 
         throws DummyCatalogException {
    if (m_catalog == null) {
      loadCatalog();
    }
    DummyCatalogEntry e = (DummyCatalogEntry)m_catalog.get(tableName);
    if (e == null) {
      throw new DummyCatalogException("Attempt to update non-existent table");
    }
    return e;
  }
  public void updateTable(String tableName, String dataFileName, String schema) 
              throws DummyCatalogException {
    DummyCatalogEntry e = findTable(tableName);
    e.update(tableName, dataFileName, schema);
  }
  public Enumeration list() {
    return new Enumeration() {
      private Enumeration e = m_catalog.keys();
      public boolean hasMoreElements() {
        return e.hasMoreElements();
      }
      public Object nextElement() {
        return e.nextElement(); 
      }
    };
  }
  public void loadCatalog() throws DummyCatalogException {
    m_catalog = new Hashtable();
    try {
      FileInputStream fs = new FileInputStream(m_catFile);
      ReadObjectInput rs = new ReadObjectInput(fs);
      readAscii(rs);
    } catch(IOException e) {
      throw new DummyCatalogException("Error in loading catalog : IOException"); 
    }
  }
  public void saveCatalog() throws DummyCatalogException {
    try {
      FileOutputStream fs = new FileOutputStream(m_catFile);
      PrintObjectOutput ps = new PrintObjectOutput(fs);
      writeAscii(ps);
      ps.close();
    } catch (IOException e) {
      throw new DummyCatalogException("Error in saving catalog " + m_catFile);
    }
  }
  public void readAscii(ReadObjectInput stream) throws IOException {
    int numEntries = stream.readInt();
    System.out.println("numEntries = " + numEntries);
    for (int i = 0; i < numEntries; i++) {
      String tableName = stream.readUTF();
      String dataFileName = stream.readUTF();
      String schema = stream.readUTF();
      System.out.println(tableName + " " + dataFileName + " " + schema);
       
      try { 
        addTable(tableName, dataFileName, schema);
      } catch(DummyCatalogException e) {
        System.out.println("Duplicate entry in catalog...ignoring..!!");
      }
    }
  }
  public void writeAscii(PrintObjectOutput stream) throws IOException {
    if (m_catalog == null) {
      return;
    }
    int numEntries  = m_catalog.size();
    stream.writeChars(numEntries + "\n");
    for (Enumeration e = list(); e.hasMoreElements(); ) {
      String tableName = (String)e.nextElement();
      try {
        DummyCatalogEntry entry = findTable(tableName);
        stream.writeChars(entry.getTableName() + " " + 
                          entry.getDataFileName() + " " +
                          entry.getSchemaName() + "\n");
      } catch (DummyCatalogException x) {
        System.out.println("Catalog writeAscii : NonExistent table??!!");
      }
    }
  }
  
  public class DummyCatalogEntry {
    private String m_tableName;
    private String m_dataFileName;
    private String m_schemaName;

    public DummyCatalogEntry(String tableName, String dataFileName, 
                             String schemaName) {
      update(tableName, dataFileName, schemaName);
    }
    public String getTableName() { return m_tableName; }
    public String getDataFileName() { return m_dataFileName; }
    public String getSchemaName() { return m_schemaName; }

    public void update(String tableName, String dataFileName, String schemaName){
      m_tableName = tableName;
      m_dataFileName = dataFileName;
      m_schemaName = schemaName;
    }
  }

  public class DummyCatalogException extends Exception {
    public DummyCatalogException() { super(); }
    public DummyCatalogException(String msg) { super(msg); }
  }
}
