#ifndef _TABLEALIAS_H_
#define _TABLEALIAS_H_

#include "RelationId.h"
#include "RelationManager.h"

class SiteDesc;
class AccessMethod;


class TableAlias {
protected:
	TableName table;
	string alias;
	Stats* stats;
	ISchema schema;
	vector<AccessMethod*> accessMethods;
	bool isGestaltM;
	bool isRemoteM;
	Interface* interf;

public:
	TableAlias(const TableName& t, const string& a);

        // for use when the table has already been resolved
        TableAlias(Interface* interface, const string& alias);

/*
	TableAlias(string tableNm, string aliasNm){
		table = new TableName(tableNm.c_str());
		alias = new string(aliasNm);
		function = NULL;
		stats = 0;
	}
*/
	bool isGestalt() const {
		return isGestaltM;
	}
	bool isRemote() const {
		return isRemoteM;
	}
	const Interface* getInterface() const {
		return interf;
	}
	virtual ~TableAlias();
	const TableName& getTable() const {
		return table;
	}
	const string& getAlias() const {
		return alias;
	}
	virtual void display(ostream& out, int detail = 0);
        //virtual Site* createSite();
        const ISchema& getISchema() const {
          return schema;
        }
	const Stats* getStats() const {
		assert(stats);
		return stats;
	}
  const vector<AccessMethod*>& getAccessMethods() const {
    return accessMethods;
  }

private:

  void init(Interface* interface);

  TableAlias(const TableAlias& x);
  TableAlias& operator=(const TableAlias& x);
};

class NumberAlias : public TableAlias {
	RelationId relId;
public:
  // There is no table name for number tables, but 
  // the optimizer needs to know table name to find indexes
  NumberAlias(RelationId relId, const string& alias)
    : TableAlias(RELATION_MNGR.createInterface(relId), alias),
      relId(relId) {}
	virtual ~NumberAlias();
	virtual void display(ostream& out, int detail = 0);
  //virtual Site* createSite();
};

//kb: move stuff from resolve.c too

#endif // _TABLEALIAS_H_
