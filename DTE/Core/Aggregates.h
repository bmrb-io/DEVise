#ifndef AGGREGATES_H
#define AGGREGATES_H

#include "types.h"
#include "myopt.h"
#include "site.h"
#include "catalog.h"

#include "TuplePtr.XPlex.h"

class GenFunction;
class AggWindow;
class Aggregates;

class Grouping{
		
	// Remeber this shares the iterator..
	public:
		Grouping(Site * iterator,int *positions=NULL,int * taboo = NULL,TypeID*types=NULL,int seqAttrPos,int count=0):
				iterator(iterator),positions(positions),taboo(taboo),types(types),seqAttrPos(seqAttrPos),count(count){
			// TupleList = new List<Tuple *>;
			TupleList.clear();
			state = NORMAL;
		}
		Tuple * getNext();
		void nextGroup();
		int tupleCompare(int *,GeneralPtr **,GeneralPtr ** ,int ,Tuple * ,Tuple * );
	private:
		Site * iterator;
		int *positions;	
		int *taboo;	
		TypeID *types;
		int seqAttrPos;
		int count;
		enum STATE {NORMAL,GROUPEND} state;
		TuplePtrXPlex TupleList;
		Tuple * next;		

		GeneralPtr ** lessPtrs;
		GeneralPtr ** equalPtrs;
		void sort();
};

class AggWindow 
{
	
	// For getting the private iterator stuff..
	friend class Aggregate;
	friend class GenFunction;
	//friend class Scan;
	public:
	AggWindow(Aggregates * agg,TypeID attrType,int pos):
		aggregate(agg),type(attrType),position(pos)
	{
		
		//Fix the function ptrs..
		// To change the sequence to adapt it to any other
		// ordering type simply insert new functions here.
		TypeID retType;
		equalOpr = compOpr = NULL;
		if (pos >= 0){
			TRY(equalOpr = getOperatorPtr("=",type,type,retType),);
			assert(equalOpr);
		}
		// TupleList =  new List<Tuple *>();
		TupleList.clear();
		windowFull = false;
		init();
	}	
	
	void init(){
		
		windowUpdated = false;
		nextStoredTuple = NULL;
		// TupleList->removeAll();
		TupleList.clear();
		state = INITIAL;
		valuesBeforePresent = 0;	
		presentPos = -1;
	}
	// To instruct this AggWindow to load the prepareCompOpr if it is needed
	// Remember this is needed only by functions  which
	// return 

	void prepareCompOpr(){
		TypeID retType;
		TRY(compOpr = getOperatorPtr("compReturnInt",type,type,retType),);
		assert(compOpr);
		if (retType != "int")
			assert(!" Illegal comparison operator implemented");
	}		
	void sort(	int *positions,TypeID * types,int count);
	void sortOn(int ,GeneralPtr);
	bool equalAttr(Tuple *n,Tuple *s){
		if(!equalOpr){
			return true;
		}
		assert(equalOpr->opPtr);
		assert(n);
		assert(s);
		IBool* val = (IBool *)(equalOpr->opPtr)(n[position],s[position]);
		assert(val);
		return val->getValue();
	}
	int compareAttr(Tuple *n,Tuple *s){
		assert(compOpr);
		return ((IInt *)(compOpr->opPtr)(n[position],s[position]))->getValue();
	}
	~AggWindow(){
		
		// delete TupleList;
		TupleList.clear();
	}
	// To fill the window..
	void fillWindow();

	void updateWindow(int low,int high,int full)
	{
		if (position < 0 && full != true){
			THROW(new Exception("Aggregate function without groupby clause"),);
		}
		if (full == 1){
			windowFull = true;
			return;
		}
		if (windowUpdated == false){
			windowLow = low;
			windowHigh = high;
			windowUpdated = true;
			return;
		}
	    if (windowLow > low )
			windowLow = low;
	    if (windowHigh < high)
			windowHigh = high;
	}

	
	// Get the tuple satisfying the position..

	Tuple * getTupleAtCurrPos();
		
