// (C) 1998- by Kevin S. Beyer

#include <iostream.h>
#include "DTE/plan/PlanParser.h"
#include "DTE/util/Timeval.h"

#include "SBMInit.h"
void initialize_system();
void shutdown_system();

//int dataPageSize;               // kb: delete me later!

//#include "Socket.h"             //kb: remove this when send stuff is fixed?
//int send_fd;
//mql_mutex_t send_mutex;


int main(int ac, const char* av[])
{
  //assert(ac==3);
  //const char* host = av[1];
  //int port = atoi(av[2]);
  //send_fd = Socket::connect(host,port);

  cerr << "warning: can't use rtrees\n";
  //initialize_system();
  PlanParser parser;
  while( cin && !parser.quitRequested() ) {
    if( parser.parse() == 0 ) {
      if( parser._plan_steps.size() > 0 ) {
        cerr << "plan parsed\n";
        double elapsed;
        {
          Timeval t;
          parser._plan_steps.run();
          elapsed = t.ElapsedTime().AsSecs();
        }
        cerr << "done running all plan steps in " << elapsed << " seconds\n";
      }
    } else {
      cerr << "\nplan did not parse!\n";
    }
  }
  //shutdown_system();
}

