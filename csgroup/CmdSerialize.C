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
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include "CmdSerialize.h"

// a serializable type is
// (i) an atomic type (typeId, length, value)
// (ii) a composite type (typeId = TYP_COMPOSITE, length, #subtypes)

// format:
// +-----------+----------+------.........----------------------------------+
// | 16 bytes  | 16 bytes | 16 bytes+ specified by 2nd field*               |
// +-----------+----------+------.........----------------------------------+
// * iff it is a composite type, the 3rd field will be 16 types more
// Serializable::INT_SIZE =16

Serializable::Serializable()
{
}

string
Serializable::raw_serialize(int arg)
{
	string	tempStr;
	char buf[Serializable::INT_SIZE];
	char format[20];
	sprintf(format, "%%-%dd", Serializable::INT_SIZE);
	sprintf(buf, format, arg);
	tempStr = string(buf);
	return tempStr;
}

string
Serializable::serialize(int arg)
{
	string typeStr= raw_serialize(Serializable::TYP_INT);
	char	buf[32];
	sprintf(buf,"%d", arg);
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
	composite_serialize(args, body);
}

string 
Serializable::composite_serialize(int argc, string body)
{
	string typeStr= raw_serialize(Serializable::TYP_COMPOSITE);
	string	argsStr = raw_serialize(argc);
	string lenStr = raw_serialize(argsStr.length()+body.length()); 
	return typeStr + lenStr + argsStr + body;
}

int 
Serializable::composite_deserialize(string body, vector<string>& vec)
{
	int pos = 0;

	// extract # of arguments
	int	args = atoi(body.substr(pos, Serializable::INT_SIZE).c_str());
	pos += Serializable::INT_SIZE;
	int	i;

	for (i=0; i< args ; ++i) 
	{
		string	str;

		// move pos to the length field, skip the typeId field
		pos += Serializable::INT_SIZE;
		int	length = atoi(body.substr(pos, Serializable::INT_SIZE).c_str());

		// skip to the body filed
		pos += Serializable::INT_SIZE;

		// store the current body
		vec.push_back(body.substr(pos, length));

		// skip to the next item
		pos += length;
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

	int	pos = 0;
	typeId = atoi(str.substr(pos, Serializable::INT_SIZE).c_str());

	pos += Serializable::INT_SIZE;
	int	length = atoi(str.substr(pos, Serializable::INT_SIZE).c_str());
	pos += Serializable::INT_SIZE;

	if (length != (str.length() - 2* Serializable::INT_SIZE))
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
		case Serializable::TYP_CHAR:
		case Serializable::TYP_BOOL:
		case Serializable::TYP_FLOAT:
		case Serializable::TYP_STRING:
			value = atoi(str.substr(pos, Serializable::INT_SIZE).c_str());
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
			for (valp; valp != vals.end(); ++ valp)
				cout << *valp <<endl;
		}
};

