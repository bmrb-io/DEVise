/*
  $Id$

  $Log$*/

#ifndef DeviseTypes_h
#define DeviseTypes_h

typedef double Coord;
typedef char Boolean;

#ifdef IBM_POWER_STATION
const int true = 1;
const int false =0;
#endif

#if defined(HPUX) || defined(SUN) || defined(PENTIUM)
inline int trunc(float num){
	return (int)num;
}
inline int trunc(double num){
	return (int)num;
}
#endif


class MinMax {
public:
	static inline Coord min(Coord a, Coord b){
		 return ( (a) < (b) ? (a) : (b));
	};

	static inline int min(int a, int b){
	 return ( (a) < (b) ? (a) : (b));
	};

	static inline unsigned min(unsigned a, unsigned b){
	 return ( (a) < (b) ? (a) : (b));
	};

	static inline unsigned long min(unsigned long a, unsigned long b){
	 return ( (a) < (b) ? (a) : (b));
	};

	static inline Coord min(Coord a, Coord b, Coord c){
		Coord answer = min(a,b);
		if (c < answer)
			answer = c;
		return answer;
	};

	static inline Coord min(Coord a, Coord b, Coord c, Coord d){
		Coord answer = min(a,b,c);
		if (d < answer)
			answer = d;
		return answer;
	};


	static inline Coord max(Coord a, Coord b){
		return ( (a) > (b) ? (a) : (b)) ;
	};

	static inline int max(int a, int b){
		return ( (a) > (b) ? (a) : (b)) ;
	};

	static inline unsigned max(unsigned a, unsigned b){
		return ( (a) > (b) ? (a) : (b)) ;
	};

	static inline Coord max(Coord a, Coord b, Coord c){
		Coord answer =max(a,b);
		if (c > answer)
			answer = c;
		return answer;
	};

	static inline Coord max(Coord a, Coord b, Coord c, Coord d){
		Coord answer=max(a,b,c);
		if (d > answer)
			answer = d;
		return answer;
	};
};

#endif