	// for ints only or by value...
	// Function to return the lowest tuple satisfying the window criterion
	// by Position is used to know if the result is by positional ref
	int getLowPos(int windowPos,bool byPosition = false);
	int getHighPos(int windowPos, bool byPosition = false);
	
	private:
		
		// Ordering attribute type and position..
		Aggregates * aggregate;
		TypeID type;
		
		GeneralPtr * compOpr;	
		GeneralPtr * equalOpr;	
		
		int position;

		int valuesBeforePresent;	

		// To maintain a list of tuples for the moving window computation
		// List<Tuple *> * TupleList;
		TuplePtrXPlex TupleList;
		
		// I need to maintain the next tuple itself, since there is no
		// way of pushing back unwanted results...

		Tuple * nextStoredTuple;

		// Window size
		int windowLow,windowHigh;
		
		// To know the position of the current tuple in the
		// list
		int presentPos;
		
		// whole thing..
		enum STATE {INITIAL,MIDDLE,FINAL} state;
		bool windowUpdated;
		bool windowFull;
	
};

class GenFunction{
		
	typedef Type * (GenFunction::*AggFunctionPtr)();

	friend class Aggregates;
	GenFunction(AggWindow *fillNextFunc,String funcName, TypeID attribType,
	int pos, int windowLow,int windowHigh,int full){
		
		if (full)
			windowFull = true;
		wLow = windowLow;
		wHigh = windowHigh;
		this->pos= pos;
		
		byPos = false;

		this->attribType = attribType;
		filler = fillNextFunc;
		funcPtr = (AggFunctionPtr)NULL;

		// Default.
		retType= "int";	

		// Find the proper function and bind it to the ptr.
		
		TRY(computeFuncPtr(funcName),);
		
		cout << " The function " << funcName << " returns " << retType << endl;
		// This is not necessary..
		//scanner = new Scan(fillNextFunc,wLow,wHigh,false);
	}
	
	// This computes all the necessary parameters..
	void computeFuncPtr(String funcName){

		TypeID ret;

		if (funcName == "avg"){
			funcPtr = avg;
			
			TypeID dumb;
			// Get the add and division operators..
			TRY(addPtr = getOperatorPtr("+",attribType,attribType,retType),);
			TRY(divPtr = getOperatorPtr("/",retType,"double",retType),);
			assert(addPtr);
			assert(divPtr);
		}
		else if (funcName == "min"){
			funcPtr = min;
			
			// Get the < operators..
			TRY(grtrPtr = getOperatorPtr(">",attribType,attribType,retType),);
			assert(grtrPtr);
			retType = attribType;
		}
		else if (funcName == "max"){
			funcPtr = max;
			
			// Get the > operators..
			TRY(lessPtr = getOperatorPtr("<",attribType,attribType,retType),);
			assert(lessPtr);
			retType = attribType;
		}
		else if (funcName == "positional_min"){
			
			// This is important to call..
			filler->prepareCompOpr();
			byPos = true;
			funcPtr = min;
			
			// Get the < operators..
			TRY(lessPtr = getOperatorPtr("<",attribType,attribType,retType),);
			assert(lessPtr);
			retType = attribType;
		}

		else if (funcName == "positional_avg"){
			
			// This is important to call..
			filler->prepareCompOpr();
			byPos = true;
			funcPtr = avg;
			
			// Get the < operators..
			TRY(addPtr = getOperatorPtr("+",attribType,attribType,retType),);
			TRY(divPtr = getOperatorPtr("/",retType,"double",retType),);
			assert(addPtr);
			assert(divPtr);
		}
		else if (funcName == "positional_max"){
			// THis is important to call..
			filler->prepareCompOpr();
			byPos = true;
			funcPtr = max;
			// Get the < operators..
			TRY(grtrPtr = getOperatorPtr(">",attribType,attribType,retType),);
			assert(grtrPtr);
			retType = attribType;
		}
		else if (funcName == "count"){
			// THis is important to call..
			funcPtr = count;
			// Get the < operators..
			retType = "int";
		}
		else if(funcName == "sum"){

			// Need position, type and + operator of the attribute to sum.
			// position is recorded in var pos.

			funcPtr = sum;
			TRY(addPtr = getOperatorPtr("+",attribType,attribType,retType),);
			assert(addPtr);
		}
		else
			assert(!" Non implemented averaging function ");
	}
	
