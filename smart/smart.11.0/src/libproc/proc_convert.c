#ifdef RCSID
static char rcsid[] = "$Header$";
#endif

/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/

#include <ctype.h>
#include "common.h"
#include "param.h"
#include "functions.h"
#include "proc.h"

extern int init_convert(), convert(), close_convert();
static PROC_TAB topproc_convert[] = {
    "convert",  init_convert,       convert,    close_convert,
};
static int num_topproc_convert =
    sizeof(topproc_convert) / sizeof(topproc_convert[0]);

extern int init_vec_inv_obj(), vec_inv_obj(), close_vec_inv_obj();
extern int init_vec_aux_obj(), vec_aux_obj(), close_vec_aux_obj();
extern int init_vecwt_aux_obj(), vecwt_aux_obj(), close_vecwt_aux_obj();
extern int init_text_dict_obj(), text_dict_obj(), close_text_dict_obj();
extern int init_text_rr_obj(), text_rr_obj(), close_text_rr_obj();
extern int init_text_tr_obj(), text_tr_obj(), close_text_tr_obj();
extern int init_text_vec_obj(), text_vec_obj(), close_text_vec_obj();
extern int init_weight_doc(), weight_doc(), close_weight_doc();
extern int init_weight_query(), weight_query(), close_weight_query();
extern int init_tr_rel_obj(), tr_rel_obj(), close_tr_rel_obj();
extern int init_vec_vec_obj(), vec_vec_obj(), close_vec_vec_obj();
extern int init_inv_inv_obj(), inv_inv_obj(), close_inv_inv_obj();
extern int init_oldrr_rrvec(), oldrr_rrvec(), close_oldrr_rrvec();
extern int init_oldtr_trvec(), oldtr_trvec(), close_oldtr_trvec();
extern int init_textloc_textloc_obj(), textloc_textloc_obj(), 
           close_textloc_textloc_obj();
static PROC_TAB proc_obj[] = {
    "vec_inv",  init_vec_inv_obj,  vec_inv_obj,   close_vec_inv_obj,
    "vec_aux",  init_vec_aux_obj,  vec_aux_obj,   close_vec_aux_obj,
    "vecwt_aux",init_vecwt_aux_obj,vecwt_aux_obj, close_vecwt_aux_obj,
    "text_dict",init_text_dict_obj,text_dict_obj, close_text_dict_obj,
    "text_rr",  init_text_rr_obj,  text_rr_obj,   close_text_rr_obj,
    "text_tr",  init_text_tr_obj,  text_tr_obj,   close_text_tr_obj,
    "text_vec", init_text_vec_obj, text_vec_obj,  close_text_vec_obj,
    "weight_doc", init_weight_doc, weight_doc,    close_weight_doc,
    "weight_query", init_weight_query, weight_query,  close_weight_query,
    "tr_rel",   init_tr_rel_obj,   tr_rel_obj,    close_tr_rel_obj,
    "vec_vec",  init_vec_vec_obj,  vec_vec_obj,   close_vec_vec_obj,
    "inv_inv",  init_inv_inv_obj,  inv_inv_obj,   close_inv_inv_obj,
    "oldrr_rrvec",  init_oldrr_rrvec,  oldrr_rrvec,   close_oldrr_rrvec,
    "oldtr_trvec",  init_oldtr_trvec,  oldtr_trvec,   close_oldtr_trvec,
    "textloc_textloc",  init_textloc_textloc_obj,  textloc_textloc_obj,   
                close_textloc_textloc_obj,
    };
static int num_proc_obj = sizeof (proc_obj) / sizeof (proc_obj[0]);



extern int init_vec_inv(), vec_inv(), close_vec_inv();
extern int init_vec_inv_tmp(), vec_inv_tmp(), close_vec_inv_tmp();
extern int init_text_dict(), text_dict(), close_text_dict();
extern int init_pp_veclist(), pp_veclist(), close_pp_veclist();
extern int init_contok_dict(), contok_dict(), close_contok_dict();
extern int init_vec_collstat(), vec_collstat(), close_vec_collstat();
static PROC_TAB proc_tup[] = {
    "vec_inv",          init_vec_inv,   vec_inv,        close_vec_inv,
    "vec_inv_tmp",      init_vec_inv_tmp, vec_inv_tmp,  close_vec_inv_tmp,
    "text_dict",        init_text_dict, text_dict,      close_text_dict,
    "pp_veclist",       init_pp_veclist,pp_veclist,     close_pp_veclist,
    "contok_dict",	init_contok_dict,contok_dict,	close_contok_dict,
    "vec_collstat",	init_vec_collstat,vec_collstat,	close_vec_collstat,
    };
static int num_proc_tup = sizeof (proc_tup) / sizeof (proc_tup[0]);


