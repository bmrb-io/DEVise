# 1 "codetree.C"

 

 

 
 

 
 
 

 
 
 

 

 

 
 
 
 
 
 
 


 

# 1 "general.h" 1

 

 

 
 

 
 
 

 
 
 

 

 

 
 
 
 
 
 
 


 




 

# 1 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/math.h" 1 3 4
 







extern "C" {


#pragma ident	"@(#)math.h	2.5	95/02/07"















 


typedef union _h_val {
  	unsigned long _i[2];
	double _d;
} _h_val;


extern const _h_val __huge_val;










 
















extern int signgam;




 


enum version {libm_ieee = -1, c_issue_4, ansi_1, strict_ansi};


extern const enum version _lib_version;




struct exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};




























 


extern double acos  (double)  ;
extern double asin  (double)  ;
extern double atan  (double)  ;
extern double atan2  (double, double)  ;
extern double cos  (double)  ;
extern double sin  (double)  ;
extern double tan  (double)  ;

extern double cosh  (double)  ;
extern double sinh  (double)  ;
extern double tanh  (double)  ;

extern double exp  (double)  ;
extern double frexp  (double, int *)  ;
extern double ldexp  (double, int)  ;
extern double log  (double)  ;
extern double log10  (double)  ;
extern double modf  (double, double *)  ;

extern double pow  (double, double)  ;
extern double sqrt  (double)  ;

extern double ceil  (double)  ;
extern double fabs  (double)  ;
extern double floor  (double)  ;
extern double fmod  (double, double)  ;



 


extern double erf  (double)  ;
extern double erfc  (double)  ;
extern double gamma  (double)  ;
extern double hypot  (double, double)  ;
extern int isnan  (double)  ;
extern double j0  (double)  ;
extern double j1  (double)  ;
extern double jn  (int, double)  ;
extern double lgamma  (double)  ;
extern double y0  (double)  ;
extern double y1  (double)  ;
extern double yn  (int, double)  ;



 


extern double acosh  (double)  ;
extern double asinh  (double)  ;
extern double atanh  (double)  ;
extern double cbrt  (double)  ;
extern double logb  (double)  ;
extern double nextafter  (double, double)  ;
extern double remainder  (double, double)  ;
extern double scalb  (double, double)  ;

 


extern double expm1  (double)  ;
extern int ilogb  (double)  ;
extern double log1p  (double)  ;
extern double rint  (double)  ;



 


extern int matherr  (struct exception *)  ;

 


extern double significand  (double)  ;

 


extern double copysign  (double, double)  ;
extern double scalbn  (double, int)  ;

 








 


extern float modff  (float, float *)  ;

# 1 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/floatingpoint.h" 1 3 4
 
 

 
 
 





