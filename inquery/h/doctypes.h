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


/*
 * "DOCTYPES_H" is a header file that defines a set of low-level datatypes
 *  and specifies the assumptions made about their implementation on the 
 *  hardware.  All other INQUERY datatypes should be defined in terms of 
 *  these datatypes.  The datatypes are defined by both #define and typedef
 *  commands, to support various programming preferences.  We do not care 
 *  which form you use, as long as you use one of them.
 */

#if !defined(DOCTYPES_H)
#define DOCTYPES_H

/*
 *  Low-level type definitions:
 */
#define INQ_BIT1	unsigned char	/* A one-bit data type */
typedef INQ_BIT1	Bit1_t;

#define INQ_CHAR7   char	        /* An ASCII character (normal) */
typedef INQ_CHAR7   Char7_t;

#define INQ_CHAR8   unsigned char	/* An (extended) ASCII character */
typedef INQ_CHAR8   Char8_t;

#define INQ_UCHAR8  unsigned char	/* An (extended) ASCII character */
typedef INQ_UCHAR8  UChar8_t;

#define INQ_INT8    signed char	/* An integer in [-127, 127] */
typedef INQ_INT8    Int8_t;

#define INQ_INT16   short int	/* An integer in [-32768, 32768] */
typedef INQ_INT16   Int16_t;

/* Need to change this for 64 bit machines. */
#if defined(__alpha) || defined(__ksr__)
  #define INQ_INT32   int	        /* An integer in [-2147483648, 2147483648] */
#else
  #define INQ_INT32   long  int	/* An integer in [-2147483648, 2147483648] */
#endif

typedef INQ_INT32   Int32_t;
#define INQ_INT32_T_MAX ((Int32_t) 0X7FFFFFFF)	/* Highest 32 bit integer */
#define INQ_INT32_T_MIN ((Int32_t) 0XFFFFFFFF)	/* Lowest 32 bit integer */

/*
 *  C-level type definitions:
 */
#define INQ_FLOAT   float		/* INQUERY makes no assumptions about size */
typedef INQ_FLOAT   Float_t;

#define INQ_CHAR    INQ_CHAR8		/* 8 bits allows Japanese characters */
typedef INQ_CHAR    Char_t;

#define INQ_UCHAR   INQ_UCHAR8		/* 8 bits allows Japanese characters */
typedef INQ_UCHAR   UChar_t;

#define INQ_INT     INQ_INT32		/* INQUERY assumes at least 32 bits */
typedef INQ_INT     Int_t;        
#define INT_T_MAX INQ_INT32_T_MAX
#define INT_T_MIN INQ_INT32_T_MIN

#define INQ_UINT    unsigned INQ_INT32
typedef INQ_UINT    UInt_t;       

#define INQ_SHORT   INQ_INT16		/* INQUERY assumes at least 16 bits */
typedef INQ_SHORT   Short_t;

/*
 *  INQUERY type definitions:
 */
#define INQ_BOOLEAN INQ_BIT1		/* INQUERY assumes at least 1 bit */
typedef INQ_BOOLEAN Boolean_t;

typedef int BoolInt_t;		/* Temporary type, to replace old "boolean"
				   type which conflicts with Solaris.  This
				   type keeps the .btl, .hdc, and .db files
				   compatible with previous versions.  This
				   should be replaced with Boolean_t the next
				   time one of those file formats changes. */

typedef Char_t *term_id_type;
typedef float node_weight_type;

typedef int (*feedback_function_t)();

/*
 *  RCS identifier:
 */
static char doctypes_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#endif
