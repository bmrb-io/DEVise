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

#if !defined(HASHDICT_H)
#define HASHDICT_H

/* ------------------------------- Inclusions ------------------------------ */
#include <stdio.h>
#include "byteorder.h"
#include "doctypes.h"
#include "docdef.h"

/* --------------------------------- Macros -------------------------------- */
#define HD_FNAMESIZE (MAX_FILE_SPEC_LENGTH + 1)
#define TD_HASHDVSR (8191)  /* see hashdict.c for explanation of this value */
#define EMPTY (-1)          /* empty hash slot */
                            /* not 0, because that is a valid offset value */


/* -----------------------------  Declarations ------------------------------*/

static char hashdict_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

typedef enum {resident, cached} load_status;

typedef struct
    {
    Int_t freq;          /* term's collection frequency */
    Int_t code;          /* huffman code for this token as an integer */
    Int_t length;     /* length of this code in bits */
    Int_t string;        /* offset of string in string space */
    } slot_entry;

typedef union
    {    
    Int_t offs;          /* offset of slot in file */
    slot_entry *ptr;     /* ptr to slot in memory */
    } slot_ptr;

typedef struct
    {
    Int_t length;        /* number of entries in slot */
    slot_ptr u;          /* offset of slot in file or ptr to slot in memory */
    } ht_entry;

typedef struct           /* linear-chain hash object: per-object variables */
    {
    char name[HD_FNAMESIZE];  /* filename of hashdict on disk */
    ht_entry *htable;    /* base of table, which is an array of htes */
    Int_t total_entries; /* number of terms in the dictionary */
    char *stringspc;     /* base of the string space */
    Int_t stringtop;     /* offs of next available location in string space */
    Int_t stringspcsize; /* size of string space */
    Int_t lastslot;      /* for insert: slot of most recent lookup */
    Int_t modified;      /* true if dictionary has been updated */
    Int_t current_slot;  /* for caching: id of currently-cached slot */
    } hashdict;


/* The following macros read and write particular structures to and
from a stream.  Since they use the machinetofile() macros, they modify
their argument.  These macros evaluate to 0 if there is no error,
non-zero otherwise.  The first argument is the name of a variable of
the correct type, and the second is a stream pointer.  

The associated DISK_IMAGE_SIZE constants give the size of the written
object on disk.  These should be used for computing file offsets,
rather than sizeof(type).
*/

#define write_hashd(hd, file)                            \
(                                                        \
    machinetofile4((hd).total_entries),                  \
    machinetofile4((hd).stringtop),                      \
    machinetofile4((hd).stringspcsize),                  \
    fwrite(&(hd).name,          HD_FNAMESIZE,  1, file), \
    fwrite(&(hd).total_entries, sizeof(int),   1, file), \
    fwrite(&(hd).stringtop,     sizeof(int),   1, file), \
    fwrite(&(hd).stringspcsize, sizeof(int),   1, file), \
    ferror(file)                                         \
)
#define HD_DISK_IMAGE_SIZE (HD_FNAMESIZE + (3 * sizeof(int)))

#define read_hashd(hd, file)                             \
(                                                        \
    fread(&(hd).name,          HD_FNAMESIZE,  1, file),  \
    fread(&(hd).total_entries, sizeof(int),   1, file),  \
    fread(&(hd).stringtop,     sizeof(int),   1, file),  \
    fread(&(hd).stringspcsize, sizeof(int),   1, file),  \
    filetomachine4((hd).total_entries),                  \
    filetomachine4((hd).stringtop),                      \
    filetomachine4((hd).stringspcsize),                  \
    ferror(file)                                         \
)


#define write_hte(ent, file)                             \
(                                                        \
    machinetofile4((ent).length),                        \
    machinetofile4((ent).u.offs),                        \
    fwrite(&(ent).length, sizeof(int), 1, file),         \
    fwrite(&(ent).u.offs, sizeof(int), 1, file),         \
    ferror(file)                                         \
)
#define HTE_DISK_IMAGE_SIZE (2 * sizeof(int))

#define read_hte(ent, file)                              \
(                                                        \
    fread(&(ent).length, sizeof(int), 1, file),          \
    fread(&(ent).u.offs, sizeof(int), 1, file),          \
    filetomachine4((ent).length),                        \
    filetomachine4((ent).u.offs),                        \
    ferror(file)                                         \
)

