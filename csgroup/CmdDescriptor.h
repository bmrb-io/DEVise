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
#ifndef _CMD_DESCRIPTOR_H
#define _CMD_DESCRIPTOR_H
#include <string>
#include "CmdSerialize.h"


/*
#include "keys.h"
//source command routing table, reserved for future use
class RoutingEntry
{
	public:
		typedef enum{
			SRC_ENTRY, MID_ENTRY, DEST_ENTRY
		}RoutingEntryProperty;

		RoutingEntry(string clientName, serverName, );
	private:
		string	sName;
		RoutingEntryProperty rep;
};
*/

/*
// routing table for command routing
class RoutingTab
{
	public:
		typedef enum
		{
			CLIENT, SERVER, COLLABORATOR
		}DestType;
		RoutingTab();
		~RoutingTab();
	private:
		DestType	_destType;
		string serialize();
		bool deserialize(string& errmsg){string err;errmsg=err;}
};
*/

class CmdSource:public Serializable
{
	public:
		typedef enum
		{
			CLIENT, 			// commands that comes from the client 
			USER, 				// commands that comes from the X-server
			INTERNAL, 			// commands that comes from CreateTData
			NETWORK, 			// commands that comes form network
			SESSION_PLAY, 		// commands from a session
			SESSION_SAVE,		// commands for saving a session
			LOGFILE,			// commands from the log
			JAVACLIENT 			// commands from JAVA client
		}SrcType;
		CmdSource(SrcType, int);
		CmdSource(){}
		~CmdSource();

		CmdSource(const CmdSource&);
		SrcType getSrctype() const
		{
			return srcType;
		}
		int getClientid() const
		{
			return cId;
		}
		string Serialize();
		void Deserialize(string buf);
		bool	isFromLog() {return fromLog;}
		void	setFromLog(){fromLog = true;}
	private:
		SrcType	srcType;
		int		cId;
		bool	fromLog;
};

class CmdDescriptor:public Serializable
{

	// external interface
	public:
		typedef enum{
			UNDEFINED=-1, FOR_SERVER, FOR_CLIENT
		}DestAttr;
		CmdDescriptor(CmdSource& srct, DestAttr des);
		CmdDescriptor(string buf);
		~CmdDescriptor();
		CmdSource* getCmdsource();
		void setRetval(bool success, char* errmsg)
		{
			this->success = success;
			if (errmsg == NULL)
			{
				string	tempStr;
				this->errmsg = tempStr;
			}
			else
				this->errmsg = string(errmsg);
		}
		bool isSuccess()
		{
			return success;
		}
		string getErrmsg()
		{
			return errmsg;
		}
		DestAttr getDest()
		{
			return destAttr;
		}
		string	Serialize();
		void Deserialize(string buf);

	private:
		// memebers to be serialized
		CmdSource	cmdSrc;
		DestAttr	destAttr;

		// members that will not be serialized
		string		errmsg;
		bool		success;
};
#endif
