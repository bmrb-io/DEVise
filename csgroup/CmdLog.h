#ifndef _CMD_LOG_H
#define _CMD_LOG_H
#include <string>
#include <vector>
#include <ostream.h>
#include "CmdSerialize.h"
#include "Dispatcher.h"

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

class CmdLogRecord: public Serializable, DispatcherCallback
{
	friend ostream& operator << (ostream& os, const CmdLogRecord& cmd);
	public:
		// out-of-band log ids
		typedef enum {
			NOT_FOUND=-4,LOG_END = -3, LOG_NEXT=-2, LOG_BEGIN = -1,
			BY_LOGID=0, BY_TIMESTAMP=1, BY_STEP=2, LOG_IDLE 
		}ByMeans;

		ByMeans currentBy;
		CmdLogRecord(char* filename);	
		~CmdLogRecord();
		virtual char* DispatchedName() { return "CmdLogRecord"; }
		virtual void Run();

		// append at the tail, curlogId point to the last record
		long logCommand(int argc, char** argv);
		bool playCommand(long logId1, long logId2);

		// manipulate pointers and update the contents for the current record 
		long seekLog(long logId);
		void viewLog();
		bool playLog(int by, int pause, long start, long end, char*& errmsg);
		void runLogRecord();

		// read information about current log record
		string 	getTimeStamp();
		long	getLogId();
		void	getCommand(vector<string>& cmd);

		// serialize
		string Serialize();
		void Deserialize(string);

		// loggin access
		// 1. Before Tcl_Eval a file, logging should be turned off
		//    after that, it have to be turned off

		void setLogStatus(bool status) { enabled = status;}
		bool getLogStatus() { return enabled;}

	private:
		long	maxlogId;
		bool	enabled;
		DispatcherID	_dispID;

		long	currentPause;
		long	currentStart;
		long	currentEnd;

		long	currentLogId;
		long	currentSaveLogId;

	private:
		long		curlogId;
		LogMarker	lm;
		int 		fd;

		enum {
			NUM_ARGS = 3 // logrecordBoundary, 
		};
		TimeStamp	ts;
		vector<string> cmd;
};
#endif
