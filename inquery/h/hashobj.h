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


#if !defined(HASHOBJ_H)
#define HASHOBJ_H

/* ------------------------------- Inclusions ------------------------------ */
#include <stdio.h>
#include "byteorder.h"
#include "docdef.h"
#include "doctypes.h"

/* --------------------------------- Macros -------------------------------- */
#define HO_FNAMESIZE (MAX_FILE_SPEC_LENGTH + 1)


/* -----------------------------  Declarations ------------------------------*/

static char hashobj_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

typedef struct            /* hash table entry */
    {
    Int_t id;             /* "value"; can be anything except FAIL (= -1) */
    Int_t string;         /* "key"; offset of string in string space */
    } hte;

typedef struct            /* hash table object: per-object variables */
    {
    char name[HO_FNAMESIZE]; /* filename of hashobj on disk */
    hte *htable;          /* base of table, which is an array of htes */
    Int_t tblsize;        /* number of entries in hash table */
    Int_t used;           /* number of entries used */
    Int_t hashdvsr2;      /* divisor for second hash function */
    char *stringspc;      /* base of the string space */
    Int_t stringspcsize;  /* size of the string space */
    Int_t stringtop;      /* offs of next available location in string space */
    Int_t hvalue;         /* temp used by hash functions */
    Int_t lastkey;        /* key of an empty entry, from last lookup */
    } hashobj;


/* ------------------------- Function Declarations --------------------------*/

hashobj *get_hashobj(Int_t);
hashobj *read_hashobj(Int_t, char *);
Int_t ho_find_ent(hashobj *t, char *s);
Int_t ho_lookup(hashobj *, char *);
Int_t ho_insert(hashobj *, char *, Int_t);
Int_t set_size_hashobj(hashobj *, double);
void free_hashobj(hashobj *);
Int_t write_hashobj(hashobj *);
char *id_translate(hashobj *, Int_t);
double ho_load_factor(void);
Int_t  ho_size(hashobj *);
Int_t  ho_used(hashobj *);

/* The following macros read and write structures to and from a stream.
Since they use the machinetofile() macros, they modify their argument.
These macros evaluate to 0 if there is no error, non-zero otherwise.
The first argument is the name of a variable of the correct type, and
the second is a stream pointer.

The associated DISK_IMAGE_SIZE constants give the size of the written
object on disk.  These should be used for computing file offsets,
rather than sizeof(type).
*/

#define write_ho(ho, file)                               \
(                                                        \
    machinetofile4((ho).tblsize),                        \
    machinetofile4((ho).used),                           \
    machinetofile4((ho).hashdvsr2),                      \
    machinetofile4((ho).stringspcsize),                  \
    machinetofile4((ho).stringtop),                      \
    fwrite(&(ho).name,      HO_FNAMESIZE,  1, file),     \
    fwrite(&(ho).tblsize,   sizeof(int),   1, file),     \
    fwrite(&(ho).used,      sizeof(int),   1, file),     \
    fwrite(&(ho).hashdvsr2, sizeof(int),   1, file),     \
    fwrite(&(ho).stringspcsize, sizeof(int),   1, file), \
    fwrite(&(ho).stringtop, sizeof(int),   1, file),     \
    ferror(file)                                         \
)
#define HO_DISK_IMAGE_SIZE (HO_FNAMESIZE + (5 * sizeof(int)))

#define read_ho(ho, file)                                \
(                                                        \
    fread(&(ho).name,      HO_FNAMESIZE,  1, file),      \
    fread(&(ho).tblsize,       sizeof(int),   1, file),  \
    fread(&(ho).used,          sizeof(int),   1, file),  \
    fread(&(ho).hashdvsr2,     sizeof(int),   1, file),  \
    fread(&(ho).stringspcsize, sizeof(int),   1, file),  \
    fread(&(ho).stringtop,     sizeof(int),   1, file),  \
    filetomachine4((ho).tblsize),                        \
    filetomachine4((ho).used),                           \
    filetomachine4((ho).hashdvsr2),                      \
    filetomachine4((ho).stringspcsize),                  \
    filetomachine4((ho).stringtop),                      \
    ferror(file)                                         \
)

#define write_htent(ent, file)                       \
(                                                    \
    machinetofile4((ent).id),                        \
    machinetofile4((ent).string),                    \
    fwrite(&(ent).id,   sizeof(int),   1, file),     \
    fwrite(&(ent).string,   sizeof(int),   1, file)  \
)
#define HTENT_DISK_IMAGE_SIZE (2 * sizeof(int))

#define read_htent(ent, file)                        \
(                                                    \
    fread(&(ent).id,     sizeof(int),   1, file),    \
    fread(&(ent).string, sizeof(int),   1, file),    \
    filetomachine4((ent).id),                        \
    filetomachine4((ent).string)                     \
)

#endif

