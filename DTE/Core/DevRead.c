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
  Implementation of DevRead (Devise Reader) class.
 */

/*
    $Id$
    $Log$
    Revision 1.8  1997/03/14 18:36:09  donjerko
    Making space for the SQL UNION operator.

    Revision 1.7  1997/03/02 00:01:45  donjerko
    Changed RecId int recId.

    Revision 1.6  1997/02/03 04:11:26  donjerko
    Catalog management moved to DTE

    Revision 1.5  1996/12/16 11:13:03  kmurli
    Changes to make the code work for separate TDataDQL etc..and also changes
    done to make Aggregates more robust

    Revision 1.4  1996/12/15 06:41:03  donjerko
    Added support for RTree indexes

    Revision 1.3  1996/12/03 20:25:37  jussi
    Updated to reflect new TData interfaces.

    Revision 1.2  1996/11/23 22:10:19  jussi
    Minor change to reflect new TData::GetRecs() parameter list.

    Revision 1.1  1996/11/23 02:00:35  donjerko
    Restructered DTE directory

    Revision 1.2  1996/11/21 19:13:32  wenger
    Fixed more compile warnings; updated devise.dali to match current
    command-line flags.

    Revision 1.1  1996/11/18 18:08:46  donjerko
    Initial DTE release.

    Revision 1.3  1996/08/15 19:55:01  wenger
    Added 'pure' targets for attrproj and devread; fixed some dynamic
    memory problems.  Found some bugs while demo'ing for soils science
    people.

    Revision 1.2  1996/08/14 20:00:47  flisakow
        Need a devread directory for each m/c arch.

    Revision 1.1  1996/08/14 19:04:29  flisakow
      Checkin of DevRead class, a near duplicate of the AttrProj class,
      it is an interface to reading data from Devise files.

 */

#define _DevRead_c_

#include <stdio.h>
#include <string.h>
#include <strstream.h>
#include <time.h>

#include "machdep.h"
#include "DevRead.h"
#include "ApParseCat.h"
#include "Util.h"
#include "TData.h"
#include "AttrList.h"
#include "DataSeg.h"

#if !defined(lint) && defined(RCSID)
static char        rcsid[] = "$RCSfile$Revision$";
#endif

static char *srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DevRead::DevRead
 * DevRead constructor.
 */
DevStatus
DevRead::Open(char *schemaFile, char *dataFile)
{
    DevStatus result = StatusOk;

    DO_DEBUG(printf("DevRead::Open(%s, %s)\n", schemaFile, dataFile));

    // Provision for having the schema in the data file.
    if ((schemaFile == NULL) || !*schemaFile)
        schemaFile = dataFile;

    // strdups because TData destructor will try to free all of these
    // strings -- make sure they're dynamic.
    schemaFile = strdup(schemaFile);
    dataFile = strdup(dataFile);

    DataSeg::Set(dataFile, dataFile, 0, 0);

    char *schemaName = ApParseCat(schemaFile, dataFile, _tDataP);
    DOASSERT(schemaName != NULL, "Can't parse schema.");

    _recBufSize = _tDataP->RecSize();
    _recBuf = new char[_recBufSize];

    _numAttr = computeNumFlds();
    _tDataP->HeadID(_nxtRecId);
    _tDataP->LastID(_lastRecId);

    _tDataP->InitGetRecs(_nxtRecId, _lastRecId);

    return(result);
}

String * DevRead::getOrderingAttrib(){
	
	String * result = NULL;
    AttrList *attrListP = _tDataP->GetAttrList();

    attrListP->InitIterator();
    while (attrListP->More())
    {
        AttrInfo *attrInfoP = attrListP->Next();

		if (attrInfoP->isSorted){
			// Good Sorted return the result..
			result = new String(attrInfoP->name);
			break;
		}
    }
    attrListP->DoneIterator();

    return(result);

}
void DevRead::reset(int lowRid, int highRid){
	long unsigned int first, last;
    _tDataP->HeadID(first);
    _tDataP->LastID(last);
    assert(lowRid >= (int) first);
    assert(highRid <= (int) last);
    _tDataP->InitGetRecs(lowRid, highRid);
    _nxtRecId = lowRid;
    _lastRecId = highRid;
}

void DevRead::setOffset(Offset offset){
	reset(offset.getOffset(), offset.getOffset());
}

/*------------------------------------------------------------------------------
 * Close the currently open schema/data files, if any.
 */
DevStatus
DevRead::Close()
{
    DO_DEBUG(printf("DevRead::Close()\n"));

    DevStatus result = StatusOk;

    delete _tDataP;
    _tDataP = NULL;

    delete [] _recBuf;
    _recBuf = NULL;

    _numAttr = 0;
    
    return(result);
}

/*------------------------------------------------------------------------------
 * function: DevRead::computeNumFlds
 * Returns information about the number of fields
 */
