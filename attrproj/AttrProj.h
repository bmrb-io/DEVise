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
  Header file for AttrProj (attribute projection) class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _AttrProj_h_
#define _AttrProj_h_


class AttrProj
{
public:
	AttrProj(char *schemaFile, char *attrProjFile, char *dataFile);
	~AttrProj();
};


#endif /* _AttrProj_h_ */

/*============================================================================*/
