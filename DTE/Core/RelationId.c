#include "RelationId.h"

#include "sysdep.h"

string RelationId::toString() const {
	ostringstream oss;
	oss << *this << ends;
	return oss.str();
}

ostream& operator<<(ostream& out, const RelationId& id)
{
	return out << id.serverId << "." << id.localId;
}
