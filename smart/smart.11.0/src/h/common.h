#ifndef COMMONH
#define COMMONH
/*        $Header$*/

#include <stdio.h>
#define MAX(A,B)  ((A) > (B) ? (A) : (B))
#define MIN(A,B)  ((A) > (B) ? (B) : (A))

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#define UNDEF   -1

#ifndef MAXLONG
#define MAXLONG 2147483647L             /* largest long int. no. */
#endif

#endif /* COMMONH */
