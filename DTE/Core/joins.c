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
  Revision 1.9  1997/07/30 21:39:23  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.8  1997/06/16 16:04:48  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.6  1997/03/28 16:07:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include"joins.h"

void Joins::typify(string  option){
	
	//SiteGroup::typify(option);
		List<Site*>* tmpL = new List<Site*>;
        tmpL->append(site1);
        tmpL->append(site2);
        if(mySelect == NULL){
            mySelect = createSelectList(name, iterator);
        }
        else{
            TRY(typifyList(mySelect, tmpL), );
        }
        numFlds = mySelect->cardinality();
          TRY(typifyList(myWhere, tmpL), );
        double selectivity = listSelectivity(myWhere);
        int card1 = site1->getStats()->cardinality;
        int card2 = site2->getStats()->cardinality;
        int cardinality = int(selectivity * card1 * card2);
        int* sizes = sizesFromList(mySelect);
        stats = new Stats(numFlds, sizes, cardinality);
        TRY(boolCheckList(myWhere), );

	string * leftorder = left->getOrderingAttrib();
	string * rightorder = right->getOrderingAttrib();
	const TypeID * leftIDs = left->getTypeIDs();
	const TypeID * rightIDs = right->getTypeIDs();
	
	if (!leftorder || !rightorder){
		THROW(new Exception("Cannot do composition on non sequences"),);
	}
	string *  leftAttribNameList = left->getAttNamesOnly();
	string *  rightAttribNameList = right->getAttNamesOnly();
	
	rightCountFlds = right->getNumFlds();
	leftCountFlds = left->getNumFlds();
	int i;
	for(i = 0;i < leftCountFlds;i++){
		if (leftAttribNameList[i] == *leftorder){
			leftSeqAttrPos = i;
			break;
		}
	}
	if (i >= leftCountFlds){
		THROW(new Exception(" Ordering attribute not projected "),);
	}
	for(i = 0;i < rightCountFlds;i++){
		if (rightAttribNameList[i] == *rightorder){
			rightSeqAttrPos = i;
			break;
		}
	}
	if (i >= rightCountFlds){
		THROW(new Exception(" Ordering attribute not projected "),);
	}
	
	string retVal;
	TRY(leftequalPtr = getOperatorPtr("=",leftIDs[leftSeqAttrPos],
			leftIDs[leftSeqAttrPos],retVal),);
	TRY(rightequalPtr = getOperatorPtr("=",rightIDs[leftSeqAttrPos],
			rightIDs[leftSeqAttrPos],retVal),);
	if (*function == "joinprev"){
		TRY(comparePtr = getOperatorPtr(">",leftIDs[leftSeqAttrPos],
			rightIDs[rightSeqAttrPos],retVal),);
	}
	else{
		TRY(comparePtr = getOperatorPtr("<",leftIDs[leftSeqAttrPos],
			rightIDs[rightSeqAttrPos],retVal),);
	}

}

const Tuple* Joins::getNext(){

// bool Joins::getNext(Tuple* retVal)
	
	assert(!"not implemented");
	return NULL;

	/*

	if (!moreInnerTup && innerRel.isEmpty()||outerRel.atEnd())
		if (!innerFill())	
			return false;
	if ((!moreOuterTup && outerRel.isEmpty()) || outerRel.atEnd())
		if (!outerFill())	
			return false;
	
	Tuple * leftTup = outerRel.get();
	Tuple * rightTup = innerRel.get();

	innerRel.step();
	if (innerRel.atEnd()){
		innerRel.rewind();
		outerRel.step();
	}
	
	while(1){
			
			if ((comparePtr->opPtr)(leftTup[leftSeqAttrPos],rightTup[rightSeqAttrPos])){
				if (evaluateList(myWhere,rightTup,leftTup))
					break;
			}

			if (*function == "joinnext"){
				if (!innerFill())
				return NULL;			
			}
			else{ 
				if (!outerFill())
					return NULL;
			}
		leftTup = outerRel.get();
		rightTup = innerRel.get();
	}
	innerRel.step();
	if (innerRel.atEnd()){
		innerRel.rewind();
		outerRel.step();
	}

	// size of the tuple should be leftCountFlds+rightCountFlds

	for(int i = 0;i < leftCountFlds;i++)
		retVal[i] = leftTup[i];
	for(int i = 0;i < rightCountFlds;i++)
		retVal[i+leftCountFlds] = rightTup[i];
	return true;

*/
}

bool Joins::outerFill(){
	
	assert(!"not implemented");
	return false;
/*
	outerRel.removeAll();
	nextOuterTup = new Tuple[left->getNumFlds()];
	assert(nextOuterTup);
	if (!moreOuterTup)
		moreOuterTup = left->getNext(nextOuterTup);
	if (!moreOuterTup)
		return false;
	
	outerRel.append(nextOuterTup);
	while(1){
		
		Tuple* next = new Tuple[left->getNumFlds()];
		bool more = left->getNext(next); 
		if (!more){
			moreOuterTup = false;
			break;
		}
		if (leftequalPtr->opPtr(nextOuterTup[leftSeqAttrPos],
				next[leftSeqAttrPos]))
				outerRel.append(next);	
		else{
			nextOuterTup = next;
			break;
		}
	}
	outerRel.rewind();
	return true;
*/

}

bool Joins::innerFill(){
	
	assert(!"not implemented");
	return NULL;

/* commented by DD

	innerRel.removeAll();
	nextInnerTup = new Tuple[right->getNumFlds()];
	assert(nextInnerTup);
	if (!moreInnerTup)
		moreInnerTup = right->getNext(nextInnerTup);
	
	if (!moreInnerTup)
		return false;
	innerRel.append(nextInnerTup);
	
	while(1){
		
		Tuple* next = new Tuple[right->getNumFlds()];
		bool more = right->getNext(next); 
		if (!more){
			moreInnerTup = false;
			break;
		}
		if (rightequalPtr->opPtr(nextInnerTup[rightSeqAttrPos],
				next[rightSeqAttrPos]))
				innerRel.append(next);	
		else{
			nextInnerTup = next;
			break;
		}
	}
	innerRel.rewind();
	return true;
*/
}

Site* JoinTable::Plan(List<Site*>*siteList,List<BaseSelection*>*selectList,
	List<BaseSelection*>*predicateList)
{
	Site * leftSite = 0,*rightSite = 0;
	if (leftJoin)
		TRY(leftSite = leftJoin->Plan(siteList,selectList,predicateList),NULL);
	
	siteList->rewind();
	while(!siteList->atEnd()){
		Site * site = siteList->get();
		bool rem = false;
		if (left && !leftSite){
			if (site->have(*(left->getAlias()))){
				leftSite = site;
				siteList->remove();
				rem = true;
			}
		}
		if (right && !rightSite){
			if (site->have(*(right->getAlias()))){
				rightSite = site;
				siteList->remove();	
				rem = true;
			}
		}
		if (!rem)
			siteList->step();	
	}
	if (!rightSite && !leftSite){ 
		THROW(new Exception(" Site for merge join couldnt be found "),NULL);
	}
	if (leftSite && rightSite){
		Site * tmp = new Joins(leftSite,rightSite,function);
		tmp->filter(selectList,predicateList);
		tmp->typify("execute");
		return tmp;
	}
	else if (leftSite)
			return  leftSite;
	else if (rightSite)
			return rightSite;
	return NULL;
}

