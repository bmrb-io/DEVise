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
  Test driver (main program) for "attribute projection" software.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/04/22 18:01:55  wenger
  First version of "attribute projection" code.  The parser (with
  the exception of instantiating any TData) compiles and runs.

 */

#include <stdio.h>
#include <sys/types.h>

#include "AttrProj.h"
#include "ApInit.h"/*TEMPTEMP?*/

/*------------------------------------------------------------------------------
 * function: main
 * description
 */
int				/* description */
main(
	int		argc,
	char **	argv)

{													/* start main */
	char *		dataFile = "/p/devise/dat/3d_test.dat";
	char *		schemaFile = "/p/devise/schema/3d_test.schema";

	if (argc > 1)
	{
		schemaFile = argv[1];
	}

	if (argc > 2)
	{
		schemaFile = argv[2];
	}

	Init::DoInit(argc, argv);/*TEMPTEMP?*/
	AttrProj	ap(schemaFile, NULL, dataFile);
}													/* end main */

/*============================================================================*/