extern int init_std_weight(), std_weight(), close_std_weight(); 
extern int init_weight_cos(), weight_cos(), close_weight_cos(); 
extern int init_weight_no(), weight_no(), close_weight_no(); 
static PROC_TAB proc_wt[] = {
    "weight",		init_std_weight,std_weight,	close_std_weight, 
    "weight_cos",	init_weight_cos,weight_cos,	close_weight_cos, 
    "weight_no",	init_weight_no,	weight_no,	close_weight_no, 
    };
static int num_proc_wt = sizeof (proc_wt) / sizeof (proc_wt[0]);

extern int init_weight_tri(), weight_tri(), close_weight_tri(); 
static PROC_TAB proc_wt_ctype[] = {
    "weight_tri",init_weight_tri,	weight_tri,	close_weight_tri, 
    "no",		INIT_EMPTY,	EMPTY,		CLOSE_EMPTY, 
    /* Included for backward compatibility. */    
    "weight_sect",init_weight_tri,	weight_tri,	close_weight_tri, 
    };
static int num_proc_wt_ctype = sizeof (proc_wt_ctype) /
         sizeof (proc_wt_ctype[0]);

int tfwt_binary(), tfwt_max(), tfwt_aug(), tfwt_square(), tfwt_log();
static PROC_TAB proc_wt_tf[] = {
    "x",               EMPTY,         EMPTY,         EMPTY,
    "n",               EMPTY,         EMPTY,         EMPTY,
    "b",               EMPTY,         tfwt_binary,   EMPTY,
    "m",               EMPTY,         tfwt_max,      EMPTY,
    "a",               EMPTY,         tfwt_aug,      EMPTY,
    "s",               EMPTY,         tfwt_square,   EMPTY,
    "l",               EMPTY,         tfwt_log,      EMPTY,
};
static int num_proc_wt_tf = sizeof (proc_wt_tf) / sizeof (proc_wt_tf[0]);

int init_idfwt_idf(), idfwt_idf(), close_idfwt_idf();
int init_idfwt_prob(), idfwt_prob(), close_idfwt_prob();
int init_idfwt_s_idf(), idfwt_s_idf(), close_idfwt_s_idf();
int init_idfwt_freq(), idfwt_freq(), close_idfwt_freq();
int init_idfwt_avphrase(), idfwt_avphrase(), close_idfwt_avphrase();
static PROC_TAB proc_wt_idf[] = {
    "x",               EMPTY,         EMPTY,         EMPTY,
    "n",               EMPTY,         EMPTY,         EMPTY,
    "t",        init_idfwt_idf,		idfwt_idf,     close_idfwt_idf,
    "i",        init_idfwt_idf,		idfwt_idf,     close_idfwt_idf,
    "p",        init_idfwt_prob,	idfwt_prob,    close_idfwt_prob,
    "s",        init_idfwt_s_idf,	idfwt_s_idf,   close_idfwt_s_idf,
    "f",        init_idfwt_freq,	idfwt_freq,    close_idfwt_freq,
    "A",        init_idfwt_avphrase,	idfwt_avphrase,close_idfwt_avphrase,
};
static int num_proc_wt_idf = sizeof (proc_wt_idf) / sizeof (proc_wt_idf[0]);

int normwt_cos(), normwt_sum(), normwt_cos_use(), normwt_cos_store();
static PROC_TAB proc_wt_norm[] = {
    "x",               EMPTY,         EMPTY,         EMPTY,
    "n",               EMPTY,         EMPTY,         EMPTY,
    "c",               EMPTY,         normwt_cos,    EMPTY,
    "s",               EMPTY,         normwt_sum,    EMPTY,
/* C is same as c, except stores norm factor */
    "C",               EMPTY,         normwt_cos_store, EMPTY,
/* P is same as c, except uses stored norm factor from last C ctype */
/* eg. if want phrase ctype with same norm factor as regular tokens */
/* Assumes phrase ctype is > regular ctype, so that C is called first */
    "P",               EMPTY,         normwt_cos_use, EMPTY,
};
static int num_proc_wt_norm = sizeof (proc_wt_norm) / sizeof (proc_wt_norm[0]);


TAB_PROC_TAB proc_convert[] = {
    "convert",  TPT_PROC, NULL, topproc_convert, &num_topproc_convert,
    "obj",	TPT_PROC, NULL,	proc_obj,    	&num_proc_obj,
    "tup",	TPT_PROC, NULL,	proc_tup,    	&num_proc_tup,
    "weight",	TPT_PROC, NULL,	proc_wt, 	&num_proc_wt,
    "wt_ctype",	TPT_PROC, NULL,	proc_wt_ctype, 	&num_proc_wt_ctype,
    "wt_tf",	TPT_PROC, NULL,	proc_wt_tf, 	&num_proc_wt_tf,
    "wt_idf",	TPT_PROC, NULL,	proc_wt_idf, 	&num_proc_wt_idf,
    "wt_norm",	TPT_PROC, NULL,	proc_wt_norm, 	&num_proc_wt_norm,
    };
int num_proc_convert = sizeof (proc_convert) / sizeof (proc_convert[0]);
