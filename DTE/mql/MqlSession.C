// (C) 1999- by Kevin S. Beyer

#include "DTE/mql/MqlSession.h"


inline string itos(int x)       // kb: move me
{
  char buf[30];
  sprintf(buf, "%d", x);
  return buf;
}

ofstream* commandOut; //kb: move me


MqlSession::MqlSession()
{
}


MqlSession::~MqlSession()
{
	clear();
}


inline void MqlSession::sendInt(int4 x)
{
	x = htonl(x);
	commandOut->write(&x, sizeof(int4));
}


void MqlSession::run(const vector<int4>& qids)
{
	vector<int4>::const_iterator i;
	for(i = qids.begin() ; i != qids.end() ; i++) {
		int4 qid = *i;
		if( queryMap.find(qid) == queryMap.end() ) {
			throw MqlError("unknown query: " + itos(qid));
		}
	}
	for(i = qids.begin() ; i != qids.end() ; i++) {
		int4 qid = *i;
		Iterator* iter = queryMap[qid];
		assert(iter);
		const DteTupleAdt& adt = iter->getAdt();
		PageBuf pageBuf;
		DteAdtPage page(adt);
		page.initPage(&pageBuf);
		//kb: need to fork threads here!
		//cout << "\n\nquery: " << qid << "\n";
		const Tuple* tup = iter->getFirst();
		while( tup != NULL ) {
			if( page.addTuple(tup) == NULL ) {
				sendPage(qid, page);
				page.reinitPage();
				tup = page.addTuple(tup);
				assert(tup != NULL && "tuple didn't fit on empty page!");
			}
			//adt.print(cout, tup);
			//cout << '\n';
			tup = iter->getNext();
		}
		if( page.getNumSlots() > 0 ) {
			sendPage(qid, page);
		}
		sendInt(QUERY_COMPLETE_OP);
		sendInt(qid);
		sendInt(STATUS_OK);
	}
}


void MqlSession::declareVariable(const string& type, const string& varName)
{
	VarMap::iterator i = varMap.find(varName);
	if( i != varMap.end() ) {
		throw MqlError("variable exists: " + varName);
	}
	DteAdt* adt = DteAdt::createAdt(type);
	if( !adt ) {
		throw MqlError("unknown type: " + type);
	}
	OptConstant* c = new OptConstant(adt, NULL);
	varMap[varName] = c;
}


void MqlSession::declareQuery(int qid, QueryTree* query)
{
	QueryMap::iterator i = queryMap.find(qid);
	if( i != queryMap.end() ) {
		throw MqlError("query exists: " + itos(qid));
	}
	const ISchema* s = query->getISchema();
	if( s == NULL ) {
		throw MqlError("query failed typecheck: " + itos(qid));
	}
	Iterator* iter = query->createExec();
	queryMap[qid] = iter;
	//kb: improve error handling
	int N = s->getNumFields();
	sendInt(SCHEMA_OP);
	sendInt(N);
	const vector<string>& attrNames = s->getAttributeNames();
	const DteTupleAdt& tupAdt = s->getAdt();
	for(int i = 0 ; i < N ; i++) {
		const DteAdt& adt = tupAdt.getAdt(i);
		int4 typeId = adt.getTypeID();
		sendInt(typeId);
		//kb: what about type args (e.g., string length)?
		sendString(attrNames[i]);
	}
}


void MqlSession::setVariable(const string& varName, OptExpr* optExpr)
{
	assert(optExpr);
	Del<OptExpr> delOptExpr = optExpr;
	VarMap::iterator i = varMap.find(varName);
	if( i == varMap.end() ) {
		throw MqlError("unknown variable: " + varName);
	}
	OptConstant* c = i->second;
	assert(c);
	const DteAdt& adt = c->getAdt();
	
	bool ok = optExpr->typeCheck(DteSymbolTable());
	if( !ok ) {
		throw MqlError("expression failed typecheck: " + varName);
	}

	const DteAdt& eadt = optExpr->getAdt();
	if( !ExecExpr::canPromote(eadt, adt) ) {
		throw MqlError("expression type does not match variable: $" +
									 varName + ":" + adt.getTypeSpec() + 
									 ",  expr:" + eadt.getTypeSpec());
	}
	
	Del<ExecExpr> expr1 = optExpr->createExec(OptExprListList());
	assert(expr1);
	Del<ExecExpr> expr = ExecExpr::getPromotion(eadt, adt, expr1.steal());
	assert(expr);
	const Type* value = expr->eval(NULL, NULL);
	assert(value);
	c->setValue(adt.allocateCopy(value));
}


OptConstant* MqlSession::findVariable(const string& varName)
{
	VarMap::iterator i = varMap.find(varName);
	if( i == varMap.end() ) {
		return NULL;
	}
	assert(i->second);
	return i->second;
}


void MqlSession::dropVariable(const string& varName)
{
	VarMap::iterator i = varMap.find(varName);
	if( i == varMap.end() ) {
		throw MqlError("variable not defined: " + varName);
	}
	delete i->second;
	varMap.erase(i);
}


void MqlSession::dropQuery(int qid)
{
	QueryMap::iterator i = queryMap.find(qid);
	if( i == queryMap.end() ) {
		throw MqlError("query not defined: " + itos(qid));
	}
	delete i->second;
	queryMap.erase(i);
}


void MqlSession::dropAllVariables()
{
	for(VarMap::iterator i = varMap.begin() ; i != varMap.end() ; i++) {
		delete i->second;
		i->second = NULL;         // just to be safe
	}
	varMap.clear();
}


void MqlSession::dropAllQueries()
{
	for(QueryMap::iterator i = queryMap.begin() ; i != queryMap.end() ; i++) {
		delete i->second;
		i->second = NULL;         // just to be safe
	}
	queryMap.clear();
}


void MqlSession::clear()
{
	dropAllQueries();
	dropAllVariables();
}


void MqlSession::sendString(const string& x)
{
	const char* cp = x.c_str();
	int4 len = x.length();
	sendInt(len);
	for(int i = 0 ; i < len ; i++) {
		commandOut->put('\0');
		commandOut->put(cp[i]);
	}
}


void MqlSession::sendPage(int4 queryId, DteAdtPage& page)
{
	extern ofstream* commandOut;
	int N = page.getNumSlots();
	cerr << "sending " << N << " tuples to query " << queryId << endl;
	sendInt(RESULT_BLOCK_OP);
	sendInt(queryId);
	sendInt(N);
	page.toNetPage(*commandOut);
	commandOut->flush();
}


//MqlSession mqlSession;
