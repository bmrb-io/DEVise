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
  Revision 1.2  1998/01/08 19:44:00  wenger
  Updated copyright to 1998.

 */
#include <string>
#include <vector>
#include <iostream.h>
#include "CmdDescriptor.h"
#include "CmdSerialize.h"

CmdSource::CmdSource(CmdSource::SrcType srct, int cid)
{
	srcType = srct;
	cId = cid;
	fromLog = false;
}
CmdSource::CmdSource(const CmdSource& cmdSrc)
{
	srcType = cmdSrc.srcType;
	cId = cmdSrc.cId;
	fromLog = cmdSrc.fromLog;
}
string
CmdSource::Serialize()
{
	return composite_serialize(3, 
		serialize((int)srcType)+serialize(cId)+serialize(fromLog));
}

void
CmdSource::Deserialize(string buf)
{
	int typeId;
	string val;
	int args;
	deserialize(buf, typeId, val);
	if (typeId != Serializable::TYP_COMPOSITE)
	{
		cerr << "Error in CmdSource Deserialization"<<endl;
	}
	else
	{
		vector<string> vec;
		int 	args;
		args = composite_deserialize(val, vec);
		 
		if (args != 3)
		{
			cerr << "Error in #args in CmdSource Deserialization"<<endl;
		}
		else
		{
			deserialize(vec[0], typeId, val);
			if (typeId != Serializable::TYP_INT)
			{
				cerr << "Error in CmdSource Deserialization:srcType"<<endl;
			}
			srcType = (CmdSource::SrcType)atoi(val.c_str());

			deserialize(vec[1], typeId, val);
			if (typeId != Serializable::TYP_INT)
			{
				cerr << "Error in CmdSource Deserialization:cId"<<endl;
			}
			cId = atoi(val.c_str());

			deserialize(vec[2], typeId, val);
			if (typeId != Serializable::TYP_BOOL)
			{
				cerr << "Error in CmdSource Deserialization:fromLog"<<endl;
			}
			fromLog = (atoi(val.c_str())==1);
		}
	}
}
CmdSource::~CmdSource()
{
}


CmdDescriptor::CmdDescriptor(string buf)
{
	Deserialize(buf);
}

CmdDescriptor::CmdDescriptor(CmdSource& srct, CmdDescriptor::DestAttr des)
:cmdSrc(srct)
{
	destAttr = des;
}

CmdDescriptor::~CmdDescriptor()
{
}

CmdSource*
CmdDescriptor::getCmdsource()
{
	return &cmdSrc;
}


string
CmdDescriptor::Serialize()
{
	return composite_serialize(2, cmdSrc.Serialize()+serialize(destAttr));
}

void
CmdDescriptor::Deserialize(string buf)
{
	int typeId;
	string val;
	int args;

	deserialize(buf, typeId, val);
	if (typeId != Serializable::TYP_COMPOSITE)
	{
		cerr << "Error in CmdDescriptor Deserialization"<<endl;
	}
	else
	{
		vector<string> vec;
		int 	args;
		args = composite_deserialize(val, vec);

		if (args != 2)
		{
			cerr << "Error in #args in CmdDescriptor Deserialization"
					<<endl;
		}
		else
		{
			cmdSrc.Deserialize(vec[0]);
			destAttr = (CmdDescriptor::DestAttr) atoi(vec[1].c_str());
		}
	}
}
