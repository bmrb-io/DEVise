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
  Revision 1.4  1997/03/28 16:07:29  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef JOINS_H
#define JOINS_H

#include "site.h"

class Joins:public SiteGroup{
		
		public:
			Joins(Site * left,Site * right,String * function):function(function)
				,left(left),right(right), SiteGroup(left,right),
					nextInnerTup(NULL), moreInnerTup(true), 
					nextOuterTup(NULL), moreOuterTup(true) { }
		
			virtual bool getNext(Tuple* next);
			virtual void typify(String );
		private:
			List<Tuple *> innerRel;
			List<Tuple *> outerRel;
			String * function;
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
			String * function;
			JoinTable * leftJoin;

			Site* Plan(List<Site *>*siteList,List<BaseSelection*>*,
					List<BaseSelection*>*);

			JoinTable(TableAlias * left,TableAlias * right=NULL 
					,String *function=NULL):
				left(left),right(right),function(function),leftJoin(NULL){
			}
			JoinTable(JoinTable * left,TableAlias* right=NULL,
				String * function=NULL):
				left(NULL),right(right),function(function),leftJoin(left){
			}	
			JoinTable(JoinTable * left,JoinTable* rightTable =NULL,
				String * function=NULL):
			 left(NULL),function(function),leftJoin(left){
				if (rightTable )
					right = rightTable->left;
				else
					right = NULL;

			}
};
#endif;
