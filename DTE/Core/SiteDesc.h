#ifndef SITE_DESC_H
#define SITE_DESC_H

#include <vector>
#include <string>
#include "myopt.h"

class AccessMethod;

class SiteDesc {
public:
	virtual vector<AccessMethod*> 
		getAccessMethods(const TableAlias& table) const = 0;
	virtual string toString() const = 0;
};

class ThisSite : public SiteDesc {
public:
	virtual vector<AccessMethod*> 
		getAccessMethods(const TableAlias& table) const;
	virtual string toString() const {
		return "this site";
	}
};

extern ThisSite THIS_SITE;

#endif
