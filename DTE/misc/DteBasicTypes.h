#ifndef _BASICTYPES_H_
#define _BASICTYPES_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/misc/DteConfig.h"
#include "sysdep.h"

typedef unsigned char uchar;

typedef signed char int1;
typedef short int2;
typedef int int4;
typedef long long int8;

typedef unsigned char uint1;
typedef unsigned short uint2;
typedef unsigned int uint;
typedef unsigned int uint4;
typedef unsigned long long uint8;

typedef float float4;
typedef double float8;
typedef long double float16;

//kb: make a Tuple class??
typedef void Type;
typedef const Type* Tuple;

//kb: make a PageBuf class??
union PageBuf { char data[DTE_PAGESIZE]; float16 forAlignment; };

// some simple bitvector code
inline
void SetBit(char* p, int bit)
{
  p += bit / 8;
  uchar b = 1 << (bit % 8);
  *p |= b;
}

inline
bool GetBit(char* p, int bit)
{
  p += bit / 8;
  uchar b = 1 << (bit % 8);
  return (*p) & b;
}


// alignment stuff

// alignment for any basic datatype

//#define ALIGN_ROUND_DOWN(n, align)  ((n) & ~((align)-1))
//#define ALIGN_ROUND_UP(n, align)    ALIGN_ROUND_DOWN((n)+(align)-1, (align))

inline int AlignDown(uint x, int align) { return x & ~((align)-1); }
inline int Align(uint x, int align) { return AlignDown(x+align-1, align); }

template<class T>
inline T* AlignDown(T* x, int align) { return (T*)AlignDown(uint(x), align); }

template<class T>
inline T* Align(T* x, int align) { return (T*)Align(uint(x), align); }



#if defined(__GNUC__) || defined(__GNUG__)
// gcc can give us the alignment

const int INT1_ALIGN = __alignof__(int1);
const int INT2_ALIGN = __alignof__(int2);
const int INT4_ALIGN = __alignof__(int4);
const int INT8_ALIGN = __alignof__(int8);
const int FLOAT4_ALIGN = __alignof__(float4);
const int FLOAT8_ALIGN = __alignof__(float8);
const int FLOAT16_ALIGN = __alignof__(float16);
const int POINTER_ALIGN = __alignof__(void*);
const int MAX_ALIGN = FLOAT16_ALIGN;

#else
// define alignment manually

#if defined(__i386)
// alignment is not really required for 386 class machines...

const int INT1_ALIGN = 1;
const int INT2_ALIGN = 2;
const int INT4_ALIGN = 4;
const int INT8_ALIGN = 4;
const int FLOAT4_ALIGN = 4;
const int FLOAT8_ALIGN = 4;
const int FLOAT16_ALIGN = 4;
const int POINTER_ALIGN = 4;
const int MAX_ALIGN = 4;

#elif defined(__sparc)

const int INT1_ALIGN = 1;
const int INT2_ALIGN = 2;
const int INT4_ALIGN = 4;
const int INT8_ALIGN = 8;
const int FLOAT4_ALIGN = 4;
const int FLOAT8_ALIGN = 8;
const int FLOAT16_ALIGN = 8;
const int POINTER_ALIGN = 4;
const int MAX_ALIGN = 8;

#else

#error Unknown architecture

#endif
#endif

#if defined(LINUX)
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#endif

const int BIG_ENDIAN = 1;
const int LITTLE_ENDIAN = 2;

#if defined(_BIG_ENDIAN)
const int MY_ENDIAN = BIG_ENDIAN;
#elif defined(_LITTLE_ENDIAN)
const int MY_ENDIAN = LITTLE_ENDIAN;
#else

// define endian manually
#if defined(__i386)
const int MY_ENDIAN = LITTLE_ENDIAN;
#elif defined(__sparc)
const int MY_ENDIAN = BIG_ENDIAN;
#else
#error Unknown architecture
#endif

#endif


// network stuff

// define host to network double
#if defined(__i386)

inline void Swap8(void* dest, const void* src)
{
  uchar* d = (uchar*)dest;
  uchar* s = (uchar*)src + 7;
  *d++ = *s--;
  *d++ = *s--;
  *d++ = *s--;
  *d++ = *s--;
  *d++ = *s--;
  *d++ = *s--;
  *d++ = *s--;
  *d++ = *s--;
}

inline double htond(const double x)
{
  double y;
  Swap8(&y, &x);
  return y;
}

inline double ntohd(const double x)
{
  double y;
  Swap8(&y, &x);
  return y;
}

#elif defined(__sparc)

#define htond(X) (X)
#define ntohd(X) (X)

#else
#error Unknown architecture
#endif


#endif // _BASICTYPES_H_
