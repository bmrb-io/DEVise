/**************************************************************************
*                         Copyright (c) 1996                              *
* by Sovereign Hill Software, Inc., Amherst, Massachusetts, U.S.A.        *
*        (413-256-6916;   http://www.sovereign-hill.com)                  *
*                          All Rights Reserved.                           *
*                                                                         *
* The SOFTWARE was developed in part at the Center for Intelligent        *
* Information Retrieval (CIIR) at the University of Massachusetts at      *
* Amherst (For more information, contact 413-545-0463 or                  *
* http://ciir.cs.umass.edu).  The SOFTWARE was acquired by Sovereign      *
* Hill Software Inc. and is the property of Sovereign Hill Software.      *
* ACSIOM is authorized, under an agreement with Sovereign Hill Software,  *
* to license the SOFTWARE for non-commercial, non-revenue-producing       *
* prototyping and internal evaluation purposes only.                      *
*                                                                         *
* This SOFTWARE was developed in significant part with private funding.   *
* The INQUERY SOFTWARE is proprietary to Sovereign Hill Software and may  *
* not be distributed in whole or in part or used in any way without       *
* authorized permission in writing from Sovereign Hill Software.          *
* Possession, use, duplication or dissemination of the INQUERY SOFTWARE   *
* and media is authorized only pursuant to a valid written license from   *
* ACSIOM or Sovereign Hill Software, Inc.                                 *
*                                                                         *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.  *
* Government is subject to restrictions as set forth in subparagraph      *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.              *
**************************************************************************/ 


/* Modifications:
   11/11/92 -- Added to rcs library.  Includes interactive prototypes 
     AEG       for file listings for a unix machine.  
   04/22/93 -- This header is now a "cover" for bringing in functions
     SMH       that are either DOS or UNIX related.  The previous file
               of this name has been renamed as dos_specific.h or
               unix_specific.h.  
   06/01/94 -- This header is now also a "cover for bringing in functions
     AL	       that are Mac related. New file mac_specific.h is added.
*/


/*
 * This header is now used to decide which of the system specific 
 * header files should be included to produce a PC or Mac INQUERY user
 * interface.  The decision as to whether to produce a DOS, Mac, or Unix
 * interface depends on the definition of _MSDOS and _MACINTOSH.  If _MSDOS 
 * is defined, then the result is assumed to be a PC client, if _MACINTOSH is defined
 * then the result is assumed to be a MAC client, otherwise a Unix
 * client.
 *
 * This is the place to put all our ifdefs and macro definitions for
 * deciding what sort of inquery we are building.  We may wish to 
 * rename this file to something else if sys-specific.h is to be more
 * general inquery "catch all" file.
 */

#ifndef SYS_SPECIFIC_H

  #define SYS_SPECIFIC_H

  /*
   *  SUN Solaris must include termio.h to get the ws data structures
   *  needed for the screen_size function.  SUN SunOS generates compile
   *  errors if termio.h is included.  This pair of defined conditions is
   *  the only way I know of to distinguish between Solaris and SunOS.
   */
  #if (defined(sparc) && defined(_SIZE_T))
     #define _SOLARIS 
  #endif

/*
 *  sys_specific.c won't compile without it.
 */
#include "doctypes.h"


  /*
   * This header need only be included when a networked inquery is
   * to be built.  This inclusion needs to be ifdef'd for network
   * inquery only.
   *
  #if defined (NETWORK)
    #include "inq2isrv.h"
  #endif
   */


