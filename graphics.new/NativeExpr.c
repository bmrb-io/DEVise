/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implemenation of expression evaluation.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1998/01/26 23:19:27  wenger
  Fixed a bug in expression evaluation code (didn't allow underscores in
  attribute names) and added better error messages.

  Revision 1.3  1997/12/23 23:35:30  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.2  1997/12/04 21:00:37  wenger
  Added standard DEVise headers.

 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "MappingInterp.h"
#include "NativeExpr.h"

//#define MAX_GDATA_ATTRS 10
#define HUGE_NUMBER  1e30

//#define DEBUG


/****************************  DATA STRUCTURE ********************************/

#define MAX_STACK_SIZE 100

enum VarType { CONST_INT, CONST_FLOAT, SYMBOL, VARIABLE, FUNCTION, OPERATOR };

enum TokenType {
  EX_ERROR,   // error during extraction
  EX_END,     // end of stream
  EX_NIL,     // none of the known tokens
  EX_NUMBER,  // any number
  EX_INT,     // integer
  EX_FLOAT,   // float
  EX_SYMBOL,  // symbol that contains alphanum chars
  EX_VAR,     // symbol that prefixed by a $
  EX_OPERATOR, // +, -, *, /
  EX_LEFTBRACE,  // (
  EX_RIGHTBRACE, // )
  EX_COMMA,   // ,
  EX_STRING,   // constant string, delimited by double quotes (")
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

/***********************  FUNCTION PROTOTYPES ********************************/
static void SkipSpace( char **ppszStr );
static int DupStr(char **ppszSrc, char **ppszDst, int iLen);
static enum TokenType GetNextToken( char **ppszStr, char **ppszToken );
int ParseExpression( char *pszExpr, struct Node **ppExprRoot  );
static int GetPriority( char *pszOperator );
static int PostOrder( struct Node *pExprRoot );
static int PopOperator( void );
static int AddOperator( char *pszToken, int iUnary );
static int InitExpr( void );
static double GetVarValue( char *pszName );
static double GetSymbolValue( char *pszName );
#if defined(EXPR_TEST)
static int TestValue( void );
#endif
static int ExtractNumber( char **ppszStr );
static int PushOperator( enum TokenType iToken, char *pszToken, int iUnary );
static int Inversible( struct Node *pExpr );
static int NumOfVar( struct Node *pExpr );
static int InOrder( struct Node *pExprRoot );
static int ExistNonInvOperator( struct Node *pExpr );



/*********************** Global variables ************************************/
static struct Node OperandStack[MAX_STACK_SIZE];
static int iOperandStackTop = -1;

static struct Operator OperatorStack[MAX_STACK_SIZE];
static int iOperatorStackTop = -1;

static struct AttrValue *pAttrValList = 0;


/*********************** Test Driver  ****************************************/
#if defined(EXPR_TEST)
int main( void )
{
  struct Node *pExprRoot, *pInverseExpr;
  char pExpr[80];
  char *pszInvVar;

  scanf( "%s", pExpr );
  if( -1 == ParseExpression( pExpr, &pExprRoot ) )
  {
    fprintf( stderr, "error during parsing expression\n" );
    return -1;
  }
 
  TestValue();  // create test var-value table
  printf( "Expr = %f\n", EvalExpr( pExprRoot ) );

  if( InverseExpr( pExprRoot, &pInverseExpr, &pszInvVar ) )
  {
    printf( "Inversible expression! %s = ", pszInvVar );

    InOrder( pInverseExpr );

    printf( "\n" );

    // PostOrder( pInverseExpr );
  }
  else
    printf( "Not an inversible expression!\n" );

  return 0;
}
#endif

#if defined(EXPR_TEST)
static int TestValue( void )
{
  InsertAttr( "$a", 1 );
  InsertAttr( "$b", 2 );
  InsertAttr( "$c", 3 );
  InsertAttr( "$d", 4 );
  printf( "\n" );
  
  return 0;
}
#endif
/*********************** end of Test Driver  *********************************/

CGraphicExpr::CGraphicExpr( MappingInterpCmd *cmd )
{
#if defined(DEBUG)
  printf("CGraphicExpr::CGraphicExpr()\n");
#endif

  int iCount;
  
  if( cmd->xCmd != NULL )
  {
    pszX = (char *)malloc( strlen( cmd->xCmd ) + 1 );
    strcpy( pszX, cmd->xCmd );

    if( -1 == ParseExpression( pszX, &pExprX ) )
      pExprX = NULL;

    iXInversible = InverseExpr( pExprX, &pXInverse, &pszXVar );
  }
  else
  {
    pszX = NULL;
    pExprX = NULL;
  }

  if( cmd->yCmd != NULL )
  {
    pszY = (char *)malloc( strlen( cmd->yCmd ) + 1 );
    strcpy( pszY, cmd->yCmd );

    if( -1 == ParseExpression( pszY, &pExprY ) )
      pExprY = NULL;

    iYInversible = InverseExpr( pExprY, &pYInverse, &pszYVar );
  }
  else
  {
    pszY = NULL;
    pExprY = NULL;
  }

  if( cmd->zCmd != NULL )
  {
    pszZ = (char *)malloc( strlen( cmd->zCmd ) + 1 );
    strcpy( pszZ, cmd->zCmd );

    if( -1 == ParseExpression( pszZ, &pExprZ ) )
      pExprZ = NULL;
    
    iZInversible = InverseExpr( pExprZ, &pZInverse, &pszZVar );
  }
  else
  {
    pszZ = NULL;
    pExprZ = NULL;
  }

  if( cmd->colorCmd != NULL )
  {
    pszColor = (char *)malloc( strlen( cmd->colorCmd ) + 1 );
    strcpy( pszColor, cmd->colorCmd );

    if( -1 == ParseExpression( pszColor, &pExprColor ) )
      pExprColor = NULL;

    iColorInversible = InverseExpr( pExprColor, &pColorInverse, &pszColorVar );
  }
  else
  {
    pszColor = NULL;
    pExprColor = NULL;
  }

  if( cmd->sizeCmd != NULL )
  {
    pszSize = (char *)malloc( strlen( cmd->sizeCmd ) + 1 );
    strcpy( pszSize, cmd->sizeCmd );

    if( -1 == ParseExpression( pszSize, &pExprSize ) )
      pExprSize = NULL;

    iSizeInversible = InverseExpr( pExprSize, &pSizeInverse, &pszSizeVar );
  }
  else
  {
    pszSize = NULL;
    pExprSize = NULL;
  }
  
  if( cmd->patternCmd != NULL )
  {
    pszPattern = (char *)malloc( strlen( cmd->patternCmd ) + 1 );
    strcpy( pszPattern, cmd->patternCmd );

    if( -1 == ParseExpression( pszPattern, &pExprPattern ) )
      pExprPattern = NULL;

    iPatternInversible = InverseExpr( pExprPattern, &pPatternInverse, 
				     &pszPatternVar );
  }
  else
  {
    pszPattern = NULL;
    pExprPattern = NULL;
  }

  if( cmd->shapeCmd != NULL )
  {
    pszShape = (char *)malloc( strlen( cmd->shapeCmd ) + 1 );
    strcpy( pszShape, cmd->shapeCmd );

    if( -1 == ParseExpression( pszShape, &pExprShape ) )
      pExprShape = NULL;

    iShapeInversible = InverseExpr( pExprShape, &pShapeInverse, &pszShapeVar );
  }
  else
  {
    pszShape = NULL;
    pExprShape = NULL;
  }

  if( cmd->orientationCmd != NULL )
  {
    pszOrientation = (char *)malloc( strlen( cmd->orientationCmd ) + 1 );
    strcpy( pszOrientation, cmd->orientationCmd );

    if( -1 == ParseExpression( pszOrientation, &pExprOrientation ) )
      pExprOrientation = NULL;

    iOrientationInversible = InverseExpr( pExprOrientation, 
					  &pOrientationInverse, 
					  &pszOrientationVar );
  }
  else
  {
    pszOrientation = NULL;
    pExprOrientation = NULL;
  }

  for( iCount = 0; iCount < MAX_GDATA_ATTRS; iCount ++ )
  {
    if( cmd->shapeAttrCmd[iCount] != NULL )
    {
      ppszGDataAttr[iCount] = 
	(char *)malloc( strlen( cmd->shapeAttrCmd[iCount] ) + 1 );
      strcpy( ppszGDataAttr[iCount], cmd->shapeAttrCmd[iCount] );

      if( -1 == ParseExpression( ppszGDataAttr[iCount], 
				 &(ppExprGDataAttr[iCount]) ) )
	ppExprGDataAttr[iCount] = NULL;

      GDataAttrInversible[iCount] = InverseExpr( ppExprGDataAttr[iCount], 
					       &(ppGDataAttrInverse[iCount]), 
					       &(ppszGDataAttrVar[iCount]) );
    }
    else
    {
      ppszGDataAttr[iCount] = NULL;
      ppExprGDataAttr[iCount] = NULL;
    }
  }
}

CGraphicExpr::~CGraphicExpr()
{
  int iCount;

  if( pszX != NULL )
    free( pszX );
  if( pszY != NULL )
    free( pszY );
  if( pszZ != NULL )
    free( pszZ );
  if( pszColor != NULL )
    free( pszColor );
  if( pszSize != NULL )
    free( pszSize );
  if( pszPattern != NULL )
    free( pszPattern );
  if( pszShape != NULL )
    free( pszShape );
  if( pszOrientation != NULL )
    free( pszOrientation );

  for( iCount = 0; iCount < MAX_GDATA_ATTRS; iCount ++ )
    if( ppszGDataAttr[iCount] != NULL )
      free( ppszGDataAttr[iCount] );
}

int CGraphicExpr::GetRange( Interval GDataRange[], Interval **ppSourceRangeList, 
			    int &iNumOfRanges )
{
  int iCount;

  *ppSourceRangeList = NULL;
  iNumOfRanges = 0;

  if( GDataRange[0].AttrName != 0 && iXInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[0].High, GDataRange[0].Low, 
	      pszXVar, iNumOfRanges, pXInverse );
  }

