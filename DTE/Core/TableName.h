#ifndef _TABLENAME_H_
#define _TABLENAME_H_

#include <vector>
#include <string>
#include "types.h"
#include "DTE/util/DteAlgo.h"


class TableName
{
  vector<string> tableName; //kb: should probably use a list<>
  //vector<string>::const_iterator pathIter;

public:

  TableName() {}

  TableName(const vector<string>& tableName) : tableName(tableName) {}

  TableName(const vector<string>* tname) : tableName(*tname) {
    delete tname;
  }

  TableName(const TableName& arg) : tableName(arg.tableName) {}

  TableName(const string& str) {
    tableName.push_back(str);
  }

  TableName(const char* path) {
    assert(*path == '.');
    const char* end = path;
    while( *end != '\0' ) {
      end++;                    // *end must be a '.' before this
      const char* start = end;
      while( *end != '\0' && *end != '.' ) {
        end++;
      }
      tableName.push_back( string(start, end - start) );
    }
  }

  TableName& operator=(const TableName& t) {
    tableName = t.tableName;
    return *this;
  }

  TableName dirName() {
    vector<string>::const_iterator a = tableName.begin();
    vector<string>::const_iterator b = tableName.end();
    if(a != b) b--;
    return TableName(vector<string>(a, b));
  }

  string fileName() {
    if( !tableName.empty() ) {
      return *(tableName.rbegin());
    }
    return string();
  }

  ~TableName() {}

  bool isEmpty() const { return tableName.empty(); }

  const string& getFirst() const {
    assert( !isEmpty() );
    return *(tableName.begin());
  }

  TableName getRemainingPath() const {
    vector<string>::const_iterator a = tableName.begin();
    vector<string>::const_iterator b = tableName.end();
    if(a != b) a++;
    return TableName(vector<string>(a, b));
  }

  void display(ostream& out) const {
    out << ".";
    print_list(out, tableName, ".");
  }

  string toString() const {
    ostringstream tmp;
    display(tmp);
    return tmp.str();
  }

  const vector<string>& returnAsList () {
    return tableName;
  }
};

//kb: move stuff from resolve.c too

#endif // _TABLENAME_H_
