/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implemenation of the MetaVisDesc class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  2000/07/12 20:49:19  wenger
  Added first version of metavisualization session description; changed
  DEVise version to 1.7.1.

 */

#include <stdio.h>
#include <sys/param.h>

#include "MetaVisDesc.h"
#include "DevError.h"
#include "DevFileHeader.h"
#include "View.h"
#include "ViewGraph.h"
#include "VisualLinkClassInfo.h"
#include "DeviseLink.h"
#include "CursorClassInfo.h"
#include "Cursor.h"
#include "SessionDesc.h"
#include "TData.h"

#define DEBUG 0

static const char *_tableFormat = "%s	%s	%s\n";

static const char *_objectTypeStr = "object_type";

static const char *_viewStr = "view";
static const char *_parentViewStr = "view_parent_view";
static const char *_titleStr = "view_title";
static const char *_fgColorStr = "view_fg_color";
static const char *_bgColorStr = "view_bg_color";
static const char *_pileStr = "view_pile";
static const char *_dimensionsStr = "view_dimensions";
static const char *_tDataStr = "view_tdata_name";
static const char *_viewXStr = "view_x";
static const char *_viewYStr = "view_y";
static const char *_viewWidthStr = "view_width";
static const char *_viewHeightStr = "view_height";
static const char *_viewFilterXLo = "view_filter_xlo";
static const char *_viewFilterXHi = "view_filter_xhi";
static const char *_viewFilterYLo = "view_filter_ylo";
static const char *_viewFilterYHi = "view_filter_yhi";

static const char *_linkStr = "link";
static const char *_linkTypeStr = "link_type";
static const char *_linkLeaderStr = "link_leader_view";
static const char *_linkViewStr = "link_view";

static const char *_cursorStr = "cursor";
static const char *_cursorTypeStr = "cursor_type";
static const char *_cursorSrcView = "cursor_src_view";
static const char *_cursorDestView = "cursor_dest_view";
static const char *_cursorUseGrid = "cursor_use_grid";
static const char *_cursorGridX = "cursor_grid_x";
static const char *_cursorGridY = "cursor_grid_y";
static const char *_cursorEdgeGrid = "cursor_edge_grid";
static const char *_cursorColor = "cursor_color";

static const char *_nullStr = "-";
static const char *_trueStr = "1";
static const char *_falseStr = "0";

