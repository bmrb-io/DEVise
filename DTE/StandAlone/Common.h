#ifndef COMMON_H
#define COMMON_H

#include <string>
#include "sysdep.h"

using namespace std;

string extractQuery(istream& cin);
void processQuery(const string& query, ostream& cout, const string& = ""); // throws

#endif
