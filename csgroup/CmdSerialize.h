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
 */
#ifndef _CMD_SERIALIZE_H
#define _CMD_SERIALIZE_H
#include <string>
#include <vector>
#if !defined(SGI) && !defined(LINUX)
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif
class Serializable
{
	public:
		enum{
			UNDEFINED = 0,
			TYP_COMPOSITE,
			TYP_INT, 
			TYP_BOOL, 
			TYP_CHAR, 
			TYP_FLOAT,
			TYP_STRING,
		};
		enum{
			INT_SIZE = 16
		};
		Serializable();
		virtual ~Serializable(){}

	protected:
		virtual string raw_serialize(int);
		virtual string composite_serialize(int argc, string body);
		virtual string composite_serialize(int argc, ...);
		virtual int composite_deserialize(string body, vector<string>& vec);

		virtual string serialize(int);
		virtual string serialize(bool);
		virtual string serialize(char);
		virtual string serialize(float);
		virtual string serialize(string);
		virtual string serialize(int, char**);
		virtual void deserialize(string& str, int& typeId, string& value);
};
#endif
