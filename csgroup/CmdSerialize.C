/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1998/04/14 19:36:11  taodb
  Fixed-size header version of CmdSerialize and Logging

  Revision 1.2  1998/04/12 00:22:20  taodb
  Added command logging facility

  Revision 1.1  1998/04/10 23:20:31  taodb
  Intial Revision

 */

#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include "CmdSerialize.h"

// a serializable type is
// (i) an atomic type (typeId, length, value)
// (ii) a composite type (typeId = TYP_COMPOSITE, length, #subtypes)

// format:
// +-----------+----------+------.........----------------------------------+
// | varsize  #| varsize #| 16 bytes+ specified by 2nd field*               |
// +-----------+----------+------.........----------------------------------+
// * iff it is a composite type, the 3rd field will be 16 types more
// Serializable::INT_SIZE =16

const char* Serializable::intMarker ="#";
Serializable::Serializable()
{
}

string
Serializable::raw_serialize(int arg)
{
	string	tempStr;
	char buf[Serializable::INT_SIZE+1];
	sprintf(buf, "%-d%s", arg, intMarker);
	tempStr = string(buf);
	return tempStr;
}

string
Serializable::serialize(int arg)
{
	string typeStr= raw_serialize(Serializable::TYP_INT);
	char	buf[128];
	sprintf(buf,"%d", arg);
	string valStr = string(buf);
	string lenStr = raw_serialize(valStr.length());
	return typeStr + lenStr + valStr;
}

string
Serializable::serialize(long arg)
{
	string typeStr= raw_serialize(Serializable::TYP_LONG);
	char	buf[128];
	sprintf(buf,"%ld", arg);
	string valStr = string(buf);
	string lenStr = raw_serialize(valStr.length());
	return typeStr + lenStr + valStr;
}

string
Serializable::serialize(bool arg)
{
	if (!(arg == 0) && !(arg ==1))
		cerr << "It is not a bool type "<<endl;

	string typeStr= raw_serialize(Serializable::TYP_BOOL);
	char	buf[32];
	sprintf(buf,"%d", arg);
	string valStr = string(buf);
	string lenStr = raw_serialize(valStr.length());
	return typeStr + lenStr + valStr;
}

string
Serializable::serialize(char arg)
{
	string typeStr= raw_serialize(Serializable::TYP_CHAR);
	char	buf[2];
	sprintf(buf,"%c", arg);
	string valStr = string(buf);
	string lenStr = raw_serialize(valStr.length());
	return typeStr + lenStr + valStr;
}

string
Serializable::serialize(string arg)
{
	string typeStr= raw_serialize(Serializable::TYP_STRING);
	string valStr = arg;
	string lenStr = raw_serialize(valStr.length());
	return typeStr + lenStr + valStr;
}

string
Serializable::serialize(float arg)
{
	string typeStr= raw_serialize(Serializable::TYP_FLOAT);
	char buf[128];
	sprintf(buf, "%f", arg);
	string valStr = string(buf);
	string lenStr = raw_serialize(valStr.length());
	return typeStr + lenStr + valStr;
}

string 
Serializable::composite_serialize(int args, ...)
{
	va_list	pvar;
	int		i;
	string	body;

	va_start(pvar, args);
	for (i=0; i< args; ++i)
	{
		body += *((string*)va_arg(pvar, void*));
	}
	va_end(pvar);
	return	composite_serialize(args, body);
}

string 
Serializable::composite_serialize(int argc, string body)
{
	string emptyStr;
	if (argc == 0)
		return emptyStr;

	string typeStr= raw_serialize(Serializable::TYP_COMPOSITE);
	string	argsStr = raw_serialize(argc);
	string lenStr = raw_serialize(argsStr.length()+body.length()); 
	return typeStr + lenStr + argsStr + body;
}

void
Serializable::check(int posend)
{
	if (posend <0)
		cerr << "Error in parsing serialized object "<<endl;
}

