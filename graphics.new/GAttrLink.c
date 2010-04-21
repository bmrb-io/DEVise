/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2002-2010
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of GAttrLink class.
 */

/*
  $Id$

  $Log$
  Revision 1.4.6.3  2010/04/07 19:06:26  wenger
  Finished implementation of "all" option in GAttrLinks, except for GUI
  to control it.

  Revision 1.4.6.2  2010/04/07 18:10:54  wenger
  The "All" option in GAttrLinks is now working except for commands to
  get and set it.

  Revision 1.4.6.1  2010/04/06 23:54:56  wenger
  Partially implemented the "All" option in GAttrLink -- a leader value
  of "All" will match all follower records (still needs a bunch of cleanup,
  and a command to turn it on and off).

  Revision 1.4  2008/01/24 22:08:32  wenger
  Got rid of a bunch of compile warnings.

  Revision 1.3  2005/12/06 20:04:00  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.2  2003/01/13 19:25:22  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.1.2.3  2005/09/06 22:04:55  wenger
  Added proper const-ness to HashTable.

  Revision 1.1.2.2  2003/09/08 20:53:20  wenger
  Fixed a bug in that the intersection of multiple GAttr links was
  not being computed correctly.

  Revision 1.1.2.1  2002/09/05 19:14:03  wenger
  Implemented GData attribute value links (but not GUI for creating
  them).

 */

using namespace std;

#include <stdio.h>

#include "GAttrLink.h"
#include "Util.h"
#include "DevError.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "Init.h"
#include "Control.h"
#include "AttrList.h"
#include "StringStorage.h"

#define DEBUG 0

template class HashTable<Coord, int>;

/*------------------------------------------------------------------------------
 * function: GAttrLink::GAttrLink
 * Constructor.
 */
