#ifndef JOINS_H
#define JOINS_H

#include "site.h"

class Joins:public SiteGroup{
		
		public:
			Joins(Site * left,Site * right,String * function):function(function)
				,left(left),right(right), SiteGroup(left,right),
					nextInnerTup(NULL),nextOuterTup(NULL){ }
		
			virtual Tuple * getNext();
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
			Tuple * nextOuterTup;
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
