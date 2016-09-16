#ifndef RD_OPERATORS_H
#define RD_OPERATORS_H

#include "RDOPT.h"

/* Rounding-off for positive numbers */
#define RoundOff(r) \
    ((long) ((RD_float) (r) + 0.5))

/* Truncation for positive numbers */
#define Truncate(r) \
    ((long) ((RD_float) (r)))

/* Discretization of a real number, for storing the histogram */
#define Discretize(r) \
   (((r) >= 0.0) ? Truncate((r)*2.0) : \
   ((long) 0 - Truncate((r)*(-2.0))))

/* Undscretization: the positive flag is needed because 0
   can be either undiscretized to 0.25 or to -0.25 */
#define UnDiscretize(v,positive) \
   ((RD_float) (((RD_float) (v))/2.0) + (((positive)) ? 0.25 : -0.25))

#define UnDiscretizePlus(v) \
   ((RD_float) (((RD_float) (v))/2.0) + (0.25))

#define UnDiscretizeMinus(v) \
   ((RD_float) (((RD_float) (v))/2.0) - (0.25))

#define Quantize(r,q) \
  (((r) >= 0.0) ? RoundOff((r)/((RD_float) (q))) : \
  ((long) 0 - RoundOff((r)/((RD_float) (0-(q))))))

#define QuantizeDis(v,q) \
  (((v) >= 0) ? RoundOff(((RD_float) (v))/((RD_float) ((q)*2))) : \
  ((long) 0 - RoundOff(((RD_float) (v))/((RD_float) (0-((q)*2))))))

  
#define IntUnQuantize(v,q) ((v)*(q))

#define RealUnQuantize(v,q) ((RD_float) ((v)*(q)))

#define QuantToLowDis(q,qval) ((((qval)<<1)-1)*(q)) 
#define QuantToHighDis(q,qval) (((((qval)<<1)+1)*(q)) -1) 

#endif /* RD_OPERATORS_H */