#define write_slot_entry(ent, file)                      \
(                                                        \
    machinetofile4((ent).freq),                          \
    machinetofile4((ent).code),                       \
    machinetofile4((ent).length),                     \
    machinetofile4((ent).string),                        \
    fwrite(&(ent).freq,      sizeof(int), 1, file),      \
    fwrite(&(ent).code,   sizeof(int), 1, file),      \
    fwrite(&(ent).length, sizeof(int), 1, file),      \
    fwrite(&(ent).string,    sizeof(int), 1, file),      \
    ferror(file)                                         \
)
#define SLENT_DISK_IMAGE_SIZE (4 * sizeof(int))

#define read_slot_entry(ent, file)                       \
(                                                        \
    fread(&(ent).freq,      sizeof(int), 1, file),       \
    fread(&(ent).code,   sizeof(int), 1, file),       \
    fread(&(ent).length, sizeof(int), 1, file),       \
    fread(&(ent).string,    sizeof(int), 1, file),       \
    filetomachine4((ent).freq),                          \
    filetomachine4((ent).code),                       \
    filetomachine4((ent).length),                     \
    filetomachine4((ent).string),                        \
    ferror(file)                                         \
)


/* sample usage
    Use "hashval" rather than "slot" to mean h(k), that is, hash("foo").
    Use "slot" to refer to the data object that is the array of slot_entries.

    slot = table[hashval].u.ptr;
    slotlen = table[hashval].u.length;
                                         // add entry at end of slot
    slot[slotlen].freq = 1;
    slot[slotlen].list_offs = junk;
    p = stralloc(s);
    slot[slotlen++].string = p;
    
    if (match(s, stringspc + slot[j].string))...

*/

extern Int_t hashd_error;
extern char hd_errmsg[];

/* ---------------------------------------------------------------------------
For convenience, a copy of the list of error messages.  See hashdict.c for
the original.

    "there is no error 0",                                    0 
    "Error allocating hash table memory",                     1 
    "Failed to create %s for open_hashdict",                  2 
    "Error allocating hash table memory",                     3 
    "Open couldn't make room for slot %d",                    4 
    "Error reading slot %d",                                  5 
    "Error allocating string space",                          6 
    "Error reading string space for %s",                      7 
    "Failed to open %s for write_hashdict",                   8 
    "Error writing string space for %s",                      9 
    "Error writing slot %d",                                 10 
    "Error writing slot %d",                                 11 
    "Error writing hashtbl",                                 12 
    "Couldn't make room for entry %d (%s) in slot %d",       13 
    "Couldn't allocate storage for %s in term dict",         14 
    "Failed to allocate duplicate string space",             15 
    "Warning: copy doesn't match original! %d, %d",          16 
    "Ran out of string space"                                17 
    "error reading hashdict structure",                      18
    "error reading hash table"                               19
    "Tried to write out a non-resident dictionary",          20
    "Called hd_count_term for a non-resident dictionary",    21
    "Called hd_count_doc for a non-resident dictionary",     22
    "Called hd_ insert for a non-resident dictionary",       23
    "Failed to (re)alloc stringspc or slot",                 24
    "Failed to read slot %d",                                25
    "Failed to realloc stringspc for slot %d",               26
    "Failed to read strings for slot %d",                    27
---------------------------------------------------------------------------- */


/* ------------------------- Function Declarations --------------------------*/

hashdict *get_hashdict(char *name);
void free_hashdict(hashdict *t);
hashdict *open_hashdict(char *name, load_status ls);
Int_t write_hashdict(hashdict *t);
Int_t hd_lookup(hashdict *t, char *s);
Int_t hd_insert(hashdict *t, char *s);
char *id_to_term(hashdict *t, Int_t);
Int_t hd_next_id(hashdict *t, Int_t prev_id);
void hd_count_doc(hashdict *t, Int_t id);
void hd_count_term(hashdict *t, Int_t id);
Int_t hd_term_freq(hashdict *t, Int_t id);
Int_t hd_term_docfreq(hashdict *t, Int_t id);
Int_t hd_valid_id(hashdict *t, Int_t id);


#endif
