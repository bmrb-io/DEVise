/* Any program that uses the Range Buffer Manager (call the managing program) *
 * is responsible for creating, managing and deleting objects. By creating an *
 * object, an integer (typedef int RBMObject) will be generated that uniquely *
 * identifies this object until it is deleted. This integer will be used      *
 * throughout the lifetime of this object. Given this integer, the managing   *
 * program should be able to uniquely identify an RBMObjectInfo, which keeps  *
 * all highest level info about this object.				      *

 * Currently, my testing program uses the function findObjectInfo() below to  *
 * achieve this identifying purpose. This is for testing purpose ONLY. Anyone *
 * who uses Range Buffer Manager should replace this function with something  *
 * meaningful in the context of his/her program!			      */

#include "RBMObject.h"

/* The managing program (defined in the above comment line 1) is responsible */
/* for createing the global RBMObjectInfo for testing purpose. */

/* There are two objects used in the current testing program. */

extern RBMObjectInfo globalRBMObjectInfo_1;
extern RBMObjectInfo globalRBMObjectInfo_2;

RBMObjectInfo *findObjectInfo(RBMObject objectId)
{
    if (objectId == 1)
    {
	/* Note: TEST_OBJECTID_1 = 1. This is object 1 used in current test. */
    	return &globalRBMObjectInfo_1;
    }
    else
    {
	/* Note: TEST_OBJECTID_2 = 2. This is object 2 used in current test. */
	assert (objectId == 2);
	return &globalRBMObjectInfo_2;
    } 
}