  if( GDataRange[1].AttrName != 0 && iYInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[1].High, GDataRange[1].Low, 
	      pszYVar, iNumOfRanges, pYInverse );
  }

  if( GDataRange[2].AttrName != 0 && iZInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[2].High, GDataRange[2].Low, 
	      pszZVar, iNumOfRanges, pZInverse );
  }

  if( GDataRange[3].AttrName != 0 && iColorInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[3].High, GDataRange[3].Low,
	      pszColorVar, iNumOfRanges, pColorInverse );
  }

  if( GDataRange[4].AttrName != 0 && iSizeInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[4].High, GDataRange[4].Low,
	      pszSizeVar, iNumOfRanges, pSizeInverse );
  }

  if( GDataRange[5].AttrName != 0 && iPatternInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[5].High, GDataRange[5].Low,
	      pszPatternVar, iNumOfRanges, pPatternInverse );
  }

  if( GDataRange[6].AttrName != 0 && iShapeInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[6].High, GDataRange[6].Low,
	      pszShapeVar, iNumOfRanges, pShapeInverse );
  }

  if( GDataRange[7].AttrName != 0 && iOrientationInversible )
  {
    AddRange( ppSourceRangeList, GDataRange[7].High, GDataRange[7].Low,
	      pszOrientationVar, iNumOfRanges, pOrientationInverse );
  }

  for( iCount = 0; iCount < MAX_GDATA_ATTRS; iCount ++ )
  {
    if( GDataRange[8+iCount].AttrName != 0 && GDataAttrInversible[iCount] )
    {
      AddRange( ppSourceRangeList, GDataRange[8 + iCount].High,
		GDataRange[8 + iCount].Low, ppszGDataAttrVar[8 + iCount],
		iNumOfRanges, ppGDataAttrInverse[8 + iCount] );
    }
  }

  return 0;
}

