#if 0
//kb: delete this file
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
  Revision 1.4  1996/12/16 11:13:05  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.3  1996/12/15 06:41:05  donjerko
  Added support for RTree indexes

  Revision 1.2  1996/12/05 16:05:59  wenger
  Added standard Devise file headers.

 */

class IndexRead : public GeneralRead {
     istream* in;
     int numFlds;
	String* attributeNames;
	GeneralRead* baseIterator;
	List<BaseSelection*>* indexPreds;
public:
	IndexRead::IndexRead(GeneralRead* baseIterator, istream* in) :
		in(in), numFlds(0), attributeNames(NULL), 
		baseIterator(baseIterator),
		indexPreds(NULL) {
	}
	void open(){ 	// Throws Exception
		assert(in);
		assert(baseIterator);
		baseIterator->open();
		// get numFlds & attributeNames;
		setStats();
	}
	virtual String * getOrderingAttrib(){
		assert(!"Ordering attrib not implemented for IndexRead");
		return NULL;
	}
	bool contains(String attribute){
		assert(attributeNames);
		for(int i = 0; i < numFlds; i++){
			if(attributeNames[i] == attribue){
				return true;
			}
		}
		return false;
	}
	void setIndexPreds(List<BaseSelection*>* preds){
		indexPreds = preds
	}
	virtual int getNumFlds(){
		return baseIterator->getNumFlds();
	}
     virtual String *getTypeIDs(){
          return baseIterator->getTypeIDs();
     }
     virtual String *getAttributeNames(){
          return baseIterator->getAttributeNames();
     }
     virtual Stats* getStats(){
          return baseIterator->getStats();
     }
     void setStats(){
          double selectivity = listSelectivity(indexPreds);
		assert(baseIterator);
          Stats* baseStats = baseIterator->getStats();
          assert(baseStats);
          int cardinality = int(selectivity * baseStats->cardinality);
          int* sizes = baseStats->sizes;
		int nf = baseIterator->getNumFlds();
          stats = new Stats(nf, sizes, cardinality);
     }
};
#endif
