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


/* Modifications:
     10/13/92 -- Added rcsid[] for record keeping.
       AEG
     10/29/92 -- Added function headers for allowing the user to change
        AEG      databases within inquery.
     ================================================
     10/29/92 -- This file was renamed from xui.h AEG
     ================================================
      4/16/93 -- Added add_rel_doc(), find_rel_doc(), remove_rel_doc() AEG
*/

#ifndef _XUI_HEADER
#define _XUI_HEADER

static Char_t ui_utils_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

/*=== Macros, Variables and Constants used in all user interfaces. ===*/

#undef MAXLEN			/* Watcom is fussy about redefining macros */
#define MAXLEN 256		/* max length of a line in unix */
#define  JAPANESE_SEPCHARS	" \n\t\f\r"
#define  ENGLISH_SEPCHARS 	" .,?:;)(\"\n\t\f\r"


#include "inq_api.h"
extern dbinfo *db;
extern belieflst *bl;

extern Int_t numqwords;
extern Char_t *newquery;

extern doc_rel_t rel_docs[MAX_DOCS_JUDGED];
extern Int_t  rel_docs_size;

extern BoolInt_t switch_remove_duplicate_docs;
extern Char_t logname[];
extern FILE  *logfile;
extern Char_t doc_out_file_name[];
extern FILE  *doc_out_file_ptr;

/*=== Functions used in all user interfaces. ===*/
extern void    db_main(int argc, char **argv);
extern dbinfo *process_commandline (int argc, char **argv);
extern void    remove_duplicate_docs (Int_t minimum_unique);
extern Boolean_t change_dbs(Char_t *new_db);
extern Int_t   check_db(Char_t *new_db);
extern Int_t   tokenize_query(Char_t *qstring);

extern Int_t   blank(Char_t *s);
extern Char_t *rmv_blanks(Char_t *s);
extern Char_t *cr_to_sp(Char_t *s, Int_t check_space);

extern Char_t *ui_lookup_doc_field (Int_t field);
extern void    ui_get_doc (dbinfo *db, Int_t doc_id, Int_t mode);
Int_t ui_get_doc_nummatches(void);
matchinfo_t *ui_get_doc_matches(void);

extern Int_t add_rel_doc(Int_t doc_id, Int_t relevant);
extern Int_t remove_rel_doc(Int_t doc_id);
extern Int_t find_rel_doc(Int_t doc_id);


#endif
/* Nothing below this line. */