// private member function
int CGraphicExpr::AddRange( Interval **ppSourceRange, Coord High, Coord Low, 
			    char *pszVar, int& iNumOfRanges, 
			    struct Node *pInverse )
{
  Coord xHigh, xLow;

  iNumOfRanges ++;

  if( iNumOfRanges == 1 )
  {
    // a new start
    *ppSourceRange = (Interval *)malloc( sizeof(Interval) );
  }
  else
    *ppSourceRange = (Interval *)realloc( *ppSourceRange, 
				       iNumOfRanges * sizeof(Interval) );

  (*ppSourceRange)[iNumOfRanges - 1].AttrName = 
    (char *)malloc( strlen( pszVar ) + 1 );
  strcpy( (*ppSourceRange)[iNumOfRanges - 1].AttrName, pszVar );

  // get high and low
  InitAttrList();
  InsertAttr( "$$", Low );
  xLow = EvalExpr( pInverse );
  
  InitAttrList();
  InsertAttr( "$$", High );
  xHigh = EvalExpr( pInverse );
  
  if( xLow > xHigh )
  {
    (*ppSourceRange)[iNumOfRanges - 1].High = xLow;
    (*ppSourceRange)[iNumOfRanges - 1].Low = xHigh;
  }
  else
  {
    (*ppSourceRange)[iNumOfRanges - 1].High = xHigh;
    (*ppSourceRange)[iNumOfRanges - 1].Low = xLow;
  }

  return 0;
}

