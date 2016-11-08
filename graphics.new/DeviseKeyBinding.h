/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2.42.1  2005/09/06 21:20:17  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.2  1997/03/28 16:09:43  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef _DEVISEKEYBINDING_H_
#define _DEVISEKEYBINDING_H_

#include "MapIntToInt.h"
#include "DeviseKey.h"

class DeviseKeyBinding
{
  public:

    DeviseKeyBinding();

    ~DeviseKeyBinding() {}

    // returns previous action
    int Bind(int key, int action);
    int Unbind(int key);

  protected:

    enum { MAX_BINDINGS = 103 };

    struct Binding {
	int key;
	int action;
    };

    static Binding _default_binding[];

    MapIntToInt _key_binding;

  private:

    // N/A
    DeviseKeyBinding(const DeviseKeyBinding& other);
    DeviseKeyBinding& operator=(const DeviseKeyBinding& other);
};




#endif // _DEVISEKEYBINDING_H_