	TypeID & getTypeID(){
		
		//cout << " Returned type == " << retType << endl;
		return retType;

	}
	// Generic function returning the tuple..
	Type * computeAggregate() {
			return funcPtr();
	}
	
	
	// All these initially get the position (both low and high ) and
	// then extract all the tuples and perform the computation.

	private:
		
		int wHigh;
		int wLow;
		
		Tuple * scanNext();
		Tuple * scanNew();
			
		// All the needed pointers..
		GeneralPtr * addPtr;
		GeneralPtr * lessPtr;
		GeneralPtr * divPtr;
		GeneralPtr * grtrPtr;
		
		// Actual functions..
		Type *avg();
		Type *min();
		Type *max();
		Type *count();
		Type* sum();

		// Position in the tuple on which to operate
		int pos;
		AggFunctionPtr funcPtr;
		
		AggWindow * filler;
		
		bool byPos;
		bool windowFull;
		int currPos;
		int highPos;

		TypeID retType;
		TypeID attribType;
};


class Aggregates : public Site {
public:
	Aggregates(
		List<BaseSelection*>* selectClause,	// queries select clause
		BaseSelection * sequenceby,	// queries select clause
		BaseSelection* withPredicate,
		List<BaseSelection*>* groupBy ,	// group by clause
		BaseSelection* having = NULL			// having clause
	) : Site(), selList(selectClause),sequenceAttr(sequenceby),
			withPredicate(withPredicate),groupBy(groupBy)
	{
		
		Site::mySelect = selList;
		isApplicableValue = false;
		alreadyChecked = false;
		iterator = NULL;
	}
	
	virtual ~Aggregates(){
		// do not delete selList;
		delete sequenceAttr;
		delete iterator;
	}
	
	virtual bool isApplicable();
	virtual List<BaseSelection*>* getSelectList();
	virtual List<BaseSelection*>* filterList();
	virtual void typify(Site* inputIterator);
	
	virtual void enumerate(){

		iterator->enumerate();
	}
	virtual void initialize(){
		assert(iterator);
		iterator->initialize();
	}
	virtual Tuple* getNext();
	
	virtual String *getOrderingAttrib(){
		return iterator->getOrderingAttrib();
	}
	// Need to check this..
	virtual void reset(int lowRid, int highRid){
			TRY(iterator->reset(lowRid, highRid), );
	}

	friend class GenFunction;

private:
	
	List<BaseSelection*>* selList;
	BaseSelection * sequenceAttr;
	bool isApplicableValue;
	bool alreadyChecked;
	TypeID seqAttrType;
	int seqAttrPos;
	BaseSelection* withPredicate;	
	Site * iterator;
	Grouping *groupIterator; 
	int withPredicatePos;	
	List<BaseSelection*>*groupBy;

	int *positions;
	int *taboo;
	TypeID *types;
	// A list of ptrs to the GeneralFunction class to actually do the 
	// aggregation.
	GenFunction **funcPtr;
	
	// Calls GenFunction to set up apprpriate agg functions.
	TypeID &setFunctionPtr(GenFunction *& functionPtr,String funcName,
		int pos,TypeID type, int low,int high,int full);
	
	void getSeqAttrType(String *,TypeID *,int);
	//void getSeqAttrType(Site *,List<BaseSelection*>*);

	// Checks if the arg corresponds to a valid signature
	bool checkAggFunc(List<BaseSelection*>* args);
	
	// Fills the next set of values in to the window
	// We need to know the type of variable in order to do this
	// So we maintain function pointers to speed up things a bit

	AggWindow * fillNextFunc;
	
	// Need this for the iterator and other stuff..
	friend class AggWindow;

};

#endif
