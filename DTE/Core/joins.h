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
  $Id$

  $Log$
  Revision 1.7  1997/08/21 21:04:30  donjerko
  Implemented view materialization

  Revision 1.6  1997/06/16 16:04:49  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.4  1997/03/28 16:07:29  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef JOINS_H
#define JOINS_H

#include "site.h"

#ifndef __GNUG__
using namespace std;
#endif

class Joins:public SiteGroup{
		
		public:
			Joins(Site * left,Site * right,string * function):function(function)
				,left(left),right(right), SiteGroup(left,right),
					nextInnerTup(NULL), moreInnerTup(true), 
					nextOuterTup(NULL), moreOuterTup(true) { }
		
			virtual const Tuple* getNext();
			virtual void typify(string );
		private:
			List<Tuple *> innerRel;
			List<Tuple *> outerRel;
			string * function;
			int  leftSeqAttrPos;
			int  rightSeqAttrPos;
			Site * left,* right;
			// May be less than or grtr than..
			GeneralPtr * comparePtr;
			GeneralPtr * leftequalPtr;
			GeneralPtr * rightequalPtr;
			Tuple * nextInnerTup;
			bool moreInnerTup;
			Tuple * nextOuterTup;
			bool moreOuterTup;
			int leftCountFlds;
			int rightCountFlds;

			bool outerFill();
			bool innerFill();
};

class JoinTable{
		
		public:
			TableAlias * left ;
			TableAlias * right;
			string * function;
			JoinTable * leftJoin;

			Site* Plan(List<Site *>*siteList,List<BaseSelection*>*,
					List<BaseSelection*>*);

			JoinTable(TableAlias * left,TableAlias * right=NULL 
					,string *function=NULL):
				left(left),right(right),function(function),leftJoin(NULL){
			}
			JoinTable(JoinTable * left,TableAlias* right=NULL,
				string * function=NULL):
				left(NULL),right(right),function(function),leftJoin(left){
			}	
			JoinTable(JoinTable * left,JoinTable* rightTable =NULL,
				string * function=NULL):
			 left(NULL),function(function),leftJoin(left){
				if (rightTable )
					right = rightTable->left;
				else
					right = NULL;

			}
};
#endif;
