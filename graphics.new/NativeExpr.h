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
  Revision 1.2  1997/12/04 21:00:38  wenger
  Added standard DEVise headers.

 */

#ifndef NativeExpr_h
#define NativeExpr_h

enum VarType { CONST_INT, CONST_FLOAT, SYMBOL, VARIABLE, FUNCTION, OPERATOR };
enum TokenType { 
  TK_ERROR,   // error during extraction
  TK_END,     // end of stream
  TK_NIL,     // none of the known tokens
  TK_NUMBER,  // any number
  TK_INT,     // integer
  TK_FLOAT,   // float 
  TK_SYMBOL,  // symbol that contains alphanum chars
  TK_VAR,     // symbol that prefixed by a $
  TK_OPERATOR, // +, -, *, /
  TK_LEFTBRACE,  // (
  TK_RIGHTBRACE, // )
  TK_COMMA,   // ,
};

struct Var {
  enum VarType iType; 
  char *pszName;  // store name of symbol, var, function, operator
  char *pszValue;
  double dValue;  // for number
  int iNoParam;
  struct Var **ppParam;
};

struct Node {
  struct Var v;
  struct Node *pLeft;
  struct Node *pRight;
};

struct Operator {
  char *pszOperator;
  int iPriority;
  int iUnary;
};

// record attribute name <-> value conversion table
struct AttrValue {
  char *pszName;
  double dVal;
  struct AttrValue* pNext;
};

class CGraphicExpr {
private:

public:
  char *pszX;  // the expression itself
  struct Node *pExprX;  // interpreted expression
  int iXInversible;  // true or false
  struct Node *pXInverse;  // inverse function
  char *pszXVar;   // inversed variable that can be calculated from inverse
                   // function if there exists inverse     
  char *pszY;
  struct Node *pExprY;
  int iYInversible;
  struct Node *pYInverse;
  char *pszYVar;

  char *pszZ;
  struct Node *pExprZ;
  int iZInversible;
  struct Node *pZInverse;
  char *pszZVar;

  char *pszColor;
  struct Node *pExprColor;
  int iColorInversible;
  struct Node *pColorInverse;
  char *pszColorVar;

  char *pszSize;
  struct Node *pExprSize;
  int iSizeInversible;
  struct Node *pSizeInverse;
  char *pszSizeVar;

  char *pszPattern;
  struct Node *pExprPattern;
  int iPatternInversible;
  struct Node *pPatternInverse;
  char *pszPatternVar;

  char *pszShape;
  struct Node *pExprShape;
  int iShapeInversible;
  struct Node *pShapeInverse;
  char *pszShapeVar;

  char *pszOrientation;
  struct Node *pExprOrientation;
  int iOrientationInversible;
  struct Node *pOrientationInverse;
  char *pszOrientationVar;

  char *ppszGDataAttr[MAX_GDATA_ATTRS];  // defined in GDataRec.h
  struct Node *ppExprGDataAttr[MAX_GDATA_ATTRS];
  int GDataAttrInversible[MAX_GDATA_ATTRS];
  struct Node *ppGDataAttrInverse[MAX_GDATA_ATTRS];
  char *ppszGDataAttrVar[MAX_GDATA_ATTRS];

  CGraphicExpr( MappingInterpCmd *cmd );
  ~CGraphicExpr();

  int GetRange( Interval GDataRange[], Interval **ppSourceRangeList, 
		int &iNumOfRanges );

private:
  int AddRange( Interval **ppSourceRangeList, Coord High, Coord Low, char *pszVar,
		int& iNumOfRanges, struct Node *pInverse );
};

#define MAX_STACK_SIZE 100

extern int InsertAttr( char *pszName, double dVal );
extern int InitAttrList( void );
extern double EvalExpr( struct Node *pExprRoot );
extern int InverseExpr( struct Node *pExpr, struct Node **ppInverseExpr,
			char **ppszInverseVar );

#endif
