/***************************************************************************
  WARNING:
    This package is sensitive to the computer architecture. Please define 
    the right constants for your architecture. In any case this package 
    works only if "float" and "double" are interpreted as IEEE standard.

    You will need to know if your computer is little or big endian with
    respect to byte ordering within a word and word ordering within a 
    double word. The assumption is that a word has 32 bits and a double 
    word has 64 bits.

    For example: SPARC stations and HP work stations are big endian with 
    respect to both byte and word ordering. The intel processors are 
    little endian with respect to both byte and word ordering.
***************************************************************************/
// comment the following definition if your architecture is little endian 
// with respect to byte ordering within words.
//#define BYTE_BIG_ENDIAN

// comment the following definition if your architecture is little endian 
// with respect to word ordering within double words.
//#define WORD_BIG_ENDIAN

#define FLOAT_EXP_MASK 0x7f800000
#define DOUBLE_EXP_MASK 0x7ff00000
#ifdef WORD_BIG_ENDIAN
#define FIRST_WORD(x) *((int*)(&(x)))
#define LAST_BYTE(x) *(((unsigned char*)(&(x)))+7)
#define LAST_SHORT(x) *(((unsigned short*)(&(x)))+3)
#define LAST_WORD(x) *(((int*)(&(x)))+1)
#define FIRST_WORD_PTR(x) ((int*)(x))
#define LAST_WORD_PTR(x) (((int*)(x))+1)
#else
#define FIRST_WORD(x) *(((int*)(&(x)))+1)
#define LAST_BYTE(x) *((unsigned char*)(&(x)))
#define LAST_SHORT(x) *((unsigned short*)(&(x)))
#define LAST_WORD(x) *((int*)(&(x)))
#define FIRST_WORD_PTR(x) (((int*)(x))+1)
#define LAST_WORD_PTR(x) ((int*)(x))
#endif
#define WORD_OF(x) *((int*)(&(x)))


#include <stdio.h>
#include <math.h>

/***************************************************************************
  The "print" functions print the contents of a floating point number as
  a string of bits. Non-relevant bits are omitted.
  The printing order is : space sign space exponent space mantissa EOLN
***************************************************************************/

void print(unsigned char a, int place)
{
  unsigned char mask = 128; 
  for(int i = 0; i < 8; i++)
    {
      if(a & mask) printf("1"); else printf("0");
      mask >>= 1;
      if(i == place) printf(" ");
    }
}

void print(float &f)
{
  unsigned char *str = (unsigned char*)(&f);
  printf(" ");
#ifdef BYTE_BIG_ENDIAN
  print(str[0],0);
  print(str[1],0);
  print(str[2],10);
  print(str[3],10);
#else
  print(str[3],0);
  print(str[2],0);
  print(str[1],10);
  print(str[0],10);
#endif
}

void print(double &d)
{
  unsigned char *str = (unsigned char*)(&d);
  printf(" ");
#ifdef BYTE_BIG_ENDIAN
#ifdef WORD_BIG_ENDIAN
  print(str[0],0);
  print(str[1],3);
  for(int i = 2; i < 8; i++) print(str[i],10);
#else
  print(str[4],0);
  print(str[5],3);
  print(str[6],10);
  print(str[7],10);
  for(int i = 0; i < 4; i++) print(str[i],10);
#endif
#else
#ifdef WORD_BIG_ENDIAN
  print(str[3],0);
  print(str[2],3);
  print(str[1],10);
  print(str[0],10);
  for(int i = 7; i >= 4; i--) print(str[i],10);
#else
  print(str[7],0);
  print(str[6],3);
  for(int i = 5; i >= 0; i--) print(str[i],10);
#endif
#endif
}


/***************************************************************************
  The "exponent" functions return the actual value of the exponent used 
  for calculating the value of a floating point number
***************************************************************************/
int exponent(float &f)
{
  return (((WORD_OF(f) & FLOAT_EXP_MASK) >> 23) - 127);
}

int exponent(double &d)
{
  return (((FIRST_WORD(d) & DOUBLE_EXP_MASK) >> 20) - 1023);
}

/***************************************************************************
  The "raw_exponent" functions return a value that is a linear transformation
  of the "exponent" function. The slope of the function is a non-negative 
  integer power of 2.
***************************************************************************/
int raw_exponent(float &f)
{
  return (WORD_OF(f) & FLOAT_EXP_MASK);
}

