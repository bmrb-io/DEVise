/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Program to test DevStatus class.
 */

/*
  $Id$

  $Log$
 */

#define _DevStatusTest_c_

#include <stdio.h>

#include "DevStatus.h"


/*
 * Static global variables.
 */
#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: main
 * Main program for TestDevStatus.
 */
int
main(int, char **)
{
  int result = 0;

/*
 * Test constructors.
 */
  DevStatus status1 = StatusOk;
  DevStatus status2(StatusInvalid);

/*
 * Equality and inequality operators.
 */
  if (status1 != StatusOk)
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  if (!(status2 == StatusInvalid))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  if (status1 == status2)
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status2 = status1;
  if (status1 != status2)
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

/*
 * Test the various conditions.
 */
  status1 = StatusInvalid;
  if (status1.IsComplete() || status1.IsError() || status1.IsWarn() ||
    status1.IsCancel())
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status1 = StatusOk;
  if (!status1.IsComplete() || status1.IsError() || status1.IsWarn() ||
    status1.IsCancel())
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status1 = StatusFailed;
  if (status1.IsComplete() || !status1.IsError() || status1.IsWarn() ||
    status1.IsCancel())
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status1 = StatusWarn;
  if (!status1.IsComplete() || !status1.IsError() || !status1.IsWarn() ||
    status1.IsCancel())
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status1 = StatusCancel;
  if (status1.IsComplete() || status1.IsError() || status1.IsWarn() ||
    !status1.IsCancel())
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status1 = StatusWarnCancel;
  if (status1.IsComplete() || !status1.IsError() || !status1.IsWarn() ||
    !status1.IsCancel())
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

/*
 * Test the += operator.
 */
  status1 = StatusOk;
  status2 = StatusOk;

  status1 += status2;
  if (status1 != StatusOk)
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusOk;
  status1 += StatusOk;

  status2 = StatusFailed;
  status2 += StatusFailed;

  DevStatus status3 = StatusWarn;
  status3 += StatusWarn;

  DevStatus status4 = StatusCancel;
  status4 += StatusCancel;

  if ((status1 != StatusOk) || (status2 != StatusFailed) ||
    (status3 != StatusWarn) || (status4 != StatusCancel))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }

  status1 = StatusWarnCancel;
  status1 += StatusWarnCancel;

  if (status1 != StatusWarnCancel)
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }




  status1 = StatusOk;
  status1 += StatusFailed;

  status2 = StatusOk;
  status2 += StatusWarn;

  status3 = StatusOk;
  status3 += StatusCancel;

  status4 = StatusOk;
  status4 += StatusWarnCancel;

  if ((status1 != StatusFailed) || (status2 != StatusWarn) ||
    (status3 != StatusCancel) || (status4 != StatusWarnCancel))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusFailed;
  status1 += StatusOk;

  status2 = StatusFailed;
  status2 += StatusWarn;

  status3 = StatusFailed;
  status3 += StatusCancel;

  status4 = StatusFailed;
  status4 += StatusWarnCancel;

  if ((status1 != StatusFailed) || (status2 != StatusFailed) ||
    (status3 != StatusFailed) || (status4 != StatusFailed))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusWarn;
  status1 += StatusOk;

  status2 = StatusWarn;
  status2 += StatusFailed;

  status3 = StatusWarn;
  status3 += StatusCancel;

  status4 = StatusWarn;
  status4 += StatusWarnCancel;

  if ((status1 != StatusWarn) || (status2 != StatusFailed) ||
    (status3 != StatusWarnCancel) || (status4 != StatusWarnCancel))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusCancel;
  status1 += StatusOk;

  status2 = StatusCancel;
  status2 += StatusFailed;

  status3 = StatusCancel;
  status3 += StatusWarn;

  status4 = StatusCancel;
  status4 += StatusWarnCancel;

  if ((status1 != StatusCancel) || (status2 != StatusFailed) ||
    (status3 != StatusWarnCancel) || (status4 != StatusWarnCancel))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusWarnCancel;
  status1 += StatusOk;

  status2 = StatusWarnCancel;
  status2 += StatusFailed;

  status3 = StatusWarnCancel;
  status3 += StatusWarn;

  status4 = StatusWarnCancel;
  status4 += StatusCancel;

  if ((status1 != StatusWarnCancel) || (status2 != StatusFailed) ||
    (status3 != StatusWarnCancel) || (status4 != StatusWarnCancel))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusOk;
  status1 += StatusInvalid;

  status2 = StatusInvalid;
  status2 += StatusOk;

  status3 = StatusOk;
  status3 += (StatusVal) 999;

  status4 = StatusInvalid;
  status4 += StatusInvalid;

  if ((status1 != StatusInvalid) || (status2 != StatusInvalid) ||
    (status3 != StatusInvalid) || (status4 != StatusInvalid))
  {
    printf("Error at %s: %d\n", __FILE__, __LINE__);
    result = 1;
  }


  status1 = StatusInvalid;
  status1.Print();

  status1 = StatusOk;
  status1.Print();

  status1 = StatusFailed;
  status1.Print();

  status1 = StatusWarn;
  status1.Print();

  status1 = StatusCancel;
  status1.Print();

  status1 = StatusWarnCancel;
  status1.Print();


  if (result == 0)
  {
    printf("Test succeeded\n");
  }
  else
  {
    printf("Test failed\n");
  }

  return result;
}

/*============================================================================*/
