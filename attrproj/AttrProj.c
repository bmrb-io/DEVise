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
  Revision 1.5  1996/05/14 15:34:48  wenger
  Added GetDataSize method to AttrProj class; removed vector.o from
  AttrProjLib.o; various cleanups.

  Revision 1.4  1996/05/01 16:19:32  wenger
  Initial version of code to project attributes now working.

  Revision 1.3  1996/04/30 18:53:32  wenger
  Attrproj now generates a single projection of all attributes of the
  real data.

  Revision 1.2  1996/04/30 15:31:50  wenger
  Attrproj code now reads records via TData object; interface to Birch
  code now in place (but not fully functional).

  Revision 1.1  1996/04/25 19:25:22  wenger
  Attribute projection code can now parse a schema, and create the
  corresponding TData object.

 */

#define _AttrProj_c_

//#define DEBUG

#include <stdio.h>
#include <string.h>

#include "AttrProj.h"
#include "ApParseCat.h"
#include "Util.h"
#include "TData.h"
#include "AttrList.h"
#include "ProjectionList.h"

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
	DO_DEBUG(printf("AttrProj::AttrProj(%s, %s, %s)\n", schemaFile,
		attrProjFile, dataFile));

	// Provision for having the schema in the data file.
	if ((schemaFile == NULL) || !strcmp(schemaFile, ""))
	{
		schemaFile = dataFile;
	}

//TEMPTEMP  do something with return value
	ParseCat(schemaFile, dataFile, _tDataP);
	ParseProjection(attrProjFile);

	_recBufSize = _tDataP->RecSize();
	_recBuf = new char[_recBufSize];

	_attrCounts = new int[_projList.GetProjCount()];
	_projSizes = new int[_projList.GetProjCount()];

	int		projNum = 0;
	Projection *	projP = _projList.GetFirstProj();
	while (projP != NULL)
	{
		_attrCounts[projNum] = projP->attrCount;
		_projSizes[projNum] = projP->attrCount * sizeof(double);

		projP = _projList.GetNextProj();
		projNum++;
	}
}

/*------------------------------------------------------------------------------
 * function: AttrProj::~AttrProj
 * AttrProj destructor.
 */
AttrProj::~AttrProj()
{
	DO_DEBUG(printf("AttrProj::~AttrProj()\n"));

	delete _tDataP;
	delete [] _recBuf;
	delete [] _attrCounts;
	delete [] _projSizes;
}

/*------------------------------------------------------------------------------
 * function: AttrProj::FirstRecId
 * Get the ID of the first record available.
 */
DevStatus
AttrProj::FirstRecId(RecId &recId)
{
	DO_DEBUG(printf("AttrProj::FirstRecId()\n"));

	DevStatus	result = StatusOk;

	if (!_tDataP->HeadID(recId)) result = StatusFailed;

	return result;
}
/*------------------------------------------------------------------------------
 * function: AttrProj::LastRecId
 * Get the record ID of the last record available.
 */
DevStatus
AttrProj::LastRecId(RecId &recId)
{
	DO_DEBUG(printf("AttrProj::LastRecId()\n"));

	DevStatus	result = StatusOk;

	if (!_tDataP->LastID(recId)) result = StatusFailed;

	return result;
}

/*------------------------------------------------------------------------------
 * function: AttrProj::GetDataSize
 * Returns information about the size of data that will be produced when
 * a record is read.
 */
DevStatus
AttrProj::GetDataSize(int &projCount, const int *&attrCounts,
	const int *&projSizes)
{
	DO_DEBUG(printf("AttrProj::GetDataSize()\n"));

	DevStatus	result = StatusOk;

	projCount = _projList.GetProjCount();
	attrCounts = _attrCounts;
	projSizes = _projSizes;

	return result;
}

/*------------------------------------------------------------------------------
 * function: AttrProj::CreateRecordList
 * Creates a record list (VectorArray) that can hold the projected
 * records specified in the attribute projection file.
 */
