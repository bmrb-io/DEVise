#ifndef UTILITY_H
#define UTILITY_H

#include<String.h>
#include<iostream.h>

String stripQuotes(char* str);	// can throw excetion

String stripQuotes(istream& in);	// can throw excetion

String addQuotes(String str);

#endif