// return True or False - whether the expression can be inversed
// pExpr is the original expression
// ppInverseExpr is the inverse expression if exists
// ppszInverseVar is the inversed variable that is the result of ppInverseExpr
int InverseExpr( struct Node *pExpr, struct Node **ppInverseExpr,
			char **ppszInverseVar )
{
  if( !Inversible( pExpr ) )
    return 0;  // not inversible

  // create $$ node that's the result of the original expression
  *ppInverseExpr = (struct Node *)calloc( sizeof( struct Node ), 1 );
  (*ppInverseExpr)->v.iType = VARIABLE;
  (*ppInverseExpr)->v.pszName = (char*)malloc( 3 );
  strcpy( (*ppInverseExpr)->v.pszName, "$$" );

  while( pExpr->v.iType != VARIABLE )
  {
    struct Node *pTmpNode;

    if( pExpr->v.iType != OPERATOR )
    {
      // should be an operator at the top node
      // but we should deal with functions also later!
      fprintf(stderr, "Expression analysis internal error inverting {?}\n");
      return 0;
    }

    pTmpNode = (struct Node *)calloc( sizeof( struct Node ), 1 );
    pTmpNode->v.iType = OPERATOR;
    pTmpNode->v.pszName = (char *)malloc( 2 );  // hold operator

    // continue inversing
    if( NumOfVar( pExpr->pLeft ) == 1 )
    {
      // left child contains the variable, combine right tree with the result

      pTmpNode->pLeft = *ppInverseExpr;
      pTmpNode->pRight = pExpr->pRight;

      if( !strcmp( pExpr->v.pszName, "+" ) )
      {
	// possibly the original expression and the inverse expression will
	// twisted together!!!! 
	// be cautious when freeing them!!!!!!

	strcpy( pTmpNode->v.pszName, "-" );
      }
      else
	if( !strcmp( pExpr->v.pszName, "-" ) )
	{
	  strcpy( pTmpNode->v.pszName, "+" );
	}
      else
	if( !strcmp( pExpr->v.pszName, "*" ) )
	{
	  strcpy( pTmpNode->v.pszName, "/" );
	}
      else
	if( !strcmp( pExpr->v.pszName, "/" ) )
	{
	  strcpy( pTmpNode->v.pszName, "*" );
	}
      else
	if( !strcmp( pExpr->v.pszName, "^" ) )
	{
	  // X^R = $$ -> X = $$^(1/R)
	  struct Node *pTmpNode1, *pTmpNode2;

	  pTmpNode1 = (struct Node *)calloc( sizeof( struct Node ), 1 );
	  pTmpNode1->v.iType = OPERATOR;
	  pTmpNode1->v.pszName = (char *)malloc( 2 );  // hold operator
	  strcpy( pTmpNode1->v.pszName, "/" );
	  pTmpNode1->pRight = pExpr->pRight;

	  pTmpNode2 = (struct Node *)calloc( sizeof( struct Node ), 1 );
	  pTmpNode2->v.iType = CONST_FLOAT;
	  pTmpNode2->v.pszValue = (char *)malloc( 2 );  // hold operator
	  strcpy( pTmpNode2->v.pszValue, "1" );
	  pTmpNode2->v.dValue = 1;

	  pTmpNode1->pLeft = pTmpNode2;

	  strcpy( pTmpNode->v.pszName, "^" );
	  pTmpNode->pRight = pTmpNode1;

	}
      else
	if( !strcmp( pExpr->v.pszName, "@" ) )
	{
	  // log
	  pTmpNode->pRight = *ppInverseExpr;
	  pTmpNode->pLeft = pExpr->pRight;
	  strcpy( pTmpNode->v.pszName, "^" );
	}
      else
      {
        fprintf(stderr, "Expression analysis internal error inverting {?}\n");
	return 0;  // invalid expression
      }

      // now both expressions change
      *ppInverseExpr = pTmpNode;
      pExpr = pExpr->pLeft;
    }  // end of if( NumOfVar.....
    else
      if( NumOfVar( pExpr->pRight ) == 1 )
      {
	// right child has the vaiable, we move the left child tree to the
	// right of the equation
      
	if( !strcmp( pExpr->v.pszName, "+" ) )
	{
	  pTmpNode->pLeft = *ppInverseExpr;
	  pTmpNode->pRight = pExpr->pLeft;
	  strcpy( pTmpNode->v.pszName, "-" );
	}
	else
	  if( !strcmp( pExpr->v.pszName, "-" ) )
	  {
	    pTmpNode->pLeft = pExpr->pLeft;
	    pTmpNode->pRight = *ppInverseExpr;
	    strcpy( pTmpNode->v.pszName, "-" );
	  }
	else
	  if( !strcmp( pExpr->v.pszName, "*" ) )
	  {
	    pTmpNode->pLeft = *ppInverseExpr;
	    pTmpNode->pRight = pExpr->pLeft;
	    strcpy( pTmpNode->v.pszName, "/" );
	  }
	else
	  if( !strcmp( pExpr->v.pszName, "/" ) )
	  {
	    pTmpNode->pLeft = pExpr->pLeft;
	    pTmpNode->pRight = *ppInverseExpr;
	    strcpy( pTmpNode->v.pszName, "/" );
	  }
	else
	  if( !strcmp( pExpr->v.pszName, "^" ) )
	  {
	    pTmpNode->pLeft = *ppInverseExpr;
	    pTmpNode->pRight = pExpr->pLeft;
	    strcpy( pTmpNode->v.pszName, "@" );
	  }
	else
	  if( !strcmp( pExpr->v.pszName, "@" ) )
	  {
	    // L@X = $$ -> X = L^(1/$$)
	    struct Node *pTmpNode1, *pTmpNode2;
	    
	    pTmpNode1 = (struct Node *)calloc( sizeof( struct Node ), 1 );
	    pTmpNode1->v.iType = OPERATOR;
	    pTmpNode1->v.pszName = (char *)malloc( 2 );  // hold operator
	    strcpy( pTmpNode1->v.pszName, "/" );
	    pTmpNode1->pRight = *ppInverseExpr;

	    pTmpNode2 = (struct Node *)calloc( sizeof( struct Node ), 1 );
	    pTmpNode2->v.iType = CONST_FLOAT;
	    pTmpNode2->v.pszValue = (char *)malloc( 2 );  // hold operator
	    strcpy( pTmpNode2->v.pszValue, "1" );
	    pTmpNode2->v.dValue = 1;

	    pTmpNode1->pLeft = pTmpNode2;

	    pTmpNode->pRight = pTmpNode1;
	    strcpy( pTmpNode->v.pszName, "^" );
	    pTmpNode->pLeft = pExpr->pLeft;
	  }
	else
	  {
            fprintf(stderr, "Expression analysis internal error inverting {?}\n");
	    return 0;
	  }

	*ppInverseExpr = pTmpNode;
	pExpr = pExpr->pRight;
      }
    else
      {
        fprintf(stderr, "Expression analysis internal error inverting {?}\n");
	return 0;
      }     
  }  // end of while

  *ppszInverseVar = (char *)malloc( strlen( pExpr->v.pszName ) + 1 );
  strcpy( *ppszInverseVar, pExpr->v.pszName );
  
  return 1;  // finish inversing
}

// determine whether the expression is inversible
// return true or false
int Inversible( struct Node *pExpr )
{
  // basically, I look up the variable in the expression, if # of variables >=
  // 2 or == 0, then not inversible, else inversible
  // if the expression contains non-inversible operators, then non-inversible

  if( NumOfVar( pExpr ) != 1 )
    return 0;

  if( ExistNonInvOperator( pExpr ) )
    return 0;

  return 1;
}

int ExistNonInvOperator( struct Node *pExpr )
{
  // [, ] (floor, ceiling) are not inversible
  if( pExpr == NULL )
    return 0;

  if( pExpr->v.iType == OPERATOR )
  {
    if( !strcmp( pExpr->v.pszName, "[" ) )
      return 1;
    else
      if( !strcmp( pExpr->v.pszName, "]" ) )
	return 1;
  }

  if( ExistNonInvOperator( pExpr->pLeft ) )
    return 1;

  if( ExistNonInvOperator( pExpr->pRight ) )
    return 1;

  return 0;
}

// return # of variables in the expression, count duplicates
int NumOfVar( struct Node *pExpr )
{
  int iCount = 0;

  if( pExpr == 0 )
    return 0;

  if( pExpr->v.iType == VARIABLE )
    iCount = 1;

  return iCount + NumOfVar( pExpr->pLeft ) + NumOfVar( pExpr->pRight );
}

// look up symbol table to find const value
double GetSymbolValue( char *pszName )
{
  // not implemented yet
  return 0;
}

// called before using expression analysis, called by ParseExpression
int InitExpr( void )
{
  iOperatorStackTop = -1;
  iOperandStackTop = -1;

  return 0;
}

