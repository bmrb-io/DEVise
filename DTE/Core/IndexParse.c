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
  Revision 1.26  1998/06/28 21:47:37  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.25  1998/03/17 17:18:54  donjerko
  Added new namespace management through relation ids.

  Revision 1.24  1998/03/13 04:02:16  donjerko
  *** empty log message ***

  Revision 1.23  1997/12/04 04:05:10  donjerko
  *** empty log message ***

  Revision 1.22  1997/11/12 23:17:23  donjerko
  Improved error checking.

  Revision 1.21  1997/11/05 00:19:37  donjerko
  Separated typechecking from optimization.

  Revision 1.20  1997/09/05 22:20:04  donjerko
  Made changes for port to NT.

  Revision 1.19  1997/08/25 15:28:08  donjerko
  Added minmax table

  Revision 1.18  1997/08/22 23:13:02  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.17  1997/08/21 21:04:19  donjerko
  Implemented view materialization

  Revision 1.16  1997/08/15 00:17:32  donjerko
  Completed the Iterator destructor code.

  Revision 1.15  1997/08/14 02:08:51  donjerko
  Index catalog is now an independent file.

  Revision 1.14  1997/08/10 20:30:54  donjerko
  Fixed the NO_RTREE option.

  Revision 1.13  1997/07/30 21:39:17  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.12  1997/07/22 15:00:52  donjerko
  *** empty log message ***

  Revision 1.11  1997/06/21 22:47:58  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.10  1997/06/16 16:04:41  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.9  1997/04/18 20:46:16  donjerko
  Added function pointers to marshall types.

  Revision 1.7  1997/03/28 16:07:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include<iostream.h>
#include<memory.h>
#include<string>
#include<assert.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>	// for perror
#include<errno.h>

#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "listop.h"
#include "ParseTree.h"
#include "Utility.h"
#include "Iterator.h"
#include "Aggregates.h"
#include "Inserter.h"
#include "MinMax.h"
#include "Interface.h"

#include "typed_rtree.h"
#include "dbJussi.h"
#include "SBMInit.h"

static const int DETAIL = 1;
LOG(extern ofstream logFile;)


//---------------------------------------------------------------------------

