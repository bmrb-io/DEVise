
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
