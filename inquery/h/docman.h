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
 * "DOCMAN_H" is a header file for the document manager
 */

#if !defined(DOCMAN_H)
#define DOCMAN_H

/* DOCMAN.H
 *
 * This file defines the interface to the Document Manager including
 * the datatypes and the functions involved.
 */

/********************/
/*   INCLUDES       */
/********************/

#include <stdio.h>
#include "doctypes.h"
#include "docdef.h"
#include "inq_err.h"
#include "dm_pub.h"

/********************/
/* TYPE DEFINITIONS */
/********************/

struct collection
{
    Char_t    *name;      /*  The unique name of the collection  */
    Char_t    *owner;     /* The "owner" of the collection.     */
    Int_t      id;        /* Collection Id. */
    int        type;      /* The type of documents in this collection. */
    DocFormat_t *fields;  /* Description of valid fields in this collection. */
    Boolean_t  fields_alloced; /* is this a static field table? */
    Char_t    *data_ident; /* Identification of the raw data */
    FILE      *data_fp;   /* Pointer to the collection data */
    Int_t      status;
    Int_t      access;
    Int_t      start_doc;
    Int_t      num_docs;
    Int_t      compression;
    Int_t      magic_num;
};


/*  INTERNAL */
typedef struct collection Collection_t;
	
/*  EXTERNAL */
typedef void *in_Collection; 


/* The document manager data structure */
struct docman_info
{
    Char_t        dbfile_name[MAX_FILENAME];
    void         *dbfile_handle;
    void         *hfd_handle;		/* FKL */
    Int_t         doc_cnt;
    Int_t         num_types;
    DocFormat_t  *doc_types;
    Int_t         num_coll;
    Int_t         num_open_coll;
    in_Collection *coll_tab;
    Float_t       version;
    Int_t         magic_num;
};

typedef struct docman_info DocManInfo_t;
typedef void *DocmanInfo;

struct byte_sequence
{
    void *bytes;
    Int_t length;
};

typedef struct byte_sequence in_ByteSequence; 

struct doc_span
{
    Int_t  offset;
    Int_t  length;
};

typedef struct doc_span DocSpan;

struct field_ent
{
    struct doc *doc;
    Int_t       type;
    Int_t       num_spans;
    DocSpan    *spans;
};

typedef struct field_ent FieldEnt;

struct doc_field_tab
{
    struct doc *doc;
    Int_t      num_fields;
    FieldEnt **field_list;
    Int_t      magic_num;
};

typedef struct doc_field_tab DocFieldTab_t;
typedef void *DocFieldTab;

struct doc 
{
    in_Collection parent;
    /* each document "belongs" to at most one persistent collection. */

    Boolean_t parent_alloced;
    /*  The parent is allocated in the client, we need a flag to free it */

    Int_t doc_id;
    /* The internal Document ID of the document. */

    Char_t *ExternalId;
    /* The external ID of the document.  This is the universal
     * mechanism for referring to the document. */

    in_ByteSequence raw_data;
    /* The actual raw text of the document */
    Boolean_t data_alloced;
    /* True if the raw data was allocated for this structure */
	
    Int_t file_offset;   /* Offset into the datafile */
    Int_t length;        /* Length of document in bytes */
    
    DocFieldTab doc_field_tab;
    /* The table of fields for this document. */

    Int_t magic_num;
};

/* INTERNAL */
typedef struct doc Document_t;

/* EXTERNAL */
typedef void *in_Document; 

/***********************/
/*  MACRO DEFINITIONS  */
/***********************/

/* Every data type which passed the DM interface boundary is encapsulated
 * via casting to a void pointer.  To ensure the validity of these data
 * types when passed back through the interface we use magic numbers and
 * provide convenient type checkers in the macros below.
 */

#define INQ_COLL_MAGIC_NUM       546372819
#define INQ_DOC_MAGIC_NUM        609478362
#define INQ_DFT_MAGIC_NUM        938472017
#define INQ_DOCMAN_MAGIC_NUM     285920478
#define INQ_HUFFDICT_MAGIC_NUM   831511612  /* FKL */