//kb: WARNING: this function is not thread (or mulitprocess) safe!!
//kb: it and the rtree code use files with fixed names.
// returns the root page id, or <0 on error
int create_rtree(const string& filename, Iterator* input,
                 const vector<int>& key_fields, 
                 const vector<int>& add_fields)
{
  assert(input);

  cout << "starting bulkload\ngetting types\n";

  const TypeIDList& input_types = input->getTypes();
  int numKeyFlds = key_fields.size();
  int numAddFlds = add_fields.size();
  int numFlds = numKeyFlds + numAddFlds;

  TypeIDList key_types;
  string rtreeISchema;
  vector<int> key_size(numKeyFlds);

  for(int i = 0 ; i < numKeyFlds ; i++) {
    const TypeID& type = input_types[key_fields[i]];
    key_types.push_back(type);
    rtreeISchema += rTreeEncode(type);
    key_size[i] = packSize(type);
  }

  TypeIDList add_types;
  vector<int> add_size(numAddFlds);

  int tot_add_size = 0;
  for(int i = 0 ; i < numAddFlds ; i++) {
    const TypeID& type = input_types[add_fields[i]];
    add_types.push_back(type);
    add_size[i] = packSize(type);
    tot_add_size += add_size[i];
  }

  vector<MarshalPtr> key_marshal = getMarshalPtrs(key_types);
  vector<MarshalPtr> add_marshal = getMarshalPtrs(add_types);


  // open the input file and write header info
  cout << "creating bulk load input\n";

  int points = 1;           // 0 = rectangle (bbox) data
  const char* load_filename = "/tmp/rtree_bulkload.in";
  const char* conv_filename = "/tmp/rtree_bulkload.conv";

  ofstream ind(load_filename, ios::out|ios::binary|ios::trunc);
  if( !ind ) {
    string msg = string("trouble opening ") + load_filename;
    THROW(new Exception(msg), -1);
  }

  ind << numKeyFlds << ' ' << tot_add_size << ' '
    << points << ' ' << rtreeISchema;
  if( !ind ) {
    string msg = string("trouble writing header to ") + load_filename;
    THROW(new Exception(msg), -1);
  }
  int fillSize  = (ind.tellp() + 1) % 8; // align on 8 byte boundary
  for(int i = 0; i < fillSize; i++){
    ind << ' ';
  }
  ind << endl;

  // find the convert program

  string convert_bulk_prog = DTE_ENV_VARS.valueOf(DTE_ENV_VARS.convert_bulk);
  if(convert_bulk_prog.empty()){
    string msg = "Please set the env var \"" + DTE_ENV_VARS.convert_bulk + 
        "\" to point to the convert_bulk executable";
    THROW(new Exception(msg), -1);
  }


  // load data into a temp file

  char* buf = new char[4096]; // no field better be bigger than this!!
  const Tuple* tup = input->getFirst();
  while(tup && ind) {
    for(int i = 0; i < numKeyFlds; i++){
      int k = key_fields[i];
      key_marshal[i](tup[k], buf);
      ind.write(buf, key_size[i]); // kb: why doesn't marshal return actual size??
    }
    for(int i = 0; i < numAddFlds; i++){
      int k = add_fields[i];
      add_marshal[i](tup[k], buf);
      ind.write(buf, add_size[i]);
    }
    tup = input->getNext();
  }
  delete [] buf;
  delete input;
  if( !ind ) {
    string msg = string("trouble writing to ") + load_filename;
    THROW(new Exception(msg), -1);
  }
  ind.close();

  // convert the data to a typeless bulkload file
  cout << "converting the bulk load input\n";
  //kb: why do this? why not write data through an rtree interface
  //kb: which can write this converted file directly??

  string cmd =
    convert_bulk_prog + " < " + load_filename + " > " + conv_filename;
  if(system(cmd.c_str()) != 0) {
    perror("system");
    string msg = "Failed to execute: " + cmd;
    msg += "\n Please set the env var \"" + DTE_ENV_VARS.convert_bulk +  
        "\" to point to the convert_bulk executable";
    THROW(new Exception(msg), -1);
  }

#if 0
  if(remove(load_filename) < 0){
    perror("remove:");
    string msg = string("Failed to remove tmp file: ") + load_filename;
    THROW(new Exception(msg), -1);
  }
#endif


  cout << "building the rtree\n";
  
  int bulk_file = open(conv_filename, O_RDWR);
  
  if(!bulk_file){
    string msg = string("Failed to open conv bulk data file:") + conv_filename;
    THROW(new Exception(msg), -1);
  }

  page_id_t root_page;
  db_mgr_jussi* db_mgr = new db_mgr_jussi(filename.c_str(), cacheMgr);
  typed_rtree_t* rtree = new typed_rtree_t(db_mgr);
  rtree->bulk_load(bulk_file, root_page, rtreeISchema.c_str());
  //cerr << "rtree debug:\n";
  //rtree->debug(stdout);
  delete rtree;
  delete db_mgr;


  int iroot_page = *((int*)root_page.data);
  cout << "index created in file " << filename 
    << "; root page=" << iroot_page << endl;

  close(bulk_file);
  if(remove(conv_filename) < 0){
    perror("remove:");
    string msg = string("Failed to remove tmp file: ") + conv_filename;
    THROW(new Exception(msg), -1);
  }

  return iroot_page;
}




//---------------------------------------------------------------------------


