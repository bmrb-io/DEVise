/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/

static char rcsid[]="$RCSfile$, $Revision$, $Date$";

/*
 *  Include Files:
 */
#include <stdarg.h>

#ifndef _VA_LIST_
#define _VA_LIST_ 1		/* Prevent va_list from being defined */
#endif				/* in stdio.h on Ultrix machines. */

#include <stdlib.h>
#include <stdio.h>

#include "inerror.h"

/*
 *  Q_ERROR prints an error message and then exits.  It accepts input
 *  in the style of printf, i.e. a format string and then 0 or more
 *  arguments of various types.
 */
void Q_error (char *format, ...)
  {

  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "ERROR:  ");
  vfprintf (stderr, format, ap);
  va_end (ap);
  fprintf (stderr, "\n");
  fflush (stderr);
/*  reset_locking(); */
  exit (1);

  } /* end of Q_error */

/*
 *  Q_INFORM prints an informational message and then returns.  It accepts
 *  input in the style of printf, i.e. a format string and then 0 or
 *  more arguments of various types.
 */
void Q_inform (char *format, ...)
  {

  va_list ap;

  va_start (ap, format);
  vfprintf (stderr, format, ap);
  va_end (ap);
  fprintf (stderr, "\n");
  fflush (stderr);

  } /* end of Q_inform */

/*
 *  Q_WARNING prints a warning message and then returns.  It accepts
 *  input in the style of printf, i.e. a format string and then 0 or
 *  more arguments of various types.
 */
void Q_warning (char *format, ...)
  {

  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "WARNING:  ");
  vfprintf (stderr, format, ap);
  va_end (ap);
  fprintf (stderr, "\n");
  fflush (stderr);

  } /* end of Q_warning */

