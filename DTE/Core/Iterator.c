#include "Iterator.h"
#include "Stats.h"

Stats* PlanOp::getStats(){

	// default stats

	return new Stats(getNumFlds());
}
