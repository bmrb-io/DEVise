#ifndef CORR_MATH_H_FILE__
#define CORR_MATH_H_FILE__


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

 void print(unsigned char a, int place);
 void print(float &f);
 void print(double &d);
 int exponent(float &f);
 int exponent(double &d);
 int raw_exponent(float &f);
 int raw_exponent(double &d);
 void next_up(float &f);
 void next_up(double &d);
 void next_down(float &f);
 void next_down(double &d);
 double sum_up(double a, double b);
 double sum_down(double a, double b);
 double sub_up(double a, double b);
 double sub_down(double a, double b);
 double mul_up(double a, double b);
 double mul_down(double a, double b);
 double div_up(double a, double b);
 double div_down(double a, double b);
 int logint(unsigned int n);
 void set_up_dot(double *A, double *B, int size);
 double sum_vector();
 void dot(double *A, double *B, int size, double &up_value, double &down_value);
 int sign(double &d);
 int sign_of_dot(double *A, double *B, int size);

void flip_bits(float& f);
void flip_bits(double& d);
float reconstruct_float(int i);
double reconstruct_double_low(int i);
double reconstruct_double_high(int i);
double reconstruct_double(int first, int last);
long long reconstruct_long_low(int i);
long long reconstruct_long_high(int i);
long long reconstruct_long(int first, int last);
int str_gt(char *a, int len_a, char *b, int len_b);
double long_to_double_low(long long v);
double long_to_double_high(long long v);

#endif