extern "C" {


#pragma ident	"@(#)floatingpoint.h	2.4 94/06/09"

 




 










# 1 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdio.h" 1 3 4
 
 

 
 
 

 






#pragma ident	"@(#)stdio.h	1.37	95/08/29 SMI"	

# 1 "/usr/include/sys/feature_tests.h" 1 3 4
 
 

 
 
 




#pragma ident	"@(#)feature_tests.h	1.7	94/12/06 SMI"


extern "C" {


 












}



# 17 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdio.h" 2 3 4



extern "C" {




typedef unsigned int	size_t;


typedef long	fpos_t;













 


















































































typedef struct	 
{




	int		_cnt;	 
	unsigned char	*_ptr;	 

	unsigned char	*_base;	 
	unsigned char	_flag;	 
	unsigned char	_file;	 
} FILE;


extern FILE		__iob[60 ];



extern FILE		*_lastbuf;
extern unsigned char	*_bufendtab[];

extern unsigned char	 _sibuf[], _sobuf[];




extern int	remove(const char *);
extern int	rename(const char *, const char *);
extern FILE	*tmpfile(void);
extern char	*tmpnam(char *);



extern int	fclose(FILE *);
extern int	fflush(FILE *);
extern FILE	*fopen(const char *, const char *);
extern FILE	*freopen(const char *, const char *, FILE *);
extern void	setbuf(FILE *, char *);
extern int	setvbuf(FILE *, char *, int, size_t);
 
extern int	fprintf(FILE *, const char *, ...);
 
extern int	fscanf(FILE *, const char *, ...);
 
extern int	printf(const char *, ...);
 
extern int	scanf(const char *, ...);
 
extern int	sprintf(char *, const char *, ...);
 
extern int	sscanf(const char *, const char *, ...);





extern int	vfprintf(FILE *, const char *, void *);
extern int	vprintf(const char *, void *);
extern int	vsprintf(char *, const char *, void *);

extern int	fgetc(FILE *);
extern char	*fgets(char *, int, FILE *);
extern int	fputc(int, FILE *);
extern int	fputs(const char *, FILE *);
extern int	getc(FILE *);
extern int	getchar(void);
extern char	*gets(char *);
extern int	putc(int, FILE *);
extern int	putchar(int);
extern int	puts(const char *);
extern int	ungetc(int, FILE *);
extern size_t	fread(void *, size_t, size_t, FILE *);
extern size_t	fwrite(const void *, size_t, size_t, FILE *);
extern int	fgetpos(FILE *, fpos_t *);
extern int	fseek(FILE *, long, int);
extern int	fsetpos(FILE *, const fpos_t *);
extern long	ftell(FILE *);
extern void	rewind(FILE *);
extern void	clearerr(FILE *);
extern int	feof(FILE *);
extern int	ferror(FILE *);
extern void	perror(const char *);

extern int	__filbuf(FILE *);
extern int	__flsbuf(int, FILE *);

 





extern FILE	*fdopen(int, const char *);
extern char	*ctermid(char *);
extern int	fileno(FILE *);



 


# 237 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdio.h" 3 4


 




extern FILE	*popen(const char *, const char *);
extern char	*cuserid(char *);
extern char	*tempnam(const char *, const char *);
extern int	getopt(int, char *const *, const char *);

extern int	getsubopt(char **, char *const *, char **);

extern char	*optarg;
extern int	optind, opterr, optopt;
extern int	getw(FILE *);
extern int	putw(int, FILE *);
extern int	pclose(FILE *);



# 341 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdio.h" 3 4


 
































# 395 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdio.h" 3 4





}



# 33 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/floatingpoint.h" 2 3 4


# 1 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/sys/ieeefp.h" 1 3 4
 






#pragma ident	"@(#)ieeefp.h	2.7 94/11/09"


extern "C" {


 


# 47 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/sys/ieeefp.h" 3 4



enum fp_direction_type {	 
	fp_nearest	= 0,
	fp_negative	= 1,
	fp_positive	= 2,
	fp_tozero	= 3
};

enum fp_precision_type {	 
	fp_single	= 0,
	fp_precision_3	= 1,
	fp_double	= 2,
	fp_extended	= 3
};

enum fp_exception_type {	 
	fp_invalid	= 0,
	fp_denormalized	= 1,
	fp_division	= 2,
	fp_overflow	= 3,
	fp_underflow	= 4,
	fp_inexact	= 5
};

enum fp_trap_enable_type {	 
	fp_trap_invalid	= 0,
	fp_trap_denormalized	= 1,
	fp_trap_division	= 2,
	fp_trap_overflow	= 3,
	fp_trap_underflow	= 4,
	fp_trap_inexact	= 5
};


# 122 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/sys/ieeefp.h" 3 4


enum fp_class_type {		 
	fp_zero		= 0,
	fp_subnormal	= 1,
	fp_normal	= 2,
	fp_infinity   	= 3,
	fp_quiet	= 4,
	fp_signaling	= 5
};


}



# 35 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/floatingpoint.h" 2 3 4














typedef int sigfpe_code_type;	 

typedef void (*sigfpe_handler_type)();	 





extern sigfpe_handler_type sigfpe  (sigfpe_code_type, sigfpe_handler_type)  ;

 


typedef float single;			



typedef unsigned extended[3];


typedef long double quadruple;	 

typedef unsigned fp_exception_field_type;
				 



 




typedef char decimal_string[512 ];	
				 

typedef struct {
	enum fp_class_type fpclass;
	int	sign;
	int	exponent;
	decimal_string ds;	 


	int	more;		 


	int 	ndigits;	 


} decimal_record;

enum decimal_form {
	fixed_form,		 


	floating_form		 

};

typedef struct {
	enum fp_direction_type rd;	
				 
	enum decimal_form df;	 

	int ndigits;		 
} decimal_mode;

enum decimal_string_form {	 
	invalid_form,		 
	whitespace_form,	 
	fixed_int_form,		 
	fixed_intdot_form,	 
	fixed_dotfrac_form,	 
	fixed_intdotfrac_form,	 
	floating_int_form,	 	
	floating_intdot_form,	 
	floating_dotfrac_form,	 
	floating_intdotfrac_form, 
	inf_form,		 
	infinity_form,		 
	nan_form,		 
	nanstring_form		 
};

extern void single_to_decimal  (single *, decimal_mode *, decimal_record *,
				fp_exception_field_type *)  ;
extern void double_to_decimal  (double *, decimal_mode *, decimal_record *,
				fp_exception_field_type *)  ;
extern void extended_to_decimal  (extended *, decimal_mode *,
				decimal_record *, fp_exception_field_type *)  ;
extern void quadruple_to_decimal  (quadruple *, decimal_mode *,
				decimal_record *, fp_exception_field_type *)  ;

extern void decimal_to_single  (single *, decimal_mode *, decimal_record *,
				fp_exception_field_type *)  ;
extern void decimal_to_double  (double *, decimal_mode *, decimal_record *,
				fp_exception_field_type *)  ;
extern void decimal_to_extended  (extended *, decimal_mode *,
				decimal_record *, fp_exception_field_type *)  ;
extern void decimal_to_quadruple  (quadruple *, decimal_mode *,
				decimal_record *, fp_exception_field_type *)  ;

extern void string_to_decimal  (char **, int, int, decimal_record *,
				enum decimal_string_form *, char **)  ;
extern void func_to_decimal  (char **, int, int, decimal_record *,
				enum decimal_string_form *, char **,
				int (*)(void), int *, int (*)(int))  ;
extern void file_to_decimal  (char **, int, int, decimal_record *,
				enum decimal_string_form *, char **,
				FILE *, int *)  ;

extern char *seconvert  (single *, int, int *, int *, char *)  ;
extern char *sfconvert  (single *, int, int *, int *, char *)  ;
extern char *sgconvert  (single *, int, int, char *)  ;
extern char *econvert  (double, int, int *, int *, char *)  ;
extern char *fconvert  (double, int, int *, int *, char *)  ;
extern char *gconvert  (double, int, int, char *)  ;
extern char *qeconvert  (quadruple *, int, int *, int *, char *)  ;
extern char *qfconvert  (quadruple *, int, int *, int *, char *)  ;
extern char *qgconvert  (quadruple *, int, int, char *)  ;

extern char *ecvt  (double, int, int *, int *)  ;
extern char *fcvt  (double, int, int *, int *)  ;
extern char *gcvt  (double, int, char *)  ;

 



extern double atof  (const char *)  ;
extern double strtod  (const char *, char **)  ;


}



