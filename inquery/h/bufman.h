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

#ifndef _BUFMAN_H_
#define _BUFMAN_H_

#include <stdio.h>
#include <stdlib.h>

#include "doctypes.h"          /* for inquery data types */


#define BUFMAN_NUM_TYPES 11
#define BUFMAN_NUM_MODES 2

#define BUFMAN_MODE_RETRIEVAL 0
#define BUFMAN_MODE_INDEXING  1


#define BUFMAN_KB ((Float_t)1024.0)
#define BUFMAN_MB ((Float_t)(1024.0 * 1024.0))

#define BUFMAN_DEFAULT_SIZE   10.0      /* default memory usage in MB */
#define BUFMAN_MIN_MEMORY      1.0      /* minimum memory req'd in MB */

#define BUFMAN_KEY_SUBREC_SIZE 		0
#define BUFMAN_FB_TERMSTORE		1     /* relev. feedback termstore */
#define BUFMAN_FB_OBJLIST		2     /* relev. feedback objlist   */
#define BUFMAN_GTR_DOC_BUFFER		3     /* gentrans DocBufBytes       */
#define BUFMAN_GTR_POST_BUFFER          4     /* gentrans PostBufBytes      */
#define BUFMAN_MFTC_BUFFER		5     /* mft cache hashtable        */
#define BUFMAN_STEM_DICT		6     /* stem dictionary (kstem)   */
#define BUFMAN_STEM_HTSIZE		7     /* stemmer size              */
#define BUFMAN_INDEX_TABLE		8     /* ndx_man index table        */
#define BUFMAN_DM_IO_BUFFER		9     /* indexing I/O buffer        */
#define BUFMAN_MERGE_BTL                10    /* merging bufsize            */


Int_t bufman_init(Float_t new_size_in_MB, Int_t mode);
Int_t bufman_get_buf_size(Int_t type);
Int_t bufman_release_buf_size (Int_t type);

#endif /* _BUFMAN_ */
