#ifndef DATA_SRC_H
#define DATA_SRC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#define DS_STRLENMAX 400 

/* names of some standard sources and destinations */

#define DSD_NAME_UNKNOWN    "<unknown>"
#define DSD_NAME_STDIN      "<stdin>"
#define DSD_NAME_STDOUT      "<stdout>"
#define DSD_NAME_STDERR      "<stderr>"
#define DSD_NAME_FD          "<fd>"
#define DSD_NAME_STREAM      "<stream>"
#define DSD_NAME_FILTER      "<filter>"
#define DSD_NAME_MEMORY      "<memory>"
#define DSD_NAME_NULL        "<null>"
#define DSD_NAME_AUTO_MEMORY "<auto-memory>"
#define DSD_NAME_PROCESS     "<process>"



/****** DataSrc state meanings: *******/
#define DS_NO_SRC 0
#define DS_SRC_ACTIVE 1
#define DS_SRC_DONE 2
#define DS_DEAD 10

#define DS_KIND_NONE -1 
#define DS_KIND_FILE 0
#define DS_KIND_FD 1
#define DS_KIND_WEB 2
#define DS_KIND_MEMORY 3
#define DS_KIND_FILTER 4
#define DS_KIND_STREAM 5 
#define DS_KIND_PROCESS 6 


struct DataSrcStruct { 
  int state; 
  int src_kind; 

  int fd; 
  FILE *fp;

  int file_eof; 

  char *next_mem_byte; 

  long web_bytes; 
  
  char srcname[DS_STRLENMAX]; 

  long bytes_to_read; /* 0 indicates read till EOF */ 
  long bytes_read;
  long total_count;
  long total_in_src; 
  long (*read_func) (struct DataSrcStruct *ds, char *buff, long nbytes);
};

typedef struct DataSrcStruct DataSrc;

/* some macros for "discerning" users */ 

/* avoid this! 
#define DS_fd(ds) (ds)->fd
*/ 

#define DS_name(ds) (ds)->srcname 

#define DS_total_count(ds) (ds)->total_count
#define DS_total_in_src(ds) (ds)->total_in_src 
#define DS_bytes_to_read(ds) (ds)->bytes_to_read
#define DS_bytes_read(ds) (ds)->bytes_read

#define DS_all_read(ds) (((ds)->total_in_src > 0) && \
		         ((ds)->total_count >= (ds)->total_in_src)) 



#define DS_file_src(ds) ((ds)->src_kind == DS_KIND_FILE) 
#define DS_fd_src(ds) ((ds)->src_kind == DS_KIND_FD) 
#define DS_stream_src(ds) ((ds)->src_kind == DS_KIND_STREAM) 
#define DS_web_src(ds) ((ds)->src_kind == DS_KIND_WEB) 
#define DS_memory_src(ds) ((ds)->src_kind == DS_KIND_MEMORY) 
#define DS_filter_src(ds) ((ds)->src_kind == DS_KIND_FILTER) 
#define DS_process_src(ds) ((ds)->src_kind == DS_KIND_PROCESS) 

#define DS_src_has_file(ds) (DS_file_src(ds) || \
			   DS_fd_src(ds)   || \
			   DS_filter_src(ds)   || \
			   DS_stream_src(ds)   || \
			   DS_web_src(ds) )

#define DS_file_eof(ds) (DS_src_has_file(ds) && (ds)->file_eof)

#define DS_web_bytes(ds) ( DS_web_src(ds) ? (ds)->web_bytes : 0) 

extern void DS_Init(DataSrc *ds, long total_in_src);

extern int DS_OpenFile(DataSrc *ds, char *fname, long bytes_to_read);

extern int DS_OpenFd(DataSrc *ds, int fd, long bytes_to_read);

extern int DS_OpenStream(DataSrc *ds, FILE *fp, long bytes_to_read);

extern int DS_OpenMemory(DataSrc *ds, char *mem, long bytes_to_read); 

extern int DS_OpenProcess(DataSrc *ds, char *argv[], long bytes_to_read); 

extern int DS_ExtendSrc(DataSrc *ds, long nbytes_more);

extern int DS_ResetSrcBytes(DataSrc *ds, long nbytes);

extern int DS_ResetTotalSrcBytes(DataSrc *ds, long nbytes);

#define DS_Read(ds, buff, nbytes) \
  ((ds)->read_func((ds),(buff),(nbytes)))

extern int DS_ReadLine(DataSrc *ds, char *buff, long buffsize);

extern void DS_Close(DataSrc *ds); 