# 218 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/math.h" 2 3 4





}



# 37 "general.h" 2


# 1 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdlib.h" 1 3 4
 
 

 
 
 




#pragma ident	"@(#)stdlib.h	1.27	95/08/28 SMI"	









extern "C" {


typedef	struct {
	int	quot;
	int	rem;
} div_t;

typedef struct {
	long	quot;
	long	rem;
} ldiv_t;


typedef struct {
	long long	quot;
	long long	rem;
} lldiv_t;












typedef long	uid_t;















typedef __wchar_t wchar_t;




extern unsigned char	__ctype[];



extern double atof(const char *);
extern int atoi(const char *);
extern long int atol(const char *);
extern double strtod(const char *, char **);
extern long int strtol(const char *, char **, int);
extern unsigned long int strtoul(const char *, char **, int);

extern int rand(void);
extern void srand(unsigned int);





extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);

extern void abort(void);
extern int atexit(void (*)(void));
extern void exit(int);
extern char *getenv(const char *);
extern int system(const char *);

extern void *bsearch(const void *, const void *, size_t, size_t,
	int (*)(const void *, const void *));
extern void qsort(void *, size_t, size_t,
	int (*)(const void *, const void *));

extern int abs(int);
extern div_t div(int, int);
extern long int labs(long);
extern ldiv_t ldiv(long, long);

extern int mbtowc(wchar_t *, const char *, size_t);
extern int mblen(const char *, size_t);
extern int wctomb(char *, wchar_t);

extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);





extern double drand48(void);
extern double erand48(unsigned short *);
extern long jrand48(unsigned short *);
extern void lcong48(unsigned short *);
extern long lrand48(void);
extern long mrand48(void);
extern long nrand48(unsigned short *);
extern unsigned short *seed48(unsigned short *);
extern void srand48(long);
extern int putenv(const char *);
extern void setkey(const char *);






extern void swab(const char *, char *, int);