int raw_exponent(double &d)
{
  return (FIRST_WORD(d) & DOUBLE_EXP_MASK);
}

/***************************************************************************
  The "next_up" functions change the value of the argument to the smallest
  possible value that is still higher than the value of the argument before
  the change.
  The functions assume the existance of a return value.
***************************************************************************/
void next_up(float &f)
{
  float f1 = f;
  WORD_OF(f1) &= FLOAT_EXP_MASK;
  float f2 = f1;
  WORD_OF(f2) |= 1; 
  f += (f2 - f1);
}

void next_up(double &d)
{
  double d1 = 0.0;
  FIRST_WORD(d1) = FIRST_WORD(d) & DOUBLE_EXP_MASK;
  double d2 = d1;
  LAST_WORD(d2) = 1;
  d += (d2 - d1);
}


/***************************************************************************
  The "next_down" functions change the value of the argument to the largest
  possible value that is still lower than the value of the argument before
  the change.
  The functions assume the existance of a return value.
***************************************************************************/
void next_down(float &f)
{
  float f1 = f;
  WORD_OF(f1) &= FLOAT_EXP_MASK;
  float f2 = f1;
  WORD_OF(f2) |= 1; 
  f -= (f2 - f1);
}

void next_down(double &d)
{
  double d1 = 0.0;
  FIRST_WORD(d1) = FIRST_WORD(d) & DOUBLE_EXP_MASK;
  double d2 = d1;
  LAST_WORD(d2) = 1;
  d -= (d2 - d1);
}


/***********************************************************************
  Functions for safe sums and subtractions
************************************************************************/

double sum_up(double a, double b)
{
  double c = a+b;
  next_up(c);
  return c;
}

double sum_down(double a, double b)
{
  double c = a+b;
  next_down(c);
  return c;
}

double sub_up(double a, double b)
{
  double c = a-b;
  next_up(c);
  return c;
}

double sub_down(double a, double b)
{
  double c = a-b;
  next_down(c);
  return c;
}

double mul_up(double a, double b)
{
  double c = a*b;
  next_up(c);
  return c;
}

double mul_down(double a, double b)
{
  double c = a*b;
  next_down(c);
  return c;
}

double div_up(double a, double b)
{
  double c = a/b;
  next_up(c);
  return c;
}

double div_down(double a, double b)
{
  double c = a/b;
  next_down(c);
  return c;
}

/***********************************************************************
  Really really ugly functions for dot product
************************************************************************/

static int log_tables[256] = {0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,
		              5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
		              6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		              6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		              7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		              7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		              7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		              7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};

int logint(unsigned int n)
{
  if(n < 128) 
    return log_tables[n];
  return (8 + logint(n >> 8));
}


#include "heap.h"
static ArrayHeap P_H;	
static double P_A[1024];

void set_up_dot(double *A, double *B, int size)
{
  double a1,a2,b1,b2;
  int i,j;
  P_H.reset();
  j = 0;
  for(i = 0; i < size; i++) if(A[i] != 0.0 && B[i] != 0.0)
    {
      if(LAST_WORD(A[i]) & 0x07ffffff != 0) 
	{
          a1 = A[i];
	  LAST_WORD(a1) &= 0xf8000000;
	  a2 = A[i] - a1;
          if(LAST_WORD(B[i]) & 0x07ffffff != 0) 
	    {
              b1 = B[i];
	      LAST_WORD(b1) &= 0xf8000000;
	      b2 = B[i] - b1;
	      P_A[j] = a1 * b1;
	      j++;
	      P_A[j] = a2 * b1;
	      j++;
	      P_A[j] = a1 * b2;
	      j++;
	      P_A[j] = a2 * b2;
	      j++;
	    }
          else
	    {
	      P_A[j] = a1 * B[i];
	      j++;
	      P_A[j] = a2 * B[i];
	      j++;
	    }
	}
      else if(LAST_WORD(B[i]) & 0x07ffffff != 0) 
	{
          b1 = B[i];
	  LAST_WORD(b1) &= 0xf8000000;
	  b2 = B[i] - b1;
	  P_A[j] = A[i] * b1;
	  j++;
	  P_A[j] = A[i] * b2;
	  j++;
        }
      else
	{
          P_A[j] = A[i] * B[i];
	  j++;
	}
    }
  for(j--; j >= 0; j--)
    P_H.insert(j,exponent(P_A[j]));
}