#define DM_VALID_COLLECTION(col)  \
        (col != NULL && col->magic_num == INQ_COLL_MAGIC_NUM)
#define DM_VALID_DOCUMENT(doc)   \
        (doc != NULL && doc->magic_num == INQ_DOC_MAGIC_NUM)
#define DM_VALID_DOC_FORMAT(df) ( df != (DocFormat_t *)NULL)
#define DM_VALID_DFT(dft)   \
     (dft != NULL && dft->magic_num == INQ_DFT_MAGIC_NUM )
#define DM_VALID_HFD(hfd)   \
     (hfd != NULL && hfd->magic_num == INQ_HUFFDICT_MAGIC_NUM )

#define DM_VALID_DOCMAN(dmi)  \
        (dmi != NULL && dmi->magic_num == INQ_DOCMAN_MAGIC_NUM)

/* Status (error) codes returned by the Document Manager */

/* Collection status codes */
#define INQ_COLL_CLOSED 0
#define INQ_COLL_OPEN 1

/* Maximum number of open collections (files) */
#define INQ_MAX_OPEN_COLL 10

/* Database access modes */
#define DM_READ_ONLY       1
#define DM_READ_WRITE      2
#define DM_CREATE          4
#define DM_EDIT_ONLY       8  /* To alter collection info but add no docs */
#define DM_PROVIDE_RECOVERY 64

/* Other usefull numbers */
#define KBYTE 1024
#define INQ_DM_DEF_BUFF_SIZE (500 * KBYTE)
#define INITIAL_COLLTABLE_ENTRIES 10
#define DBFILE_FIXED_SIZE (250 * KBYTE)

/***********************/
/* FUNCTION PROTOTYPES */
/***********************/

