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

#include "PSDisplay.h"
#ifndef LIBCS
#include "Control.h"
#include "Journal.h"
#include "Init.h"
#endif

//#define DEBUG

/*******************************************************************
Open a new display
********************************************************************/

PSDisplay::PSDisplay(char *name)
{
  /* do something */
}

/*******************************************************************
Allocate color by name
********************************************************************/

void PSDisplay::AllocColor(char *name, Color globalColor)
{
#ifdef DEBUG
  printf("PSDisplay::AllocColor(%s)\n", name);
#endif

  /* do something */
}

/*********************************************************************
Allocate color by RGB
*********************************************************************/

void PSDisplay::AllocColor(double r, double g, double b, Color globalColor)
{
#ifdef DEBUG
  printf("PSDisplay::AllocColor(%.2f,%.2f,%.2f)\n", r, g, b);
#endif

  /* do something */
}

/*************************************************************
Create a new window 
***************************************************************/

WindowRep *PSDisplay::CreateWindowRep(char *name, Coord x, Coord y,
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

void PSDisplay::DestroyWindowRep(WindowRep *win)
{
  delete win;
}
