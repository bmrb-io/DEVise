#include "Sort.h"
#include "StandardRead.h"
#include "Inserter.h"
#include "types.h"

#define MAX_MEM		16000000
#define RUNS		10


void Sort::initialize(){
	// creates all the runs
	// Generate run using input->getNext
	// when ready to output use inserter->insert(tuple) 
	// To create temp files use Inserter(), open ...
	// MemorySize is a constant (compile time)

	// use function pointers for sorting (types.h: getOperatorPtr).
}

Tuple* Sort::getNext(){
	// open all the runs
	// To read temp file use StandardRead ...
	// use heap to get max/min
	// return tuple
	return NULL;
}
