// (C) 1999- by Kevin S. Beyer

#include <iostream.h>
#include "InitShut.h"
#include "sysdep.h"
#include "exception.h"
#include "Engine.h"
#include "DTE/util/Socket.h"
#include "DTE/mql/MqlSession.h"


extern ofstream* commandOut; //kb: move me
ifstream* commandIn = NULL;
extern bool quitRequested;


static string extractQuery(istream& in)
{
	string query;
	char c = '\0';
	while( in && c != ';' ){
		in.get(c);
		query += c;
	}
	if( !in ) {
		return "";
	}
	return query;
}

#define CATCH2(A,B) if(currExcept){A; delete currExcept; currExcept = NULL; B;}

static void sendInt(int4 x)
{
	x = htonl(x);
	commandOut->write(&x, sizeof(int4));
}

static void flush()
{
	commandOut->flush();
}


static void sendCommandComplete(int4 status)
{
	sendInt(MqlSession::COMMAND_COMPLETE_OP);
	sendInt(status);
	flush();
}


static void processQuery(const string& query)
{
	cerr << "Received query: " << query << endl;

	Engine engine(query);
	engine.optimize();
	CATCH2( sendCommandComplete(MqlSession::STATUS_ERROR), return );
	if( !quitRequested ) {
		int numFlds = engine.getNumFields();
		if(numFlds > 0) {
			DteTupleAdt tupAdt = engine.getAdt();
			const Tuple* tup = engine.getFirst();
			CATCH2( sendCommandComplete(MqlSession::STATUS_ERROR), return );
			while(tup) {
				tupAdt.print(cerr, tup);
				cerr << '\n';
				tup = engine.getNext();
				CATCH2( sendCommandComplete(MqlSession::STATUS_ERROR), return );
			}
		}
	}
	sendCommandComplete(MqlSession::STATUS_OK);
}

inline void getAsciiLine(istream& in, char* buf, int len)
{
  in.ipfx(0); // skip whitespace
  in.getline(buf, len);
  len = strlen(buf) - 1;
  if( buf[len] == '\r' ) buf[len] = '\0';
}


static void acceptConnection(int port)
{
  int sd = Socket::bind(port);
  assert(sd >= 0);

  int fd = Socket::accept(sd);
  assert(fd >= 0);
  close(sd);

  commandIn = new ifstream(fd);
  commandOut = new ofstream(fd);

  istream& in = *commandIn;
  ostream& out = *commandOut;
  in.setf(ios::skipws);

  char cmd[1024];

  getAsciiLine(in, cmd, sizeof(cmd));
  cerr << "received: " << cmd << endl;
  if( strcasecmp(cmd, "hello mql") != 0 ) {
    cerr << "no hello: '" << cmd << "'" << endl;
    exit(1);
  }
  
  in.ipfx(0);
  in.getline(cmd, sizeof(cmd), ':');
  cerr << "received: " << cmd << endl;
  if( strcasecmp(cmd, "version") != 0 ) {
    cerr << "no version: " << cmd << endl;
    exit(1);
  }

  getAsciiLine(in, cmd, sizeof(cmd));
  cerr << "received: " << cmd << endl;
  if( strcasecmp(cmd, "0.3") != 0 ) {
    cerr << "unknown version: " << cmd << endl;
    exit(1);
  }

  out << "mql ok" << endl;
  cerr << "sent ok" << endl;
}



int main(int ac, const char* av[])
{
  try {
		assert(ac==2);
		int port = atoi(av[1]);
		
		acceptConnection(port);
		
		initialize_system();
		
		//PlanParser parser(commandIn);
		//parser.debug();
		//while(*commandIn && !parser.quitRequested()) {
		while(!quitRequested) {
			commandIn->ipfx(0);
			commandIn->peek();
			if( !commandIn->good() ) break;
			string query = extractQuery(*commandIn);
			if( query != "" ) {
				processQuery(query);
			}
		}
		shutdown_system();
		
		// read commands until client closes
		while( commandIn->good() && !commandIn->eof() ) {
			cerr << "skipping...\n";
			char buf[1000];
			commandIn->read(buf, 1000);
		}
  } catch(MqlError e) {
    cerr << "uncaught MQL exception: " << e.getMessage() << '\n';
  } catch(...) {
    cerr << "uncaught exception of unknown type!\n";
    return 1;
  }

  return 0;
}

