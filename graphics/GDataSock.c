/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implemenation of GDataSock (send GData to socket) class.
 */

/*
  $Id$

  $Log$
  Revision 1.6  1998/11/04 20:33:44  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.5  1998/08/21 22:16:48  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.4  1998/02/03 23:46:01  wenger
  Fixed a problem Hongyu had with getting GData on socket; fixed bugs
  283 and 285 (resulted from problems in color manager merge);
  conditionaled out some debug output.

  Revision 1.3  1998/01/31 20:02:02  wenger
  Fixed bugs 277, 278, and 279; GData sent on socket now has <ctl-D>
  written at the end.

  Revision 1.2  1997/11/24 23:14:21  weaver
  Changes for the new ColorManager.

  Revision 1.1  1997/11/18 23:26:34  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "GDataSock.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "Shape.h"
#include "DevError.h"
#include "Control.h"
#include "StringStorage.h"

//#define DEBUG

int GDataSock::_objectCount = 0;

struct AttrVals {
  char *name;
  AttrType type;
  Coord numericalValue;
  char *stringValue;
};

#define IncAndCheckOffset(buf, offset, bufSize, status) { \
  offset += strlen(&buf[offset]); \
  if (offset > bufSize - 100) { \
    reportErrNosys("Buffer overflow"); \
    status += StatusFailed; \
  } \
}

#if !defined(lint) && defined(RCSID)
static char             rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: GDataSock::GDataSock
 * Constructor.
 */