extern long a64l(const char *);
extern int dup2(int, int);
extern char *ecvt(double, int, int *, int *);
extern char *fcvt(double, int, int *, int *);
extern char *qecvt(long double, int, int *, int *);
extern char *qfcvt(long double, int, int *, int *);
extern char *qgcvt(long double, int, char *);
extern char *getcwd(char *, size_t);
extern char *getlogin(void);
extern int getopt(int, char *const *, const char *);
extern int getsubopt(char **, char *const *, char **);
extern char *optarg;
extern int optind, opterr, optopt;
extern char *getpass(const char *);
extern int getpw(uid_t, char *);
extern char *gcvt(double, int, char *);
extern int isatty(int);
extern char *l64a(long);
extern void *memalign(size_t, size_t);
extern char *mktemp(char *);
extern char *realpath(char *, char *);
extern char *ttyname(int);
extern int ttyslot(void);
extern void *valloc(size_t);
extern char *ptsname(int);
extern int  grantpt(int);
extern int  unlockpt(int);


extern long long atoll(const char *);
extern long long llabs(long long);
extern lldiv_t lldiv(long long, long long);
extern char *lltostr(long long, char *);
extern long long strtoll(const char *, char **, int);
extern unsigned long long strtoull(const char *, char **, int);
extern char *ulltostr(unsigned long long, char *);




# 282 "/s/gcc-2.7.2/sunx86_55/lib/gcc-lib/i386-unknown-solaris2.5/2.7.2/include/stdlib.h" 3 4



}



# 39 "general.h" 2





 





typedef char Char_Line[80];
typedef unsigned char byte;
typedef unsigned long word;


 

 

 


















 

 

 

inline int Min(int a, int b)
  { return (a < b ? a : b); }

inline int Max(int a, int b)
  { return (a > b ? a : b); }

inline long Round(double x)
  { return (x >= 0.0 ? long(x + 0.5) : long(x - 0.5)); }

inline float Sqr(float x)
  { return x * x; }

inline double dBW(double x) { return 4.34294481904 * log(x); }

inline double dBW_inv(double x) { return exp(0.2302585093 * x); }


 

 

 


void Warning(char *);

void Error(char *);

void Test_Pointer(void * ptr, char * msg = (  0) );

void Pause(void);

char * Input_Line(char * msg = (  0) , char * res = (  0) );

int Input_Int(char *);

float Input_Float(char *);

int  Input_Answer(char *);

FILE * Open_File(char * msg, char * mode);


 

 

 

class Chronometer
{
   

    int stat;          

    long mark, elp;    

   

  public:

    Chronometer(void) { elp = stat = 0; }

   

    void reset(void) { elp = stat = 0; }

    void start(char * s = (  0) );

    void stop(void);

    float read(void);

    void display(char * s = (  0) );

};   



 

 
# 32 "codetree.C" 2

# 1 "image_bw.h" 1

 

 

 
 

 
 
 

 
 
 

 

 

 
 
 
 
 
 
 


 








typedef float Pel_Type;




struct Image_Coord { int x, y; };


 

class Image_BW
{
   

    Image_Coord dim, pdim;

    int levels, bytes, mean, shift, smoothing;

    Pel_Type ** coeff;

   

    int max_levels(int);

    void assign_mem(Image_Coord, int);

    void free_mem(void);

    void extend(void);

   

  public :

    Image_BW(void) { levels = -1; }

    ~Image_BW(void) { free_mem(); }

   

    Pel_Type & operator[](const Image_Coord & c) {
      return coeff[c.x][c.y]; }

    Pel_Type & operator()(int i, int j) {
      return coeff[i][j]; }

    Pel_Type * address(const Image_Coord & c) {
      return coeff[c.x] + c.y; }

    Image_Coord dimension(void) { return dim; }

    Image_Coord pyramid_dim(void) { return pdim; }

    int transform_mean(void) { return mean; }

    int mean_shift(void) { return shift; }

    int smoothing_factor(void) { return smoothing; }

    int pyramid_levels(void) { return levels; }

    int pixel_bytes(void) { return bytes; }

    float compare(char * file_name);

    void read_pic(Image_Coord, char * file_name, int nbytes = 1);

    void write_pic(char * file_name);

    void reset(Image_Coord);

    void reset(Image_Coord, int nbytes, int m, int mshift,
               int smoothing_factor = 0);

    void transform(int smoothing_factor = 0);

    void recover(void);

    void dispose(void) { free_mem(); }

};   

 

 
# 33 "codetree.C" 2





# 1 "aritcode.h" 1

 

 

 
 

 
 
 

 
 
 


 






 

 

 

