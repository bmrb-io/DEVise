/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

#ifndef SBMINIT_H
#define SBMINIT_H

#ifndef __GNUG__
using namespace std;
#endif

extern char* bulk_temp_file_name;

extern int _debug;

const int poolSize = 1024;
const int pageSize = 4096;

class MemMgr;
class CacheMgr;

extern MemMgr *memMgr;

extern CacheMgr *cacheMgr;

#endif