// ---------------------------------------------------------------------------
// Write session description for metavisualization.
DevStatus
MetaVisDesc::Write(const char *filename, Boolean writeHeader,
  Boolean writeComments)
{
#if (DEBUG >= 1)
    printf("MetaVisDesc::Write(%s, %d)\n", filename, writeHeader);
#endif

    DevStatus result(StatusOk);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        char errBuf[MAXPATHLEN+100];
        sprintf(errBuf, "Unable to open file '%s'\n", filename);
        reportErrSys(errBuf);
        result += StatusFailed;
    } else {
        result += WriteDesc(file, writeHeader, writeComments);

        if (fclose(file) != 0) {
            char errBuf[MAXPATHLEN+100];
            sprintf(errBuf, "Unable to close file '%s'\n", filename);
            reportErrSys(errBuf);
            result += StatusFailed;
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// Write session description for metavisualization.
DevStatus
MetaVisDesc::WriteDesc(FILE *file, Boolean writeHeader, Boolean writeComments)
{
#if (DEBUG >= 2)
    printf("MetaVisDesc::WriteDesc()\n");
#endif

    DevStatus result(StatusOk);

    if (writeHeader && result.IsComplete()) {
        result += WriteHeader(file);
    }

    if (result.IsComplete()) {
        result += WriteAllViews(file, writeComments);
    }

    if (result.IsComplete()) {
        result += WriteAllLinks(file, writeComments);
    }

    if (result.IsComplete()) {
        result += WriteAllCursors(file, writeComments);
    }

    // TEMP write piles?

    return result;
}

// ---------------------------------------------------------------------------
// Write file header.
DevStatus
MetaVisDesc::WriteHeader(FILE *file)
{
#if (DEBUG >= 3)
    printf("MetaVisDesc::WriteHeader()\n");
#endif

    DevStatus result(StatusOk);
    
    char *header = DevFileHeader::Get(FILE_TYPE_METAVIS);

    int length = strlen(header);
    if (fwrite(header, length, 1, file) != 1) {
        reportErrSys("Unable to write file header");
        result += StatusFailed;
    }
    fprintf(file, "\n");

    return result;
}

// ---------------------------------------------------------------------------
// Write all view info.
DevStatus
MetaVisDesc::WriteAllViews(FILE *file, Boolean writeComments)
{
#if (DEBUG >= 3)
    printf("MetaVisDesc::WriteAllViews()\n");
#endif

    DevStatus result(StatusOk);

    if (writeComments) {
        fprintf(file, "\n# Views\n");
    }

    int index = View::InitViewIterator();
    while (View::MoreView(index) && result.IsComplete()) {
        View *view = View::NextView(index);
	result += WriteView(file, view, writeComments);
    }
    View::DoneViewIterator(index);

    return result;
}

// ---------------------------------------------------------------------------
// Write all link info.
DevStatus
MetaVisDesc::WriteAllLinks(FILE *file, Boolean writeComments)
{
#if (DEBUG >= 3)
    printf("MetaVisDesc::WriteAllAllLinks()\n");
#endif

    DevStatus result(StatusOk);

    if (writeComments) {
        fprintf(file, "\n# Links\n");
    }

    int index = DevLink::InitIterator();
    while (DevLink::More(index) && result.IsComplete()) {
        VisualLinkClassInfo *info = DevLink::Next(index);
	if (info != NULL) {
	    DeviseLink *link = (DeviseLink *)info->GetInstance();
	    if (link != NULL) {
	        WriteLink(file, link, writeComments);
	    }
	}
    }
    DevLink::DoneIterator(index);

    //TEMP -- perhaps add pile links here

    return result;
}

// ---------------------------------------------------------------------------
// Write all cursor info.
DevStatus
MetaVisDesc::WriteAllCursors(FILE *file, Boolean writeComments)
{
#if (DEBUG >= 3)
    printf("MetaVisDesc::WriteAllCursors()\n");
#endif

    DevStatus result(StatusOk);

    if (writeComments) {
        fprintf(file, "\n# Cursors\n");
    }

    int index = DevCursor::InitIterator();
    while (DevCursor::More(index) && result.IsComplete()) {
        CursorClassInfo *info = DevCursor::Next(index);
	if (info != NULL) {
	    DeviseCursor *cursor = (DeviseCursor *)info->GetInstance();
	    if (cursor != NULL) {
	        result += WriteCursor(file, cursor, writeComments);
	    }
	}
    }
    DevCursor::DoneIterator(index);

    return result;
}

// ---------------------------------------------------------------------------
// Write info about the given view.
DevStatus
MetaVisDesc::WriteView(FILE *file, View *view, Boolean writeComments)
{
#if (DEBUG >= 4)
    printf("MetaVisDesc::WriteView(%s)\n", view->GetName());
#endif

    DevStatus result(StatusOk);

    if (writeComments) {
        fprintf(file, "\n");
    }

    //
    // Object type.
    //
    fprintf(file, _tableFormat, view->GetName(), _objectTypeStr, _viewStr);

    //
    // Title.
    //
    const char *viewTitle = _nullStr;
    if (view->_label.occupyTop) {
        viewTitle = view->_label.name;
    }
    fprintf(file, _tableFormat, view->GetName(), _titleStr, viewTitle);

    //
    // Parent view.
    //
    const char *viewParent = _nullStr;
    if (view->IsChildView() && view->GetParent()) {
        viewParent = view->GetParent()->GetName();
    }
    fprintf(file, _tableFormat, view->GetName(), _parentViewStr, viewParent);

    //
    // Foreground and background colors.
    //
    string fgColor = _nullStr, bgColor = _nullStr;
    PColorID pColor = view->GetForeground();
    RGB rgb;
    if (PM_GetRGB(pColor, rgb)) {
        fgColor = rgb.ToString();
    }
    fprintf(file, _tableFormat, view->GetName(), _fgColorStr, fgColor.c_str());

    pColor = view->GetBackground();
    if (PM_GetRGB(pColor, rgb)) {
        bgColor = rgb.ToString();
    }
    fprintf(file, _tableFormat, view->GetName(), _bgColorStr, bgColor.c_str());

    //
    // Pile.
    //
    const char *pileName = _nullStr;
    if (view->GetParentPileStack()) {
        pileName = view->GetParentPileStack()->GetName();
    }
    fprintf(file, _tableFormat, view->GetName(), _pileStr, pileName);

    //
    // Number of dimensions.
    //
    char buf[128];
    sprintf(buf, "%d", view->GetNumDimensions());
    fprintf(file, _tableFormat, view->GetName(), _dimensionsStr, buf);

    //
    // TData name.
    //
    TDataMap *map = ((ViewGraph *)view)->GetFirstMap();
    if (map) {
	TData *td = map->GetLogTData();
	if (td) {
            fprintf(file, _tableFormat, view->GetName(), _tDataStr,
	      td->GetName());
        }
    }

    //
    // Size and location relative to screen.
    //
    //TEMP -- location needs changes for batch mode
    int viewX, viewY;
    unsigned viewWidth, viewHeight;
    view->RealGeometry(viewX, viewY, viewWidth, viewHeight);
    view->AbsoluteOrigin(viewX, viewY);
    char tmpBuf[64];

    sprintf(tmpBuf, "%d", viewX);
    fprintf(file, _tableFormat, view->GetName(), _viewXStr, tmpBuf);

    sprintf(tmpBuf, "%d", viewY);
    fprintf(file, _tableFormat, view->GetName(), _viewYStr, tmpBuf);

    sprintf(tmpBuf, "%d", viewWidth);
    fprintf(file, _tableFormat, view->GetName(), _viewWidthStr, tmpBuf);

    sprintf(tmpBuf, "%d", viewHeight);
    fprintf(file, _tableFormat, view->GetName(), _viewHeightStr, tmpBuf);

    const VisualFilter *filter = view->GetVisualFilter();

    if (view->GetXAxisAttrType() == DateAttr) {
        sprintf(tmpBuf, "%.7g (%s)", filter->xLow, DateString(filter->xLow));
        fprintf(file, _tableFormat, view->GetName(), _viewFilterXLo, tmpBuf);
        sprintf(tmpBuf, "%.7g (%s)", filter->xHigh, DateString(filter->xHigh));
        fprintf(file, _tableFormat, view->GetName(), _viewFilterXHi, tmpBuf);
    } else {
        sprintf(tmpBuf, "%.7g", filter->xLow);
        fprintf(file, _tableFormat, view->GetName(), _viewFilterXLo, tmpBuf);
        sprintf(tmpBuf, "%.7g", filter->xHigh);
        fprintf(file, _tableFormat, view->GetName(), _viewFilterXHi, tmpBuf);
    }

    if (view->GetYAxisAttrType() == DateAttr) {
        sprintf(tmpBuf, "%.7g (%s)", filter->yLow, DateString(filter->yLow));
        fprintf(file, _tableFormat, view->GetName(), _viewFilterYLo, tmpBuf);
        sprintf(tmpBuf, "%.7g (%s)", filter->yHigh, DateString(filter->yHigh));
        fprintf(file, _tableFormat, view->GetName(), _viewFilterYHi, tmpBuf);
    } else {
        sprintf(tmpBuf, "%.7g", filter->yLow);
        fprintf(file, _tableFormat, view->GetName(), _viewFilterYLo, tmpBuf);
        sprintf(tmpBuf, "%.7g", filter->yHigh);
        fprintf(file, _tableFormat, view->GetName(), _viewFilterYHi, tmpBuf);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Write info about the given link.
DevStatus
MetaVisDesc::WriteLink(FILE *file, DeviseLink *link, Boolean writeComments)
{
#if (DEBUG >= 4)
    printf("MetaVisDesc::WriteLink(%s)\n", link->GetName());
#endif

    DevStatus result(StatusOk);

    if (writeComments) {
        fprintf(file, "\n");
    }

    //
    // Object type.
    //
    fprintf(file, _tableFormat, link->GetName(), _objectTypeStr, _linkStr);

    //
    // Link type.
    //
    char linkType[32];
    result += SessionDesc::LinkTypeString(link, linkType);
    fprintf(file, _tableFormat, link->GetName(), _linkTypeStr, linkType);

    //
    // Leader view.
    //
    View *leader = link->GetMasterView();
    if (leader) {
        fprintf(file, _tableFormat, link->GetName(), _linkLeaderStr,
	  leader->GetName());
    }

    //
    // Other views.
    //
    int viewIndex = link->InitIterator();
    while (link->More(viewIndex)) {
        View *view = link->Next(viewIndex);
        fprintf(file, _tableFormat, link->GetName(), _linkViewStr,
	  view->GetName());
    }
    link->DoneIterator(viewIndex);

    return result;
}

// ---------------------------------------------------------------------------
// Write info about the given cursor.
DevStatus
MetaVisDesc::WriteCursor(FILE *file, DeviseCursor *cursor,
  Boolean writeComments)
{
#if (DEBUG >= 4)
    printf("MetaVisDesc::WriteCursor(%s)\n", cursor->GetName());
#endif

    DevStatus result(StatusOk);

    if (writeComments) {
        fprintf(file, "\n");
    }

    //
    // Object type.
    //
    fprintf(file, _tableFormat, cursor->GetName(), _objectTypeStr, _cursorStr);

    //
    // Cursor type.
    //
    char *cursorType;
    VisualFlag flag = cursor->GetFlag();
    if ((flag & VISUAL_X) && (flag & VISUAL_Y)) {
      cursorType = "XY";
    } else if (flag & VISUAL_X) {
      cursorType = "X";
    } else if (flag & VISUAL_Y) {
      cursorType = "Y";
    } else {
      cursorType = "unknown";
    }
    fprintf(file, _tableFormat, cursor->GetName(), _cursorTypeStr, cursorType);

    //
    // Source view.
    //
    View *src = cursor->GetSource();
    if (src) {
        fprintf(file, _tableFormat, cursor->GetName(), _cursorSrcView,
	  src->GetName());
    }

    //
    // Destination view.
    //
    View *dest = cursor->GetDst();
    if (dest) {
        fprintf(file, _tableFormat, cursor->GetName(), _cursorDestView,
	  dest->GetName());
    }

    //
    // Grid info.
    //
    Boolean useGrid;
    Coord gridX, gridY;
    Boolean edgeGrid;
    cursor->GetGrid(useGrid, gridX, gridY, edgeGrid);
    fprintf(file, _tableFormat, cursor->GetName(), _cursorUseGrid,
      useGrid ? _trueStr : _falseStr);
    if (useGrid) {
	char tmpBuf[64];

	sprintf(tmpBuf, "%g", gridX);
        fprintf(file, _tableFormat, cursor->GetName(), _cursorGridX,
	  tmpBuf);

	sprintf(tmpBuf, "%g", gridY);
        fprintf(file, _tableFormat, cursor->GetName(), _cursorGridY,
	  tmpBuf);

        fprintf(file, _tableFormat, cursor->GetName(), _cursorEdgeGrid,
	  edgeGrid ? _trueStr : _falseStr);
    }

    //
    // Color.
    //
    string colorStr = _nullStr;
    PColorID pColor = cursor->GetCursorColor();
    RGB rgb;
    if (PM_GetRGB(pColor, rgb)) {
        colorStr = rgb.ToString();
    }
    fprintf(file, _tableFormat, cursor->GetName(), _cursorColor,
      colorStr.c_str());

    return result;
}

/*============================================================================*/
