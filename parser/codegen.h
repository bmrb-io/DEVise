/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4  1995/09/23  00:03:52  ravim
  Parser changed to ANSI style - g++ now used uniformly

  Revision 1.3  1995/09/22 15:56:20  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:35:16  jussi
  Added CVS header.
*/

#ifndef CODEGEN_H
#define CODEGEN_H

void CodeGen(MappingRec *rec, FILE *mapFile);

#endif
