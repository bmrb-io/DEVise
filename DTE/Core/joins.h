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
				/*cout << " JOINTABLE 1 called " << endl;
				if(left)
					cout << "1left " << *left->getTable();
				if(right)
					cout << " 1Right " << *right->getTable();
				if (function)
					cout << " 1Fucntion " << *function;
				cout << endl;*/
			}
			JoinTable(JoinTable * left,TableAlias* right=NULL,
				String * function=NULL):
				left(NULL),right(right),function(function),leftJoin(left){
				/*if(left)
					cout << "2left " << *left->left->getTable();
				if(right)
					cout << " 2Right " << *right->getTable();
				if (function)
					cout << " 2Fucntion " << *function;
				cout << endl;*/
			}	
			JoinTable(JoinTable * left,JoinTable* right=NULL,
				String * function=NULL):
			 left(NULL),right(right->left),function(function),leftJoin(left){
				/*if(left)
					cout << "3LEFT " << *left->left->getTable();
				if(right)
					cout << " 3RIGHT " << *right->left->getTable();
				if (function)
					cout << " 3FUCNTION " << *function;
				cout << endl;*/
			}
};
#endif;
