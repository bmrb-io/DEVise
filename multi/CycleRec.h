/* CycleRec.h */
#ifndef CycleRec_h
#define CycleRec_h

struct CycleRec {
	int time;
	int last_time;
	int length;
	int occ;
	int working;
	int cpu_time;
	int suspended;
	double eff;
};

#endif
