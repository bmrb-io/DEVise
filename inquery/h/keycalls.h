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


#if !defined(KEYCALLS_H)
#define KEYCALLS_H

#include <stdio.h>

/* select appropriate function calls */
#define CREATE_KEY create_key
#define OPEN_KEY   open_key
#define CLOSE_KEY  close_key
#define GET_REC    get_rec
#define PUT_REC    put_rec
#define FLUSH_KEY  flush_key
#define REINIT_KEY reinit_key

/* changed void *rec (common third arg) to char *rec */
/* define appropriate functions */
Char_t *version_key();
void  key_rewind(Char_t *fcb);
Int_t get_ptr   (Char_t *fcb, Char_t *key, struct pointer *p);
Int_t put_ptr   (Char_t *fcb, Char_t *key, struct pointer *p);
Int_t delete_ptr(Char_t *fcb, Char_t *key);
Int_t next_ptr  (Char_t *fcb, Char_t *key, struct pointer *p);
Int_t prev_ptr  (Char_t *fcb, Char_t *key, struct pointer *p);
FILE  *get_fcb_FILE (Char_t *f);
Int_t get_rec   (Char_t *fcb, Char_t *key, Char_t *rec, Int_t *lc, Int_t max_lc);
Int_t put_rec   (Char_t *fcb, Char_t *key, Char_t *rec, Int_t lc);
Int_t put_new_rec (Char_t *fcb, Char_t *key, Char_t *rec, Int_t lc);
Int_t delete_rec(Char_t *fcb, Char_t *key);
Int_t next_rec  (Char_t *fcb, Char_t *key, Char_t *rec, Int_t *lc, Int_t max_lc);
Int_t prev_rec  (Char_t *fcb, Char_t *key, Char_t *rec, Int_t *lc, Int_t max_lc);
Int_t open_key  (Char_t *fcb, Char_t *id, Int_t fcb_lc, Int_t mode, Int_t open_mode);
Int_t reinit_key(Char_t *fcb,FILE *key_file, Int_t lc, Int_t insert_mode, Int_t open_as);
Int_t create_key(Char_t *fcb, Char_t *id, Int_t fcb_lc, Int_t mode, Boolean_t use_multi_file);
Int_t close_key (Char_t *fcb, Int_t open_mode);
Int_t flush_key (Char_t *fcb, Int_t open_mode);
void  log_key_tree(Char_t *fcb,FILE *,Char_t[]);
void restore_key_tree(Char_t *fcb ,FILE *logfile);
BoolInt_t read_fib(FILE *, Char_t *fcb);
void key_set_provide_recovery(Char_t *f, Boolean_t value);
Int_t key_set_max_size(Char_t *fcb, int new_size);
Int_t reset_current_file(Char_t *fcb , Char_t id[] );
Int_t remove_record_files(Char_t *fcb , Char_t id[] );
BoolInt_t check_keyfile_ok(Char_t *f);
void print_key_error(int status);




/* this prototype is here mainly to avoid compiler warnings about
   implicit declarations and because of the brain dead organization of
   key file.  Do not use this function unless you know what it does */
Int_t mf_get_file_pointer(Char_t *f, Int_t file_id, FILE **fp);


static Char_t keycalls_h_rcsid[]="$RCSfile$, $Revision$, $Date$";



#endif



