/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of expression evaluation class(es).
 */

/*
  $Id$

  $Log$
  Revision 1.4  1998/01/30 21:53:19  wenger
  Did some cleaning up of the MappingInterp and NativeExpr code
  (NativeExpr still needs a lot more); NativeExpr code can now
  parse expressions containing constant strings (they are treated
  as numerical zero for now) (this fixes bug 275).

  Revision 1.3  1997/12/23 23:35:31  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.2  1997/12/04 21:00:38  wenger
  Added standard DEVise headers.

 */

#ifndef NativeExpr_h
#define NativeExpr_h

class Node;

class CGraphicExpr {
private:

public:
  char *pszX;  // the expression itself
  Node *pExprX;  // interpreted expression
  int iXInversible;  // true or false
  Node *pXInverse;  // inverse function
  char *pszXVar;   // inversed variable that can be calculated from inverse
                   // function if there exists inverse     
  char *pszY;
  Node *pExprY;
  int iYInversible;
  Node *pYInverse;
  char *pszYVar;

  char *pszZ;
  Node *pExprZ;
  int iZInversible;
  Node *pZInverse;
  char *pszZVar;

  char *pszColor;
  Node *pExprColor;
  int iColorInversible;
  Node *pColorInverse;
  char *pszColorVar;

  char *pszSize;
  Node *pExprSize;
  int iSizeInversible;
  Node *pSizeInverse;
  char *pszSizeVar;

  char *pszPattern;
  Node *pExprPattern;
  int iPatternInversible;
  Node *pPatternInverse;
  char *pszPatternVar;

  char *pszShape;
  Node *pExprShape;
  int iShapeInversible;
  Node *pShapeInverse;
  char *pszShapeVar;

  char *pszOrientation;
  Node *pExprOrientation;
  int iOrientationInversible;
  Node *pOrientationInverse;
  char *pszOrientationVar;

  char *ppszGDataAttr[MAX_SHAPE_ATTRS];  // defined in GDataRec.h
  Node *ppExprGDataAttr[MAX_SHAPE_ATTRS];
  int GDataAttrInversible[MAX_SHAPE_ATTRS];
  Node *ppGDataAttrInverse[MAX_SHAPE_ATTRS];
  char *ppszGDataAttrVar[MAX_SHAPE_ATTRS];

  CGraphicExpr( MappingInterpCmd *cmd );
  ~CGraphicExpr();

  int GetRange( Interval GDataRange[], Interval **ppSourceRangeList, 
		int &iNumOfRanges );

private:
  int AddRange( Interval **ppSourceRangeList, Coord High, Coord Low, char *pszVar,
		int& iNumOfRanges, Node *pInverse );
};

#define MAX_STACK_SIZE 100

//TEMPTEMP -- are the attrs static?
extern int InsertAttr( char *pszName, double dVal );
extern int InitAttrList( void );
extern double EvalExpr( Node *pExprRoot );
extern int InverseExpr( Node *pExpr, Node **ppInverseExpr,
			char **ppszInverseVar );

#endif
