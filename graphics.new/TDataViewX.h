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
  Revision 1.20  1997/08/28 18:21:15  wenger
  Moved duplicate code from ViewScatter, TDataViewX, and ViewLens classes
  up into ViewGraph (parent class).

  Revision 1.19  1997/08/20 22:11:12  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.18.8.1  1997/08/07 16:56:45  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.18.6.1  1997/05/21 20:40:50  weaver
  Changes for new ColorManager

  Revision 1.18  1997/03/20 22:23:18  guangshu
  Changed QueyDone.

  Revision 1.17  1997/02/14 16:47:48  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.16  1997/02/03 19:45:35  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.15.4.1  1997/02/11 23:29:00  guangshu
  Moved the definition of BStatList to ViewGraph in order to collect
  statistics for scattered view.

  Revision 1.15  1996/11/26 16:51:39  ssl
  Added support for piled viws

  Revision 1.14  1996/11/13 16:57:12  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.13  1996/09/27 15:53:22  wenger
  Fixed a number of memory leaks.

  Revision 1.12  1996/06/13 00:16:31  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.11  1996/05/31 15:41:25  jussi
  Added support for record links.

  Revision 1.10  1996/04/22 21:38:06  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.9  1996/04/10 02:24:39  jussi
  Added support for > 1 mappings in a view.

  Revision 1.8  1996/04/05 20:13:31  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.7  1996/01/30 00:02:09  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.6  1995/12/29 22:42:32  jussi
  Added support for line connectors.

  Revision 1.5  1995/12/05 17:07:58  jussi
  Statistics are now part of ViewGraph, the subclass of TDataviewX.

  Revision 1.4  1995/11/28  05:26:28  ravim
  Support for statistics.

  Revision 1.3  1995/11/25 01:20:19  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.2  1995/09/05 22:16:05  jussi
  Added CVS header.
*/

//******************************************************************************
//
//******************************************************************************

#ifndef __TDATAVIEWX_H
#define __TDATAVIEWX_H

//******************************************************************************
// Libraries
//******************************************************************************

#include "ViewGraph.h"
#include "TDataCMap.h"
#include "DList.h"

#include "Color.h"

//******************************************************************************
// class TDataViewX
//******************************************************************************

class TDataViewX : public ViewGraph
{
		friend class TDataViewX_GDataBinCallback;

	private:

		TDataCMap*	_cMap;
		int			_totalGData;
		int			_numBatches;
		Boolean		_batchRecs;
		Boolean		_dispSymbols;
		Boolean		_dispConnectors;

		// Callback Adapters
		TDataViewX_GDataBinCallback*	gDataBinCallback;

	public:

		// Constructors and Destructors
		TDataViewX(char* name, VisualFilter& initFilter, QueryProc* qp,
				   PColorID fgid = GetPColorID(defForeColor),
				   PColorID bgid = GetPColorID(defBackColor),
				   AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
				   Action* action = NULL);
		virtual ~TDataViewX(void);

		// Public Methods
		virtual void	InsertMapping(TDataMap *map);

	protected:

		virtual Boolean		DisplaySymbols() { return _dispSymbols; }
		virtual Boolean		DisplaySymbols(Boolean state);
		virtual Boolean		DisplayConnectors() { return _dispConnectors; }
		virtual Boolean		DisplayConnectors(Boolean state);

		virtual void		PrintStat();

		// Override methods (View)
		virtual void	DerivedAbortQuery(void);

	protected:
  
		// Callback methods (GDataBinCallback)
		virtual void	ReturnGDataBinRecs(TDataMap* map, void** recs,
										   int numRecs, int& recordsProcessed);

		virtual void	ReturnGDataBinConnectors(TDataCMap* cmap,
												 Connector** connectors,
												 int num);

		// Callback methods (QueryCallback)
		virtual void	QueryInit(void* userData);
		virtual void	QueryDone(int bytes, void* userData,
								  TDataMap* map = NULL);
		virtual void*	GetObj(void)	{ return this; }
		virtual RecordLinkList*	GetRecordLinkList(void) { return &_slaveLink; }
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed);
};

//******************************************************************************
// class TDataViewX_GDataBinCallback
//******************************************************************************

class TDataViewX_GDataBinCallback : public GDataBinCallback
{
	private:

		TDataViewX*		_parent;

	public:

		TDataViewX_GDataBinCallback(TDataViewX* parent)
			: _parent(parent) {}

		virtual void	ReturnGDataBinRecs(TDataMap* map, void** recs,
										   int numRecs, int& recordsProcessed)
		{
			_parent->ReturnGDataBinRecs(map, recs, numRecs, recordsProcessed);
		}

		virtual void	ReturnGDataBinConnectors(TDataCMap* cmap,
												 Connector** connectors,
												 int num)
		{
			_parent->ReturnGDataBinConnectors(cmap, connectors, num);
		}
};

//******************************************************************************
#endif
