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
  Header file for AttrProj (attribute projection) class.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1996/05/01 16:19:35  wenger
  Initial version of code to project attributes now working.

  Revision 1.2  1996/04/30 15:31:52  wenger
  Attrproj code now reads records via TData object; interface to Birch
  code now in place (but not fully functional).

  Revision 1.1  1996/04/25 19:25:23  wenger
  Attribute projection code can now parse a schema, and create the
  corresponding TData object.

 */

#ifndef _AttrProj_h_
#define _AttrProj_h_


#include "DeviseTypes.h"
#include "RecId.h"
#include "VectorArray.h"
#include "ProjectionList.h"

class TData;

class AttrProj
{
public:
	AttrProj(char *schemaFile, char *attrProjFile, char *dataFile);
	~AttrProj();

	/* Get the first and last record ID for the data. */
	DevStatus FirstRecId(RecId &recId);
	DevStatus LastRecId(RecId &recId);

	/* Get information about the number, cardinality, and size (in bytes)
	 * of the records that will be returned when data is read. */
	DevStatus GetDataSize(int &projCount, const int *&attrCounts,
		const int *&projSizes);

	/* Create a VectorArray properly set up to hold the attribute projections
	 * specified in the attribute projection file. */
	DevStatus CreateRecordList(VectorArray *&vecArray);

	/* Read a record from the data, projecting its attributes onto all
	 * of the combinations specified in the attribute projection file.
	 * vecArray must have already been set up. */
	DevStatus ReadRec(RecId recId, VectorArray &vecArray);

private:
	DevStatus ParseProjection(char *attrProjFile);

	TData *		_tDataP;

	char *		_recBuf;
	int			_recBufSize;

	ProjectionList	_projList;
	int *		_attrCounts;	// For GetDataSize.
	int *		_projSizes;		// For GetDataSize.
};


#endif /* _AttrProj_h_ */

/*============================================================================*/