DevStatus
AttrProj::CreateRecordList(VectorArray *&vecArrayP)
{
	DO_DEBUG(printf("AttrProj::CreateRecordList()\n"));

	DevStatus	result = StatusOk;

	vecArrayP = new VectorArray(_projList.GetProjCount());

	int		projNum = 0;
	Projection * projP = _projList.GetFirstProj();
	while (projP != NULL)
	{
		vecArrayP->Init(projNum, projP->attrCount);
		projP = _projList.GetNextProj();
		projNum++;
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: AttrProj::ReadRec
 * Read a record from the data source, and project it onto the attribute
 * combinations corresponding to this object.
 */
DevStatus
AttrProj::ReadRec(RecId recId, VectorArray &vecArray)
{
	DO_DEBUG(printf("AttrProj::ReadRec(%d)\n", (int) recId));

	int			dataSize;
	int			numRecs;
	DevStatus	result = StatusOk;

	_tDataP->InitGetRecs(recId, recId, RecIdOrder);

	if (!_tDataP->GetRecs(_recBuf, _recBufSize, recId, numRecs, dataSize, NULL))
	{
		result = StatusFailed;
	}
	else
	{
		AttrList *	attrListP = _tDataP->GetAttrList();
		int		projNum = 0;
		Projection * projP = _projList.GetFirstProj();
		while (projP != NULL)
		{
			Vector *	vectorP = vecArray.GetVector(projNum);
			int		projAttrNum;
			for (projAttrNum = 0; projAttrNum < projP->attrCount;
				projAttrNum++)
			{
				int			attrNum = projP->attrList[projAttrNum];
				AttrInfo *	attrInfoP = attrListP->Get(attrNum);
				double		doubleVal;
				float		floatVal;
				int			intVal;


				switch (attrInfoP->type)
				{
				case IntAttr:
					intVal = *(int *)(_recBuf + attrInfoP->offset);
					DO_DEBUG(printf("        %d\n", intVal));
					vectorP->value[projAttrNum] = (double) intVal;
					break;

				case FloatAttr:
					floatVal = *(float *)(_recBuf + attrInfoP->offset);
					DO_DEBUG(printf("        %f\n", floatVal));
					vectorP->value[projAttrNum] = (double) floatVal;
					break;

				case DoubleAttr:
					doubleVal = *(double *)(_recBuf + attrInfoP->offset);
					DO_DEBUG(printf("        %f\n", doubleVal));
					vectorP->value[projAttrNum] = (double) doubleVal;
					break;

				case StringAttr:
					DOASSERT(false, "Can't deal with string attribute");
					break;

				case DateAttr:
					DOASSERT(false, "Can't deal with date attribute");
					break;

				default:
					DOASSERT(false, "Illegal attribute type");
					break;
				}
			}

			projP = _projList.GetNextProj();
			projNum++;
		}







#if 0
		int			attrCount = attrListP->NumAttrs();
		int			attrNum;
		Vector *	vectorP = vecArray.GetVector(0);

		for (attrNum = 0; attrNum < attrCount; attrNum++)
		{
			AttrInfo *	attrInfoP = attrListP->Get(attrNum);
			double		doubleVal;
			float		floatVal;
			int			intVal;

			switch (attrInfoP->type)
			{
			case IntAttr:
				intVal = *(int *)(_recBuf + attrInfoP->offset);
				DO_DEBUG(printf("        %d\n", intVal));
				vectorP->value[attrNum] = (double) intVal;
				break;

			case FloatAttr:
				floatVal = *(float *)(_recBuf + attrInfoP->offset);
				DO_DEBUG(//printf("        %f\n", floatVal));
				vectorP->value[attrNum] = (double) floatVal;
				break;

			case DoubleAttr:
				doubleVal = *(double *)(_recBuf + attrInfoP->offset);
				DO_DEBUG(//printf("        %f\n", doubleVal));
				vectorP->value[attrNum] = (double) doubleVal;
				break;

			case StringAttr:
				DOASSERT(false, "Can't deal with string attribute");
				break;

			case DateAttr:
				DOASSERT(false, "Can't deal with date attribute");
				break;

			default:
				DOASSERT(false, "Illegal attribute type");
				break;
			}
		}
#endif
	}

	return result;
}

/*------------------------------------------------------------------------------
 * function: AttrProj::ParseProjection
 * Parse the attribute projection file and build up the corresponding
 * data structures.
 */
DevStatus
AttrProj::ParseProjection(char *attrProjFile)
{
	DO_DEBUG(printf("AttrProj::ParseProjection()\n"));

	DevStatus	result = StatusOk;

	FILE *		file = fopen(attrProjFile, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Can't open attribute projection file\n");
		result = StatusFailed;
	}
	else
	{
		const int	bufSize = 256;
		char		buf[bufSize];
		char		separators[] = " \t";

		/* Get each line in the attribute projection file. */
		while (fgets(buf, bufSize, file) != NULL)
		{
			/* TEMPTEMP -- we should look for some kind of comment char. */

			StripTrailingNewline(buf);
			DO_DEBUG(printf("%s\n", buf));

			Projection	projection;
			projection.attrCount = atoi(strtok(buf, separators));
			DO_DEBUG(printf("projection.attrCount = %d\n",
				projection.attrCount));
			projection.attrList = new int[projection.attrCount];

			AttrList *	attrListP = _tDataP->GetAttrList();
			int			attrCount = attrListP->NumAttrs();
			int			projAttrNum = 0;
			char *		token;

			/* Find each attribute specified for this projection. */
			while ((token = strtok(NULL, separators)) != NULL)
			{
				projection.attrList[projAttrNum] = illegalAttr;
				DO_DEBUG(printf("  token = %s", token));

				int			attrNum;

				/* Now find the attribute in the TData corresponding to
				 * the name specified in the projection. */
				for (attrNum = 0; attrNum < attrCount; attrNum++)
				{
					AttrInfo *	attrInfoP = attrListP->Get(attrNum);

					if (!strcmp(token, attrInfoP->name))
					{
						DO_DEBUG(printf(" attrNum = %d\n", attrNum));
						projection.attrList[projAttrNum] = attrNum;
						break;
					}
				}
				DOASSERT(projection.attrList[projAttrNum] != illegalAttr,	
					"Illegal attribute name in attribute projection file");
				projAttrNum++;
			}
			DOASSERT(projAttrNum == projection.attrCount,
				"Incorrect number of attributes in projection file");


			_projList.AddProjection(projection);

		}

		fclose(file);
	}

	return result;
}

/*============================================================================*/
