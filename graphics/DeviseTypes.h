/*
  $Id$

  $Log$
  Revision 1.3  1996/04/15 19:32:05  wenger
  Consolidated the two (nearly identical) functions for
  reading/parsing physical schema, and cleaned up the
  ParseCat.c module quite a bit.  As part of this, I added
  a new enum DevStatus (StatusOk, StatusFailed, etc.).

  Revision 1.2  1995/09/05 21:12:39  jussi
  Added/update CVS header.
*/

#ifndef DeviseTypes_h
#define DeviseTypes_h

typedef double Coord;
typedef char Boolean;

#if defined(HPUX) || defined(SUN) || defined(SOLARIS)
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

enum DevStatus {
	StatusInvalid = 0,
	StatusOk = 10000,
	StatusFailed,
	StatusWarn,
	StatusCancel,
	StatusWarnCancel
};

#endif
