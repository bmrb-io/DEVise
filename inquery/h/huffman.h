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



#include "huffdict.h"

#define MAX_TOKEN_LENGTH 3		/* Max length of token, not inc. NULL */
#define MAX_CODE_LENGTH 32		/* Length of code patterns in bits */
#define MAX_DIGITS 4			/* Max number of numerals in a token */
#define HUFF_END_DOC_TOK "\xff\x00"	/* The 1 byte string, <255><NULL> */
#define MAX_FILENAME 128


/***********/
/* Defines */
/***********/

#define BUFSIZE (64 * 1024)


/**************/
/* Structures */
/**************/

typedef Char_t token_t[MAX_TOKEN_LENGTH + 1];

typedef struct
{
Int_t number_of_codes[MAX_CODE_LENGTH];  /* number of bit patterns of each length */
Int_t first_codes[MAX_CODE_LENGTH];      /* first pattern for each length */
hashdict *hd;
token_t *tokens;        /* pointer to array of chars (the tokens) */
token_t *symbols;       /* the same tokens in sorted order */
Int_t *tok_freq_id;     /* original index, term freq, and hashdict id */
Char_t name[MAX_FILENAME];
Char_t name2[MAX_FILENAME];
Int_t magic_num;
} huffdict;


/***********************/
/* Function prototypes */
/***********************/

/*****************/
/* API functions */
/*****************/

/* First Compression Pass */

Int_t huff_init_tokenizing(Char_t *hd_file_name, Char_t *code_file_name,
                           void **hfd);

Int_t huff_tokenize(Char_t *doc, Char_t *end, huffdict *hfd);

Int_t huff_term_tokenizing(huffdict *hfd);



/* Second Compression Pass */

Int_t huff_init_compression(Char_t *hd_file_name, void **hfd);

Int_t huff_toks_to_codes(huffdict *hfd);

Int_t huff_compress(Char_t *doc, Char_t *end, Char_t *comp_text,
                    Int_t *comp_length, huffdict *hfd);

Int_t huff_term_compression(huffdict *hfd);



/* Decompress */

Int_t huff_init_decompression(Char_t *hd_file_name, Char_t *code_file_name,
                            void **hfd);

Int_t huff_decompress(Char_t *doc, Int_t *length, Char_t *comp_text,
                      Int_t comp_length, huffdict *hfd);

Int_t huff_term_decompression(void *hfd_ptr);

