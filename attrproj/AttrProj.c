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

#include "AttrProj.h"
#include "ApParseCat.h"
#include "Util.h"
#include "TData.h"
#include "AttrList.h"

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
	DO_DEBUG(printf("AttrProj::AttrProj()\n"));

//TEMPTEMP  do something with return value
	ParseCat(schemaFile, dataFile, _tDataP);

	_recBufSize = _tDataP->RecSize();
	_recBuf = new char[_recBufSize];

	_projectionCount = 1;/*TEMPTEMP*/
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
 * function: AttrProj::CreateRecordList
 * Creates a record list (VectorArray) that can hold the projected
 * records specified in the attribute projection file.
 */
DevStatus
AttrProj::CreateRecordList(VectorArray *&vecArrayP)
{
	DO_DEBUG(printf("AttrProj::CreateRecordList()\n"));

	DevStatus	result = StatusOk;

	vecArrayP = new VectorArray(_projectionCount);
	vecArrayP->Init(0, _tDataP->GetAttrList()->NumAttrs());/*TEMPTEMP*/

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
				//printf("        %d\n", intVal);/*TEMPTEMP*/
				vectorP->value[attrNum] = (double) intVal;
				break;

			case FloatAttr:
				floatVal = *(float *)(_recBuf + attrInfoP->offset);
				//printf("        %f\n", floatVal);/*TEMPTEMP*/
				vectorP->value[attrNum] = (double) floatVal;
				break;

			case DoubleAttr:
				doubleVal = *(double *)(_recBuf + attrInfoP->offset);
				//printf("        %f\n", doubleVal);/*TEMPTEMP*/
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
	}

	return result;
}

/*============================================================================*/
