#ifndef _GCC_STRINGSTREAM_H_
#define _GCC_STRINGSTREAM_H_

// (C) 1998- Kevin S. Beyer

//kb: perhaps sysdep.h can go away if we use cygwin/egcs for windows

/*
  These classes makes the old strstreambuf class work as a
  stringstreams.  These class should be removed for compilers that
  have proper stringstream support.
*/

#ifdef __GNUG__

#include <strstream.h>
#include <string>

class ostringstreambase : virtual public ios
{
protected:

  ostringstreambase() { init(&__my_sb); }

public:

  strstreambuf* rdbuf() { return &__my_sb; }

private:

  strstreambuf __my_sb;

  ostringstreambase(const ostringstreambase& x);
  ostringstreambase& operator=(const ostringstreambase& x);
};


class ostringstream : public ostringstreambase, public ostream
{
public:

  ostringstream() {}

  //ostringstream(const string& s) { *this << s; }

  string str() /* const */ {
    int len = rdbuf()->pcount();
    const char* cp = rdbuf()->str();
    rdbuf()->freeze(0);         // allow more writes
    return string(cp, len);
  }

private:

  ostringstream(const ostringstream& x);
  ostringstream& operator=(const ostringstream& x);
};


class istringstream : virtual public ios, public istream
{
public:

  istringstream(const string& s)
    : myString(s), __my_sb(myString.c_str(), myString.length()) {
    init(&__my_sb);
  }

  strstreambuf* rdbuf() { return &__my_sb; }

private:

  string myString;
  strstreambuf __my_sb;

  istringstream(const istringstream& x);
  istringstream& operator=(const istringstream& x);
};


class stringstream : public ostringstreambase, public iostream
{
public:

  stringstream() {}

  string str() /* const */ {
    int len = rdbuf()->pcount();
    const char* cp = rdbuf()->str();
    rdbuf()->freeze(0);         // allow more writes
    return string(cp, len);
  }

private:

  stringstream(const stringstream& x);
  stringstream& operator=(const stringstream& x);
};


#endif // only gcc

#endif // _GCC_STRINGSTREAM_H_