class Adaptive_Model
{
   

    int max_f, nsb, * cum_f, * freq, * indx_to_sb, * sb_to_indx;

   

    friend class Encoder;

    friend class Decoder;

   

    void update(int index);

    int select_symbol(long value, long * l, long * h);

    void new_interval(int symb, long * l, long * h);

   

  public :

    Adaptive_Model(void) { nsb = 0;  cum_f = (  0) ; }

    Adaptive_Model(int ns) { nsb = 0;  cum_f = (  0) ;  set_symbols(ns); }

    ~Adaptive_Model(void) { nsb = 0;  delete cum_f; }

   

    void reset(void);

    void set_symbols(int ns);

};   


 

class Encoder
{
   

    FILE * out_file;

    int bit_buffer, bit_index, closed, temp;

    long byte_counter, symbol_counter, low, high, bits_to_follow;

   

    void bit_plus_follow(int b);

    void update_interval(void);

    void reset(char * file_name);

    void stop(void);

   

  public :

    Encoder(void) { closed = 1; }

   

    long bytes_used(void) { return byte_counter; }

    long symbols_encoded(void) { return symbol_counter; }

    void open_file(void);

    void close_file(char * file_name);

    void open_file(char * file_name);

    void close_file(void);

    void code_symbol(int s, Adaptive_Model &);

    void code_bits(int bits, int word);

    int code_bit(int bit);

};   


 

class Decoder
{
   

    FILE * in_file;

    int bit_buffer, bit_index, extra_bits, ext_stop, closed;

    long low, high, value, symbol_counter, byte_counter, mss_symbols;

   

    void input_byte(void);

    void update_interval(void);

   

  public :

    Decoder(void) { closed = 1; }

   

    int end_of_file(void) { return (symbol_counter >= mss_symbols); }

    long bytes_used(void) { return byte_counter; }

    long symbols_decoded(void) { return symbol_counter; }

    long message_symbols(void) { return mss_symbols; }

    void open_file(char * file_name);

    void close_file(void);

    int decode_symbol(Adaptive_Model &);

    int decode_bits(int bits);

    int decode_bit(void);

};   

 

 

 

inline int Encoder::code_bit(int bit)
{
  long lm1 = low - 1, range = high - lm1;
  if (bit) {
    high = lm1 + range;  low += range >> 1; }
  else
    high = lm1 + (range >> 1);

  update_interval();
  return bit;
}

 

inline int Decoder::decode_bit(void)
{
  long lm1 = low - 1, range = high - lm1;
  int bit = int((((value - lm1) << 1) - 1) / range);
  if (bit) {
    high = lm1 + range;  low += range >> 1; }
  else
    high = lm1 + (range >> 1);

  update_interval();
  return bit;
}

 

 
# 38 "codetree.C" 2




 

struct Tree_Node
{
  Image_Coord coord;
  long state;
  Tree_Node * next;
};


 

const int SHF_x[4] = { 0, 0, 1, 1 }, SHF_y[4] = { 0, 1, 0, 1 };

char names[3][80], * pic_f_name = names[0], * cod_f_name = names[1],
      * new_f_name = names[2];

Chronometer code_time, total_time;

int pel_bytes, threshold_bits, min_bits, pyramid_levels, smoothing,
    mean, mean_shift;

long byte_budget, root_code;

Pel_Type act_value, threshold;

float bit_rate, rate_mult;

Tree_Node * LISP_head, * LISP_end;

Image_Coord dimension, pyramid_dim, root_dim;

Image_BW image;









  Decoder data_file;





  Adaptive_Model group_model[5], node_model[34], desc_model[34];





  Image_Coord LSP_dim;

  int LSP_plane, LSP_part, LSP_idx;

  float bias_const_1, bias_const_2, bias[32];

  float * LSP_mark[32], *** LSP_mtx, ** LSP_ptr;




 

int  Sorting_Pass(void);

int  Refinement_Pass(void);

void Node_Test(Tree_Node *);

void Desc_Test(Tree_Node *);



  int Node_Transition(int, const Image_Coord &, int *);

  int Desc_Transition(int, const Image_Coord &);











 

 

 

int Max_Levels(int n)
{
  int l1, l2;
  for (l1 = 0; !(n & 1); l1++) n >>= 1;
  for (l2 = l1 - 2; n; l2++) n >>= 1;
  return (l1 < l2 ? l1 : l2) - 1;
}

 

