#include "url.h"
#include "Inserter.h"
#include "StandardRead.h"
//#include "MemoryMgr.h"
#include "Sort.h"


void Inserter::open(const string& filename, int mode = ios::app)
{
  this->filename = filename;
  out.close();
  out.clear();
  out.open(filename.c_str(), mode);
  if( !out ) {
    string err("Could not open file: ");
    err += filename;
    THROW(new Exception(err), );
  }
}

void Inserter::open(const string& filename, const DteTupleAdt& tupAdt, 
                    int mode = ios::app)
{
  this->tupAdt = tupAdt;
  open(filename, mode);
}


void Inserter::insert(const Tuple* tuple)
{
  tupAdt.toAscii(out, tuple);
  out << '\n';
  if( !out ) {
    //THROW
    cerr << "error writing to file: " << filename << endl;
  }
}


#if 0
//kb: nothing in the file now
void Inserter::open(const ISchema& schema, string urlString, int mode)
{ // throws
        tupAdt = schema.getAdt();
	URL url(urlString);// thows
	TRY(out = url.getOutputStream(mode), NVOID );
}
#endif

#if 0
//kb: Modifier not used
void Modifier::replace(const string* key, const Type* object,
  const string* key2)
{
	int objIndex = (key2 == NULL ? 1 : 2);
	ifstream* ins = new ifstream(fileName.c_str());
	if(!ins->good()){
		string msg = "Could not open file: " + fileName;
		THROW(new Exception(msg), NVOID);
		// throw Exception(msg);
	}
	assert(ins && ins->good());
	int numFlds = schema.getNumFields();
	const TypeIDList& types = schema.getTypeIDs();
	StandReadExec* iter = new StandReadExec(types, ins);
     iter->initialize();
	const Tuple* tmpTuple;
	List<Tuple*> tupleList;
	TupleLoader tupleLoader;

	TypeID* typeIDs = makeArray(types);
	TRY(tupleLoader.open(numFlds, typeIDs), NVOID);
	delete [] typeIDs;

	TRY(tmpTuple = iter->getNext(), NVOID);

	bool isReplaced = false;
	
	Tuple* copy;
	while(tmpTuple){
		if(predicate(key, key2, tmpTuple)){
			ConstTuple* localTup = new ConstTuple[numFlds];
			for(int i = 0; i < numFlds; i++){
				localTup[i] = tmpTuple[i];
			}
			localTup[objIndex] = object;
			copy = tupleLoader.insert(localTup);
			isReplaced = true;
		}
		else{
			copy = tupleLoader.insert(tmpTuple);
		}
		tupleList.append(copy);
		TRY(tmpTuple = iter->getNext(), NVOID );
	}
	delete iter;
	if(!isReplaced){
		ConstTuple* localTup = new ConstTuple[numFlds];
		localTup[0] = IString::getTypePtr(key->c_str());
		if(key2){
			localTup[1] = IString::getTypePtr(key2->c_str());
		}
		localTup[objIndex] = object;
		copy = tupleLoader.insert(localTup);
		tupleList.append(copy);
		delete [] localTup;
	}
	Inserter newFile;
	TRY(newFile.open(schema, fileName, ios::out), NVOID );
	for(tupleList.rewind(); !tupleList.atEnd(); tupleList.step()){
		Tuple* tuple = tupleList.get();
		newFile.insert(tuple);
	}
}
#endif

UniqueInserter::UniqueInserter(const string& filename,
                               const DteTupleAdt& tupAdt, int mode)
: Inserter(), finalFile(filename), mode(mode)
{
  char fname[L_tmpnam];
  tmpnam(fname);
  tmpFile = fname;
  TRY(open(tmpFile, tupAdt), NVOID);
}

UniqueInserter::~UniqueInserter()
{
	// do duplicate elimination here

	//cerr << tmpFile << endl;

	TRY(open(finalFile, mode), NVOID);

	ifstream* istr = new ifstream(tmpFile.c_str(), ios::in|ios::nocreate);
	assert(*istr);

        // istr deleted by inp
	TRY(StandReadExec* inp = new StandReadExec(tupAdt, istr), NVOID);

        FieldList* sort_fields = new FieldList;
	int numFlds = tupAdt.getNumFields();
	for(int i = 0; i < numFlds; i++) {
          sort_fields->push_back(Field(tupAdt.getAdt(i), i));
        }

	TRY(Iterator* inp2 = new SortExec(inp, sort_fields, Ascending, true),
            NVOID);

	const Tuple* tup;
	for(tup = inp2->getFirst(); tup; tup = inp2->getNext()){
		insert(tup);
	}

	// do not delete istr

	delete inp2;
	remove(tmpFile.c_str());
}


//kb: move Deleter functions
#include "Deleter.h"
#include "DTE/types/DteBoolAdt.h"
#include "ExecExpr.h"

void Deleter::deleteWhere(ExecExpr* where)
{
  char tempFname[L_tmpnam];
  tmpnam(tempFname);
  {
    ofstream tempFile(tempFname);
    assert(tempFile);
    if( where ) {
      StandReadExec sr(tupAdt, filename);
      const Tuple* tup;
      for(tup = sr.getFirst() ; tup ; tup = sr.getNext()) {
        if( !DteBoolAdt::cast(where->eval(tup, NULL)) ) {
          tupAdt.toAscii(tempFile, tup);
          tempFile << '\n';
          if( !tempFile ) {
            cerr << "error deleting from " << filename << endl;
            cerr << "the file was not modified\n";
            assert(0);
          }
        }
      }
    }
  }
  {
    ifstream tempFile(tempFname);
    ofstream realFile(filename.c_str(), ios::out|ios::trunc);
    realFile << tempFile.rdbuf();
    if( !realFile ) {
      cerr << "error replacing file " << filename << endl;
      cerr << "the modified file is at: " << tempFname << endl;
      assert(0);
    }
  }
  unlink(tempFname);
  delete where;
}