// called before insert any attributes
int InitAttrList( void )
{
  struct AttrValue *pTemp1, *pTemp2;

  if( pAttrValList == 0 )
    return 0;

  // free all storage occupied by previous instance
  pTemp1 = pAttrValList;

  while( pTemp1 != 0 )
  {
    pTemp2 = pTemp1->pNext;
    free( pTemp1 );
    pTemp1 = pTemp2;
  }

  pAttrValList = 0;
  return 0;
}

// insert a new attribute and its value
int InsertAttr( char *pszName, double dVal )
{
  struct AttrValue *pNew;

  pNew = (struct AttrValue *)calloc( 1, sizeof( struct AttrValue ) );
  if( pNew == NULL )
    return -1;

  pNew->pszName = (char *)malloc( strlen( pszName ) + 1 );
  strcpy( pNew->pszName, pszName );

  pNew->dVal = dVal;

  // insert new item into the head of list
  pNew->pNext = pAttrValList;
  pAttrValList = pNew;

  //printf( "%s = %f ", pszName, dVal );

  return 0;
}

// look up attribute value list
// pszName always points to a $variable
double GetVarValue( char *pszName )
{
  struct AttrValue *pRun = pAttrValList;
  int iCmp;

  if( pszName == NULL )
    return 0;

  while( pRun != NULL )
  {
    // handle both $variable or non$variable
    if( *(pRun->pszName) == '$' )
      iCmp = strcmp( pRun->pszName, pszName );
    else
      iCmp = strcmp( pRun->pszName, pszName + 1 );

    if( !iCmp )
      return pRun->dVal;
    else
      pRun = pRun->pNext;
  }
   
  return 0; // not found --> return 0 or error?
}

// evaluate the expression
double EvalExpr( struct Node *pExprRoot )
{
  if( NULL == pExprRoot )
    return 0;

  switch( pExprRoot->v.iType )
    {
    case CONST_INT:
    case CONST_FLOAT:
      return pExprRoot->v.dValue;

    case VARIABLE:
      return GetVarValue( pExprRoot->v.pszName );

    case SYMBOL:
      return GetSymbolValue( pExprRoot->v.pszName );

    case OPERATOR:
      {
	double dLValue, dRValue;

	dLValue = EvalExpr( pExprRoot->pLeft );
	dRValue = EvalExpr( pExprRoot->pRight );

	if( !strcmp( pExprRoot->v.pszName, "+" ) )
	  return dLValue + dRValue;
	else
	  if( !strcmp( pExprRoot->v.pszName, "-" ) )
	    return dLValue - dRValue;  // also deal with unary operator
	else
	  if( !strcmp( pExprRoot->v.pszName, "*" ) )
	    return dLValue * dRValue;
	else
	  if( !strcmp( pExprRoot->v.pszName, "/" ) )
	  {
	    if( dRValue == 0 )
	      return HUGE_NUMBER;

	    return dLValue / dRValue;  //potential div by 0!!!
	   }
	 else
	   if( !strcmp( pExprRoot->v.pszName, "[" ) )
	   {
	     // floor operator      
	     return floor(dRValue);
	   }
	 else
	   if( !strcmp( pExprRoot->v.pszName, "]" ) )
	   {
	     // ceiling operator
	     return  dRValue - floor(dRValue);
	   }
	 else
	   if( !strcmp( pExprRoot->v.pszName, "^" ) )
	   {
	     // exponential operator
	     return pow( dLValue, dRValue );
	   }
	 else
	   if( !strcmp( pExprRoot->v.pszName, "@" ) )
	   {
	     // log operator
	     return log( dLValue ) / log( dRValue );
	   }
	 else
	  return 0;  // unknown operator
      }

    case FUNCTION:  // not implemented yet
    default:
      return 0;
    }  // end of switch
}

int InOrder( struct Node *pExprRoot )
{
  if( pExprRoot == NULL )
    return 0;

  if( pExprRoot->pLeft != NULL || pExprRoot->pRight != NULL )
    printf( "(" );

  if( pExprRoot->pLeft != NULL )
    InOrder( pExprRoot->pLeft );

  switch( pExprRoot->v.iType )
    {
    case CONST_INT:
    case CONST_FLOAT:
      printf( "%s", pExprRoot->v.pszValue );
      break;

    case SYMBOL:
    case VARIABLE:
    case FUNCTION:
    case OPERATOR:
      printf( "%s", pExprRoot->v.pszName );
      break;

    default:
      break;
    }

  if( pExprRoot->pRight != NULL )
    InOrder( pExprRoot->pRight );

  if( pExprRoot->pRight != NULL || pExprRoot->pLeft != NULL )
    printf( ")" );

  return 0;
}

// post-order print out the expression
int PostOrder( struct Node *pExprRoot )
{
  if( pExprRoot == NULL )
    return 0;

  if( pExprRoot->pLeft != NULL )
    PostOrder( pExprRoot->pLeft );

  if( pExprRoot->pRight != NULL )
    PostOrder( pExprRoot->pRight );

  switch( pExprRoot->v.iType )
    {
    case CONST_INT:
    case CONST_FLOAT:
      printf( "%s", pExprRoot->v.pszValue );
      break;

    case SYMBOL:
    case VARIABLE:
    case FUNCTION:
    case OPERATOR:
      printf( "%s", pExprRoot->v.pszName );
      break;

    default:
      break;
    }

  return 0;
}