# 160 "codetree.C"

inline float Magn_Val(float & p, int & s) { return fabs(p); }


 

int  Sorting_Pass(void)
{

  act_value = threshold + (bias[LSP_plane] = bias_const_1 * threshold);


  Tree_Node * prev = LISP_head, * node = prev;

  for (; node = node->next; prev = node)
    if ((node->state & 0x55L) != 0x55L) {
      Node_Test(node);
      if (data_file.bytes_used() >= byte_budget) return 1 ; }

  for (node = prev = LISP_head; node = node->next; prev = node) {
    if ((node->state & 0xAAL) != 0xAAL) {
      Desc_Test(node);
      if (data_file.bytes_used() >= byte_budget) return 1 ; }

    if ((node->state & 0xFFL) == 0xFFL) {
      if (LISP_end == node) LISP_end = prev;
      prev->next = node->next;  delete node;  node = prev; }

  }

  return 0 ;
}

 

void Process_Image(void)
{



  bias_const_1 = 0.38 - 0.02 * smoothing;
  bias_const_2 = 0.25 + 0.07 * smoothing;
  threshold = pow(2, threshold_bits);  min_bits = -32;
  LSP_idx = LSP_part = LSP_plane = 0;  LSP_ptr = 0;
  LSP_dim.x = pyramid_dim.x >> 2;  LSP_dim.y = pyramid_dim.y << 2;
  Test_Pointer( LSP_mtx  = new   float ** [  LSP_dim.x ],   "LSP" ) ;



  int i, j, k, n = -1, t;
  for (i = 0; i <= 4; i++) {
    group_model[i].set_symbols(2);
    for (j = 0; j <= 4 - i; j++)
      for (k = 0;  k <= 4 - i - j; k++, n++)
        if (n >= 0) {
          if (t = i + k) node_model[n].set_symbols(1 << t);
          if (t = i + j) desc_model[n].set_symbols(1 << t); } }


  Image_Coord cd;
  Tree_Node * prev, * node;
  Test_Pointer( node  = new   Tree_Node ,   "LISP entry" ) ;
  LISP_head = LISP_end = node;

  int m = pyramid_levels;
  root_dim.x = pyramid_dim.x >> m;  root_dim.y = pyramid_dim.y >> m;
  long st = 0x2L + (root_code = long(m + 1) << 9);
  for (cd.x = 0; cd.x < root_dim.x; cd.x += 2)
    for (cd.y = 0; cd.y < root_dim.y; cd.y += 2) {
      Test_Pointer( node  = new   Tree_Node ,   "LISP entry" ) ;
      node->coord = cd;  node->state = st;
      LISP_end->next = node;  LISP_end = node; }
  LISP_end->next = (  0) ;

  for (;--threshold_bits >= min_bits; threshold /= 2) {
    if (Sorting_Pass()) break;
    if (Refinement_Pass()) break; }





  for (m = LSP_part; m > 0;) delete [] LSP_mtx[--m];
  delete [] LSP_mtx;

  code_time.stop();

  for (node = LISP_head; node;) {
    prev = node;  node = node->next;  delete prev; }
}


 
 

# 332 "codetree.C"


 
 

void Output_Signs(int nb, int sd, const Image_Coord & pc)
{
  Image_Coord nc;



  int m = 1, b = data_file.decode_bits(nb);


  for (int i = 0; i < 4; i++, sd >>= 2)
    if (sd & 1) {
    nc.x = pc.x + SHF_x[i];  nc.y = pc.y + SHF_y[i];







      if (!LSP_idx) {
        Test_Pointer( LSP_ptr  = new   float * [  LSP_dim.y ],   "LSP" ) ;
        LSP_mtx[LSP_part++] = LSP_ptr;  LSP_idx = LSP_dim.y; }
      float * ptr = LSP_ptr[--LSP_idx] = image.address(nc);




      *ptr = (b & m ? -act_value : act_value);


      m <<= 1; }




}

 

