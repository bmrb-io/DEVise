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

//#define DEBUG

#include <stdio.h>
#include <string.h>

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

    DataSeg::Set(schemaFile, dataFile, 0, 0);

    char *schemaName = ApParseCat(schemaFile, dataFile, _tDataP);
    DOASSERT(schemaName != NULL, "Can't parse schema.");

    _recBufSize = _tDataP->RecSize();
    _recBuf = new char[_recBufSize];

    _numAttr = computeNumFlds();
    _tDataP->HeadID(_nxtRecId);
    _tDataP->LastID(_lastRecId);

    _tDataP->InitGetRecs(_nxtRecId, _lastRecId, RecIdOrder);

    return(result);
}

void DevRead::reset(int lowRid, int highRid){
	long unsigned int first, last;
    _tDataP->HeadID(first);
    _tDataP->LastID(last);
    assert(lowRid >= (int) first);
    assert(highRid <= (int) last);
    _tDataP->InitGetRecs(lowRid, highRid, RecIdOrder);
    _nxtRecId = lowRid;
    _lastRecId = highRid;
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
    while (attrListP->More())
    {
        AttrInfo *attrInfoP = attrListP->Next();

        switch (attrInfoP->type) {

        case IntAttr:
            result[j++] = "int";
            break;

        case FloatAttr:
            result[j++] = "float";
            break;

        case DoubleAttr:
            result[j++] = "double";
            break;

        case StringAttr:
            result[j++] = "string";
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
    result[0] = "RecId";
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
     //   _tDataP->InitGetRecs(_nxtRecId, _nxtRecId, RecIdOrder);

        int            dataSize;
        int            numRecs;

        if (_tDataP->GetRecs(_recBuf, _recBufSize, _nxtRecId,
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
                        // NEW DATE (new date)
                        // result[attrNum] = (Type*) new IDate(tmp);
                        result[attrNum] = NULL;
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
    }

    return(result);
}

/*============================================================================*/