extern long DS_SrcBytes(DataSrc *ds); 

extern int DS_UseFilter(DataSrc *ds, char *filter, int NumFirstBytes,
			char *FirstBytes); 


/******* DataDest declarations **************/

#define DD_STRLENMAX DS_STRLENMAX

/****** DataDest state meanings: *******/
#define DD_NO_DEST 0
#define DD_DEST_ACTIVE 1
#define DD_DEAD 10

#define DD_KIND_NONE -1
#define DD_KIND_FILE 0
#define DD_KIND_FD 1
#define DD_KIND_MEMORY 2
#define DD_KIND_AUTO_MEMORY 3
#define DD_KIND_STREAM 4 
#define DD_KIND_NULL 5 

struct DataDestStruct { 
  int state; 
  int dest_kind; 

  int fd; 
  FILE *fp;

  int file_eof; 

  long mem_size; 
  char *next_mem_byte; 
  int use_auto_mem; /* if dest becomes full, switch automatically
		   to malloced memory. application is responsible
		   for freeing this memory. No other source can
		   be opened once the dest has switched to auto mem*/
  long auto_mem_size;  
  char *auto_mem; 
  long auto_mem_step; 
  
  char destname[DD_STRLENMAX]; 
  long bytes_written;
  long total_count;
  long (*write_func) 
    (struct DataDestStruct *dd, char *buff, long nbytes);
};

typedef struct DataDestStruct DataDest;

/* some macros for "discerning" users */ 


#define DD_name(dd) (dd)->destname 

#define DD_total_count(dd) (dd)->total_count
#define DD_bytes_written(dd) (dd)->bytes_written

#define DD_file_dest(dd) ((dd)->dest_kind == DD_KIND_FILE) 
#define DD_fd_dest(dd) ((dd)->dest_kind == DD_KIND_FD) 
#define DD_stream_dest(dd) ((dd)->dest_kind == DD_KIND_STREAM) 
#define DD_memory_dest(dd) ((dd)->dest_kind == DD_KIND_MEMORY) 
#define DD_auto_memory_dest(dd) ((dd)->dest_kind == DD_KIND_AUTO_MEMORY) 
#define DD_null_dest(dd) ((dd)->dest_kind == DD_KIND_NULL) 

#define DD_dest_has_file(dd) (DD_file_dest(dd) || \
			DD_stream_dest(dd) || DD_fd_dest(dd)) 

#define DD_file_eof(dd) (DD_dest_has_file(dd) && (dd)->file_eof)

extern void DD_Init(DataDest *dd); 

#define DD_Use_Auto_Mem(dd) (dd)->use_auto_mem = 1
#define DD_Set_Auto_Mem_Step(dd, step) (dd)->auto_mem_step = (step)

#define DD_using_auto_mem(dd) (((dd)->use_auto_mem) && \
			       ((dd)->auto_mem_size > 0) && \
                                DD_auto_memory_dest(dd) )
#define DD_auto_mem_size(dd) (DD_using_auto_mem(dd) ? \
			   (dd)->auto_mem_size : 0 )
#define DD_auto_mem_level(dd) (DD_using_auto_mem(dd) ? \
			   (dd)->bytes_written : 0 )

#define DD_auto_mem(dd) (DD_using_auto_mem(dd) ? (dd)->auto_mem : 0)

#define DD_Free_Auto_Mem(dd) \
  if (DD_using_auto_mem(dd)) { \
    free((dd)->auto_mem); \
    (dd)->auto_mem = (char *) 0; \
    (dd)->auto_mem_size = 0; \
    (dd)->state = DD_NO_DEST; \
  } 


extern int DD_OpenFile(DataDest *dd, char *fname);

extern int DD_OpenFd(DataDest *dd, int fd);

extern int DD_OpenStream(DataDest *dd, FILE *fp); 

extern int DD_OpenMemory(DataDest *dd, char *mem, long mem_bytes); 

extern int DD_OpenNull(DataDest *dd); 

#define DD_Write(dd, buff, nbytes) \
  ((dd)->write_func((dd),(buff),(nbytes)))

#define DD_WriteLine(dd, line) \
  ((dd)->write_func((dd),(line),strlen(line)) + \
   (dd)->write_func((dd),"\n",1) )

#define DD_Flush(dd) \
  if (DD_file_dest(dd) && (dd)->fp ) { fflush((dd)->fp);}

extern void DD_Close(DataDest *dd); 

extern long DD_printf(DataDest *dd, char *format, ...);

#endif

