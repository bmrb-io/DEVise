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
 */

#include"joins.h"

void Joins::typify(String  option){
	
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

	String * leftorder = left->getOrderingAttrib();
	String * rightorder = right->getOrderingAttrib();
	TypeID * leftIDs = left->getTypeIDs();
	TypeID * rightIDs = right->getTypeIDs();
	
	if (!leftorder || !rightorder){
		THROW(new Exception("Cannot do composition on non sequences"),);
	}
	String *  leftAttribNameList = left->getAttNamesOnly();
	String *  rightAttribNameList = right->getAttNamesOnly();
	
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
	
	String retVal;
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

Tuple * Joins::getNext()
{
	
	if (!nextInnerTup && innerRel.isEmpty()||outerRel.atEnd())
		if (!innerFill())	
			return NULL;
	if ((!nextOuterTup && outerRel.isEmpty()) || outerRel.atEnd())
		if (!outerFill())	
			return NULL;
	
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
	Tuple * retVal = new Tuple[leftCountFlds+rightCountFlds];
	for(int i = 0;i < leftCountFlds;i++)
		retVal[i] = leftTup[i];
	for(int i = 0;i < rightCountFlds;i++)
		retVal[i+leftCountFlds] = rightTup[i];
	return retVal;
}

bool Joins::outerFill(){
	
	outerRel.removeAll();
	if (!nextOuterTup)
		nextOuterTup = left->getNext();
	if (!nextOuterTup)
		return false;
	
	outerRel.append(nextOuterTup);
	while(1){
		
		Tuple * next = left->getNext(); 
		if (!next){
			nextOuterTup = NULL;
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
}

bool Joins::innerFill(){
	
	innerRel.removeAll();
	if (!nextInnerTup)
		nextInnerTup = right->getNext();
	
	if (!nextInnerTup)
		return false;
	innerRel.append(nextInnerTup);
	
	while(1){
		
		Tuple * next = right->getNext(); 
		if (!next){
			nextInnerTup = NULL;
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
			if (site->have(left->getAlias())){
				leftSite = site;
				siteList->remove();
				rem = true;
			}
		}
		if (right && !rightSite){
			if (site->have(right->getAlias())){
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