void Desc_Test(Tree_Node * act_node)
{
  Image_Coord pc = act_node->coord;




  int st_trans = Desc_Transition(int(act_node->state & 0xFFL), pc);


  if (!st_trans) return;

  act_node->state |= st_trans;  st_trans >>= 1;

  Tree_Node * new_node;
  long ns = (act_node->state & 0xFF00L) - 0x200L;
  if (ns < 0x300L) ns |= 0xAAL;
  int  root = ((act_node->state & 0xFF00L) >= root_code);

  for (int i = 0; i < 4; i++, st_trans >>= 2)
    if (st_trans & 0x1) {
      Test_Pointer( new_node  = new   Tree_Node ,   "LISP entry" ) ;
      if (root) {
        if (i == 3) ns -= 0x100L;
        new_node->coord.x = pc.x + SHF_x[i] * root_dim.x;
        new_node->coord.y = pc.y + SHF_y[i] * root_dim.y; }
      else {
        new_node->coord.x = (pc.x + SHF_x[i]) << 1;
        new_node->coord.y = (pc.y + SHF_y[i]) << 1; }
      new_node->next = (  0) ;  new_node->state = ns;
      LISP_end->next = new_node;  LISP_end = new_node;
      Node_Test(new_node); }
}

 

int Node_Transition(int old_st, const Image_Coord & pc, int * signs)
{
  int st_trans = 0, os = old_st, ps = old_st, mask = 1;
  int msk[3], count[4];

  for (int i = 0; i < 4; i++) count[i] = 0;
  for (i = 0; i < 4; i++, ps >>= 2) ++count[ps&0x3];
  msk[0] = msk[1] = 1;  msk[2] = 1 << count[0];
  int mod = count[2] - count[0] * count[1] - 1 +
    (count[0] * (count[0] * (count[0] - 18) + 107)) / 6 +
    (count[1] * (11 - count[1])) / 2;




  int b = data_file.decode_symbol(node_model[mod]);


  for (*signs = i = 0; i < 4; i++, mask <<= 2, os >>= 2) {
    if (old_st & mask) continue;
    ps = os & 0x3;




    if (b & msk[ps]) {

      st_trans |= mask;  ++(*signs); }
    msk[ps] <<= 1; }





  return st_trans;
}

 

int Desc_Transition(int old_st, const Image_Coord & pc)
{
  int os = old_st, ps = os, st_trans = 0, mask = 2;
  int msk[3], count[4];

  for (int i = 0; i < 4; i++) count[i] = 0;
  for (i = 0; i < 4; i++, ps >>= 2) ++count[ps & 0x3];
  msk[0] = 1;  msk[1] = msk[2] = 1 << count[0];
  int mod = count[2] - count[0] * count[1] - 1 +
    (count[0] * (count[0] * (count[0] - 18) + 107)) / 6 +
    (count[1] * (11 - count[1])) / 2;




  if ((old_st & 0xAA) == 0)
    if (data_file.decode_symbol(group_model[count[0]])) return 0;
  int b = data_file.decode_symbol(desc_model[mod]);


  for (i = 0; i < 4; i++, mask <<= 2, os >>= 2) {
    if (old_st & mask) continue;
    ps = os & 0x3;








    if (b & msk[ps]) {

      st_trans |= mask; }
    msk[ps] <<= 1; }








  return st_trans;
}



 
 

# 639 "codetree.C"


 
 



void Node_Test(Tree_Node * act_node)
{
  int st_trans, signs;
  st_trans = Node_Transition(int(act_node->state & 0xFFL),
    act_node->coord, &signs);

  if (st_trans) {
    act_node->state |= long(st_trans);
    Output_Signs(signs, st_trans, act_node->coord); }
}



 

int  Refinement_Pass(void)
{

  float t, ht1, ht2;

  int b, p, m, n = LSP_plane, k = LSP_dim.y;
  float * ptr, * smk, ** mtx = LSP_mtx[0];

  LSP_mark[LSP_plane++] = (LSP_ptr ? LSP_ptr[LSP_idx] : 0);

  for (m = p = 0; p < n; p++) {

    t = bias[p];
    bias[p] = threshold * (0.5 - bias_const_2 / Sqr(LSP_plane - p));
    ht1 = bias[p] - t;  ht2 = ht1 + threshold;

    for (smk = LSP_mark[p], ptr = 0; ptr != smk;) {
      if (!k) {
        mtx = LSP_mtx[++m];  k = LSP_dim.y; }
      ptr = mtx[--k];




      t = (data_file.decode_bit() ? ht2 : ht1);
      if (*ptr > 0) *ptr += t; else *ptr -= t;

      if (data_file.bytes_used() >= byte_budget) return 1 ; } }

  return 0 ;
}




 
 

# 832 "codetree.C"


