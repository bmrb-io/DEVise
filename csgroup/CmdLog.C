#include <string>
#include <vector>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "CmdLog.h"
#include "CmdContainer.h"
#include "Scheduler.h"
#define DEBUG

TimeStamp::TimeStamp()
{
}
TimeStamp::~TimeStamp()
{
}

void
TimeStamp::setCurTime()
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	tv_sec = tv.tv_sec; 
}

string
TimeStamp::getTime() const
{
	char	buf[10];
	sprintf(buf, "%ld", tv_sec);
	string ts(buf);	
	return ts;
}

string
TimeStamp::Serialize()
{
	return serialize(tv_sec);
}

void
TimeStamp::setTime(string val)
{
	int	typeId;

	tv_sec = atol(val.c_str());
}

string LogMarker::recLeftB ="?-*>";
string LogMarker::recRightB="<-*?";

LogMarker::LogMarker(int length)
{
	len = length;
}

LogMarker::~LogMarker()
{
}

string
LogMarker::Serialize()
{
	char buf[128];
	sprintf(buf, "%d",len);
	return recLeftB+string(buf)+recRightB;
}

// return the size of the marker
int
LogMarker::Deserialize(int fd)
{
	int		i;
	char buf[128];
	int nbytes;
	string	errStr = "Syntax error in LogMarker Deserialization";

	nbytes = read(fd, buf, recLeftB.length());
	if (nbytes == 0)
		return -1;
	if (nbytes <0)
	{
		perror("Read error:");
		return -1;
	}

	if ((unsigned)nbytes != recLeftB.length())
	{
		cerr << errStr<<endl;
		return -1;
	}
	for (i=0; i< 128; ++i)
	{
		nbytes = read(fd, &buf[i], 1);
		if (!isdigit(buf[i]))
		{
			read(fd, &buf[i+1], recRightB.length() -1);
			buf[i + recRightB.length()] = 0;
			if (!(string(&buf[i])== recRightB))
			{
				cerr <<"Syntax error in right marker:"<<&buf[i]<<endl;
				return -1;
			}
			buf[i] =0;
			len = atoi(buf);
			return len;
		}
	}
	return -1;
}

// return lm + body
string
CmdLogRecord::Serialize()
{
	int	i;
	int	args = cmd.size();
	string tempStr; 

	for (i=0; i <args; ++i)
	{
		string	temp = cmd[i];
		tempStr += serialize(temp);
	}

	string	body;

	body = composite_serialize( CmdLogRecord::NUM_ARGS, serialize(curlogId)+ ts.Serialize()+
		composite_serialize(args, tempStr));

	lm = LogMarker(body.length());
	return lm.Serialize() + body;
}

// buf = body
void
CmdLogRecord::Deserialize(string buf)
{
	vector <string> vec;
	int	typeId;
	string value;

	deserialize(buf, typeId, value);
	if (typeId != Serializable::TYP_COMPOSITE)
	{
		cerr << "*** Composite type expected"<<endl;
		return;
	}
	else
	if (composite_deserialize(value, vec) != CmdLogRecord::NUM_ARGS)
	{
		cerr << "***Wrong nubmer of arguments"<<endl;
		return;
	}

	// deserialize the logId
	deserialize(vec[0], typeId, value);
	if (typeId != Serializable::TYP_LONG)
	{
		cerr << "***Long type expected \n";
		return;
	}
	else
	{
		curlogId = atol(value.c_str());
	}

	// deserialize the time stamp
	deserialize(vec[1], typeId, value);
	ts.setTime(value);

	// deserialize the command
	deserialize(vec[2], typeId, value);
	if (typeId != Serializable::TYP_COMPOSITE)
	{
		cerr << "***Composite type expected \n";
		return;
	}
	else
	{
		vector <string> vec;
		int	args;
		int	i;
		args = composite_deserialize(value, vec);
		cmd.erase(cmd.begin(), cmd.end());
		for (i=0; i< args; ++i)
		{
			string	cmdOne;
			deserialize(vec[i], typeId, cmdOne);
			cmd.push_back(cmdOne);
		}
	}
}

CmdLogRecord::~CmdLogRecord()
{
	// push everything to the disk
	fsync(fd);
	close(fd);
}