/***********************  FUNCTION DEFINITION ********************************/
// return 0 if succeed
int PushOperand( enum TokenType iToken, char *pszToken )
{
  // printf( "%s\n", pszToken );

  iOperandStackTop ++;  // always add 1 before insert
  if( iOperatorStackTop >= MAX_STACK_SIZE )
  {
    fprintf( stderr, "Expression stack overflow.\n" );
    return -1;
  }

  // initialize pointers
  OperandStack[iOperandStackTop].pLeft = 0;
  OperandStack[iOperandStackTop].pRight = 0;

  switch( iToken )
    {
    case EX_NUMBER:
    case EX_INT:
    case EX_FLOAT:
      OperandStack[iOperandStackTop].v.iType = CONST_FLOAT;
      OperandStack[iOperandStackTop].v.pszValue = 
	(char *)malloc( strlen(pszToken) + 1 );
      strcpy( OperandStack[iOperandStackTop].v.pszValue, pszToken );
      OperandStack[iOperandStackTop].v.dValue = strtod( pszToken, NULL );
      break;

    case EX_SYMBOL:
      OperandStack[iOperandStackTop].v.iType = SYMBOL;
      OperandStack[iOperandStackTop].v.pszName =
	(char *)malloc( strlen(pszToken) + 1 );
      strcpy( OperandStack[iOperandStackTop].v.pszName, pszToken );
      break;

    case EX_VAR:
      OperandStack[iOperandStackTop].v.iType = VARIABLE;
      OperandStack[iOperandStackTop].v.pszName =
	(char *)malloc( strlen(pszToken) + 1 );
      strcpy( OperandStack[iOperandStackTop].v.pszName, pszToken );
      break;

    default:
      fprintf( stderr,
	"Expression analysis internal error pushing token {%s}.\n", pszToken);
      return -1;
    }

  return 0;
}

// return 0 if succeed, -1 if failed
int PushOperator( enum TokenType iToken, char *pszToken, int iUnary )
{

//  if( pszToken != NULL )
//    printf( "%s\n", pszToken );

  switch( iToken )
    {
    case EX_OPERATOR:
MorePop:
      if( ( iOperatorStackTop == -1 ) // no operator now
	  || iUnary    // is unary operator, has highest priority
	  || ( GetPriority( pszToken ) >   // has greater priority
	       OperatorStack[iOperatorStackTop].iPriority ) )
      {
	return AddOperator( pszToken, iUnary );
      }
      else
      {
	// 1. pop out top 2 operands and top 1 operator
	// 2. form a small tree and save the root as top operand
	// 3. push the new operator into stack

	if( pszToken == NULL && iOperatorStackTop < 0 )
	  return 0;  // finish parsing the expression

	if( -1 == PopOperator( ) )
	  return -1;  // some error during parsing expression

	goto MorePop;

      }

    case EX_LEFTBRACE:
      return AddOperator( pszToken, 0 );

    case EX_RIGHTBRACE:
      while( iOperatorStackTop >= 0 )
      {
	// pop out all operators and operands between nearest pair of brackets

	if( !strcmp( OperatorStack[iOperatorStackTop].pszOperator, "(" ) )
	{
	  // match left bracket, just return
	  iOperatorStackTop --;
	  return 0;
	}
	
	if( -1 == PopOperator( ) )
	  return -1;  // some error during parsing expression
      }

      return -1;  // still not find left bracket, error

    default:
      fprintf(stderr, "Expression analysis internal error pushing token {%s}",
	pszToken );
      return -1;
    }

  return 0;
}

// pop out top 1 operator and 2 operands and form a expression node and insert
// back into operand stack
// now unary operator is supported
int PopOperator( void )
{
  // 1. pop out top 2 operands and top 1 operator
  // 2. form a small tree and save the root as top operand

  struct Node *pTempNode1, *pTempNode2;
	
  if( iOperandStackTop < 0 )  // no operands available
    return -1;
	  
  if( !OperatorStack[iOperatorStackTop].iUnary )
  {
    // needs 2 operands
    if( iOperandStackTop < 1 )
      return -1;  // not enough operands available

    pTempNode1 = (struct Node *)calloc( 1, sizeof( struct Node ) );
    *pTempNode1 = OperandStack[iOperandStackTop - 1]; // left child
  }
  else
    pTempNode1 = NULL;

  pTempNode2 = (struct Node *)calloc( 1, sizeof( struct Node ) );
  *pTempNode2 = OperandStack[iOperandStackTop]; // right child

  // pop out 1 or 2 operands and insert a new
  if( !OperatorStack[iOperatorStackTop].iUnary )
    iOperandStackTop --;  // only non-unary operator need shrink the operands 
			  // stack

  OperandStack[iOperandStackTop].pLeft = pTempNode1;
  OperandStack[iOperandStackTop].pRight = pTempNode2;
  OperandStack[iOperandStackTop].v.iType = OPERATOR;
  OperandStack[iOperandStackTop].v.pszName = 
    OperatorStack[iOperatorStackTop].pszOperator;

  iOperatorStackTop --;  // 'cause 1 operator has been popped out

  return 0;
}

// add one more operator to the operator stack top
int AddOperator( char *pszToken, int iUnary )
{
  if( pszToken == NULL )
    return 0;  // end of expression

  // just insert this one
  iOperatorStackTop ++;
  if( iOperatorStackTop > MAX_STACK_SIZE )
  {
    fprintf( stderr, "Expression stack overflow.\n" );
    return -1;
  }
	
  OperatorStack[iOperatorStackTop].pszOperator = 
    (char *)malloc( strlen(pszToken) + 1 );
  strcpy( OperatorStack[iOperatorStackTop].pszOperator, pszToken );
  OperatorStack[iOperatorStackTop].iPriority = GetPriority( pszToken );
  OperatorStack[iOperatorStackTop].iUnary = iUnary;
  return 0;
}

