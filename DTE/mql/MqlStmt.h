#ifndef _MQLSTMT_H_
#define _MQLSTMT_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/mql/MqlSession.h"
#include "ExecOp.h"

class SimpleStmt : public ParseTree
{
public:

  const ISchema* getISchema() {
    return &STRING_SCHEMA;
  }

  virtual Iterator* createExec() {
    try {
      runStmt();
    } catch(MqlError e) {
      return new SingleAnswerIt("error: " + e.getMessage());
    }
    return new SingleAnswerIt("ok");
  }

  virtual void runStmt() = 0;
};


class DeclareVariableStmt : public SimpleStmt
{
  string type;
  string name;

public:

  DeclareVariableStmt(const string& type, const string& name)
    : type(type), name(name) {}

  ~DeclareVariableStmt() {}

  virtual void runStmt() {
    mqlSession.declareVariable(type, name);
  }
};


class DeclareQueryStmt : public SimpleStmt
{
  int qid;
  QueryTree* query;

public:

  DeclareQueryStmt(int qid, QueryTree* query)
    : qid(qid), query(query) {}

  ~DeclareQueryStmt() {}

  virtual void runStmt() {
    mqlSession.declareQuery(qid, query);
  }
};


class SetVariableStmt : public SimpleStmt
{
  string name;
  OptExpr* expr;

public:

  SetVariableStmt(const string& varName, OptExpr* expr)
    : name(varName), expr(expr) {}

  ~SetVariableStmt() {}

  virtual void runStmt() {
    mqlSession.setVariable(name, expr);
  }
};


class RunStmt : public SimpleStmt
{
  vector<int> qids;

public:

  RunStmt(const vector<int>& qids)
    : qids(qids) {}

  ~RunStmt() {}

  virtual void runStmt() {
    mqlSession.run(qids);
  }
};


class DropVariableStmt : public SimpleStmt
{
  string varName;

public:

  DropVariableStmt(const string& varName)
    : varName(varName) {}

  ~DropVariableStmt() {}

  virtual void runStmt() {
    mqlSession.dropVariable(varName);
  }
};


class DropQueryStmt : public SimpleStmt
{
  int qid;

public:

  DropQueryStmt(int qid)
    : qid(qid) {}

  ~DropQueryStmt() {}

  virtual void runStmt() {
    mqlSession.dropQuery(qid);
  }
};


#endif // _MQLSTMT_H_
