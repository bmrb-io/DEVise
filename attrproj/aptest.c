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
 */

#include <stdio.h>
#include <sys/types.h>

#include <ParseCat.h>

/*------------------------------------------------------------------------------
 * function: main
 * description
 */
int				/* description */
main(
	int		argc,
	char **	argv)

{													/* start main */
	char *		schemaFile = "/p/devise/schema/3d_test.schema";

	if (argc > 1)
	{
		schemaFile = argv[1];
	}

/*TEMPTEMP*/fprintf(stderr, "Processing schema file: %s\n", schemaFile);

#if 1 /*TEMPTEMP*/
//TEMPTEMP	do something with return value
	ParseCat(schemaFile);
#endif
}													/* end main */

/*============================================================================*/
