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
  Revision 1.4  1996/04/30 18:53:40  wenger
  Attrproj now generates a single projection of all attributes of the
  real data.

  Revision 1.3  1996/04/30 15:31:57  wenger
  Attrproj code now reads records via TData object; interface to Birch
  code now in place (but not fully functional).

  Revision 1.2  1996/04/25 19:25:25  wenger
  Attribute projection code can now parse a schema, and create the
  corresponding TData object.

  Revision 1.1  1996/04/22 18:01:55  wenger
  First version of "attribute projection" code.  The parser (with
  the exception of instantiating any TData) compiles and runs.

 */

#include <stdio.h>
#include <sys/types.h>

#include "AttrProj.h"
#include "ApInit.h"

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
	int			result = 0;
	char *		projectionFile = "/u/w/e/wenger/public/attrproj/projection";
	char *		schemaFile = "/p/devise/schema/3d_test.schema";

	if (argc > 1)
	{
		schemaFile = argv[1];
	}

	if (argc > 2)
	{
		dataFile = argv[2];
	}

	if (argc > 3)
	{
		projectionFile = argv[3];
	}

	/* Initialize Devise stuff for command-line arguments. */
	Init::DoInit(argc, argv);

	AttrProj		ap(schemaFile, projectionFile, dataFile);
	RecId			firstId;
	RecId			lastId;
	RecId			recId;
	VectorArray *	vecArrayP;

	ap.CreateRecordList(vecArrayP);

	ap.FirstRecId(firstId);
	ap.LastRecId(lastId);

	for (recId = firstId; recId <= lastId; recId++)
	{
		printf("\nRecord %d:\n", (int) recId);

		ap.ReadRec(recId, *vecArrayP);
		int			vecCount = vecArrayP->GetVecCount();

		int		vecNum;
		for (vecNum = 0; vecNum < vecCount; vecNum++)
		{
			Vector *	vecP = vecArrayP->GetVector(vecNum);
			int			vecDim = vecP->dim;
			int			count;

			printf("  Projection %d:\n   ", vecNum);
			for (count = 0; count < vecDim; count++)
			{
				printf(" %f", vecP->value[count]);
			}
			printf("\n");
		}
	}

	delete vecArrayP;

	return result;
}													/* end main */

/*============================================================================*/
