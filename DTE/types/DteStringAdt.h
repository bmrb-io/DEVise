#ifndef _DTESTRINGADT_H_
#define _DTESTRINGADT_H_

// (C) 1999- by Kevin S. Beyer

#include "DteAdt.h"
#include <string>

class DteStringAdt
: public DteAdt
{
protected:

  int maxLength;

public:

  typedef const char* BasicType;
  typedef string ManagedType;

  const static int typeID = 3;
  const static char* const typeName = "string";

  //kb: keep this??
  //DteStringAdt() : DteAdt(true, 1) { maxLength = 0; }

  DteStringAdt(int maxLen, bool nullable = false) : DteAdt(nullable, 1)
    { maxLength = maxLen; }

  virtual ~DteStringAdt() {}

  virtual int getTypeID() const { return typeID; }

  virtual string getTypeName() const { return typeName; }

  virtual string getTypeSpec() const {
    ostringstream s;
    s << typeName << '(' << maxLength << ')';
    return s.str();
  }

  virtual DteAdt* clone() const
    { return new DteStringAdt(maxLength, nullable); }

  static char* cast(void* x) { return (char*)x; }
  static const char* cast(const void* x) { return (const char*)x; }

  static Type* getTypePtr(char* x) { return x; }
  static const Type* getTypePtr(string& x) { return x.c_str(); }

  virtual int copyNonNull(const void* x, char* to, int size) const {
    int len = strlen(cast(x)) + 1;
    if( size < len ) return -1;
    memcpy(to, x, len);
    return len;
  }

  virtual void deallocate(void* x) const {
    delete [] cast(x);
  }

  static Type* allocateCopy(const char* x) {
    const char* c1 = cast(x);
    int len = strlen(c1);
    char* c2 = new char[len+1];
    strcpy(c2, c1);
    return c2;
  }

  static Type* allocateCopy(const string& x) {
    return allocateCopy(x.c_str());
  }

  virtual Type* allocateCopy(const void* x) const {
    return allocateCopy(cast(x));
  }

  virtual bool canCompare() const { return true; }

  virtual int compareNonNull(const void* x, const void* y) const
    { return strcmp(cast(x), cast(y)); }

  virtual bool canHash() const { return true; }

  virtual int hashNonNull(const void* x) const { 
    // stolen from g++/stl_hash_fun.h
    int h = 0;
    const char* s = cast(x);
    for ( ; *s; ++s) {
      h = 5*h + *s;
    }
    return h;

    // stolen from java.lang.String
    //     if (len < 16) {
    //       for (int i = len ; i > 0; i--) {
    //         h = (h * 37) + val[off++];
    //       }
    //     } else {
    //       // only sample some characters
    //       int skip = len / 8;
    //       for (int i = len ; i > 0; i -= skip, off += skip) {
    //         h = (h * 39) + val[off];
    //       }
    //     }
    //     return h;
  }

  // Strings don't need to swizzle
  virtual void swizzle(void* x, char* page) const {}
  virtual void unswizzle(void* x, char* page) const {}

  virtual void printNonNull(ostream& s, const void* x) const {
    const char* cp = cast(x);
    int len = strlen(cp);
    if( len > maxLength ) {
      cerr << "warning: string truncated from "
           << len << " to " << maxLength << " bytes\n";
      len = maxLength;
    }
    // using double quotes because devise/tcl needs them
    s << '\"';
    for(int i = 0 ; i < len ; i++) {
      if( cp[i] == '\\' || cp[i] == '\"' ) {
        s << '\\';
      }
      s << cp[i];
    }
    s << '\"';
  }

  virtual void toAscii(ostream& s, const void* x) const {
    DteStringAdt::printNonNull(s, x);
  }

protected:

  int readQuoted(istream& s, char* to, int size, char endQuote) const {
    int space = maxLength;
    if( space > size ) {
      space = size;
    }
    int len = 0;
    char c;
    s.get(c);
    while( s && c != endQuote && len < space ) {
      if( c == '\\' ) {
        s.get(c);
      }
      to[len++] = c;
      s.get(c);
    }
    if( c != endQuote && len == maxLength ) { // string too long
      cerr << "warning: string too long (max " << maxLength
        << " chars) at offset " << s.tellg() << endl;
      while( s && c != endQuote ) {
        if( c == '\\' ) {
          s.get(c);
        }
        s.get(c);
      }
    }
    if( !s ) return -2;
    if( len < size ) {
      to[len++] = '\0';
      return len;
    }
    return -1;
  }

public:

  virtual int fromAscii(istream& s, char* to, int size) const {
    char c;
    s.ipfx(0);                  // skip ws
    s.get(c);
    if( !s ) return -2;

    switch( c ) {
    case '\'':
      return readQuoted(s, to, size, '\'');
    case '\"':
      return readQuoted(s, to, size, '\"');
    case '{':
      return readQuoted(s, to, size, '}');
    }

    // read space-separated
    int space = maxLength;
    if( space > size ) {
      space = size;
    }
    int len = 0;
    while( s && !isspace(c) && len < space ) {
      if( c == '\\' ) {
        s.get(c);
      }
      to[len++] = c;
      s.get(c);
    }
    if( !isspace(c) && len == maxLength ) { // string too long
      cerr << "warning: string too long (max " << maxLength
        << " chars) at offset " << s.tellg() << endl;
      while( s && !isspace(c) ) {
        if( c == '\\' ) {
          s.get(c);
        }
        s.get(c);
      }
    }
    if( !s ) return -2;
    if( len < size ) {
      to[len++] = '\0';
      return len;
    }
    return -1;
  }

  virtual void toBinary(ostream& s, const void* x) const {
    int4 len = strlen(cast(x));
    if( len > maxLength ) {
      cerr << "warning: string truncated from "
           << len << " to " << maxLength << " bytes\n";
      len = maxLength;
    }
    s.write(&len, sizeof(int4));
    s.write(x, len);
  }

  virtual int fromBinary(istream& s, char* to, int size) const {
    int4 len;
    s.read(&len, sizeof(int4));
    assert(len <= maxLength);
    if( len < size ) {
      s.read(to, len);
      to[len] = '\0';
      return len + 1;
    }
    return -1;
  }

  // This code writes strings in faux-unicode
  virtual void toNet(ostream& s, const void* x) const {
    const char* cp = cast(x);
    int4 len = strlen(cp);
    if( len > maxLength ) {
      cerr << "warning: string truncated from "
           << len << " to " << maxLength << " bytes\n";
      len = maxLength;
    }
    int4 nlen = htonl(len);
    s.write(&nlen, sizeof(int4));
    for(int i = 0 ; i < len ; i++) {
      s.put('\0');
      s.put(cp[i]);
    }
  }

  virtual int fromNet(istream& s, char* to, int size) const {
    int4 len;
    s.read(&len, sizeof(int4));
    len = ntohl(len);
    assert(len <= maxLength);
    if( len < size ) {
      char c;
      for(int i = 0 ; i < len ; i++) {
        s.get(c);
        assert(c == '\0');
        s.get(to[i]);
      }
      to[len] = '\0';
      return len + 1;
    }
    return -1;
  }

  virtual int  getMaxSize() const      { return maxLength; }
  virtual bool isVariableSize() const  { return true; }

private:

  // N/A
  DteStringAdt(const DteStringAdt& x);
  DteStringAdt& operator=(const DteStringAdt& x);
};


#endif // _DTESTRINGADT_H_
