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

/*
  $Id$

  $Log$
  Revision 1.15  1997/12/04 04:05:14  donjerko
  *** empty log message ***

  Revision 1.14  1997/11/05 00:19:41  donjerko
  Separated typechecking from optimization.

  Revision 1.13  1997/09/05 22:20:09  donjerko
  Made changes for port to NT.

  Revision 1.12  1997/08/21 21:04:26  donjerko
  Implemented view materialization

  Revision 1.11  1997/08/14 02:08:53  donjerko
  Index catalog is now an independent file.

  Revision 1.10  1997/08/12 19:58:42  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.9  1997/07/30 21:39:18  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.8  1997/07/22 15:00:54  donjerko
  *** empty log message ***

  Revision 1.7  1997/06/30 23:04:56  donjerko
  CVS:

  Revision 1.6  1997/06/21 22:48:01  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.5  1997/06/16 16:04:44  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "ParseTree.h"
//#include "site.h"
#include "catalog.h"
#include "Interface.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/util/Del.h"
#include "ExecOp.h"

//kb: move this strdup
char* strdup(const char* s)
{
  char* t = new char[strlen(s)+1];
  strcpy(t, s);
  return t;
}

Iterator* ISchemaParse::createExec(){
        Del<Interface> interf;
	TRY(interf = ROOT_CATALOG.createInterface(tableName), NULL);
	TRY(const ISchema* schema = interf->getISchema(), NULL);
        ostringstream out;
        out << *schema;
        string schstr = out.str();
        char* cp = strdup(schstr.c_str());
        return new SingleAnswerIt(cp, DteStringAdt(schstr.length()));
}
