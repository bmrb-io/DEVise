#ifndef _MQLERROR_H_
#define _MQLERROR_H_

// (C) 1999- by Kevin S. Beyer

#include <string>
//kb: make me: #include "DteError.h"

class MqlError
{
  string msg;

public:

  MqlError(const string& msg) : msg(msg) {}

  const string& getMessage() const { return msg; }
};



#endif // _MQLERROR_H_
