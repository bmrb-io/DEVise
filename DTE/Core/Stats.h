#ifndef STATS_H
#define STATS_H

#include <vector>

typedef float Cardinality;

class Stats{
	vector<int> fldSizes;
	Cardinality cardinality;
public:
	const static string KEYWD;
	Stats(){}
	Stats(int numFlds){

          // default stats

          fldSizes.reserve(numFlds);
          for(int i = 0; i < numFlds; i++){
               fldSizes[i] = 10;
          }
          cardinality = 1000;
	}
	const vector<int>& getFldSizes(){
		return fldSizes;
	}
	Cardinality getCardinality() const {
		return cardinality;
	}
	void read(istream& in){
		in >> cardinality;
		int numFlds;
		in >> numFlds;
          fldSizes.reserve(numFlds);
		for(int i = 0; i < numFlds; i++){
			in >> fldSizes[i];
		}
	}
	void display(ostream& out){
		out << "cardinal: " << cardinality << "; ";
		out << "fldSizes: ";
		for(int i = 0; i < fldSizes.size(); i++){
			out << fldSizes[i] << " ";
		}
	}
};

#endif
