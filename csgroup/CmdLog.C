#include <string>
#include <vector>
#include <stdio.h>
#include <ctype.h>
#include "CmdLog.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

//#define DEBUG

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
		//cout << "Cmd="<<cmd[i]<<endl;
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
#ifdef DEBUG
	cout <<"vectors:"<< vec[0]<<"|"<<vec[1]<<"|"<<vec[2]<<endl;
#endif
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
	maxlogId = -1;
	fd = open(filename, O_RDWR|O_CREAT);
	if (fd <0)
	{
		cerr << "Cannot create log file:"<<filename<<endl;
		exit(-1);
	}
	seekLog(LOG_END);
}

long
CmdLogRecord::logCommand(int argc, char** argv)
{
	// move curlogId to the end of the log, if no log
	// curlogId = -1;
	if ((maxlogId != curlogId)||(curlogId == -1))
		seekLog(LOG_END);

	curlogId ++;
	maxlogId ++;
		
	// record current time
	ts.setCurTime();

	// make in-memory version
	cmd.erase(cmd.begin(), cmd.end());
	int	i;
	for (i=0; i< argc; ++i)
	{
		string	temp = string(argv[i]);
		cmd.push_back(temp);
	}

	// push to disk
	string	tempStr = Serialize();
#ifdef DEBUG
	cout <<"Debug String:"<<tempStr<<endl;
#endif
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
	long	savelogId = -1;
	long	logId = -1;

	logId = seekLog(CmdLogRecord::LOG_BEGIN);
	do 
	{
		cout << *this;
		savelogId = logId;
		logId = seekLog(CmdLogRecord::LOG_NEXT);
	} while (savelogId != logId);
}
/*
//
// Sample log-viewer for the command logging facility and testing routines
//
main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "logfilename expected"<<endl;
		exit(-1);
	}
	CmdLogRecord 	*cmdLog = new CmdLogRecord(argv[1]);
	cmdLog->viewLog();
}
*/