#if (defined(__MWERKS__) && defined(__cplusplus))
extern "C" {
#endif

/* DATA TYPE ACCESS FUNCTIONS */

/* Byte Sequence Functions: */
in_ByteSequence byte_sequence_create(void *data, Int_t length, in_ByteSequence *bs);
Int_t byte_sequence_destroy(in_ByteSequence *bs);
void *byte_sequence_data(in_ByteSequence bs);
Int_t byte_sequence_length(in_ByteSequence bs);
Int_t byte_sequence_convert_to_string(in_ByteSequence bs, Char_t **string);
Int_t string_convert_to_byte_sequence(Char_t *string, in_ByteSequence *bs);

/* Not yet implemented: */
/* Stream byte_sequence_open(ByteSequence); */

/* Document Field functions: */	

DocFieldTab dm_dft_create(in_Document *doc);
Int_t dm_dft_destroy(DocFieldTab *ext_dft);
Int_t dm_dft_get_doc_id(DocFieldTab *dft, void **doc_id);
Int_t dm_dft_get_num_fields(DocFieldTab *dft, Int_t *num_fields);
Int_t dm_dft_add_field(DocFieldTab *ext_dft, FieldEnt *fe);
Int_t dm_dft_get_ith_field(DocFieldTab *dft, Int_t i, FieldEnt **field_ent);

FieldEnt *dm_field_ent_create(void);
Int_t dm_field_ent_add_span(FieldEnt *fe, Int_t offset, Int_t length);
Int_t dm_field_ent_set_type(FieldEnt *fe, Int_t type);

Int_t dm_fe_get_type(FieldEnt *fe, Int_t *type);
Int_t dm_fe_get_field_name(FieldEnt *fe, Char_t **name);
Int_t dm_fe_get_field_flags(FieldEnt *fe, char *f1, char *f2, 
			    char *f3, Boolean_t *f4, char *f5);
Int_t dm_fe_get_disp_flag(FieldEnt *fe, char *disp_flag);
Int_t dm_fe_get_num_spans(FieldEnt *fe, Int_t *num_spans);
Int_t dm_fe_get_ith_span(FieldEnt *fe, Int_t i, in_ByteSequence *span);

/* Document Functions */

in_Document dm_document_create(in_Collection parent);
Int_t dm_document_get_collection(in_Document ext_doc, in_Collection *coll);
Int_t dm_document_get_internal_id(in_Document, Int_t *Id);
Int_t dm_document_get_raw_data(in_Document, in_ByteSequence *bs);
Int_t dm_document_get_fields(in_Document doc, DocFieldTab *dft);
Int_t dm_document_get_num_fields(in_Document doc, Int_t *num_fields);
Int_t dm_document_get_num_spans(in_Document doc, Int_t *num_spans);
Int_t dm_document_get_field(in_Document doc, Char_t *field_name, 
			    FieldEnt **field);
Int_t dm_document_get_ith_field(in_Document doc, Int_t i, FieldEnt **field);
Int_t dm_document_set_raw_data(in_Document ext_doc, in_ByteSequence bs);
Int_t dm_document_free(in_Document doc);


/* Not yet implemented: */
Int_t dm_document_get_external_id(in_Document, Char_t **Xid); 

/*  Field Descriptor Functions: */
Int_t dm_field_desc_dup(FieldDesc *fd_in, FieldDesc **fd_out);
Int_t dm_field_desc_free(FieldDesc *desc);

/*  Field Descriptor Table Functions: */
FieldDesc *dm_field_desc_tab_create(Int_t num_fields);
Int_t dm_field_desc_tab_add_field(FieldDesc **fd_tab, Char_t *name, 
				  Char_t *desc, char norm_index, 
				  char field_index, char display_text, 
				  Boolean_t run_recog, char coop_mode);
Int_t dm_field_desc_tab_copy(FieldDesc *in_tab, FieldDesc **out_ptr);
Int_t dm_field_desc_tab_free(FieldDesc *fdt_ptr);
Int_t dm_field_desc_tab_get_num_fields(FieldDesc *fd_tab);
Int_t dm_field_desc_tab_get_field(FieldDesc *fd_tab, Char_t *field_name,
				  FieldDesc **fd_entry);
int dm_field_desc_tab_cmp(FieldDesc *tab1, FieldDesc *tab2);


/* Document Format Table Functions: */

DocFormat_t *dm_doc_format_create(Int_t doc_type);
Int_t dm_doc_format_free(DocFormat_t *fdt);
Int_t dm_doc_format_set_translator(DocFormat_t *doc_fmt, Int_t (*trans_funct)());
Int_t dm_doc_format_set_type_name(DocFormat_t *doc_fmt, Char_t *name);

Int_t dm_doc_format_add_field_tab(DocFormat_t *fdt, FieldDesc *fd_tab);
Int_t dm_doc_format_get_num_fields(DocFormat_t *fdt, Int_t *num_fields);

Int_t dm_get_field_name(DocFormat_t *fdt, Int_t id, Char_t **name);
Int_t dm_get_field_description(DocFormat_t *fdt, Int_t id, Char_t **disc);
Int_t dm_get_field_flags(DocFormat_t *fdt, Int_t id, char *norm_index, 
			 char *field_index, char *display, Boolean_t *recog, 
			 char *coop);
Int_t dm_get_field_id_from_name(in_Collection coll, Char_t *name,
			 Int_t *field_id);


/* Collection Functions: */

in_Collection dm_collection_create(Char_t *name);
Int_t dm_collection_destroy(in_Collection coll);

Int_t dm_collection_set_owner(in_Collection coll, Char_t *owner);
Int_t dm_collection_set_data_ident(in_Collection coll, Char_t *data_ident);
Int_t dm_collection_set_type(in_Collection coll, int type);
Int_t dm_collection_set_doc_format(in_Collection coll, DocFormat_t *fdt);

Int_t dm_collection_get_data_ident(in_Collection coll, Char_t **data_ident);
Int_t dm_collection_get_doc_format(in_Collection coll, DocFormat_t **fdt);
Int_t dm_collection_get_type(in_Collection coll, int *type);
Int_t dm_collection_get_id(in_Collection coll, Int_t *id);
Int_t dm_collection_get_name(in_Collection coll, Char_t **name);
Int_t dm_collection_get_owner(in_Collection coll, Char_t **owner);
Int_t dm_collection_get_num_docs(in_Collection coll, Int_t *num);
Int_t dm_collection_get_num_fields(in_Collection coll, Int_t *num);
Int_t dm_collection_get_field_flags(in_Collection coll, Int_t i, char *f1,
				    char *f2, char *f3, Boolean_t *f4, 
				    char *f5);
Int_t dm_collection_get_field_name(in_Collection coll, Int_t i, Char_t **name);
Int_t dm_collection_get_field_description(in_Collection coll, Int_t i, 
					  Char_t **desc);


/* Not yet implemented */
/* Int_t dm_collection_sync(Collection);   */


/* DOCUMENT MANAGER INTERFACE FUNCTIONS */

Int_t dm_initialize(Char_t *dbinfo, Int_t mode, void *lock_hand, void *hfd_handle, 
		    void **dm_ptr);
Int_t dm_refresh(void *dm_ptr);
Int_t dm_terminate(void *dm_ptr);

Int_t dm_add_doc_format(void *dm_ptr, DocFormat_t *doc_type);
Int_t dm_collection_init(void *dm_ptr, Char_t *name, Char_t *typename,
			 in_Collection *coll);

Int_t dm_find_collection(void *dm_ptr, Char_t *name, in_Collection *coll_ptr);
Int_t dm_collection_open(void *dm_ptr, in_Collection coll);
Int_t dm_collection_close(in_Collection coll);

Int_t dm_collection_add_docs(void *dm_ptr, in_Collection coll, 
			     Char_t *data_ident, void *db, int mode);

Int_t dm_document_mark_span(in_Document ext_doc, Char_t *fieldname, 
			    Int_t offset, Int_t length);
Int_t dm_document_set_xid(in_Document doc, Char_t *xid);

Int_t dm_collection_remove_doc(in_Collection coll, in_Document doc);
Int_t dm_getdoc_by_external_id(void *dm_ptr, Char_t *Xid, in_Document *ret_doc);
Int_t dm_getdoc_by_internal_id(void *dm_ptr, Int_t id, in_Document *ret_doc);
Int_t dm_get_field_string_in_doc(in_Document doc, Char_t *field_name, 
				 Char_t **field_str);

Int_t dm_get_display_string_in_doc(in_Document ext_doc, Int_t disp_type, 
				   Char_t **field_str);

void  dm_free_field_string(Char_t *string);

Int_t dm_get_field_descript(void *dm_ptr, Char_t *field_name, 
			    Char_t **field_descript);

Int_t dm_collection_list_available(void *dm_ptr, Char_t ***list);
Int_t dm_list_data_idents(void *dm_ptr, Char_t ***list);
Int_t dm_free_list_data_idents(Char_t ***list_ptr);
Int_t dm_collection_change_data_ident(void *dmi_ptr, Char_t *oldname, 
				      Char_t *newname);

Int_t dm_collection_get_mtime(in_Collection ext_coll, Int_t *ts);
Int_t dm_document_get_data_ident(in_Document ext_doc, Char_t **data_ident);
Int_t dm_collection_compress(void *dm_ptr, in_Collection coll, 
			     char *comp_filename);

Int_t dm_document_get_type(in_Document ext_doc, Char_t **doc_type);

Int_t dm_external_to_internal_docid(void *dm_handle, char *xid, int *id);
Int_t dm_internal_to_external_id(void *dm_handle, int doc_id, char **doc_xid);

Int_t dm_collection_add_field(in_Collection doc_coll,
			 Char_t *name, Char_t *description,
			 Boolean_t norm_index, Boolean_t field_index,
			 char display_text, Boolean_t run_recog,
			 char coop_mode);

Int_t dm_get_doc_types(void *dm_handle, Int_t *num_types, 
		       Char_t ***type_names);
Int_t dm_free_doc_types(void *dm_handle, Char_t **type_names);
Int_t dm_doctype_get_field_desc_tab(void *dm_ptr, Char_t *type_name, 
				    Int_t *num_fields, 
				    FieldDesc **fld_desc_tab);

int dm_doc_type_is_valid(void *dm_ptr, Char_t *typename);

#if (defined(__MWERKS__) && defined(__cplusplus))
}
#endif

#endif /* DOCMAN_H */