/* A/UX complains about conflicting malloc types from stdlib.h and malloc.h,
     so malloc.h is not included in SMALL model compilation.  Also, A/UX
     ioctl.h contains defs which are in ttyio.h, so the second header is not
     needed for SMALL model compilation.                                 
*/
   #ifdef LARGE

   /*
    *  SUNs do not have a ttyio.h
    */
    #ifndef sparc
      #include <sys/ttyio.h>     /* defined in ioctl.h for A/UX */
    #endif

   #endif

  /*
   * We will eventually want to decide on at least four possible 
   * INQUERY interfaces:
   *  1) DOS PC Decnet Client
   *  2) Unix Client
   *  3) DOS PC tty inquery
   *  4) Unix tty inquery
   *  5) Mac TCP/IP Client
   *  6) Mac tty inquery
   * For now we decide between MSDOS, Mac, or Unix Client
   */
  #if (!defined(_MSDOS) && !defined(__MWERKS__))
      #include <sys/types.h>
      #if !defined(_WATCOM)
        #include <memory.h>
        /* Needed for the fstat() call in sys_get_mtime() */
        #include <sys/stat.h>
        #include <dirent.h>
        #include <sys/ioctl.h>
        #include "unix_specific.h"
      #else
        #include <direct.h>
		  #ifndef _MSC_VER
            #include "wat_spec.h"
		  #endif
      #endif
      typedef struct dirent DirEnt_t;
      
      #if !defined(sgi)
        #if defined(mips)
         #include <sys/ttyio.h>
        #endif
      #else
        #include <sys/termio.h>
      #endif
	
      #define STDOUT (1)          /* for isatty() call */
	
      extern int isatty();
      #if !defined( _SOLARIS ) && !defined(INQ_ARCH_sgi) && !defined(INQ_ARCH_dgux)
         extern void ioctl();
      #endif
#	else 
#		if defined(__MWERKS__)   
/* Macintosh specific stuff goes here */

/*	I haven't found a library that implements this stuff on Mac. So, I'll
	do it myself. However, it might not be crucial anyway */
	
#		define	F_ULOCK	1	/*	Unlocks a previously locked region in the file. */
#		define	F_LOCK	2	/*	Locks the region for exclusive use. This request causes
								the calling process to sleep if the region overlaps a
								locked region, and to resume when it is granted the lock. */
                      		
#		define	F_TLOCK	3	/*	Same as F_LOCK, except that the request returns an
								error if the region overlaps a locked region. */

#		define	F_TEST	4	/*	Tests to see if another process has already locked a
								region. The lockf() function returns 0 (zero) if the
								region is unlocked. If the region is locked, then -1 is
								returned and errno is set to [EACCES]. */

#ifdef __cplusplus
extern "C" {
#endif

int lockf(int filedes, int request, long int size);
							/*	0 on success, -1 on failure */
							
#ifdef __cplusplus
}
#endif


#		else
#			include "dos_spec.h"
   
    /* for screen utilities -- need to link MicroSoft graphics.lib */
#			include <graph.h> 
#		endif    
#	endif /* DOS or Unix Client */

/*
 *  SUN/OS UNIX doesn't have the ANSI C memmove function.  Add it, if
 *  necessary.
 */
#ifdef sparc
    void *memmove (void *dest, const void *src, size_t len);
#endif

/* This is to take care of reading binary files for watcom, because
 * the sun does not handle the b properly.
*/

#if (defined(_WATCOM) || defined(__MWERKS__))
#define Q_BIN_APPEND "ab"
#define Q_BIN_READ_PLUS "rb+"
#define Q_BIN_READ_ONLY "rb"
#define Q_BIN_WRITE_PLUS "wb+"
#define Q_BIN_WRITE_ONLY "wb"
#define Q_BIN_APPEND "ab"
#if !defined(__MWERKS__)
#  define DIR_SEP_CHAR '\\'
#  define DIR_SEP_STRING "\\"
#  define Q_DOTINF ".INF"
#  define Q_DOTBRF ".BRF"
#  define Q_DOTCNT ".CNT"
#  define Q_DOTHHD ".HHD"
#  define Q_DOTHRC ".HRC"
#  define TEMPNAM(a,b) tmpnam(b)
#else
#  define DIR_SEP_CHAR ':'
#  define DIR_SEP_STRING ":"
#  define Q_DOTINF ".inf"
#  define Q_DOTBRF ".brf"
#  define Q_DOTCNT ".cnt"
#  define Q_DOTHHD ".hhd"
#  define Q_DOTHRC ".hrc"
#  define TEMPNAM(a,b) tmpnam(b)
#endif
#define VIDEOCONFIG videoconfig
#else
#define Q_BIN_APPEND "a"
#define Q_BIN_READ_PLUS "r+"
#define Q_BIN_READ_ONLY "r"
#define Q_BIN_WRITE_PLUS "w+"
#define Q_BIN_WRITE_ONLY "w"
#define Q_BIN_APPEND "a"
#define DIR_SEP_CHAR '/'
#define DIR_SEP_STRING "/"
#define Q_DOTINF ".inf"
#define Q_DOTBRF ".brf"
#define Q_DOTCNT ".cnt"
#define Q_DOTHHD ".hhd"
#define Q_DOTHRC ".hrc"
#define VIDEOCONFIG _videoconfig
#define TEMPNAM(a,b) tempnam(a,b)
#endif
 
