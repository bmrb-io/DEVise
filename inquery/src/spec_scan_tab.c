/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/

/*
 * SPEC_SCAN_TAB.C
 * 
 * This is where the modular scanner table is kept.  The scanners listed
 * here will be those used in a specific parser.  Thus to make a new
 * parser with a new set of special scanners (or possibly a new 
 * parser-scanner) one needs to only edit this file and link the new
 * scanners with the rest of the parsing objects.
 */

#include <stdio.h>

#include "doctypes.h"
#include "parse_sys.h"


static char spec_scan_table_c_rcsid[]="$RCSfile$, $Revision$, $Date$";

/* Init functions prototypes: 
 *
 * Before we get to the table, however,
 * the init functions must be declared.
 *
 * Be sure to provide an init function declaration if desired.
 */

extern Int_t scan_sentence(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_tagger(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_tagger_handler(void *ic, Int_t action);
extern Int_t scan_company(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_person(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_number(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_date(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_location(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_city(void *ic, Char_t *text, Int_t length, TokenList *tok_table);
extern Int_t scan_acronym(void *ic, Char_t *text, Int_t length, TokenList *tok_table);

/* The Scanner Table:
 *
 * Here is where the first four fields of the scanner table are
 * initialized.  Each entry consists of the following:
 * the scanner name,
 * the initialization function for the scanner (see above),
 * the on/off flag,
 * and the scanner type.
 *
 * The scanner is used if the on/off flag is 1,
 * it is not used by default if the flag is 0.
 *
 * The scanner types consist of the following:
 *
 *     SPECIAL_SCANNER -- Special concept recognizors.  These must
 *                        be listed first in the table.
 *
 * The table must be terminated with an entry which contains a
 * NULL in the scanner name slot.
 */

ScanInfo_t default_scanner[]=
{      
#ifndef INROUTE
     {"scan_sentence",	scan_sentence, NULL,                 0},
     {"scan_tagger",	scan_tagger,   scan_tagger_handler,  0},
     {"scan_company",	scan_company,  NULL,                 1},
     {"scan_date",	scan_date,     NULL,                 1},
     {"scan_location",	scan_location, NULL,                 1},
     {"scan_city",	scan_city,     NULL,                 1},
     /*     {"scan_person",	scan_person,   NULL,                 0}, */
#endif /* INROUTE */
     { NULL, NULL, NULL, 0}
};


