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
  $Id$

  $Log$
  Revision 1.4  1996/01/29 23:57:28  jussi
  Added a few colors and removed duplicate ones.

  Revision 1.3  1995/12/05 21:57:55  jussi
  Added some 20 missing colors which were defined in ColorMgr.c
  but not listed here. Now Devise can refer to all colors by the
  color constants. Also added the copyright notice.

  Revision 1.2  1995/09/05 21:12:27  jussi
  Added/updated CVS header.
*/

#ifndef Color_h
#define Color_h

typedef unsigned long Color;

const Color BlackColor		= 0;
const Color WhiteColor		= 1;
const Color RedColor		= 2;
const Color BlueColor		= 3;
const Color OrangeColor		= 4;
const Color TanColor		= 5;
const Color GreenColor		= 6;
const Color PurpleColor		= 7;
const Color AquamarineColor	= 8;
const Color PeruColor		= 9;
const Color ChocolateColor	= 10;
const Color TomatoColor		= 11;
const Color PinkColor		= 12;
const Color PlumColor		= 13;
const Color AzureColor		= 14;
const Color CyanColor		= 15;
const Color SeaGreenColor	= 16;
const Color KhakiColor		= 17;
const Color GoldenRodColor	= 18;
const Color YellowColor		= 19;
const Color SiennaColor		= 20;
const Color LightCoralColor	= 21;
const Color AntiqueWhiteColor	= 22;
const Color LemonChiffonColor	= 23;
const Color LightGrayColor	= 24;
const Color LavenderColor	= 25;
const Color LavenderBlushColor	= 26;
const Color MistyRoseColor	= 27;
const Color NavyBlueColor	= 28;
const Color SlateBlueColor	= 29;
const Color MediumBlueColor	= 30;
const Color DeepSkyBlueColor	= 31;
const Color SkyBlueColor	= 32;
const Color RedColor1		= 33;
const Color RedColor2		= 34;
const Color GreenColor1		= 35;
const Color GreenColor2		= 36;
const Color BlueColor1		= 37;
const Color BlueColor2		= 38;
const Color BlueColor3		= 39;
const Color BlueColor4		= 40;
const Color GoldColor1		= 41;
const Color GoldColor2		= 42;

const Color ForegroundColor	= BlackColor;
const Color BackgroundColor	= AntiqueWhiteColor;
const Color HighlightColor	= BlackColor;

#endif