// return priority of operators, the bigger, the larger
int GetPriority( char *pszOperator )
{
  if( pszOperator == NULL )
    return -1;  // lowest possible priority for NULL
  else
    if( !strcmp( pszOperator, "+" ) )
      return 1;
  else
    if( !strcmp( pszOperator, "-" ) )
      return 1;
  else
    if( !strcmp( pszOperator, "*" ) )
      return 2;
  else
    if( !strcmp( pszOperator, "/" ) )
      return 2;
  else
    if( !strcmp( pszOperator, "(" ) )
      return 0;  // for computation convenience
  else
    if( !strcmp( pszOperator, ")" ) )
      return 3;  // for computation convenience, always assume ) has lowest
		 // priority
  else
    if( !strcmp( pszOperator, "^" ) )
      return 3;  // exponential operator
  else
    if( !strcmp( pszOperator, "[" ) )
      return 4;  // floor operator, unary
  else
    if( !strcmp( pszOperator, "]" ) )
      return 4;  // roof/ceiling operator, unary
  else
    if( !strcmp( pszOperator, "@" ) )
      return 3;  // log: x@y = log(x) base y
  else
    return -1;  // unknown operator
}

// pszExpr points to the expression to be parsed
// ppExprRoot is an out parameter, will point to the root of the expression 
// tree
// RETURN 0 if success, -1 if failed
int ParseExpression( char *pszExpr, struct Node **ppExprRoot  )
{
#if defined(DEBUG)
  printf("ParseExpression(%s)\n", pszExpr);
#endif

  char *pRunPtr = pszExpr;
  char *pszToken = NULL;  // point to token returned
  enum TokenType iPrevToken = EX_NIL;  // start of expression
  enum TokenType iNowToken;
  
  InitExpr();

  while( *pRunPtr != 0 )
  {  // not reaching the end of the expression yet
    iNowToken = GetNextToken( &pRunPtr, &pszToken );
#if defined(DEBUG)
    printf("  token = {%s}; token type = %d\n", pszToken != NULL ? pszToken :
      "NULL", iNowToken);
#endif
    switch( iNowToken )
      {
      case EX_VAR:
	if( 0 != PushOperand( EX_VAR, pszToken ) )
	  return -1;
	break;

      case EX_SYMBOL:
	if( 0 != PushOperand( EX_SYMBOL, pszToken ) )
	  return -1;
	break;

      case EX_NUMBER:
	if( 0 != PushOperand( EX_NUMBER, pszToken ) )
	  return -1;
	break;

      case EX_OPERATOR:
	if( !strcmp( pszToken, "[" ) || !strcmp( pszToken, "]" ) )
	{
	  // unary operator: floor & ceiling
	  if( 0 != PushOperator( EX_OPERATOR, pszToken, 1 ) )
	    return -1;
	  break;
	}
	if( !strcmp( pszToken, "+" ) || !strcmp( pszToken, "-" ) )
	{
	  // check if is unary
	  if( iPrevToken == EX_NIL || iPrevToken == EX_LEFTBRACE )
	  {
	    // unary operator
	    if( 0 != PushOperator( EX_OPERATOR, pszToken, 1 ) )
	      return -1;
	    break;
	  }
	}

	if( 0 != PushOperator( EX_OPERATOR, pszToken, 0 ) )
	  return -1;
	break;

      case EX_LEFTBRACE:
	if( 0 != PushOperator( EX_LEFTBRACE, pszToken, 0 ) )
	  return -1;
	break;

      case EX_RIGHTBRACE:
	if( 0 != PushOperator( EX_RIGHTBRACE, pszToken, 0 ) )
	  return -1;
	break;

      case EX_STRING:
	// Treat a constant string as numerical zero, at least until
	// we can actually deal with strings.
	if( 0 != PushOperand( EX_NUMBER, "0" ) )
	  return -1;
	break;

      default:
	fprintf(stderr, "Expression analysis internal error parsing {%s}\n",
	  pszExpr);
        fprintf(stderr, "  Can't recognize token in {%s}\n",
	  pRunPtr != NULL ? pRunPtr : "NULL");
	return -1;
      }  // end of switch

    iPrevToken = iNowToken;

    free( pszToken );  // because the GetNextToken allocates the space
  }  // end of while

  while( iOperatorStackTop >= 0 )
  {
    // reach the end of the expression but still stack not empty
    // pop out all the remaining operators
    if( 0 != PushOperator( EX_OPERATOR, NULL, 0 ) )
      return -1;  // NULL for end of expression
  }

  if( iOperandStackTop != 0 || iOperatorStackTop != -1 )
    return -1;  // final state incorrect

  (*ppExprRoot) = (struct Node*)malloc( sizeof( struct Node ) );
  if( *ppExprRoot == NULL )
    return -1;

  (**ppExprRoot) = OperandStack[iOperandStackTop];

  return 0;
}

// ppszStr points to the beginning of string that needs to extract a token from
// also an out parameter that will point to the position that is to start 
// scanning next time
// ppszToken is an out parameter, will point to the token extracted or NULL
// expect the caller to free this storage
// RETURN: the type of the token just extracted 
enum TokenType GetNextToken( char **ppszStr, char **ppszToken )
{
  enum TokenType RetTok = EX_END;
  int iCount;

  if( ( ppszStr == NULL ) || ( *ppszStr == NULL ) ) {
    printf("NULL input string to GetNextToken().\n");
    return EX_ERROR;
  }

#if defined(DEBUG)
  printf("GetNextToken(%s)\n", *ppszStr);
#endif

