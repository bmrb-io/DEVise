#ifndef _GCC_STRINGSTREAM_H_
#define _GCC_STRINGSTREAM_H_

// (C) 1998- Kevin S. Beyer

// only include this for gcc
//kb: add to devise/src/include/sysdep.h
//kb: perhaps sysdep.h can go away if we use cygwin/egcs for windows
#ifdef __GNUG__

#include <strstream.h>
#include <string>

/*
  This class makes the old strstream class work as a stringstream.
  This class should be removed ifdef'd out for compilers that have
  proper stringstream support.
  Users of this class should not take advantage of the fact that
  stringstream is derived from strstream.
*/
class ostringstream
: public ostrstream
{
public:

  ostringstream() {}

  ostringstream(const string& s) : ostrstream() {
    *this << s;
  }

  string str() /* const */ {
    int len = ostrstream::pcount();
    const char* cp = ostrstream::str();
    ostrstream::freeze(0);      // allow more writes
    return string(cp, len);
  }

private:

  ostringstream(const ostringstream& x);
  ostringstream& operator=(const ostringstream& x);
};


/*
  This class makes the old strstream class work as a istringstream.
  This class should be removed ifdef'd out for compilers that have
  proper stringstream support.
  Users of this class should not take advantage of the fact that
  istringstream is derived from strstreambase.
*/
class istringstream
//: public strstreambase, public istream
: public istrstream
{
public:

  istringstream(const string& s) : istrstream(s.c_str()) {}

  // not yet supported:
  // void str(const string& s) 

private:

  istringstream(const istringstream& x);
  istringstream& operator=(const istringstream& x);
};

#endif // only gcc

#endif // _GCC_STRINGSTREAM_H_
