// (C) 1999- by Kevin S. Beyer

#include <iostream.h>
#include "PriorityQueue.h"
#include "../util/Timeval.h"

int main()
{
  const int N = 1000000;
  PriorityQueue<int> Q(N);
  Timeval tm;
  for(int i = 0 ; i < 1000000 ; i++) {
    int x = lrand48();
    //int x = N - i;
    //cerr << "push " << x << endl;
    Q.push(x);
  }
  cerr << "push: " << tm.ElapsedTime() << endl;
  cout << "upH: " << upH << endl;
  upH = 0;
  tm.SetToCurrentTime();
  cerr << "now pop\n";
  int prev = 0;
  int k = 0;
  while( !Q.empty() ) {
    k++;
    int x = Q.top(); Q.pop();
    if( x < prev ) {
      cerr << "oops " << x << ' ' << prev << endl;
    }
    //  if( x != prev + 1 ) {
    //    cerr << "oops, bad rec: " << x << ' ' << k << ' ' << prev << endl;
    //  }
    prev = x;
  }
  if( k != N ) {
    cerr << "not enough returned! " << k << ' ' << N << endl;
  }
  cerr << "pop: " << tm.ElapsedTime() << endl;
  cout << k << " recs\n";
  cout << "ups: " << upCount << endl;
  cout << "upH: " << upH << endl;
  cout << "dnH: " << dnH << endl;
}