double sum_vector()
{
 
  if(P_H.get_size() == 1) 
    return P_A[P_H.pop()];

  int curr_exp = P_H.top_priority();
  int current = P_H.pop();
  while(P_H.get_size() > 0)
    {
      int next_exp = P_H.top_priority();
      int exp_diff = curr_exp - next_exp - logint(P_H.get_size());
      if(exp_diff > 52)
        {
          return P_A[current];
        }
      if(exp_diff > 1)
        {
          return P_A[current] + sum_vector();
        }
      
      int next = P_H.pop();
      if(LAST_BYTE(P_A[current]) == 0)
	{
          if(LAST_SHORT(P_A[next]) == 0)
	    P_A[current] += P_A[next];
          else
	    {
              double a = P_A[next];
	      LAST_BYTE(a) = 0;
	      P_A[next] -= a;
	      P_A[current] += a;
	      if(P_A[next] != 0.0) P_H.insert(next,exponent(P_A[next]));
	    }
	}
      else
	{
	  double b = P_A[current];
	  LAST_BYTE(P_A[current]) = 0;
	  b -= P_A[current];
	  if(LAST_SHORT(P_A[next]) == 0)
	    {
	      P_A[current] += P_A[next];
	      P_A[next] = b;
	      if(P_A[next] != 0.0) P_H.insert(next,exponent(b));
            }
          else
	    {
              double a = P_A[next];
	      LAST_SHORT(a) = 0;
	      P_A[next] -= a;
	      P_A[current] += a;
	      P_A[next] += b;
	      if(P_A[next] != 0.0) P_H.insert(next,exponent(P_A[next]));
	    }
	}
      if(P_A[current] == 0.0)
	{
          if(P_H.get_size() > 0)
	    {
              curr_exp = P_H.top_priority();
	      current = P_H.pop();
	    }
          else return 0.0;
	}
      else
	{
          curr_exp = exponent(P_A[current]);
          if(P_H.get_size() > 0 && curr_exp < P_H.top_priority())
	    {
              P_H.insert(current,curr_exp);
	      curr_exp = P_H.top_priority();
	      current = P_H.pop();
	    }
        }
    }
  return P_A[current];
}

void dot(double *A, double *B, int size, double &up_value, double &down_value)
{
   set_up_dot(A,B,size);
   if(P_H.get_size() == 0)
     up_value = down_value = 0.0;
   else
     {
       up_value = down_value = sum_vector();
       next_up(up_value);
       next_down(down_value);
     }
}

int sign(double &d)
{
  if(d > 0.0) return 1;
  if(d < 0.0) return -1;
  return 0;
}

int sign_of_dot(double *A, double *B, int size)
{
double dsign = 0.0;
for(int i = 0; i < size; i++) if(A[i] != 0.0 && B[i] != 0.0) dsign += A[i] * B[i];
return sign(dsign);
  set_up_dot(A,B,size);
  if(P_H.get_size() == 0)
     return 0;

  /* compute sign */
  if(P_H.get_size() == 1) 
    {
      return sign(P_A[P_H.pop()]);
    }

  int curr_exp = P_H.top_priority();
  int current = P_H.pop();


  while(P_H.get_size() > 0)
    {
      int next_exp = P_H.top_priority();
      int exp_diff = curr_exp - next_exp - logint(P_H.get_size());
      if(exp_diff > 1)
        {
          return sign(P_A[current]);
        }
      int next = P_H.pop();
      if(LAST_BYTE(P_A[current]) == 0)
	{
          if(*((unsigned short *)(P_A+next))  == 0)
          if(LAST_SHORT(P_A[next])  == 0)
	    {
	      P_A[current] += P_A[next];
	    }
          else
	    {
              double a = P_A[next];
	      LAST_SHORT(a) = 0;
	      P_A[next] -= a;
	      P_A[current] += a;
	      if(P_A[next] != 0.0)
		{
                  P_H.insert(next,exponent(P_A[next]));
                }
	    }
	}
      else
	{
	  double b = P_A[current];
	  LAST_BYTE(P_A[current]) = 0;
	  b -= P_A[current];
	  if(LAST_SHORT(P_A[next]) == 0)
	    {
	      P_A[current] += P_A[next];
	      P_A[next] = b;
	      if(P_A[next] != 0.0)
                {
                  P_H.insert(next,exponent(b));
                }
            }
          else
	    {
              double a = P_A[next];
	      LAST_SHORT(a) = 0;
	      P_A[next] -= a;
	      P_A[current] += a;
	      P_A[next] += b;
	      if(P_A[next] != 0.0) 
                {
		  P_H.insert(next,exponent(P_A[next]));
		}
	    }
	}
      if(P_A[current] == 0.0)
	{
          if(P_H.get_size() > 0) 
	    {
	      curr_exp = P_H.top_priority();
	      current = P_H.pop();
            }
          else 
            {
	      return 0;
	    }
	}
      else 
	{
	  curr_exp = exponent(P_A[current]);
          if(P_H.get_size() > 0 && curr_exp < P_H.top_priority())
	    {
              P_H.insert(current,curr_exp);
	      curr_exp = P_H.top_priority();
	      current = P_H.pop();
	    }
	}
    }
  return sign(P_A[current]);
}