CmdLogRecord::CmdLogRecord(char* filename)
{
	enabled = true;
	currentBy = CmdLogRecord::LOG_IDLE;
	_dispID = NULL;
	currentPause = 0;
	currentStart = LOG_BEGIN;
	currentEnd = LOG_END;
	currentLogId = LOG_BEGIN;
	currentSaveLogId = LOG_BEGIN;

	maxlogId = -1;
	fd = open(filename, O_RDWR|O_CREAT);
	if (fd <0)
	{
		cerr << "Cannot create log file:"<<filename<<endl;
		exit(-1);
	}
	seekLog(LOG_END);
	 _dispID = Dispatcher::Current()->Register(this);
}

long
CmdLogRecord::logCommand(int argc, char** argv)
{
	if (!enabled)
	{
		return -1;
	}

	// move currentLogId to the end of the log, if no log
	// currentLogId = -1;
	char	**nargv = new (char*)[argc](NULL);
	int	i;
	for (i=0; i< argc; ++i)
	{
		nargv[i] = strdup(argv[i]);
	}

	if ((maxlogId != currentLogId)||(currentLogId ==LOG_BEGIN ))
		seekLog(LOG_END);

	curlogId ++;
	maxlogId ++;
		
	// record current time
	ts.setCurTime();

	// make in-memory version
	cmd.erase(cmd.begin(), cmd.end());
	for (i=0; i< argc; ++i)
	{
		string	temp = string(nargv[i]);
		cmd.push_back(temp);
	}
	delete []nargv;

	// push to disk
	string	tempStr = Serialize();
	write(fd, tempStr.c_str(), tempStr.length());
	return curlogId;
}

// return current log Id, return -1 for empty log
// +----------------+---------
// | cur log record | next record
// +----------------+---------
//                   ^ file pointer
long
CmdLogRecord::seekLog(long logId)
{
	char 	*dynamicbuf = NULL;

	// initalize file and log pointer
	if (logId != CmdLogRecord::LOG_NEXT)
	{
		lseek(fd, 0, SEEK_SET);
		curlogId = -1;
	}

	while (1)
	{
		LogMarker 	lm;
		string		body;
		int			nbytes;
		int			len;

		// get length of the body
		len = lm.Deserialize(fd);
		if (len <0)
		{
			if (logId == CmdLogRecord::LOG_END)
				maxlogId = curlogId;
			return curlogId;
		}

		dynamicbuf = new char[len+1];
		nbytes = read(fd, dynamicbuf, len);
		dynamicbuf[len] = 0;
		if (nbytes !=len)
		{
			cerr << "Body contents error"<<endl;
			delete [] dynamicbuf;
			return -1;
		}
		// deserialize buf
		Deserialize(string(dynamicbuf));
		delete [] dynamicbuf;

		if (maxlogId < curlogId )
			maxlogId = curlogId;

		if ((logId == CmdLogRecord::LOG_BEGIN)||(logId == 
			CmdLogRecord::LOG_NEXT))
		{
			return curlogId;
		}

		if (logId == CmdLogRecord::LOG_END)
			continue;

		if (curlogId == logId)
			return logId;
	}
}

string 
CmdLogRecord::getTimeStamp()
{
	return ts.getTime();
}

long 
CmdLogRecord::getLogId()
{
	return curlogId;
}

void
CmdLogRecord::getCommand(vector<string>& command)
{
	command = cmd;
}

ostream& operator << (ostream& os, const CmdLogRecord& cmdLog)
{
		os <<"\n****** LogId:"<<cmdLog.curlogId;
		os <<" Time:"<<cmdLog.ts.getTime();
		os <<" Commands:";
		vector <string>::const_iterator itr = cmdLog.cmd.begin();
		while (itr != cmdLog.cmd.end())
		{
			cout <<*itr++ << " ";
		}
		os <<endl;
		return os;
}

void 
CmdLogRecord::viewLog()
{
	long	currentSaveLogId = -1;
	long	logId = -1;

	logId = seekLog(CmdLogRecord::LOG_BEGIN);
	do 
	{
		cout << *this;
		currentSaveLogId = logId;
		logId = seekLog(CmdLogRecord::LOG_NEXT);
	} while (currentSaveLogId != logId);
}