GAttrLink::GAttrLink(const char *name, const char *leaderAttrName,
    const char *followerAttrName) :
    MasterSlaveLink(name, VISUAL_GATTR)
{
#if (DEBUG >= 1)
  printf("GAttrLink(0x%p)::GAttrLink(%s, %s, %s)\n", this, name,
      leaderAttrName, followerAttrName);
#endif

  _leaderAttrName = CopyString(leaderAttrName);
  _followerAttrName = CopyString(followerAttrName);

  const int hashTableSize = 1024;
  _selectionValues = new HashTable<Coord, int>(hashTableSize, CoordHash, NULL);

  _enableAll = false;

  _objectValid.Set();
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::~GAttrLink
 * Destructor.
 */
GAttrLink::~GAttrLink()
{
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::~GAttrLink()\n", _name);
#endif

  FreeString(_leaderAttrName);
  _leaderAttrName = NULL;
  FreeString(_followerAttrName);
  _followerAttrName = NULL;

  delete _selectionValues;
  _selectionValues = NULL;
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::SetFlag
 * Set flag (link type).
 */
void
GAttrLink::SetFlag(VisualFlag flag)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::SetFlag(%d)\n", _name, flag);
#endif

  reportErrNosys("Cannot change link type of GData attribute link\n");
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::SetMasterView
 * Set the master/leader view for this link (create the hash table for the
 * attribute values).
 */
void
GAttrLink::SetMasterView(ViewGraph *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::SetMasterView(%s)\n", _name,
      view != NULL ? view->GetName() : "NULL");
#endif

  MasterSlaveLink::SetMasterView(view);

  CheckAttrs();
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::InsertView
 * Insert a new follower view for this link.
 */
void
GAttrLink::InsertView(ViewGraph *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  MasterSlaveLink::InsertView(view);

  CheckAttrs();
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::DeleteView
 * Delete a view from this link.
 */
bool
GAttrLink::DeleteView(ViewGraph *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::DeleteView(%s)\n", _name, view->GetName());
#endif

  Boolean isLeader = (view == _masterView);

  bool result = MasterSlaveLink::DeleteView(view);

  if (result) {
    if (isLeader) {
      SetMasterView(NULL);
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::Initialize
 * Initialize this link (clear the table of attribute values).
 */
void
GAttrLink::Initialize()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::Initialize()\n", _name);
#endif

  if (_disableUpdates) {
#if (DEBUG >= 1)
    printf("  GData attribute link updates disabled -- returning\n");
#endif
    return;
  }

  if (_selectionValues->clear() != 0) {
    reportErrNosys("Error clearing hash table");
  }
  _hasAll = false;

  CheckAttrs();
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::Done
 * Propagate updates when insertions to link are done.
 */
void
GAttrLink::Done()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::Done()\n", _name);
#endif

  if (_disableUpdates) {
#if (DEBUG >= 1)
    printf("  GData attribute link updates disabled -- returning\n");
#endif
    return;
  }

  //
  // Tell the follower views to redraw.
  //
  MasterSlaveLink::Done();
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::Abort
 * Aborts link.
 */
void
GAttrLink::Abort()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::Abort()\n", _name);
#endif
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::SetLinkType
 * Set (record) link type -- illegal on this class.
 */
void
GAttrLink::SetLinkType(RecordLinkType type)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::SetLinkType(%d)\n", _name, type);
#endif

  reportErrNosys("Cannot change record link type of GData attribute link\n");
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::SetLeaderAttr
 * Set the leader attribute for this link.
 */
void
GAttrLink::SetLeaderAttr(char *leaderAttrName)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::SetLeaderAttr(%s)\n", _name, leaderAttrName);
#endif

  if (strcmp(leaderAttrName, _leaderAttrName)) {
    FreeString(_leaderAttrName);
    _leaderAttrName = CopyString(leaderAttrName);

    CheckAttrs();

    if (_masterView != NULL) _masterView->Refresh();
  }
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::SetFollowerAttr
 * Set the follower attribute for this link.
 */
void
GAttrLink::SetFollowerAttr(char *followerAttrName)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::SetFollowerAttr(%s)\n", _name, followerAttrName);
#endif

  if (strcmp(followerAttrName, _followerAttrName)) {
    FreeString(_followerAttrName);
    _followerAttrName = CopyString(followerAttrName);

    CheckAttrs();

    // Force all follower views to refresh.
    int index = InitIterator();
    while (More(index)) {
      View *view = Next(index);
      view->Refresh();
    }
    DoneIterator(index);
  }
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::InsertRecord
 * Insert a leader view's GData record (really its relevant value) into the
 * table of selection values.
 */
void
GAttrLink::InsertRecord(TDataMap *tdMap, const char *gDataRec)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 3)
  printf("GAttrLink(%s)::InsertRecord(%p, %p)\n", _name, tdMap, gDataRec);
#endif

  Coord value;
  const int bufLen = 128;
  char strBuf[bufLen];
  char *strPtr = NULL;
  int strBufLen = 0;
  if (_enableAll) {
    strPtr = strBuf;
    strBufLen = bufLen;
  }
  if (GetAttrValue(tdMap, gDataRec, _leaderAttrName, value, strPtr,
      strBufLen)) {
    int tmpVal = 0;
    _selectionValues->insert(value, tmpVal);
    if (strPtr && (!strcmp(strPtr, "All") || !strcmp(strPtr, "all"))) {
      _hasAll = true;
    }
  }
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::TestRecord
 * Test whether a follower view's GData record matches one of the selection
 * values.  Returns true iff there is a match (this GData record should be
 * drawn by the follower view).
 */
bool
GAttrLink::TestRecord(TDataMap *tdMap, const char *gDataRec)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 3)
  printf("GAttrLink(%s)::TestRecord(%p, %p)\n", _name, tdMap, gDataRec);
#endif

  bool result = false;

  Coord value;
  if (_hasAll) {
    result = true;
  } else if (GetAttrValue(tdMap, gDataRec, _followerAttrName, value)) {
    int tmpVal = 0;
    if (_selectionValues->lookup(value, tmpVal) == 0) {
      result = true;
    }
  }

#if (DEBUG >= 3)
  printf("  GAttrLink()::TestRecord() returns %d\n", result);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::CheckAttrs
 * Check compatibility of leader and follower attributes.
 */
bool
GAttrLink::CheckAttrs()
{
#if (DEBUG >= 1)
  printf("GAttrLink(%s)::CheckAttrs()\n", GetName());
#endif

  bool result = true;

  //
  // Give a warning if the leader or follower attribute is recId.
  // (We should probably check whether the views have the same TData,
  // but I don't want to implement that right now.
  //
  if (!strcmp(_leaderAttrName, recIdName) ||
      !strcmp(_followerAttrName, recIdName)) {
    printf("Warning: use of recId in GAttr links is not recommended at "
        "this time; please be sure that leader and follower views have "
	"the same TData\n");
  }

  //
  // Check compatibility of leader and follower attributes.
  //
  const int bufLen = 1024;
  char errBuf[bufLen];

  // This had better be true!
  if (_masterView != NULL) {
    TDataMap *tdMap = _masterView->GetFirstMap();

    // This had better be true!
    if (tdMap != NULL) {
      AttrList *attrList = tdMap->GDataAttrList();
      AttrInfo *attrInfo = attrList->Find(_leaderAttrName);
      StringStorage *leaderStringTable = GetStringTable(tdMap,
          _leaderAttrName);
      if (attrInfo == NULL) {
	int formatted = snprintf(errBuf, bufLen, "Warning: GAttr link "
	    "leader view %s has no values for leader attribute %s",
	    _masterView->GetName(), _leaderAttrName);
        checkAndTermBuf(errBuf, bufLen, formatted);
	reportErrNosys(errBuf);
	result = false;

      } else {
        AttrType leaderAttrType = attrInfo->type;

        int index = InitIterator();
        while (More(index)) {
          ViewGraph *view = Next(index);
	  tdMap = view->GetFirstMap();

          // This had better be true!
          if (tdMap != NULL) {
	    attrList = tdMap->GDataAttrList();
	    attrInfo = attrList->Find(_followerAttrName);
	    if (attrInfo == NULL) {
	      int formatted = snprintf(errBuf, bufLen, "Warning: GAttr link "
	          "follower view %s has no values for follower attribute %s",
	          view->GetName(), _followerAttrName);
              checkAndTermBuf(errBuf, bufLen, formatted);
	      reportErrNosys(errBuf);
	      result = false;

	    } else {
              AttrType followerAttrType = attrInfo->type;

	      if (leaderAttrType == StringAttr &&
	          followerAttrType != StringAttr) {
		int formatted = snprintf(errBuf, bufLen, "Warning: GAttr "
		    "link leader attribute (%s in %s) is string; follower "
		    "attribute (%s in %s) is non-string -- link will "
		    "probably not work correctly!!", _leaderAttrName,
		    _masterView->GetName(), _followerAttrName,
		    view->GetName());
		checkAndTermBuf(errBuf, bufLen, formatted);
		reportErrNosys(errBuf);
	        result = false;

	      } else if (leaderAttrType != StringAttr &&
	          followerAttrType == StringAttr) {
		int formatted = snprintf(errBuf, bufLen, "Warning: GAttr "
		    "link leader attribute (%s in %s) is non-string; follower "
		    "attribute (%s in %s) is string -- link will "
		    "probably not work correctly!!", _leaderAttrName,
		    _masterView->GetName(), _followerAttrName,
		    view->GetName());
		checkAndTermBuf(errBuf, bufLen, formatted);
		reportErrNosys(errBuf);
	        result = false;

	      } else if (leaderAttrType == DateAttr &&
	          followerAttrType != DateAttr) {
		int formatted = snprintf(errBuf, bufLen, "Warning: GAttr "
		    "link leader attribute (%s in %s) is date; follower "
		    "attribute (%s in %s) is non-date -- link will "
		    "probably not work correctly!!", _leaderAttrName,
		    _masterView->GetName(), _followerAttrName,
		    view->GetName());
		checkAndTermBuf(errBuf, bufLen, formatted);
		reportErrNosys(errBuf);
	        result = false;

	      } else if (leaderAttrType != DateAttr &&
	          followerAttrType == DateAttr) {
		int formatted = snprintf(errBuf, bufLen, "Warning: GAttr "
		    "link leader attribute (%s in %s) is non-date; follower "
		    "attribute (%s in %s) is date -- link will "
		    "probably not work correctly!!", _leaderAttrName,
		    _masterView->GetName(), _followerAttrName,
		    view->GetName());
		checkAndTermBuf(errBuf, bufLen, formatted);
		reportErrNosys(errBuf);
	        result = false;

	      } else {
	        // Attribute types are compatible.
		if (leaderAttrType != StringAttr) {
		  // Make sure we're not using different string tables
		  // in the leader and follower views.
		  // Note: if we *are* using different string tables, I
		  // guess we could get the actual string values and
		  // compare them when we're checking follower records.
                  StringStorage *followerStringTable = GetStringTable(
		      tdMap, _followerAttrName);
		  if (leaderStringTable != followerStringTable) {
		    int formatted = snprintf(errBuf, bufLen, "Warning: "
		        "GAttr link leader (%s) and follower (%s) views "
			"use different string tables for the linked "
			"attributes (%s and %s; this may cause GAttrLink "
			"to fail", _masterView->GetName(), view->GetName(),
			_leaderAttrName, _followerAttrName);
		    checkAndTermBuf(errBuf, bufLen, formatted);
		    reportErrNosys(errBuf);
		  }
		}
	      }
	    }
	  }
        }
        DoneIterator(index);
      }
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::GetAttrValue
 * Get the value of the given attribute in the given GData record.  Returns
 * true iff value is valid.  Note that an invalid value is not necessarily
 * an error: that value simply may not be specified in the given GData.
 *
 * Note: this method could probably be speeded up quite a bit by caching
 * info for each view about whether the GData has the given attribute value,
 * what it's offset is, etc.  However, that would require a bunch of extra
 * work to make sure the cached info stayed up-to-date.  RKW 2002-09-04.
 */
bool
GAttrLink::GetAttrValue(TDataMap *tdMap, const char *gDataRec,
    const char *attrName, Coord &value, char *strBuf, int bufLen)
{
  bool result = false;
  value = 0.0;
  if (strBuf) {
    strBuf[0] = '\0';
  }

  AttrList *attrList = tdMap->GDataAttrList();

  int shapeAttrLen = strlen(gdataShapeAttrName);

  bool badAttrName = false;

  //
  // Note: check of attrList must be done inside the ifs so we can tell
  // the difference between an invalid GData attribute name and one for
  // which there simply happens to be no value.  RKW 2002-09-04.
  //
  AttrInfo *attrInfo = NULL;
  if (!strcmp(attrName, recIdName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = (Coord)tdMap->GetRecId(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataXName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = tdMap->GetX(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataYName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = tdMap->GetY(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataZName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = tdMap->GetZ(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataColorName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = (Coord)tdMap->GetColor(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataSizeName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = tdMap->GetSize(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataPatternName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = (Coord)tdMap->GetPattern(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataOrientationName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = tdMap->GetOrientation(gDataRec);
      result = true;
    }

  } else if (!strcmp(attrName, gdataShapeName)) {
    if ((attrInfo = attrList->Find(attrName)) != NULL) {
      value = (Coord)tdMap->GetShape(gDataRec);
      result = true;
    }

  } else if (!strncmp(attrName, gdataShapeAttrName, shapeAttrLen)) {
    int attrNum = atoi(&attrName[shapeAttrLen]);
    if (!isdigit(attrName[shapeAttrLen]) || attrNum < 0 ||
        attrNum >= MAX_SHAPE_ATTRS) {
      badAttrName = true;
    } else {
      if ((attrInfo = attrList->Find(attrName)) != NULL) {
        value = tdMap->GetShapeAttr(gDataRec, attrNum);
        result = true;
      }
    }

  } else {
    badAttrName = true;
  }

  if (result && strBuf) {
    if (attrInfo->type == StringAttr) {
      StringStorage *stringTable = GetStringTable(tdMap, attrName);

      char *strVal;
      int code = stringTable->Lookup((int)value, strVal);
      if (code < 0) {
	char errBuf[128];
	int formatted = snprintf(errBuf, sizeof(errBuf),
	    "String not found for index %d\n", (int) value);
	checkAndTermBuf2(errBuf, formatted);
	reportErrNosys(errBuf);
      } else {
        int formatted = snprintf(strBuf, bufLen, "%s", strVal);
	checkAndTermBuf(strBuf, bufLen, formatted);
      }
    }
  }

  if (badAttrName) {
    const int bufLen = 1024;
    char buf[bufLen];
    int formatted = snprintf(buf, bufLen, "Invalid GData attribute name: %s",
        attrName);
    checkAndTermBuf(buf, bufLen, formatted);
    reportErrNosys(buf);
  }

#if (DEBUG >= 3)
  if (result) {
    printf("  GAttrLink::GetAttrValue(%s) returns %g\n", attrName, value);
  } else {
    printf("  GAttrLink::GetAttrValue(%s) returns invalid\n", attrName);
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::CoordHash
 * Hash function on Coord value.
 */
int
GAttrLink::CoordHash(const Coord &value, int numBuckets)
{
  union {
    Coord in;
    unsigned int out;
  } tmpHash;

  tmpHash.in = value;
  int result = tmpHash.out % numBuckets;

  //DOASSERT(result >= 0, "Hash function error");

  return result;
}

/*------------------------------------------------------------------------------
 * function: GAttrLink::GetStringTable
 * Get the string table for a given TDataMap and attribute name.
 */
StringStorage *
GAttrLink::GetStringTable(TDataMap *tdMap, const char *attrName)
{
  TDataMap::TableType type = TDataMap::TableInvalid;
  if (!strcmp(attrName, gdataXName)) {
    type = TDataMap::TableX;
  } else if (!strcmp(attrName, gdataYName)) {
    type = TDataMap::TableY;
  } else if (!strcmp(attrName, gdataZName)) {
    type = TDataMap::TableZ;
  } else {
    type = TDataMap::TableGen;
  }

  StringStorage *stringTable = tdMap->GetStringTable(type);

  return stringTable;
}

/*============================================================================*/
