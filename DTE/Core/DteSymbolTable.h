#ifndef _DTESYMBOLTABLE_H_
#define _DTESYMBOLTABLE_H_

// (C) 1999- by Kevin S. Beyer

#include <map>
#include "types.h"
#include "TableAlias.h"
//#include "DTE/util/Del.h"


class DteSymbolTable
{
  class AttrDesc {
  public:
    int slot;
    DteAdt* adt;
    AttrDesc() : slot(-1), adt(NULL) {}
    AttrDesc(const AttrDesc& f) : slot(f.slot), adt(NULL) {
      if( f.adt ) adt = f.adt->clone();
    }
    ~AttrDesc() { delete adt; }
  private:
    AttrDesc(int slot, const DteAdt& adt) : slot(slot), adt(adt.clone()) {}
    AttrDesc& operator=(const AttrDesc& f) {
      slot = f.slot;
      if( f.adt ) {
        adt = f.adt->clone();
      } else {
        adt = NULL;
      }
      return *this;
    }
  };

  typedef map<string, AttrDesc> AttrMap;

  class TableDesc {
  public:
    int tabId;
    AttrMap attrMap;
    TableDesc() : tabId(-1), attrMap() {}
    TableDesc(const TableDesc& x) : tabId(x.tabId), attrMap(x.attrMap) {}
    ~TableDesc() {}
  private:
    TableDesc& operator=(const TableDesc& x) {
      tabId = x.tabId;
      attrMap = x.attrMap;
      return *this;
    }
  };

  typedef map<string, TableDesc> TableMap;

  TableMap tableMap;

public:

  DteSymbolTable() {}

  DteSymbolTable(const vector<TableAlias*>& tabs) {
    int N = tabs.size();
    for(int i = 0 ; i < N ; i++) {
      const TableAlias& ta = *(tabs[i]);
      const ISchema& schema = ta.getISchema();
      const vector<string>& attrs = schema.getAttributeNames();
      const DteTupleAdt& tupAdt = schema.getAdt();
      TableDesc& td = tableMap[ta.getAlias()];
      assert(td.tabId < 0 && "duplicate alias in from clause!");
      td.tabId = i;
      int M = schema.getNumFields();
      for(int j = 0 ; j < M ; j++) {
        AttrDesc& ad = td.attrMap[attrs[j]];
        assert(ad.slot < 0 && "duplicate field name in table!");
        ad.slot = j;
        ad.adt = tupAdt.getAdt(j).clone();
      }
    }
  }

  ~DteSymbolTable() {}

  bool findAttr(const string& attrName,
                int& tabId, int& slot, DteAdt*& adt) const {
    TableMap::const_iterator foundTab = tableMap.end();
    AttrMap::const_iterator foundAttr;
    TableMap::const_iterator ti;
    for(ti = tableMap.begin() ; ti != tableMap.end() ; ti++) {
      const AttrMap& am = ti->second.attrMap;
      AttrMap::const_iterator ai = am.find(attrName);
      if( ai != am.end() ) {
        if( foundTab != tableMap.end() ) {
          cerr << "attribute " << attrName << " is ambiguous\n";
          //THROW?
          return false;
        }
        // found first instance
        foundTab = ti;
        foundAttr = ai;
      }
    }
    if( foundTab == tableMap.end() ) {
      cerr << "attribute " << attrName << " not found in any table\n";
      //THROW?
      return false;
    }
    tabId = foundTab->second.tabId;
    slot = foundAttr->second.slot;
    adt = foundAttr->second.adt->clone();
    return true;
  }

  // adt must be deleted by caller
  bool findAttr(const string& tableAlias, const string& attrName,
                int& tabId, int& slot, DteAdt*& adt) const {
    if( tableAlias == "" ) {
      return findAttr(attrName, tabId, slot, adt);
    }
    TableMap::const_iterator ti = tableMap.find(tableAlias);
    if( ti == tableMap.end() ) {
      cerr << "table " << tableAlias << " not found\n";
      //THROW?
      return false;
    }
    const AttrMap& am = ti->second.attrMap;
    AttrMap::const_iterator ai = am.find(attrName);
    if( ai == am.end() ) {
      cerr << "attribute " << attrName << " not found in table "
           << tableAlias << endl;
      //THROW?
      return false;
    }
    tabId = ti->second.tabId;
    slot = ai->second.slot;
    adt = ai->second.adt->clone();
    return true;
  }

private:
  DteSymbolTable(const DteSymbolTable& x);
  DteSymbolTable& operator=(const DteSymbolTable& x);
};

#endif // _DTESYMBOLTABLE_H_