int main(int numb_arg, char * arg[])
{
  if (numb_arg != 2)
    printf("\n\t* Please read the file `codetree.doc' for "
      "instructions\n\t\tand Copyright/Patent information.\n\t* Use"
      " `%s -u' for command-line usage information\n\n", arg[0]);
  else {
    printf("\n* Usage to compare the recovered image with the original:"
      "\n\n%s  coded_file  original_image  bpp_rate\n", arg[0]);
    printf("\n* Usage to save the recovered image:\n\n%s -s  "
      "coded_file  new_image_file  bpp_rate", arg[0]);



    puts(" \n");  exit(0); }

  FILE * plot_file;
  float mse, psnr, max_rate = 0.0, rate_inc, peak_2 = 0.0;
  int  cont, comp, interactive, cml;

  if (cml = ((numb_arg == 4) || (numb_arg == 5))) {
    interactive = 0 ;
    if (numb_arg == 4) {
      cod_f_name = arg[1];  pic_f_name = arg[2];
      bit_rate = atof(arg[3]);  comp = 1 ;  peak_2 = 65025.0; }
    else {
      if ((arg[1][0] != '-') || (arg[1][1] != 's'))
        Error("invalid command-line option");
      cod_f_name = arg[2];  new_f_name = arg[3];
      bit_rate = atof(arg[4]);  comp = 0 ; } }
  else {
    interactive = Input_Answer("Interactive mode");
    Input_Line("Name of the compressed image file", cod_f_name);
    comp = !interactive ||
      Input_Answer("Recovered image will be compared to original");
    if (comp)
      Input_Line("File with original image", pic_f_name);
    if (!interactive) {
      plot_file = Open_File("File with rate x PSNR results", "wt");
      bit_rate = Input_Float("First rate (bits/pixel)");
      max_rate = Input_Float("Last rate  (bits/pixel)");
      rate_inc = Input_Float("Rate increment"); } }

  do {

    data_file.open_file(cod_f_name);
    dimension.x = data_file.decode_bits(6) << 8;
    dimension.y = data_file.decode_bits(6) << 8;
    dimension.x += data_file.decode_bits(8);
    dimension.y += data_file.decode_bits(8);
    rate_mult = 8.0 / (float(dimension.x) * float(dimension.y));
    threshold_bits = data_file.decode_bits(5);
    pel_bytes = 1 + data_file.decode_bits(1);
    smoothing = data_file.decode_bits(3);
    mean_shift = data_file.decode_bits(4);
    mean = data_file.decode_bits(10);

    printf("\n  Image size = %d x %d, %d byte(s)/pixel\n",
      dimension.x, dimension.y, pel_bytes);
    if (smoothing)
      printf("  Coded with smoothing factor %d\n", smoothing);
    if (interactive)



      bit_rate = Input_Float("Code rate (bits/pixel)");





    byte_budget = long(bit_rate / rate_mult) - 1;

    if (comp && (peak_2 == 0.0))
      peak_2 = (pel_bytes == 1 ? 65025.0 :
        Sqr(Input_Float("Peak value for PSNR computations")));

    code_time.reset();  total_time.reset();  total_time.start();
    image.reset(dimension, pel_bytes, mean, mean_shift, smoothing);
    pyramid_dim = image.pyramid_dim();
    pyramid_levels = Min(Max_Levels(pyramid_dim.x),
                         Max_Levels(pyramid_dim.y));

    code_time.start("\n  Starting image recovery...");
    Process_Image();
    code_time.display(" Image recovered in");

    data_file.close_file();
    image.recover();
    total_time.stop();

    if (comp) {
      printf("\n  Rate =%6.3f bits/pixel --> ", rate_mult *
        data_file.bytes_used());
      mse = image.compare(pic_f_name);
      if (mse > 0.0) {
        psnr = dBW(peak_2 / mse);
        printf("mean squared-error =%8.3f =%7.2f dB PSNR\n",
          mse, psnr); }
      else
        puts("no differences encountered"); }

    total_time.display("\n  Total execution time (I/O included) =");

    if (interactive) {
      if (Input_Answer("Save new image to disk")) {
        Input_Line("Name of the new image file", new_f_name);
        image.write_pic(new_f_name); }
      cont = Input_Answer("Test new rate"); }
    else
      if (cml) {
        if (!comp) image.write_pic(new_f_name);
        cont = 0 ; }
      else {
        fprintf(plot_file, "%9.4f %9.4f\n", bit_rate, psnr);
        bit_rate += rate_inc;
        cont = (bit_rate <= max_rate); }

  } while (cont);

  if (!(cml || interactive)) fclose(plot_file);
  puts(" ");

  return 0;
}



 

 