void flip_bits(float& f)
{
  if(f < 0) *((int*)(&f)) ^= 0x7fffffff;
}

void flip_bits(double& d)
{
  if(d < 0)
    {
      FIRST_WORD(d) ^= 0x7fffffff;
      LAST_WORD(d) ^= 0xffffffff;
    }
}

float reconstruct_float(int i)
{
  float result = *((float*)(&i));
  flip_bits(result);
  return result;
}

double reconstruct_double_low(int i)
{
  double result;
  FIRST_WORD(result) = i;
  if(i < 0)
    LAST_WORD(result) = 0;
  else
    LAST_WORD(result) = 0xffffffff;
  flip_bits(result);
  return result;
}

double reconstruct_double_high(int i)
{
  double result;
  FIRST_WORD(result) = i;
  if(i > 0)
    LAST_WORD(result) = 0;
  else
    LAST_WORD(result) = 0xffffffff;
  flip_bits(result);
  return result;
}

double reconstruct_double(int first, int last)
{
  double result;
  FIRST_WORD(result) = first;
  LAST_WORD(result) = last;
  flip_bits(result);
  return result;
}

long long reconstruct_long_low(int i)
{
  long long result;
  FIRST_WORD(result) = i;
  if(i < 0)
    LAST_WORD(result) = 0xffffffff;
  else
    LAST_WORD(result) = 0;
  return result;
}

long long reconstruct_long_high(int i)
{
  long long result;
  FIRST_WORD(result) = i;
  if(i > 0)
    LAST_WORD(result) = 0xffffffff;
  else
    LAST_WORD(result) = 0;
  return result;
}

long long reconstruct_long(int first, int last)
{
  long long result;
  FIRST_WORD(result) = first;
  LAST_WORD(result) = last;
  return result;
}

int str_gt(char *a, int len_a, char *b, int len_b)
{
//printf("Comparing strings %d $",len_a);
//for(int I = 0; I < len_a; I++) printf("%c",a[I]); 
//printf("$ to %d $",len_b);
//for(int I = 0; I < len_b; I++) printf("%c",b[I]); 
//printf("$\n");


  int i;
  if(len_a >= len_b)
    for(i = 0; i <len_b; i++)
      if(a[i] > b[i]) 
	{
//printf("result >\n");
	  return 1;
        }
      else if(a[i] < b[i])
	{
//printf("result <\n"); 
	  return 0;
        }
  else 
    for(i = 0; i <len_a; i++)
      if(a[i] > b[i])
	{
//printf("result >\n");
	  return 1;
        }
      else if(a[i] < b[i])
	{
//printf("result <\n"); 
	  return 0;
        }
//printf("result <=\n");
  return 0;
}

double long_to_double_low(long long v)
{
  double result = (double)v;
  if(FIRST_WORD(v) > 1048575 || FIRST_WORD(v) < -1048575) 
    next_down(result);
  return result;
}

double long_to_double_high(long long v)
{
  double result = (double)v;
  if(FIRST_WORD(v) > 1048575 || FIRST_WORD(v) < -1048575) 
    next_up(result);
  return result;
}