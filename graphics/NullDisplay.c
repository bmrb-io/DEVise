/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$*/

#include "NullDisplay.h"
#ifndef LIBCS
#include "Control.h"
#include "Journal.h"
#include "Init.h"
#endif

//#define DEBUG

/*******************************************************************
Open a new display
********************************************************************/

NullDisplay::NullDisplay(char *name)
{
  /* do something */
}

/*******************************************************************
Allocate color by name
********************************************************************/

void NullDisplay::AllocColor(char *name, Color globalColor)
{
#ifdef DEBUG
  printf("NullDisplay::AllocColor(%s)\n", name);
#endif

  /* do something */
}

/*********************************************************************
Allocate color by RGB
*********************************************************************/

void NullDisplay::AllocColor(double r, double g, double b, Color globalColor)
{
#ifdef DEBUG
  printf("NullDisplay::AllocColor(%.2f,%.2f,%.2f)\n", r, g, b);
#endif

  /* do something */
}

/*************************************************************
Create a new window 
***************************************************************/

WindowRep *NullDisplay::CreateWindowRep(char *name, Coord x, Coord y,
                                        Coord width, Coord height, 
                                        Color fgnd, Color bgnd, 
                                        WindowRep *parentRep,
                                        Coord min_width, Coord min_height,
                                        Boolean relative, Boolean winBoundary)
{
  /* do something */
  return 0;
}

/**************************************************************
Destroy a window
**************************************************************/

void NullDisplay::DestroyWindowRep(WindowRep *win)
{
  delete win;
}