Iterator* IndexParse::createExec(){

	LOG(logFile << "Creating ");
	if(standAlone){
		LOG(logFile << "StandAlone ");
	}
	LOG(logFile << "index " << *indexName;)
	LOG(logFile << " on ");
	LOG(tableName->display(logFile));
	LOG(logFile << " (";)
	LOG(displayList(logFile, keyAttrs, ", ");)
	LOG(logFile << ")");
	if(additionalAttrs){
		LOG(displayList(logFile, additionalAttrs, ", "));
	}
	LOG(logFile << endl;)

	string tablename = tableName->toString();
     TRY(Site* site = ROOT_CATALOG.find(tableName), 0);

	assert(site);
	site->addTable(new TableAlias(tableName, new string(*indexName)));

	site->filter(NULL);

	int numKeyFlds = keyAttrs->cardinality();
	int numAddFlds = additionalAttrs->cardinality();

	string* keyFlds = new string[numKeyFlds];
	string* addFlds = new string[numAddFlds];
	keyAttrs->rewind();
	for(int i = 0; !keyAttrs->atEnd(); i++, keyAttrs->step()){
		assert(i < numKeyFlds);
		keyFlds[i] = *keyAttrs->get();
	}
	additionalAttrs->rewind();
	for(int i = 0; !additionalAttrs->atEnd(); i++, additionalAttrs->step()){
		assert(i < numAddFlds);
		addFlds[i] = *additionalAttrs->get();
	}

	string option = "execute";
	TRY(site->typify(option), 0);
	LOG(logFile << "Typified sites\n");
	LOG(logFile << site->getName());
	LOG(site->display(logFile));
	LOG(logFile << endl);

	int numTFlds = site->getNumFlds();
	const string* attrNms = site->getAttNamesOnly();

	int indirect[numTFlds];
	for(int i = 0; i < numKeyFlds; i++){
		int j;
		for(j = 0; j < numTFlds; j++){
			if(attrNms[j] == keyFlds[i]){
				indirect[i] = j;
				break;
			}
		}
		if(j == numTFlds){
			cerr << "Cannot find attribute: " << keyFlds[i] << endl;
		}
	}
	for(int i = 0; i < numAddFlds; i++){
		int j;
		for(j = 0; j < numTFlds; j++){
			if(attrNms[j] == addFlds[i]){
				indirect[i + numKeyFlds] = j;
				break;
			}
		}
		if(j == numTFlds){
			cerr << "Cannot find attribute: " << addFlds[i] << endl;
		}
	}
	//int numFlds = numKeyFlds + numAddFlds;
	const string* types = site->getTypeIDs();

        // create the index

	string fileToContainRTree = "./testRTree";  // fix this later

	LOG(logFile << "Creating executable:\n";)
	TRY(Iterator* inpIter = site->createExec(), 0);

        vector<int> key_fields(numKeyFlds);
        for(int i = 0 ; i < numKeyFlds ; i++) {
          key_fields[i] = i;
        }
        vector<int> add_fields(numAddFlds);
        for(int i = 0 ; i < numAddFlds ; i++) {
          add_fields[i] = numKeyFlds + i;
        }

	int indPgId =
          create_rtree(fileToContainRTree, inpIter, key_fields, add_fields);


        // register index in catalogs

	TypeID* keyTypes = new TypeID[numKeyFlds];
	TypeID* addTypes = new TypeID[numAddFlds];
	for(int i = 0; i < numKeyFlds; i++){
		keyTypes[i] = types[indirect[i]];
	}
	for(int i = 0; i < numAddFlds; i++){
		addTypes[i] = types[indirect[numKeyFlds + i]];
	}

	Tuple tuple[3];
	tuple[0] = (Type*) tablename.c_str();
	tuple[1] = (Type*) indexName->c_str();

//	assert(!"not implemented");	// add file name here

	IndexDesc tmpid(numKeyFlds, keyFlds, numAddFlds, addFlds,
			!standAlone, indPgId, keyTypes, addTypes);	
	tuple[2] = &tmpid;
	Inserter inserter;
	ostream* out = getIndexTableOutStream(ios::app);
	inserter.open(out, INDEX_SCHEMA.getNumFlds(), INDEX_SCHEMA.getTypeIDs());
	inserter.insert(tuple);

	return NULL;
}



//---------------------------------------------------------------------------


