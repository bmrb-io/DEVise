#ifndef _RADIXCUBE_H_
#define _RADIXCUBE_H_

// (C) 1998- by Kevin S. Beyer

#include "Iterator.h"
#include "DteProc.h"
#include <vector>


class RadixCubeProc
: public DteThread
{
public:

  RadixCubeProc(Iterator* iter, vector<int> dimCard, int measures,
                int minCount, bool doDedup);

  virtual ~RadixCubeProc();

  virtual void run();

protected:

  Iterator* iter;

  bool doDedup;

private:

  // N/A
  RadixCubeProc(const RadixCubeProc& x);
  RadixCubeProc& operator=(const RadixCubeProc& x);
};


#endif // _RADIXCUBE_H_