#if (defined(__MWERKS__) && defined(__cplusplus))
extern "C" {
#endif
/*
 * expand the first element of the filename thru the environment variable
 * if it starts with $ or is uppercase 
 */
Char_t *expfile(Char_t *name);

/* strip the last /element off this file name */
Char_t *strpelm(Char_t *name); 

/*
 * Sys_get_mtime() gets the modification time of the open file
 * matching the file descriptor file_no
 */
Int_t sys_get_mtime(int file_no);
Int_t sys_get_mtime_from_file(char *filename);


int my_ftruncate(int iTemp1, int iTemp2);

/*
 * set the extension on this filename to the specified one.  Don't 
 * supply the "." 
 */
void set_extension(Char_t *name, Char_t *ext);

/* find the last occurance of the character in this string */
Char_t *find_last(Char_t *string, Char_t target);

Char_t **get_database_list(Char_t *dir_name);

/* return just the file name from this pathname */
Char_t *get_file_name(Char_t *file_string);

Boolean_t possible_absolute_path(char *path);

/* return the absolute path to the specified filespec */
char *absolute_path(char *filespec);

/* is this path an absolute filename */
Boolean_t is_absolute_path(Char_t *file);

/* return the absolute path of filespec */
char *absolute_path(char *filespec);

/* add the file name to the directory name checking delimiters */ 
Char_t *add_file(Char_t *name, Char_t *directory, Char_t *filename);

/* strcasecmp1: a case-insensitve implementation of strcmp */
int strcasecmp1(char *s, char *t);

/* function to get the environment */
Char_t *g_getenv(Char_t *name);

/* set or change an environment variable */
void inq_setenv(Char_t *name, Char_t *value);

/* find the first occurence of a target string within a string */
Char_t *find_first(Char_t *string, Char_t *target, Int_t target_length);

/* strips the node name from the string and returns a pointer to the rest */
Char_t *strip_node(Char_t *string);

/* returns the node string */
Char_t *node_string(Char_t *string);

Char_t ** inq_file_matcher (Char_t *base, Char_t *extension);

/* returns amount of dynamic memory allocated by OS to the process */
long int Q_dynamic_memory_size (Int_t reset);

/* local implementation of strdup() */
char *strdup1(const char *s);

/* local implementation of strcasecmp() */
int strcasecmp1(char *s, char *t);

/* allow systems to set signal handlers appropriately */
void q_sys_reaper(void);
int q_sys_set_reaper(void (*sig_func)());
int q_sys_unset_reaper(void);
int q_sys_set_int_handler(void (*sig_func)());
int q_sys_unset_reaper(void);

int spawn_process(char **args, int set_gid, int close_fd); /* fork()/exec() */
char *string_error(int s);          /* strerror() */

/* local implementation of basename() */
void basename2(char *inS, char *outS, char *suffix);

/* local implementation of dirname() */
void dirname2 (char *inS, char *outS);

/* local implementation of unlink() */

int unlink2 (const char *path);

/* system limit for max # open file descriptors */
int q_max_files();

/* Get the listing of a specified file */
Char_t **get_file_list (Char_t *dir);

/* Get the process identifier */
int getpid1();

/* Free memory allocated by get_file_list() */
void free_file_list (Char_t **dir_list);

#if (defined(__MWERKS__) && defined(__cplusplus))
}
#endif


#if defined (_MSC_VER)
#       define sleep( seconds ) Sleep( seconds * 1000 )
#endif

/* API's and other exported functions in windows DLL's 
 * need to be compiled using the standard calling convention
 * in order to be accessible to VB. Use the INQ_API(type) or
 * INQ_EXPORT(type) modifiers for such cases in the declaration
 * and definition of the function. Define DLL_FOR_VB as a
 * preprocessor directive.
 */

#if defined(_MSC_VER) && defined(DLL_FOR_VB)
#	define INQ_API( typedecl ) __declspec( dllexport ) typedecl __stdcall 
#	define INQ_EXPORT __declspec( dllexport ) 
#else
#	define INQ_API( typedecl ) typedecl
#	define INQ_EXPORT
#endif



#endif  /* SYS_SPECIFIC_H */


