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

/* definitions required by various parsers */

#if !defined(PARSE_H)
#define PARSE_H

static char parse_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#define INITIAL_BUFFER    10000
#define MIN_WORD_SIZE         1      /* 2 */
#define MAX_STEM_SIZE        80
#define TAG_MODE              0
#define WORD_MODE             1


/*   define the parse status table struct */
typedef struct {
  int  DocCount;     /* number of documents encountered so far */
  char DocID[80];    /* current document id number */
  char *CurrField;   /* the name of the field being processed */
  char *Buffer;      /* words and numbers of the document */
}P_REC;

typedef char *STRING;


extern P_REC *pTable;    /* pointer to the parse table */

extern int  position;
extern int  wordPos; 
extern int  fieldCount;
extern int  paragraph;
extern int  sentence;
extern char stemming[10], stopping[10];
extern int  totalWords;
extern int  BufferSize;
extern int  base_term_id;

/* FUNCTION PROTYPES */

void process_end_of_record(void);
void initBuff(void);
void installField(char *fieldStr);
void incrementDocCount(void);
void displayPTable(void);
void updateFieldStats(void);
void processField(void);
void processPError(void);
void addToBuffer(char *tokenStr) ;


#endif
