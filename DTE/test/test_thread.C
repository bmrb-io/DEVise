// (C) 1999- by Kevin S. Beyer

#include <iostream.h>
#include "DTE/misc/DteThread.h"

static DteMonitor monitor;
int count = 0;

class pingpong_t
: public DteThread
{
public:

  pingpong_t(bool a);

  ~pingpong_t();

  void run();

protected:

  bool _a;

private:

  pingpong_t(const pingpong_t& x);
  pingpong_t& operator=(const pingpong_t& x);
};



pingpong_t::pingpong_t(bool a)
: _a(a)
{
}


pingpong_t::~pingpong_t()
{
}


void pingpong_t::run()
{
  if( _a ) {
    for(int i = 0 ; i < 1000 ; i++) {
      monitor.acquire();
      while( count > 20 ) {
cout << "wait A\n";
         monitor.wait();
cout << "unwait A\n";
      }
      count++;
      cout << "A: " << count << endl;
      monitor.signal();
    }
    cout << "done A\n";

  } else {

    for(int i = 0 ; i < 1000 ; i++) {
      monitor.acquire();
      while( count <= 0 ) {
cout << "wait B\n";
        monitor.wait();
cout << "unwait B\n";
      }
      count--;
      cout << "B: " << count << endl;
      monitor.signal();
    }
    cout << "done B\n";

  }
}




int main()
{
  pingpong_t a(true);
  pingpong_t b(false);
  b.fork();
  a.fork();
  a.wait();
  b.wait();
  cout << "done: " << count << endl;
}
