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
  Revision 1.19  1998/07/10 13:45:08  beyer
  changes and bug fixes to get rtrees working in the plan reader.

  Revision 1.18  1998/06/28 21:47:41  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.17  1998/03/17 17:18:59  donjerko
  Added new namespace management through relation ids.

  Revision 1.16  1997/11/12 23:17:31  donjerko
  Improved error checking.

  Revision 1.15  1997/11/05 00:19:41  donjerko
  Separated typechecking from optimization.

  Revision 1.14  1997/10/02 02:27:28  donjerko
  Implementing moving aggregates.

  Revision 1.13  1997/09/05 22:20:09  donjerko
  Made changes for port to NT.

  Revision 1.12  1997/08/21 21:04:25  donjerko
  Implemented view materialization

  Revision 1.11  1997/08/10 20:30:56  donjerko
  Fixed the NO_RTREE option.

  Revision 1.10  1997/07/22 15:00:53  donjerko
  *** empty log message ***

  Revision 1.9  1997/06/21 22:48:01  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.8  1997/06/16 16:04:44  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.7  1997/06/14 22:34:35  liping
  re-write min/max and recId request with SQL queries

  Revision 1.5  1997/03/28 16:07:26  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef RTREE_READ_H
#define RTREE_READ_H

#if 0
//kb: fix RTrees

#include "myopt.h"
#include "Iterator.h"
#include "StandardRead.h"
#include "BoundingBox.h"


#ifndef __GNUG__
using namespace std;
#endif

class typed_key_t;
class typed_rtree_t;
class typed_cursor_t;
class db_mgr_jussi;


//kb: definition is in IndexParse.c, but should probably be moved...
int create_rtree(const string& filename, Iterator* input,
                 const vector<int>& key_fields, 
                 const vector<int>& add_fields);



struct RTreePred {
	bool bounded[2];
	bool closed[2];
	ConstantSelection* values[2];
	TypeID typeID;
	RTreePred(){
		bounded[0] = bounded[1] = false;
		closed[0] = closed[1] = true;
		values[0] = values[1] = NULL;
	}
	RTreePred(string opName, ConstantSelection* constant);
	~RTreePred(){
		// do not delete values;
	}
	void setTypeID(TypeID t){
		typeID = t;
		TRY(Type* negInf = createNegInf(typeID), NVOID );
		TRY(Type* posInf = createPosInf(typeID), NVOID );
		values[0] = new ConstantSelection(typeID, negInf);
		values[1] = new ConstantSelection(typeID, posInf);
	}
	void update(string opName, BaseSelection* constant){ // throws
		assert(constant->selectID() == CONST_ID);
		ConstantSelection* cconstant = (ConstantSelection*) constant;
		TRY(cconstant = cconstant->promote(typeID), NVOID );
		assert(cconstant->getTypeID() == typeID);
		RTreePred tmp(opName, cconstant);
		intersect(tmp);
	}
	void intersect(const RTreePred& pred);
	string toString() const {
		string retVal;
		if(closed[0]){
			retVal += "[";
		}
		else {
			retVal += "<";
		}
		if(bounded[0]){
			retVal += values[0]->toString();
		}
		else {
			retVal += "-Infinity";
		}
		retVal += ", ";
		if(bounded[1]){
			retVal += values[1]->toString();
		}
		else {
			retVal += "Infinity";
		}
		if(closed[1]){
			retVal += "]";
		}
		else {
			retVal += ">";
		}
		return retVal;
	}
};



class RTreeReadExec : public Iterator
{
public:

  RTreeReadExec(const string& _filename, int _root_page,
                BoundingBox* _bbox, const TypeIDList& _added_types);

  virtual ~RTreeReadExec();

  virtual void initialize();

  virtual const Tuple* getNext();

  virtual const TypeIDList& getTypes();

protected:

  string filename;
  int root_page;
  BoundingBox* bbox;
  TypeIDList added_types;
  TypeIDList types;             // key (bbox) types + added types

  db_mgr_jussi* db_mgr;
  typed_rtree_t* rtree;
  typed_cursor_t* cursor;
  typed_key_t* queryBox;
  vector<int> data_off;

  Type** tuple;
  int numKeyFlds;
  int numAddFlds;
  int dataSize;

  vector<UnmarshalPtr> keyUnmarshalPtrs;
  vector<UnmarshalPtr> dataUnmarshalPtrs;

private:

  RTreeReadExec(const RTreeReadExec& x);
  RTreeReadExec& operator=(const RTreeReadExec& x);
};



class RTreeIndex : public StandardRead {

	// Private data members like numFlds, attributeNames and typeIDs
	// refer to this index not the original file

	IndexDesc* indexDesc;
	RTreePred* rTreeQuery;
public:
	RTreeIndex(IndexDesc* indexDesc) : 
		StandardRead(), indexDesc(indexDesc) {
		numFlds = indexDesc->getTotNumFlds();
		stats = new Stats(numFlds);
		typeIDs = indexDesc->getAllTypeIDs();
		attributeNames = indexDesc->getAllAttrNms();
		rTreeQuery = new RTreePred[indexDesc->getNumKeyFlds()];
		for(int i = 0; i < indexDesc->getNumKeyFlds(); i++){
			rTreeQuery[i].setTypeID(typeIDs[i]);
		}
	}
	virtual ~RTreeIndex();
	bool canUse(BaseSelection* predicate);	// Throws exception
	void write(ostream& out){
		out << numFlds << " ";
		for(int i = 0; i < numFlds; i++){
			out << typeIDs[i] << " " << attributeNames[i] << " ";
		}
	}
     virtual ostream& display(ostream& out){
          string tmp;
		int numKeyFlds = getNumKeyFlds();
          for(int i = 0; i < numKeyFlds; i++){
               tmp += "Attr " + attributeNames[i] + ": ";
               tmp += rTreeQuery[i].toString() + "\n";
          }
          out << tmp;
          return out;
     }
	void setRTreeQuery(RTreePred* rTreeQuery){
		this->rTreeQuery = rTreeQuery;
	}
	int getNumKeyFlds(){
		return indexDesc->getNumKeyFlds();
	}
	int getNumAddFlds(){
		return indexDesc->getNumAddFlds();
	}
	Iterator* createExec();
};
#endif

#endif