int DevRead::computeNumFlds()
{
    if (!_tDataP)
        return(0);

    DO_DEBUG(printf("DevRead::computeNumFlds()\n"));

    int attrCount = 0;

    AttrList *attrListP = _tDataP->GetAttrList();

    attrListP->InitIterator();
    while (attrListP->More())
    {
        AttrInfo *attrInfoP = attrListP->Next();
        attrCount++;
    }
    attrListP->DoneIterator();

    return attrCount + 1;	// Add the RecId field
}

/*------------------------------------------------------------------------------
 * function: DevRead::getTypeIDs
 * Returns information about the types (as Strings)
 */
String *DevRead::getTypeIDs()
{
    DO_DEBUG(printf("DevRead::getTypeIDs()\n"));

    if (!_tDataP)
        return(NULL);

    String *result = new String [_numAttr];

    result[0] = "int";	// First field is the RecId
    int j = 1;

    AttrList *attrListP = _tDataP->GetAttrList();

    attrListP->InitIterator();
    ostrstream tmp;
    int len;
    char* tmpc;
    while (attrListP->More())
    {
        AttrInfo *attrInfoP = attrListP->Next();

        switch (attrInfoP->type) {

        case IntAttr:
            result[j++] = "int";
            break;

        case FloatAttr:
            result[j++] = "double";
            break;

        case DoubleAttr:
            result[j++] = "double";
            break;

        case StringAttr:
	   	  len = attrInfoP->length;
		  assert(len > 0);
		  tmp << "string" << len << ends;
		  tmpc = tmp.str();
            result[j++] = String(tmpc);
		  delete tmpc;
            break;

        case DateAttr:
            result[j++] = "date";
            break;

        default:
            result[j++] = "<Unknown Type>";
            break;
        }
    }
    attrListP->DoneIterator();

    return(result);
}

/*------------------------------------------------------------------------------
 * function: DevRead::getAttributeNames
 * Returns information about the names of attributes (as Strings)
 */
String *DevRead::getAttributeNames()
{
    DO_DEBUG(printf("DevRead::getAttributeNames()\n"));

    if (!_tDataP)
        return(NULL);

    String *result = new String [_numAttr];
    result[0] = "recId";
    int j = 1;

    AttrList *attrListP = _tDataP->GetAttrList();

    attrListP->InitIterator();
    while (attrListP->More())
    {
        AttrInfo *attrInfoP = attrListP->Next();

        if (attrInfoP->name)
            result[j++] = attrInfoP->name;
        else
            result[j++] = "<UnNamed>";
    }
    attrListP->DoneIterator();

    return(result);
}

/*------------------------------------------------------------------------------
 * function: DevRead::getNext
 * Read a record, return NULL when done.
 */
Tuple *DevRead::getNext()
{
    Tuple *result = NULL;

    DO_DEBUG(printf("DevRead::getNext()\n"));

    if (_tDataP && (_nxtRecId <= _lastRecId)) {
        TData::TDHandle handle = _tDataP->InitGetRecs(_nxtRecId, _nxtRecId);

        int            dataSize;
        int            numRecs;

        if (_tDataP->GetRecs(handle, _recBuf, _recBufSize, _nxtRecId,
                             numRecs, dataSize))
        {
            result = new Tuple[_numAttr];

            AttrList *attrListP = _tDataP->GetAttrList();

            int tmp = _nxtRecId;			// add RecId at the first place 
	       result[0] = (Type*) new IInt(tmp);
            int       attrNum = 1;

            attrListP->InitIterator();
            while (attrListP->More())
            {
                AttrInfo *attrInfoP = attrListP->Next();
                char *attrVal = _recBuf + attrInfoP->offset;

                switch (attrInfoP->type) {

                case IntAttr: {
                        int tmp;
                        memcpy(&tmp, attrVal, sizeof(int));
                        result[attrNum] = (Type*) new IInt(tmp);
                        break;
                    }

                case FloatAttr: {
                        float tmp;
                        memcpy(&tmp, attrVal, sizeof(float));
                        result[attrNum] = (Type*) new IDouble((double) tmp);
                        break;
                    }

                case DoubleAttr: {
                        double tmp;
                        memcpy(&tmp, attrVal, sizeof(double));
                        result[attrNum] = (Type*) new IDouble(tmp);
                        break;
                    }

                case StringAttr:
                    result[attrNum] = (Type*) new IString(attrVal);
                    break;

                case DateAttr: {
                        time_t tmp;
                        memcpy(&tmp, attrVal, sizeof(time_t));
                        result[attrNum] = (Type*) new IDate(tmp);
                        break;
                    }

                default:
                    result[attrNum] = NULL;
                    break;
                }

                attrNum++;
            }
            attrListP->DoneIterator();

            _nxtRecId++;    // Prepare to read the next record
        }

        _tDataP->DoneGetRecs(handle);
    }

    return(result);
}

/*============================================================================*/
