/* Command.h: for reading command from a file responding to it */
#ifndef Command_h
#define Command_h
#include "Dispatcher.h"
#include "VisualArg.h"
#include "Timer.h"

class BufMgr;
struct File;
class QueryProc;

class Command : private TimerCallback, private DispatcherCallback {
public:
	/* read input from standard input */
	Command(QueryProc *qp);

	/* read input from file */
	Command(char *fname, QueryProc *qp);

private:
	enum CmdState { WaitTime, WaitCmd, ReadInput, Eof };

	/* Do initialization */
	void Init();

	/* from DispatcherCallback */
	char *DispatchedName(){
		return "Command";
	}
	void Run();

	/* from TimerCallback */
	void TimerWake(int);

	/* process a command line */
	void ProcessLine(char *line);

	QueryProc *_queryProc;

	/* Create a new visual filter based on old filter and comand .
	index = index to start looking for new command.  Return true
	if a new filter is found, else return false*/
	Boolean UpdateVisualFilter(const VisualFilter oldFiler, 
		VisualFilter &newFilter, int &index, const int argc, const char **argv);
	
	void PrintState();

	FILE *_input;	/* where to read command */
	CmdState _state; /* current state */
};
#endif
