// CatalogRefresher.java

// DTE Window Project

// Yunrui Li
// CS Dept. UW-Madison
// Copyright (c) 1999

// An instance of a CatalogRefresher may change catalogs.
public interface CatalogRefresher
{
  // Home table change handler.
  public void updateHome ();

  // General catalog table change handler.
  public void updateTable (String table);
}
