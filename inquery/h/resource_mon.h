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

#ifndef _RESOURCE_MON_H
#define _RESOURCE_MON_H

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#if !defined(__MWERKS__)
#	include <sys/times.h>
#       include <sys/resource.h>
#endif

#ifndef CLOCKS_PER_SEC
# define CLOCKS_PER_SEC 60
#endif

/*
**********************************************************************
**  RESOURCE MONITORING DATA TYPES
**********************************************************************
*/
struct res_tms{
      clock_t tms_utime;
      clock_t tms_stime;
};

typedef struct res_tms res_tms_t;

struct res_monitor{
  struct timeval t_start;     /* t_start measures start of wallclock time */
  struct timeval t_end;       /* t_end measures end of wallclock time */
  res_tms_t      ru_start;     /* ru_start start of resources (getrusage) */
  res_tms_t      ru_end;       /* ru_end end of resources (getrusage) */
};

typedef struct res_monitor res_mon_t;

/*
**  While I want to use the struct rusage, we need to include
**     the wallclock time.
*/
#ifndef _RES_USAGE_T
#define _RES_USAGE_T
struct res_usage{
  clock_t    utime;   /* in microseconds */
  clock_t    stime;   /* in microseconds */
  int    wallclock;   /* in microseconds */
  int    memory;
  int    file_in;
  int    file_out;
  int    page_faults;
};

typedef struct res_usage res_usage_t;
#endif /* _RES_USAGE_T */


/*
**********************************************************************
**  RESOURCE MONITORING FUNCTION PROTOTYPES
**********************************************************************
**
**  After recording resource usage, one can:
**      log resource usage to a file.
**      return resource usage to user.
**      put resource usage in a message (client/server)
*/
void res_usage_create(res_mon_t *resources, res_usage_t *usage_stats);
void res_usage_print(FILE *in_fp, res_usage_t usage_stats);
void res_times(res_tms_t *buffer);
void res_gettime(struct timeval *tp, void *timezone);

/*
**********************************************************************
**  RESOURCE MEASURING MACROS
**********************************************************************
**
**  Use MACROS for tagging start & stop times to 
**    make this as non-intrusive as possible.
*/

/* Prototype------------------------- */
/* RES_MON_START(res_mon_t resources) */
/*            Initializes a res_mon_t monitoring structure */
/*---------------------------------------------------------*/
#define RES_MON_START(resources){\
      int status; \
      res_times(&(resources.ru_start));\
      res_gettime(&(resources.t_start), NULL);}


/* Prototype------------------------- */
/* RES_MON_END(res_mon_t resources, res_usage_t usage_stats) */
/*            Terminates the resource monitoring and calculates the 
**            resources used since RES_MON_START.  The resultant
**            values are returned in usage_stats.             */
/*---------------------------------------------------------*/
#define RES_MON_END(resources, usage_stats){\
      int status;\
      res_gettime(&(resources.t_end), NULL);\
      res_times(&(resources.ru_end));\
      res_usage_create(&resources, &usage_stats);}
      /* res_usage_print(NULL, usage_stats); } */


#endif /* _RESOURCE_MON_H */







