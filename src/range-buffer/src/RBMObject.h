#ifndef __OBJECT_H__
#define __OBJECT_H__

/* Kevin's page/record stuff */
#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"

/* Uniquely identify an object, within or across sessions */
/* Given an RBMObject, there is a unique RBMObjectInfo, */
/* which has all the informations about this object */
typedef int RBMObject;

/* A RBMObjectInfo has all the info about an object.		*/
/* Currently it has only 2 pieces of info:			*/
/*	Bounding Box Attribute number 				*/
/* 		and						*/
/*	DteTupleAdt pointer					*/

struct RBMObjectInfo
{
    /* Number of the bounding box attribute. */
    /* E.g., if the object's first attribute is the bbox attr, */
    /* boundingBoxAttrNumber should be 0 then. */
    int boundingBoxAttrNumber;

    /* pointer to DteTupleAdt */
    DteTupleAdt *tupleAdt;
};

#endif