void 
CmdLogRecord::runLogRecord()
{
	int args = cmd.size();

	// deserialize the same command descriptor
	CmdDescriptor   cmdDes(cmd[args-1]);

	// add fromLog flag
	CmdSource*  cmdSrcp;
	cmdSrcp = cmdDes.getCmdsource();
	cmdSrcp->setFromLog();

	// play one command from the log
	char** argv = new (char*)[args-1];
	int	i;
	for (i=0; i< args -1 ; ++i)
	{
		argv[i] =(char*) cmd[i].c_str();
#ifdef DEBUG
		cout << argv[i]<<" ";
#endif
	}

	cmdContainerp->Run(args-1, argv, 
		DeviseCommand::getDefaultControl(), cmdDes);

	delete []argv;

	/* run waitForQueries to get everything done
	char	*cmd = "waitForQueries";
	cmdContainerp->Run(1, &cmd, 
		DeviseCommand::getDefaultControl(), cmdDes);
	*/
}


bool
CmdLogRecord::playLog(int by, int pause, long start, long end, char*& errmsg)
{
	long	logId = -1;
	static	char*	errmsgs[3] =
		{
			"Wrong by-type for play log",
			"by_timestamp not implmented yet",
			"Log cannot be played, since it is busy"
		};
					
	// prevent repeated calling from the clients
	// have to be serialized at the cmd container level for playLog commands
	if (currentBy != LOG_IDLE)
	{
		errmsg = errmsgs[2];
		cerr <<errmsg<<endl;
		return false;
	}

	currentBy =(CmdLogRecord::ByMeans) by;
	currentPause = pause;
	currentStart = start;
	currentEnd = end;

	if (currentBy != CmdLogRecord::BY_STEP)
	{
		currentLogId = LOG_BEGIN;
		currentSaveLogId = LOG_BEGIN;
	}

	// request to be activated after pause seconds
	Scheduler::Current()->RequestTimedCallback(_dispID, pause);

	return true;
}

// get scheduled to run
// side-effect, it is requierd a new CmdLogRecord object will schedule the Run
// each time. When run is done, the object will be automatically deleted within
// this program!!!!
void
CmdLogRecord::Run()
{
	switch (currentBy) 
	{
		case CmdLogRecord::BY_LOGID:
			// pull pointer to the begining iff it is the Run
			// issued within playLog()

			currentSaveLogId = currentLogId;
			if (currentLogId == LOG_BEGIN)
				currentLogId = seekLog(currentStart);
			else
				currentLogId = seekLog(CmdLogRecord::LOG_NEXT);

			// schedule next events if we can
			if ((currentSaveLogId != currentLogId)&&
				((currentEnd>0)?(currentSaveLogId <currentEnd):true))
				Scheduler::Current()->RequestTimedCallback(_dispID, 
					currentPause);
			else
			{
				currentBy = LOG_IDLE;
				delete this;
			}

			if (currentLogId != currentSaveLogId)
			{
#ifdef DEBUG
				cout << "LogId= "<<currentLogId<<" Command= "<<endl;
#endif
				runLogRecord();
#ifdef DEBUG
				cout << endl;
#endif
			}
			break;

		case CmdLogRecord::BY_TIMESTAMP:
			cerr << "Time stamp-based log play is not supported yet"<<endl;
			currentBy = LOG_IDLE;
			delete this;
			return;
			break;

		case CmdLogRecord::BY_STEP:
			currentSaveLogId = currentLogId;
			if (currentLogId == LOG_BEGIN)
				currentLogId = seekLog(currentStart);
			else
				currentLogId = seekLog(CmdLogRecord::LOG_NEXT);
			if (currentLogId != currentSaveLogId)
				runLogRecord();
			currentBy = LOG_IDLE;
			delete this;
			break;

		default:
			currentBy = LOG_IDLE;
			delete this;
			cerr << "Wrong by-type for playing log\n";
			return;
	}
}

//
// Sample log-viewer for the command logging facility and testing routines
//
/*
main(int argc, char** av)
{
	if (argc != 2)
	{
		cerr << "logfilename expected"<<endl;
		exit(-1);
	}
	char* argv[4] = {"one","two","/p/devise/world/haha/in2/three","four"};
	CmdLogRecord 	*cmdLog = new CmdLogRecord(av[1]);

	cmdLog->logCommand(4, &argv[0]);
	cmdLog->logCommand(3, &argv[1]);
	cmdLog->logCommand(2, &argv[2]);
	cmdLog->logCommand(1, &argv[3]);
	delete cmdLog;

	CmdLogRecord 	*cmdLog1 = new CmdLogRecord(av[1]);
	cmdLog1->viewLog();
}
*/
