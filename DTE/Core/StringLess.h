#ifndef STRING_LESS_H
#define STRING_LESS_H

#include <string>

struct StringLess {
	bool operator()(const string& s1, const string& s2) const {
		return s1 < s2;
	}
};

#endif
