#ifndef SYSFUNCH
#define SYSFUNCH
/*        $Header$ */
/* Declarations of major functions within standard C libraries */
/* Once all of the major systems get their act together (and I follow
   suit!), this file should just include system header files from 
   /usr/include.  Until then... */

#ifdef NeXT
#include <libc.h>
#include <stdlib.h>
#include <math.h>
#else

int open(), close(), write(), read(), link(), stat(), unlink();
int chdir(), mkdir();
long lseek();

#ifdef AIX
#define vfork fork
#endif /* AIX */

int vfork(), setregid(), getgid(), execl(), wait(), system(), pipe();
void _exit();
int dup2(), execlp();
int getpid();
int getrusage();
int getuid();
int gettimeofday();
void exit();


#ifdef AIX
int sprintf();
#else
//char *sprintf();
#endif  /* AIX */

#ifdef AIX
#include <malloc.h>
#else
char *malloc(), *realloc(), *calloc(), *valloc();
#endif /* AIX */
int bcopy(), bzero();
int free();
#ifndef AIX
int qsort();
#endif AIX
int getopt();
#ifndef mips
size_t fwrite(), fread();
#ifdef AIX
void perror();
#else
//int perror();
#endif /* AIX */
#endif /* mips */
int fprintf(), fclose(), fstat(), fflush(), fputs();

int rand(), srand();

char *gets();
int atoi(), printf(), puts(), fseek();
long atol();
int scanf(), fscanf(), sscanf();
double atof();

double sqrt(), log(), floor(), exp(), pow();

int strncmp(), strlen(), strcmp();
char *strcpy(), *strncpy(), *strcat(), *strncat();
char *mktemp();
char *re_comp();
int re_exec();

char *getlogin(), *getenv(), *getwd();

/* declare _filbuf for the time being to keep gcc from complaining about */
/* the macro getchar() */
int _filbuf(), _flsbuf();
int ungetc();

#ifndef toupper
int toupper(), tolower();
#endif

#ifdef MMAP
int munmap();
#endif

#endif /* NeXT */

int creat();

#endif /* SYSFUNCH */