  SkipSpace( ppszStr );

  if( **ppszStr != 0 )
  {
    switch( **ppszStr )
    {
    case '(':
      RetTok = EX_LEFTBRACE;
      break;

    case ')':
      RetTok = EX_RIGHTBRACE;
      break;

    case ',':
      RetTok = EX_COMMA;
      break;

    case '+':
    case '-':
    case '*':
    case '/':
    case '^':  // exponential
    case ']':  // floor
    case '[':  // ceiling
    case '@':  // log
      RetTok = EX_OPERATOR;
      break;

    case '$':  // variable
      iCount = 1;   // number of chars in the variable 
      (*ppszStr) ++;
      
      // Underscore is a valid character in a variable name.  RKW.
      while(isalnum(**ppszStr) || **ppszStr == '_')
      {
	(*ppszStr) ++;
	iCount ++;
      }

      if(iCount == 1) {
        fprintf(stderr, "Only $, no variable name.\n");
	return EX_NIL;  // only $, no chars
      }

      if(-1 == DupStr(ppszStr, ppszToken, iCount)) {
	fprintf(stderr, "Out of memory at %s: %d.\n", __FILE__, __LINE__);
	return EX_ERROR;
      }

      return EX_VAR;

    case '"':  // string
      //find end of string
      iCount = 1;  // number of chars in this symbol
      (*ppszStr) ++;

      while(true)
      {
	if (**ppszStr == '\0')
	{
	  fprintf(stderr, "Warning: no closing quote on string <%s>\n",
	    *ppszStr - iCount);
	  break;
	}

        iCount ++;
        (*ppszStr) ++;

        if (**ppszStr == '"') {
          iCount ++;
          (*ppszStr) ++;
	  break;
	}
      }

      if(-1 == DupStr(ppszStr, ppszToken, iCount)) {
        fprintf(stderr, "Out of memory at %s: %d.\n", __FILE__, __LINE__);
        return EX_ERROR;
      }

      return EX_STRING;

    default:  // symbols, numbers, unknowns
      if(isalpha(**ppszStr))
      {
	// candidate for symbol
	iCount = 1;  // number of chars in this symbol
	(*ppszStr) ++;

	while(isalnum(**ppszStr))
	{
	  iCount ++;
	  (*ppszStr) ++;
	}

	if(-1 == DupStr(ppszStr, ppszToken, iCount)) {
	  fprintf(stderr, "Out of memory at %s: %d.\n", __FILE__, __LINE__);
	  return EX_ERROR;
	}

	return EX_SYMBOL;
      }
      else 
	if( isdigit(**ppszStr) || **ppszStr == '.' )
	{
	  // candidate for number
	  iCount = ExtractNumber( ppszStr );

	  if(-1 == DupStr(ppszStr, ppszToken, iCount)) {
	    fprintf(stderr, "Out of memory at %s: %d.\n", __FILE__, __LINE__);
	    return EX_ERROR;
	  }

	  return EX_NUMBER;
	}

      fprintf(stderr, "Unrecognized character {%c}\n", **ppszStr);
      return EX_NIL;  // unrecognized character
    }  // end of switch
  
    // continue from all cases except default & $
    *ppszToken = (char *)malloc(2);
    if(*ppszToken == NULL) {
      fprintf(stderr, "Out of memory at %s: %d.\n", __FILE__, __LINE__);
      return EX_ERROR;
    }

    **ppszToken = **ppszStr;  // copy the character
    *(*ppszToken + 1) = 0;
    (*ppszStr) ++;
  }  // end of if

  return RetTok;
}

// extract a number from input string
// return number of chars read
int ExtractNumber( char **ppszStr )
{
  int iCount = 0;

  // extract the integer part  xxxx from xxxx.yyyyEzzz
  while( isdigit( **ppszStr ) )
  {
    iCount ++;
    (*ppszStr) ++;
  }
  
  if( **ppszStr == '.' )
  {
    // read part yyyy
    (*ppszStr) ++;
    iCount ++;

    while( isdigit(**ppszStr) )
    {
      iCount ++;
      (*ppszStr) ++;
    }
  }

  if( **ppszStr == 'E' || **ppszStr == 'e' )
  {
    // read Exp part zzz
    (*ppszStr) ++;
    iCount ++;

    // potentially contain + or - in zzz
    if( **ppszStr == '+' || **ppszStr == '-' )
    {
      if( !isdigit( *(*ppszStr + 1) ) )
      {
	(*ppszStr) --;
	iCount --; // back off

	return iCount;
      }
      else
      {
	(*ppszStr) ++;  // skip over + or -
	iCount ++;
      }
    }

    if( !isdigit( **ppszStr ) )
    {
      // back off, zzz is not number
      (*ppszStr) --;
      iCount --;

      return iCount;
    }

    // now read the real zzz part
    while( isdigit( **ppszStr ) )
    {
      (*ppszStr) ++;
      iCount ++;
    }
  }

  return iCount;
}

// routine to skip spaces in the beginning of the string
void SkipSpace( char **ppszStr )
{
  if( ppszStr == 0 || *ppszStr == 0 )
    return;

  while( ( **ppszStr != 0 ) && isspace(**ppszStr) )
    (*ppszStr) ++;

  return;
}

// special veriosn to copy string with length iLen, start from *ppszSrc-iLen
int DupStr(char **ppszSrc, char **ppszDst, int iLen)
{
  *ppszDst = (char *)malloc(iLen + 1);
  if( *ppszDst == NULL )
    return -1;

  strncpy(*ppszDst, *ppszSrc - iLen, iLen);
  *(*ppszDst + iLen) = 0;

  return 0;
}
