#include <String.h>
#include <iostream.h>
#include <strstream.h>
#include "exception.h"

#include "Utility.h"

String stripQuotes(char* str){
	strstream tmp;
	tmp << str << ends;
	return stripQuotes(tmp);
}

String stripQuotes(istream& in){	// can throw excetion

	// strips backslashes from the string (and outer quotes)

	String value;
	char tmp;
	in >> tmp;
	if(!in){
		return "";
	}
	if(tmp != '"'){
		THROW(new Exception("Leading \" expected"), "");
	}
	bool escape = false;
	while(1){
		in.get(tmp);
		if(!in){
			String e = "Closing \" expected";
			THROW(new Exception(e), "");
		}
		if(escape){
			escape = false;
			switch(tmp){
			case '\\':
				value += '\\';
				break;
			case '"':
				value += '"';
				break;
			default:
				assert(0);
			}
		}
		else if(tmp == '\\'){
			escape = true;
		}
		else if(tmp == '"'){
			break;
		}
		else{
			value += tmp;
		}
	}
	return value;
}

String addQuotes(String in){	// can throw excetion

	// adds backslashes from the string (and outer quotes)

	String value;
	char tmp;
	value += '"';
	for(unsigned int i = 0; i < in.length(); i++){
		tmp = in[i];
		switch(tmp){
		case '\\':
			value += "\\\\";
			break;
		case '"':
			value += "\\\"";
			break;
		default:
			value += tmp;
		}
	}
	value += '"';
	return value;
}
