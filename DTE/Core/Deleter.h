#ifndef _DELETER_H_
#define _DELETER_H_

// (C) 1999- by Kevin S. Beyer

#include "types.h"

class ExecExpr;


//kb: class Deleter only works for local ascii StandardTables
//kb: class Deleter should be made abstract, with StandardDeleter derived
class Deleter
{
protected:

  DteTupleAdt tupAdt;
  string filename;

public:

  Deleter(const string& filename, const DteTupleAdt& tupAdt)
    : tupAdt(tupAdt), filename(filename) {}

  ~Deleter() {}

  // 'where' will be deleted by this object
  // if 'where' is NULL, all records are deleted.
  void deleteWhere(ExecExpr* where); // throws
};


#endif // _DELETER_H_
