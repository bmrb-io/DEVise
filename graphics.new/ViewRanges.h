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
  $Id$

  $Log$
  Revision 1.5.10.1  1997/05/21 20:40:54  weaver
  Changes for new ColorManager

  Revision 1.5  1996/08/04 21:59:58  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.4  1996/05/07 16:41:49  jussi
  Updated constructor and added HandleKey, HandlePress and HandlePopup
  to reflect new interface between View and its derived classes.

  Revision 1.3  1996/04/05 20:13:34  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.2  1995/09/05 22:16:19  jussi
  Added CVS header.
*/

//******************************************************************************
//
//******************************************************************************

#ifndef __VIEWRANGES_H
#define __VIEWRANGES_H

//******************************************************************************
// Libraries
//******************************************************************************

#include "View.h"
#include "RangeCallback.h"

#include "Color.h"

//******************************************************************************
// class ViewRanges
//******************************************************************************

class ViewRanges : public View
{
		friend class ViewRanges_RangeCallback;

	private:

		RangeSource*	_source;		// Source of ranges
		Boolean			_horizontal;

		// Callback Adapters
		ViewRanges_RangeCallback*	rangeCallback;

	public:

		ViewRanges(char* name, ViewWin* parent, VisualFilter& initFilter,
				   RangeSource* source, Boolean horizontal);
		virtual ~ViewRanges(void);

		// Override methods (View)
		virtual void	DerivedStartQuery(VisualFilter& filter, int timestamp);
		virtual void	DerivedAbortQuery(void) {}

	protected:

		// Callback methods (RangeCallback)
		virtual void	RangeInserted(long low, long high);
		virtual void	RangeDeleted(long low, long high);

		// Callback methods (WindowRepCallback)
		virtual void	HandlePress(WindowRep* w, int xlow, int ylow,
									int xhigh, int yhigh, int button) {}
		virtual void	HandleKey(WindowRep* w ,int key, int x, int y) {}
		virtual Boolean	HandlePopUp(WindowRep*, int x, int y, int button,
									char**& msgs, int& numMsgs)
		{ return false; }
};

//******************************************************************************
// class ViewLens_ViewCallback
//******************************************************************************

class ViewRanges_RangeCallback : public RangeCallback
{
	private:

		ViewRanges*	_parent;
		
	public:

		ViewRanges_RangeCallback(ViewRanges* parent)
			: _parent(parent) {}

		virtual void	RangeInserted(long low, long high)
		{
			_parent->RangeInserted(low, high);
		}

		virtual void	RangeDeleted(long low, long high)
		{
			_parent->RangeDeleted(low, high);
		}
};

//******************************************************************************
#endif
