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
 * "DOCMEM_H" is a header file for declarations and macros for 
 * use with docmem.c.
 *
 *  Created:  Fall, 1991
 *
 *  1/2/92  JC  Added general, generic, types.
 */

#if !defined(DOCMEM_H)
#define DOCMEM_H

#ifndef MAX
#include "docdef.h"
#endif

#if defined(__MWERKS__)
#  include <StdDef.h>
#else
#  include <sys/types.h>  /* For size_t */
#endif

#include "doctypes.h"


static char docmem_h_rcsid[]="$RCSfile$, $Revision$, $Date$";


/*
 *  Types:
 */

  /*
   *  This is what vectors *really* look like.  It would be better if users
   *  couldn't see this, but the macro VECTOR_MAKE_ROOM_FOR_N needs to have
   *  it visible.  The dmem_vector_header_t must contain exactly the same
   *  set of fields as dmem_vector_t, *except* for the contents field and
   *  any fields that follow the contents field.  This makes it easy for
   *  the macros to take a pointer to the contents field and back up to
   *  find the beginning of the dmem_vector_t.
   */
  typedef struct
    {
    Int_t magic_num;		/* Must be set to MAGIC_NUM */
    Int_t length;
    } dmem_vector_header_t;	/* KEEP IN SYNCH WITH dmem_vector_t */

  typedef struct
    {
    Int_t magic_num;		/* Must be set to MAGIC_NUM */
    Int_t length;
    char   contents[1];
    } dmem_vector_t;		/* KEEP IN SYNCH WITH dmem_vector_t */

/*
 *  Macros:
 */

  /*
   *  VECTOR_MAKE_ROOM_FOR_N ensures that there is room in the specified
   *  vector for elt_count elements of elt_size.  It returns a boolean value.
   *  This macro is intended particularly for situations in which one expects
   *  the vector to remain relatively small.
   *  Paramters:
   *    void     **vector_ptr:  Points to the vector (can be NULL).
   *    long int elt_count:     Desired minimum length, in number of elements.
   *    long int elt_size:      Size of each element, in bytes.
   */
#define vector_make_room_for_n(vector_ptr, elt_count, elt_size) \
  ((*vector_ptr == NULL) ? \
   vector_create ((void **) vector_ptr, elt_count, elt_size) : \
   (elt_count > ((dmem_vector_t *) (((char *) *vector_ptr) - \
				    sizeof (dmem_vector_header_t)))->length) ?\
   vector_enlarge_to_n ((void **) vector_ptr, elt_count, elt_size) : \
   ((dmem_vector_t *) (((char *) *vector_ptr) - \
		       sizeof (dmem_vector_header_t)))->length)
  
  /*
   *  VECTOR_MAKE_ROOM_FOR_LARGE_N ensures that there is room in the specified
   *  vector for elt_count elements of elt_size.  It returns a boolean value.
   *  This macro is intended particularly for situations in which one expects
   *  the vector to be, or become, large.
   *  Paramters:
   *    void     **vector_ptr:  Points to the vector (can be NULL).
   *    long int elt_count:     Desired minimum length, in number of elements.
   *    long int elt_size:      Size of each element, in bytes.
   */
#define vector_make_room_for_large_n(vector_ptr, elt_count, elt_size) \
  ((*vector_ptr == NULL) ? \
   vector_create ((void **) vector_ptr, (MAX (elt_count, 128)), elt_size) : \
   (elt_count > ((dmem_vector_t *) (((char *) *vector_ptr) - \
				    sizeof (dmem_vector_header_t)))->length) ?\
   vector_enlarge_to_n ((void **) vector_ptr, elt_count, elt_size) : \
   ((dmem_vector_t *) (((char *) *vector_ptr) - \
		       sizeof (dmem_vector_header_t)))->length)
  
/*
 *  External Routines:
 */
void *IN_malloc(size_t size);
Boolean_t IN_free(void *ptr);
Int_t  vector_create(void **vector_ptr, Int_t elt_count, Int_t elt_size);
Int_t  vector_enlarge_to_n(void **vector_ptr, Int_t elt_count, Int_t elt_size);
Boolean_t  vector_free(void **ptr);
Boolean_t free_parsed_query(Char_t *query);
#endif
