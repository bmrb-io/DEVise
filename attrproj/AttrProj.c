/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of AttrProj (attribute projection) class.
 */

/*
  $Id$

  $Log$
 */

#define _AttrProj_c_

#include <stdio.h>

#include "AttrProj.h"
#include "ApParseCat.h"


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: AttrProj::AttrProj
 * AttrProj constructor.
 */
AttrProj::AttrProj(char *schemaFile, char *attrProjFile, char *dataFile)
{
/*TEMPTEMP*/fprintf(stderr, "Processing schema file: %s\n", schemaFile);

#if 1 /*TEMPTEMP*/
//TEMPTEMP  do something with return value
	ParseCat(schemaFile, dataFile);
#endif
}

/*------------------------------------------------------------------------------
 * function: AttrProj::~AttrProj
 * AttrProj destructor.
 */
AttrProj::~AttrProj()
{
}

/*============================================================================*/