int 
Serializable::composite_deserialize(string body, vector<string>& vec)
{
	int pos = 0;
	int	pos_end= 0;

	// extract # of arguments
	pos_end = body.find(string(intMarker), pos);
	check(pos_end);
	int	args = atoi(body.substr(pos, pos_end - pos).c_str());

	pos = pos_end + strlen(intMarker);
	int	i;

	for (i=0; i< args ; ++i) 
	{
		string	str;
		int		headPos;
		int		typeIdLen;
		int		lengthLen;

		// move pos to the length field, skip the typeId field
		headPos = pos;
		pos_end = body.find(string(intMarker), pos);
		check(pos_end);
		typeIdLen = pos_end - pos + strlen(intMarker);
		pos = pos_end + strlen(intMarker);
			
		// get the length of this item
		pos_end = body.find(string(intMarker), pos);
		check(pos_end);
		int length = atoi(body.substr(pos, pos_end - pos).c_str());
		lengthLen = pos_end - pos + strlen(intMarker);

		// store the current whole item including its header
		int	itemLen = length + typeIdLen + lengthLen;
		vec.push_back(body.substr(headPos, itemLen));

		// skip to the next item
		pos = headPos + itemLen;
	}
	return args;
}

string
Serializable::serialize(int argc, char** argv)
{
	int		i;
	string	valStr;
	
	for (i=0; i < argc; ++i)
	{
		valStr += serialize(string(argv[i]));
	}
	return composite_serialize(argc, valStr);
}

void
Serializable::deserialize(string& str, int& typeId, string& value)
{
	string	emptyStr;
	const char* pt;
	int		lengthLen;
	int		typeIdLen;

	int	pos = 0;
	int pos_end = 0;

	pos_end =str.find(string(intMarker), pos);
	check(pos_end);
	typeId = atoi(str.substr(pos, pos_end - pos).c_str());
	typeIdLen = pos_end - pos + strlen(intMarker);
	pos = pos_end + strlen(intMarker);

	pos_end = str.find(string(intMarker), pos);
	check(pos_end);
	pt = str.substr(pos, pos_end - pos).c_str();
	int	length = atoi(pt);
	lengthLen = pos_end - pos + strlen(intMarker);
	pos = pos_end + strlen(intMarker);

	if ((unsigned)length != (str.length() - typeIdLen - lengthLen))
	{
		cout <<str.length() <<" "<<length<<endl;
		cerr << "The serialized message is illegal"<<endl;
		typeId = Serializable::UNDEFINED;
		return;
	}

	switch (typeId)
	{
		case Serializable::TYP_COMPOSITE:

			// strip the header, and return all the meat, including the args indicator
			value = str.substr(pos, length);
			break;

		case Serializable::TYP_INT:
		case Serializable::TYP_LONG:
		case Serializable::TYP_CHAR:
		case Serializable::TYP_BOOL:
		case Serializable::TYP_FLOAT:
		case Serializable::TYP_STRING:
			value = str.substr(pos, length);
			break;
		default:
			cerr << "Illegal type specified"<<endl;
			return;

	}
	return;
};

class Test:public Serializable
{
	public:
		Test(){a=1; b='2'; c=0; d="3"; e=4.4; f[0]="abcdef"; f[1]="hijk";}
		~Test(){}
		int 	a;
		char 	b;
		bool	c;
		string	d;
		float	e;
		char *f[2];
		string	buf;
		string Serialize()
		{
			buf = composite_serialize(6,serialize(a)+serialize(b)+serialize(c)+serialize(d)+
				 serialize(e) +serialize(2, &f[0]));

			cout <<buf <<endl;
			return buf;
		}
		void Deserialize(string buf)
		{
			int typeId; 
			string value;

			deserialize(buf, typeId, value);
			assert(typeId == Serializable::TYP_COMPOSITE);

			vector <string>	vals;
			composite_deserialize(value, vals);

			vector <string>::iterator valp = vals.begin();
			for (; valp != vals.end(); ++ valp)
				cout << *valp <<endl;
		}
};