GDataSock::GDataSock(Params &params)
{
  _status = StatusOk;

  if (params.file != NULL &&
      (strlen(params.file) == 0 || !strcmp(params.file, "\"\""))) {
    params.file = NULL;
  }

#if defined(DEBUG)
  printf("GDataSock(0x%p)::GDataSock(%d, {%s}, %d, '%c')\n", this,
    params.portNum, params.file != NULL ? params.file : "NULL",
    params.sendText, params.separator);
#endif

  // Note: at the present time I'm only allowing one view to output to the
  // data socket to keep things simple.  If we want to allow multiple views
  // to output to sockets, I think we would need a separate socket for
  // each view, and the rest of DEVise can't handle that right now.
  // RKW Nov. 18, 1997.
  if (_objectCount > 0) {
    reportErrNosys("Only one view can output to socket or file at a time");
    _status = StatusFailed;
  }

  _objectCount++;

  if (!params.sendText) {
    reportErrNosys("Sending binary GData is not yet implemented\n");
    _status = StatusFailed;
  }

  if (_status.IsComplete()) {
    _params = params;
    _params.file = CopyString(_params.file);

    if (_params.file != NULL) {
#if defined(DEBUG)
      printf("  Opening file <%s>.\n", _params.file);
#endif
      _fd = open(_params.file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (_fd < 0) {
        char buf[2048];
        sprintf(buf, "Can't open data channel (to file {%s})", _params.file);
        reportErrSys(buf);
        _status = StatusFailed;
      }
    } else {
#if defined(DEBUG)
      printf("  Opening data socket.\n");
#endif
      ControlPanel *control = ControlPanel::Instance();
      control->OpenDataChannel(_params.portNum);
      _fd = control->getFd();
      if (_fd < 0) {
        reportErrNosys("Can't open data channel (socket)");
        _status = StatusFailed;
      }
    }
  } else {
    // Set values that will be safe for the destructor.
    _params.portNum = -1;
    _params.file = NULL;
    _params.sendText = true;
    _params.separator = ' ';
    _fd = -1;
  }
}

/*------------------------------------------------------------------------------
 * function: GDataSock::~GDataSock
 * Destructor.
 */
GDataSock::~GDataSock()
{
#if defined(DEBUG)
  printf("GDataSock(0x%p)::~GDataSock()\n", this);
#endif

  _objectCount--;

  if (_objectCount < 0) {
    reportErrNosys("Destroyed more objects than were created");
  }

  if (_fd >= 0) {
    char controlD = '\x04';
    if (write(_fd, &controlD, 1) != 1) {
      reportErrSys("Error writing to data channel");
    }

    if (close(_fd) != 0) {
      reportErrSys("Error closing data channel");
    }
  }

  delete _params.file;
}

/*------------------------------------------------------------------------------
 * function: GDataSock::Send
 * Send GData records to a socket.
 */
DevStatus
GDataSock::Send(ViewGraph *view, void **gdataArray, TDataMap *map,
		int recCount)
{
#if defined(DEBUG)
  printf("GDataSock(0x%p)::Send(0x%p, %d)\n", this, gdataArray, recCount);
#endif

  DevStatus result(StatusOk);

  if (_status.IsComplete()) {
    GDataAttrOffset *offsets = map->GetGDataOffset();
    StringStorage *stringTable = map->GetStringTable(TDataMap::TableGen);
    AttrList *attrList = map->GDataAttrList();

    // Note: at least for now, I am assuming that for string attributes
    // you want the string value for ShapeAttr0 - ShapeAttr9 and the
    // numerical value otherwise.  This may need some refinement in the
    // future.  RKW Nov. 18, 1997.

    int recNum;
    for (recNum = 0; recNum < recCount; recNum++) {
      DevStatus tmpResult(StatusOk);

      char *gdata = (char *) gdataArray[recNum];

      //
      // Get the values of the attributes.
      //
      Coord x = ShapeGetX(gdata, map, offsets);
      Coord y = ShapeGetY(gdata, map, offsets);
      Coord z = GetZ(gdata, map, offsets);

//      Coord color = GetColor(view, gdata, map, offsets);
      Coord		pcid = GetPColorID(gdata, map, offsets);

      Coord size = GetSize(gdata, map, offsets);
      Coord pattern = GetPattern(gdata, map, offsets);
      Coord orientation = GetOrientation(gdata, map, offsets);
      Coord symbolType = GetShape(gdata, map, offsets);

      const int shapeAttrCount = 10;
      AttrVals shapeAttrs[shapeAttrCount] = {
	  {"shapeAttr_0", IntAttr, 0.0, NULL},
	  {"shapeAttr_1", IntAttr, 0.0, NULL},
	  {"shapeAttr_2", IntAttr, 0.0, NULL},
	  {"shapeAttr_3", IntAttr, 0.0, NULL},
	  {"shapeAttr_4", IntAttr, 0.0, NULL},
	  {"shapeAttr_5", IntAttr, 0.0, NULL},
	  {"shapeAttr_6", IntAttr, 0.0, NULL},
	  {"shapeAttr_7", IntAttr, 0.0, NULL},
	  {"shapeAttr_8", IntAttr, 0.0, NULL},
	  {"shapeAttr_9", IntAttr, 0.0, NULL} };

      int attrNum;
      ShapeAttr *defaultAttrs = map->GetDefaultShapeAttrs();
      for (attrNum = 0; attrNum < shapeAttrCount; attrNum++) {
	//
	// Get the numerical value for this shape attribute.
	//
	if (offsets->shapeAttrOffset[attrNum] < 0) {
	  shapeAttrs[attrNum].numericalValue = defaultAttrs[attrNum];
	} else {
	  shapeAttrs[attrNum].numericalValue = GetAttr(gdata,
	      shapeAttrOffset[attrNum], Coord, offsets);
	}

	//
	// Figure out whether this attribute is a string type, and if so
	// get the string value.
	//
        AttrInfo *attrInfo = attrList->Find(shapeAttrs[attrNum].name);
	// Assume attribute is not a string if we can't get AttrInfo.
	if (attrInfo != NULL) {
	  shapeAttrs[attrNum].type = attrInfo->type;
	  if (shapeAttrs[attrNum].type == StringAttr) {
	    if (stringTable->Lookup( (int) shapeAttrs[attrNum].numericalValue,
		shapeAttrs[attrNum].stringValue) < 0) {
	      char buf[1024];
	      sprintf(buf, "String not found for %s", attrInfo->name);
	      reportErrNosys(buf);
	      tmpResult += StatusFailed;
	    }
	  }
	}
      }


      if (_params.sendText) {
        //
        // Format a buffer for this GData record.
        //
	const int bufSize = 2048;
	char buf[bufSize];
	int offset = 0;
	char sep = _params.separator;

	if (tmpResult.IsComplete()) {
	  if (sprintf(&buf[offset], "%g%c%g%c%g%c", x, sep, y, sep, z,
	      sep) < 0) {
	    reportErrSys("Error formatting output");
	    tmpResult = StatusFailed;
	  } else {
	    IncAndCheckOffset(buf, offset, bufSize, tmpResult);
	  }
	}

	if (tmpResult.IsComplete()) {
	  if (sprintf(&buf[offset], "%g%c%g%c%g%c%g%c%g", pcid, sep, size,
	      sep, pattern, sep, orientation, sep, symbolType) < 0) {
	    reportErrSys("Error formatting output");
	    tmpResult = StatusFailed;
	  } else {
	    IncAndCheckOffset(buf, offset, bufSize, tmpResult);
	  }
	}

        for (attrNum = 0; attrNum < shapeAttrCount && tmpResult.IsComplete();
	    attrNum++) {
	  if (shapeAttrs[attrNum].type == StringAttr) {
	    if (sprintf(&buf[offset], "%c%s", sep,
		shapeAttrs[attrNum].stringValue) < 0) {
	      reportErrSys("Error formatting output");
	      tmpResult = StatusFailed;
	    } else {
	      IncAndCheckOffset(buf, offset, bufSize, tmpResult);
	    }
	  } else {
	    if (sprintf(&buf[offset], "%c%g", sep,
		shapeAttrs[attrNum].numericalValue) < 0) {
	      reportErrSys("Error formatting output");
	      tmpResult = StatusFailed;
	    } else {
	      IncAndCheckOffset(buf, offset, bufSize, tmpResult);
	    }
	  }
        }

	if (tmpResult.IsComplete()) {
	  if (sprintf(&buf[offset], "\n") < 0) {
	    reportErrSys("Error formatting output");
	    tmpResult = StatusFailed;
	  } else {
	    IncAndCheckOffset(buf, offset, bufSize, tmpResult);
	  }
        }

	//
	// Dump the buffer to the output.
	//
	// Note: I'm dumping the whole record at once so that we never
	// output a partial record if there is some kind of error partway
	// through.  RKW Nov. 18, 1997.
	if (tmpResult.IsComplete()) {
	  size_t byteCount = (size_t) offset;
	  if (write(_fd, buf, byteCount) !=
#if defined(SOLARIS)
	      (ssize_t)
#endif // SOLARIS
	      byteCount) {
	    reportErrSys("Error writing to data channel");
	    result = StatusFailed;
	  }
	}
      } else {
	reportErrNosys("Sending binary GData is not yet implemented\n");
	tmpResult = StatusFailed;
      }

      result += tmpResult;
    }
  } else {
    reportErrNosys("Attempted operation on object with bad status");
    result = StatusFailed;
  }

  return result;
}

/*============================================================================*/
