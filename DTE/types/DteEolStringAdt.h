#ifndef _DTEEOLSTRINGADT_H_
#define _DTEEOLSTRINGADT_H_

// (C) 1999- by Kevin S. Beyer

// This class is a bit of a kludge.  The intention is that this class
// behaves exactly like a string, except when reading and writing to
// ascii files.  This class is used to enable reading of the current
// catalog tables.

class DteEolStringAdt
: public DteStringAdt
{
  char term;

public:

  //const static int typeID = 8;
  const static char* const typeName = "eolstring";

  DteEolStringAdt(int maxlen, char terminator = '\n', bool nullable = false)
    : DteStringAdt(maxlen, nullable), term(terminator) {}

  virtual ~DteEolStringAdt() {}

  // return String typeID
  //int getTypeID() const { return typeID; }

  // EolString returns "string" for its TypeName
  //string getTypeName() const { return typeName; }

  string getTypeSpec() const {
    ostringstream s;
    s << typeName << '(' << maxLength << ",'" << term << "')";
    return s.str();
  }

  virtual DteAdt* clone() const {
    return new DteEolStringAdt(maxLength, term, nullable);
  }

  // use String::print
  //virtual void print(ostream& s, const void* x) const {
  //}

  virtual void toAscii(ostream& s, const void* x) const {
    const char* cp = cast(x);
    int len = strlen(cp);
    if( cp[len-1] == term ) len--;
    for(int i = 0 ; i < len ; i++) {
      char c = cp[i];
      if( c == '\\' || c == term ) {
        s << '\\';
      }
      s << c;
    }
    s << term;
  }

  // EolStrings can have embedded terminators if preceeded by '\'
  virtual int fromAscii(istream& s, char* to, int size) const {
    s.ipfx(0);                  // skip ws
    int used = readQuoted(s, to, size - 2, term);
    if( used < 0 ) return used;
    if( used == maxLength + 1 ) {
      cerr << "warning: EolString too long" << endl;
    }
    to[used-1] = ';';
    to[used] = '\0';
    return used + 1;
    /*
    int space = maxLength - 1;
    if( space > size ) {
      space = size;
    }
    s.get(to, space-1, term);
    char c;
    s.get(c);
    if( !s ) return -2;
    if( c != term ) {           // string too long
      cerr << "warning: EolString too long (max " << maxLength
        << " chars) at offset " << s.tellg() << endl;
      while(s && s.get() != term ) {
        // nada
      }
    }
    int len = strlen(to);
    to[len++] = term;
    to[len++] = '\0';
    return len;
    */
  }

private:

  DteEolStringAdt(const DteEolStringAdt& x);
  DteEolStringAdt& operator=(const DteEolStringAdt& x);
};

#endif // _DTEEOLSTRINGADT_H_
