#ifndef _MQLSESSION_H_
#define _MQLSESSION_H_

// (C) 1999- by Kevin S. Beyer

#include <map>
#include "Iterator.h"
#include "DteSymbolTable.h"
#include "ExecExpr.h"
#include "myopt.h"
#include "ParseTree.h"
#include "DTE/util/Del.h"
#include "DTE/mql/MqlError.h"
#include "DTE/comm/DteAdtPage.h"


class MqlSession
{
public:

  typedef map<string, OptConstant*> VarMap;

  // warning: this type will likely change in the near future
  typedef map<int, Iterator*> QueryMap;

	enum OpCode {
		COMMAND_COMPLETE_OP = 1,
		QUERY_COMPLETE_OP = 2,
		RESULT_BLOCK_OP = 3,
		SCHEMA_OP = 4
	};

	enum StatusCode {
		STATUS_OK = 0,
		STATUS_ERROR = 1,
	};

  MqlSession();

  ~MqlSession();

  void run(const vector<int4>& qids);

  void declareVariable(const string& type, const string& varName);

  void declareQuery(int qid, QueryTree* query);

  void setVariable(const string& varName, OptExpr* optExpr);

  OptConstant* findVariable(const string& varName);

  void dropVariable(const string& varName);

  void dropQuery(int qid);

  void dropAllVariables();

  void dropAllQueries();

  void clear();

protected:

  VarMap varMap;

  QueryMap queryMap;

	static void sendInt(int4 x);

	static void sendString(const string& x);

	static void sendPage(int4 queryId, DteAdtPage& page);

private:

  MqlSession(const MqlSession& x);
  MqlSession& operator=(const MqlSession& x);
};


extern MqlSession mqlSession;


#endif // _MQLSESSION_H_
