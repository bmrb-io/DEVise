/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.14  1999/10/18 17:32:35  wenger
  Fixed bug 515 (small problem with cursor dragging) and bug 516 (we now
  eliminate any extraneous control-D's in GData strings).

  Revision 1.13  1999/08/19 13:54:15  wenger
  Changes for JavaScreen support: all 15 shape attributes now sent in
  GData; added zoom in/out argument to JAVAC_MouseAction_RubberBand;
  JAVAC_MouseAction_DoubleClick changed to JAVAC_ShowRecords.

  Revision 1.12  1999/08/13 17:22:18  wenger
  Custom view layouts are now saved to session files; removed now unused
  TileLayout code.

  Revision 1.11  1999/06/29 20:23:49  wenger
  When sending GData to a file or socket, strings including the separator
  character are now surrounded by braces; DEVise color numbers are converted
  to RGB values by default (can be changed with the GUI).

  Revision 1.10  1999/05/26 19:50:42  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.9  1999/05/21 14:52:10  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.8  1998/12/10 21:53:16  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.7  1998/11/06 17:59:33  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

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
int GDataSock::_sockOutputCount = 0;

struct AttrVals {
  char *name;
  AttrType type;
  Coord numericalValue;
  char *stringValue;
};

// See GDataRec.h.
struct RecordVals {
  Coord _x;
  Coord _y;
  Coord _z;
  PColorID _color;
  Coord _size;
  Pattern _pattern;
  Coord _orientation;
  ShapeID _symbolType;
  AttrVals _shapeAttrs[MAX_SHAPE_ATTRS];
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

static const char _controlD = '\x04';

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

  _params = params;
  _params.file = CopyString(_params.file);

  // Note: at the present time I'm only allowing one view to output to the
  // data socket to keep things simple.  If we want to allow multiple views
  // to output to sockets, I think we would need a separate socket for
  // each view, and the rest of DEVise can't handle that right now.
  // RKW Nov. 18, 1997.
  if (_params.file == NULL) {
    if (_sockOutputCount > 0) {
      reportErrNosys("Only one view can output to socket at a time");
      _status = StatusFailed;
    }
    _sockOutputCount++;
  }

  _objectCount++;

  if (!_params.sendText) {
    reportErrNosys("Sending binary GData is not yet implemented\n");
    _status = StatusFailed;
  }

  if (_status.IsComplete()) {
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

  if (_params.file == NULL) {
    _sockOutputCount--;
  }
  _objectCount--;

  if (_objectCount < 0) {
    reportErrNosys("Destroyed more objects than were created");
  }

  if (_fd >= 0) {
    if (write(_fd, &_controlD, 1) != 1) {
      reportErrSys("Error writing to data channel");
    }

    if (close(_fd) != 0) {
      reportErrSys("Error closing data channel");
    }
  }

  FreeString(_params.file);
}

/*------------------------------------------------------------------------------
 * function: GDataSock::Send
 * Send GData records to a socket.
 */
DevStatus
GDataSock::Send(ViewGraph *view, void **gdataArray, TDataMap *tdMap,
		int recCount)
{
#if defined(DEBUG)
  printf("GDataSock(0x%p)::Send(0x%p, %d)\n", this, gdataArray, recCount);
#endif

  DevStatus result(StatusOk);

  if (_status.IsComplete()) {
    StringStorage *stringTable = tdMap->GetStringTable(TDataMap::TableGen);
    AttrList *attrList = tdMap->GDataAttrList();

    AttrInfo *attrInfos[MAX_SHAPE_ATTRS];
    for (int attrNum = 0; attrNum < MAX_SHAPE_ATTRS; attrNum++) {
      char buf[128];
      sprintf(buf, "shapeAttr_%d", attrNum);
      attrInfos[attrNum] = attrList->Find(buf);
    }

    // Note: at least for now, I am assuming that for string attributes
    // you want the string value for ShapeAttr0 - ShapeAttr9 and the
    // numerical value otherwise.  This may need some refinement in the
    // future.  RKW Nov. 18, 1997.

    for (int recNum = 0; recNum < recCount; recNum++) {
      DevStatus tmpResult(StatusOk);

      char *gdata = (char *) gdataArray[recNum];
      RecordVals vals;
      tmpResult += GetRecordVals(attrInfos, tdMap, gdata, stringTable, vals);


      if (_params.sendText) {
	tmpResult += PrintRecordVals(vals);
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


/*------------------------------------------------------------------------------
 * function: GDataSock::GetRecordVals()
 * Get values for one record.
 */
DevStatus
GDataSock::GetRecordVals(AttrInfo **attrInfos, TDataMap *tdMap, char *gdata,
    StringStorage *stringTable, RecordVals &vals)
{
  DevStatus result(StatusOk);

  //
  // Get the values of the attributes.
  //
  vals._x = tdMap->GetX(gdata);
  vals._y = tdMap->GetY(gdata);
  vals._z = tdMap->GetZ(gdata);

  vals._color = tdMap->GetColor(gdata);

  vals._size = tdMap->GetSize(gdata);
  vals._pattern = tdMap->GetPattern(gdata);
  vals._orientation = tdMap->GetOrientation(gdata);
  vals._symbolType = tdMap->GetShape(gdata);

  for (int attrNum = 0; attrNum < MAX_SHAPE_ATTRS; attrNum++) {
    result += GetShapeAttr(attrNum, attrInfos[attrNum], tdMap, gdata,
        stringTable, vals._shapeAttrs[attrNum]);
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: GDataSock::GetShapeAttr
 * Gets a shape attribute value.
 */
DevStatus
GDataSock::GetShapeAttr(int attrNum, const AttrInfo *attrInfo,
    TDataMap *tdMap, const char *gdata, StringStorage *stringTable,
    AttrVals &attrVal)
{
  DevStatus result(StatusOk);

  if (attrInfo) {
    attrVal.type = attrInfo->type;

    //
    // Get the numerical value for this shape attribute.
    //
    attrVal.numericalValue = tdMap->GetShapeAttr(gdata, attrNum);

    //
    // Figure out whether this attribute is a string type, and if so
    // get the string value.
    //
    if (attrVal.type == StringAttr) {
      if (stringTable->Lookup( (int) attrVal.numericalValue,
          attrVal.stringValue) < 0) {
        char buf[1024];
        sprintf(buf, "String not found for %s", attrInfo->name);
        reportErrNosys(buf);
        result += StatusFailed;
      }
    }
  } else {
    // This attribute is not specified.
    attrVal.type = StringAttr;
    attrVal.stringValue = "";
  }

  return result;
}


/*------------------------------------------------------------------------------
 * function: GDataSock::PrintRecordVals
 * Print the values for one record.
 */
DevStatus
GDataSock::PrintRecordVals(const RecordVals &vals)
{
  DevStatus result(StatusOk);

  //
  // Format a buffer for this GData record.
  //
  const int bufSize = 2048;
  char buf[bufSize];
  int offset = 0;
  char sep = _params.separator;

  if (result.IsComplete()) {
    if (sprintf(&buf[offset], "%g%c%g%c%g%c", vals._x, sep, vals._y,
        sep, vals._z, sep) < 0) {
      reportErrSys("Error formatting output");
      result = StatusFailed;
    } else {
      IncAndCheckOffset(buf, offset, bufSize, result);
    }
  }

  if (result.IsComplete()) {
    if (_params.rgbColor) {
      RGB rgb;
      if (PM_GetRGB(vals._color, rgb)) {
        string colorStr = rgb.ToString();

        if (sprintf(&buf[offset], "%s%c", colorStr.c_str(), sep) < 0) {
          reportErrSys("Error formatting output");
          result = StatusFailed;
        } else {
          IncAndCheckOffset(buf, offset, bufSize, result);
        }
      } else {
	char errBuf[256];
        sprintf(errBuf, "Error converting PColorID %ld to RGB", vals._color);
        reportErrNosys(errBuf);
	result = StatusFailed;
      }
    } else {
      if (sprintf(&buf[offset], "%g%c", (Coord)vals._color, sep) < 0) {
        reportErrSys("Error formatting output");
        result = StatusFailed;
      } else {
        IncAndCheckOffset(buf, offset, bufSize, result);
      }
    }
  }

  if (result.IsComplete()) {
    if (sprintf(&buf[offset], "%g%c%g%c%g%c%g", vals._size, sep,
        (Coord)vals._pattern, sep, vals._orientation, sep,
	(Coord)vals._symbolType) < 0) {
      reportErrSys("Error formatting output");
      result = StatusFailed;
    } else {
      IncAndCheckOffset(buf, offset, bufSize, result);
    }
  }

  for (int attrNum = 0;
      attrNum < MAX_SHAPE_ATTRS && result.IsComplete(); attrNum++) {
    result += PrintShapeAttr(vals._shapeAttrs[attrNum], buf, offset,
      bufSize);
  }

  if (result.IsComplete()) {
    if (sprintf(&buf[offset], "\n") < 0) {
      reportErrSys("Error formatting output");
      result = StatusFailed;
    } else {
      IncAndCheckOffset(buf, offset, bufSize, result);
    }
  }

  //
  // Dump the buffer to the output.
  //
  // Note: I'm dumping the whole record at once so that we never
  // output a partial record if there is some kind of error partway
  // through.  RKW Nov. 18, 1997.
  if (result.IsComplete()) {
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

  return result;
}

/*------------------------------------------------------------------------------
 * function: GDataSock::PrintShapeAttr
 * Prints a shape attribute value to the given buffer.
 */
DevStatus
GDataSock::PrintShapeAttr(const AttrVals &attrVal, char buf[], int &offset,
    int bufSize)
{
  DevStatus result(StatusOk);

  // Note: the way the code is currently written, we can't tell the difference
  // in the output between an attribute with an empty string, and an attribute
  // with no value specified at all.  I'm not sure whether or not this is
  // important.  RKW 1999-06-29.

  if (attrVal.type == StringAttr) {
    char *formatStr;
    if (strlen(attrVal.stringValue) == 0 ||
        strchr(attrVal.stringValue, _params.separator)) {
      // String is empty or contains separator char -- put braces around it.
      formatStr = "%c{%s}";
    } else {
      formatStr = "%c%s";
    }

    if (sprintf(&buf[offset], formatStr, _params.separator,
        attrVal.stringValue) < 0) {
      reportErrSys("Error formatting output");
      result = StatusFailed;
    } else {
      IncAndCheckOffset(buf, offset, bufSize, result);
    }
  } else {
    if (sprintf(&buf[offset], "%c%g", _params.separator,
	attrVal.numericalValue) < 0) {
      reportErrSys("Error formatting output");
      result = StatusFailed;
    } else {
      IncAndCheckOffset(buf, offset, bufSize, result);
    }
  }

  //
  // This is a kludge to get rid of any stray control-D's
  char *tmpC = buf;
  while (*tmpC) {
    if (*tmpC == _controlD) {
      *tmpC = '%';
    }
    tmpC++;
  }

  return result;
}

/*============================================================================*/
