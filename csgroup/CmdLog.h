#ifndef _CMD_LOG_H
#define _CMD_LOG_H
#include "CmdSerialize.h"
#include <ostream.h>

class CmdLogRecord;
ostream& operator << (ostream& os, const CmdLogRecord& cmd);
//
// command log structure:
//		logId, timeStamp, DeviseCommand, #args, arg1, arg2, ....
class TimeStamp:public Serializable
{
	public:
		TimeStamp();
		~TimeStamp();
		void setCurTime();
		string getTime() const;
		string	Serialize();
		void setTime(string buf);
	private:
		long	tv_sec;
};

// mark the boundary of a log record
class LogMarker:public Serializable{
	static string recLeftB;
	static string recRightB;
	public:
		LogMarker(){};
		LogMarker(int length);
		~LogMarker();
		string Serialize();
		int Deserialize(int fd);
		int getLen();
	private:
		int	len;
};

// log file format:
// <lm, body> <lm, body> ....
// <lm> = <refLeftB, length, refRightB>
// where length refers to the length of the body

class CmdLogRecord: public Serializable
{
	friend ostream& operator << (ostream& os, const CmdLogRecord& cmd);
	public:
		// out-of-band log ids
		enum {
			NOT_FOUND=-4,LOG_END = -3, LOG_BEGIN = -2, LOG_NEXT=-1
		};

		CmdLogRecord(char* filename);	
		~CmdLogRecord();

		// append at the tail, curlogId point to the last record
		int logCommand(int argc, char** argv);

		// manipulate pointers and update the contents for the current record 
		long seekLog(long logId);
		void viewLog();

		// read information about current log record
		string 	getTimeStamp();
		long	getLogId();
		void	getCommand(vector<string>& cmd);

		// serialize
		string Serialize();
		void Deserialize(string);
	private:
		long	maxlogId;

	private:
		LogMarker	lm;
		int 		fd;

		enum {
			NUM_ARGS = 3 // logrecordBoundary, 
		};
		long		curlogId;
		TimeStamp	ts;
		vector<string> cmd;
};
#endif