#if defined(OLD_STUFF)
//***


	int numFlds = numKeyFlds + numAddFlds;
	const string* types = site->getTypeIDs();
	MarshalPtr marshalPtrs[numFlds];
	int sizes[numFlds];
	int fixedSize = 0; 
	for(int i = 0; i < numFlds; i++){
		marshalPtrs[i] = getMarshalPtr(types[indirect[i]]);
		sizes[i] = packSize(types[indirect[i]]);
		fixedSize += sizes[i];
	}
	string rtreeISchema;
	for(int i = 0; i < numKeyFlds; i++){
		rtreeISchema += rTreeEncode(types[indirect[i]]);
	}

	int recIDSize = 0;
	for(int i = 0; i < numAddFlds; i++){
		recIDSize += sizes[i + numKeyFlds];
	}
	if(!standAlone) {
		recIDSize += sizeof(Offset);
	}
	int points = 1; // set to 0 for rectangles
	ostrstream line1;
	line1 << numKeyFlds << " " << recIDSize << " " << points << " ";
	line1 << rtreeISchema;
	int fillSize  = (line1.pcount() + 1) % 8;	// allign on 8 byte boundary
	for(int i = 0; i < fillSize; i++){
		line1 << " ";
	}
	line1 << endl;
	string bulkfile = *indexName + ".bulk";
	ofstream ind(bulkfile.c_str());
	assert(ind);
	ind << line1.rdbuf();
	int tupSize;
	Offset offset;

	LOG(logFile << "Creating executable:\n";)
	TRY(Iterator* inpIter = site->createExec(), 0);

        RandomAccessIterator* randIter = NULL;
        if( inpIter->GetIteratorType() == Iterator::RANDOM_ITERATOR ) {
          randIter = (RandomAccessIterator*)inpIter;
        } else {
          assert(!standAlone);  // standAlone (like primary) index needs
                                // random access
        }

	inpIter->initialize();
	const Tuple* tup = inpIter->getNext();
	if(!standAlone) {
          offset = randIter->getOffset();
          // cout << "offset = " << offset << endl;
        }

	MinAggregate mins[numTFlds];
	MaxAggregate maxs[numTFlds];

	for(int i = 0; i < numTFlds; i++){
		TRY(mins[i].typify(types[i]), NULL);
		TRY(maxs[i].typify(types[i]), NULL);
	}

	ExecAggregate* minExs[numTFlds];
	ExecAggregate* maxExs[numTFlds];

	for(int i = 0; i < numTFlds; i++){
		minExs[i] = mins[i].createExec();
		assert(minExs[i]);
		maxExs[i] = maxs[i].createExec();
		assert(maxExs[i]);
	}

	if(tup){ // make sure this is not empty

          for(int i = 0; i < numTFlds; i++){
			minExs[i]->initialize(tup[i]);
			maxExs[i]->initialize(tup[i]);
          }

		char flatTup[fixedSize];
		const Type* indexTup[numFlds];
		for(int i = 0; i < numFlds; i++){
			indexTup[i] = tup[indirect[i]];
		}
		marshal(indexTup, flatTup, marshalPtrs, sizes, numFlds);
		ind.write(flatTup, fixedSize);
		if(!standAlone){
			ind.write((char*) &offset, sizeof(Offset));
			offset = randIter->getOffset();
			// cout << "offset = " << offset << endl;
		}
		while((tup = inpIter->getNext())){

			for(int i = 0; i < numTFlds; i++){
				minExs[i]->update(tup[i]);
				maxExs[i]->update(tup[i]);
			}

			for(int i = 0; i < numFlds; i++){
				indexTup[i] = tup[indirect[i]];
			}
			marshal(indexTup, flatTup, marshalPtrs, sizes, numFlds);
			ind.write(flatTup, fixedSize);
			if(!standAlone){
				offset = randIter->getOffset();
				// cout << "offset = " << offset << endl;
				ind.write((char*) &offset, sizeof(Offset));
			}
		}
	}
	ind.close();
	string convBulk = bulkfile + ".conv";

	string convert_bulk = DTE_ENV_VARS.valueOf(DTE_ENV_VARS.convert_bulk);

	if(convert_bulk.empty()){
		string msg = "Please set the env var \"" + 
			DTE_ENV_VARS.convert_bulk + 
			"\" to point to the convert_bulk executable";
		THROW(new Exception(msg), NULL);
	}

	string cmd = convert_bulk + " < " + bulkfile + " > " + convBulk;
	if(system(cmd.c_str()) != 0){
		perror("system");
		string msg = "Failed to execute: " + cmd;
		msg += "\n Please set the env var \"" + 
				DTE_ENV_VARS.convert_bulk +  
				"\" to point to the convert_bulk executable";
		THROW(new Exception(msg), NULL);
		// throw Exception(msg);
	}

	int bulk_file = open(convBulk.c_str(), O_RDWR, 0600);

	if(!bulk_file){
		string msg = "Failed to open conv bulk data file:" + convBulk;
		THROW(new Exception(msg), NULL);
		// throw Exception(msg);
	}

	page_id_t root1;

	const char* fileToContainRTree = "./testRTree";  // fix this later

	db_mgr_jussi db_mgr(fileToContainRTree, cacheMgr);

	typed_rtree_t rtree(&db_mgr);

	rtree.bulk_load(bulk_file, root1, rtreeISchema.c_str());

	// this has created index

	printf("Created index\n");

	close(bulk_file);
	if(remove(bulkfile.c_str()) < 0){
		perror("remove:");
		string msg = string("Failed to remove tmp file: ") + bulkfile;
		THROW(new Exception(msg), NULL);
		// throw Exception(msg);
	}
	if(remove(convBulk.c_str()) < 0){
		perror("remove:");
		string msg = string("Failed to remove tmp file: ") + convBulk;
		THROW(new Exception(msg), NULL);
		// throw Exception(msg);
	}



//...clip...


	Tuple minTup[numTFlds];
	Tuple maxTup[numTFlds];
	for(int i = 0; i < numTFlds; i++){
		minTup[i] = minExs[i]->getValue();
		maxTup[i] = maxExs[i]->getValue();
	}

	Inserter mmFile;
	string mmFilePath = MinMax::getPathName(tablename);
	ISchema tableSchema(numTFlds, types, attrNms);
	TRY(mmFile.open(tableSchema, mmFilePath, ios::out), NULL);

	mmFile.insert(minTup);
	mmFile.insert(maxTup);

	StandardInterface interf(tableSchema, mmFilePath);
	MinMax::replace(tablename, &interf);
	CATCH(cerr << "Warning: "; cerr << currExcept->toString() << endl;);

	for(int i = 0; i < numTFlds; i++){
		delete minExs[i];
		delete maxExs[i];
	}


#endif
