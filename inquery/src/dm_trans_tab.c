/***************************************************************************
*                              Copyright (c)                               *
*  Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*                              1990 - 1996.                                *
*                                                                          *
*                          All Rights Reserved.                            *
*                                                                          *
* The INQUERY SOFTWARE was developed at the University of Massachusetts    *
* at Amherst Computer Science Department within the National Center for    *
* Intelligent Information Retrieval (CIIR). For more information, contact  *
* 413-545-0463 or http://ciir.cs.umass.edu.                                *
*                                                                          *
* This SOFTWARE was developed in significant part with private funding.    *
* The INQUERY SOFTWARE is proprietary to ACSIOM and may not be distributed *
* in whole or in part or used in any way without authorized permission in  *
* writing from ACSIOM.  Possession, use, duplication or dissemination of   *
* the INQUERY SOFTWARE and media is authorized only pursuant to a valid    *
* written license from ACSIOM.                                             *
*                                                                          *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.   *
* Government is subject to restrictions as set forth in subparagraph       *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause  *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.               *
*                                                                          *
*        Contact ACSIOM at (413)545-6311 for more information              *
***************************************************************************/ 

/*
 * "DM_TRANS_TAB_C" is a C file that contains the document manager's
 * document format and translator table.  Each entry describes one 
 * recognized document format.
 */

/*
 *  RCS identifier for module:
 */

static char dm_trans_tab_c_rcsid[]="$RCSfile$, $Revision$, $Date$";


#include "docman.h"

extern Int_t email_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode); 
extern Int_t sgml_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode); 
extern Int_t file_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode);
extern Int_t cacm_dots_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode);
extern Int_t html_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode);
extern Int_t marc_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode);
extern Int_t fbis_trans(Char_t *buff, Int_t num, in_Document doc, Int_t mode); 

/* The field description table has the following format:
 *
 * FIELD-NAME,	DESCRIPTION,	    NORM-INDEX, FIELD-INDEX,	DISPLAY,    RUN-RECOGS, COOP MODES
 */
FieldDesc email_fields[] = {
  {"DOCID",     "Message ID",		OFF,	INQ_NORMAL,	INQ_ID,		OFF,	INQ_COOP },
  {"FROM",	"FROM field",		ON,	INQ_NORMAL,	INQ_TEXT,	OFF,	INQ_COOP },
  {"TO",	"TO field",		ON,	INQ_NORMAL,	INQ_TEXT,	OFF,	INQ_COOP },
  {"CC",	"CC field",	        ON,	INQ_NORMAL,	INQ_TEXT,	OFF,	INQ_COOP },
  {"SUBJECT",	"SUBJECT field",	ON,	INQ_NORMAL,	INQ_TITLE,	OFF,	INQ_COOP },
  {"DATE",	"DATE field",		ON,	INQ_NORMAL,	INQ_SOURCE,	ON,	INQ_COOP },
  {"BODY",	"Message Text",		ON,	INQ_NONE,	INQ_TEXT,	OFF,	INQ_COOP },
  { NULL, NULL, 0, 0, 0, 0, 0 }
};

FieldDesc sgml_fields[] =
{  {"TITLE",    "Document Title",       ON,     INQ_NORMAL,     INQ_TITLE,      ON,     INQ_COOP  },
   {"SOURCE",   "Document Source",      OFF,    INQ_NORMAL,     INQ_SOURCE,     OFF,    INQ_COOP  },
   {"DOCID",    "External Doc Id",      OFF,    INQ_NORMAL,     INQ_ID,         OFF,    INQ_COOP  },
   {"DDATE",    "Document Date",        OFF,    INQ_NORMAL,     INQ_SOURCE,     ON,     INQ_COOP  },
   {"CDATE",    "Content Date",         OFF,    INQ_NORMAL,     INQ_NULL_TYPE,  OFF,    INQ_COOP  },
   {"AUTHOR",   "Document Author",      ON,     INQ_NORMAL,     INQ_SOURCE,     ON,     INQ_COOP  },
   {"TEXT",     "Body of Text",         ON,     INQ_NONE,       INQ_TEXT,       ON,     INQ_COOP  },
   {"MESH",     "Medical Mesh Field",   ON,     INQ_NORMAL,     INQ_TEXT,       OFF,    INQ_COOP  },
   {"NOINDEX",  "Non-indexed Field",    OFF,    INQ_NONE,       INQ_NULL_TYPE,  OFF,    INQ_OVERRIDE },
   { NULL, NULL, 0, 0, 0, 0, 0 }
};

FieldDesc cdots_fields[] =
{  {"TITLE",    "Article Title",        ON,     INQ_NORMAL,     INQ_TITLE,      OFF,    INQ_COOP  },
   {"DOCID",    "External Doc Id",      OFF,    INQ_NORMAL,     INQ_ID,         OFF,    INQ_COOP  },
   {"SOURCE",   "Journal Source",       OFF,    INQ_NORMAL,     INQ_SOURCE,     OFF,    INQ_COOP  },
   {"AUTHOR",   "Article Author",       OFF,    INQ_NORMAL,     INQ_SOURCE,     OFF,    INQ_COOP  },
   {"ABSTRACT", "Article Abstract",     ON,     INQ_NONE,       INQ_TEXT,       OFF,    INQ_COOP  },
   {"KEYWORDS", "Keywords",             ON,     INQ_NONE,       INQ_TEXT,       OFF,    INQ_COOP  },
   {"CATEGORY", "Category",             OFF,    INQ_NORMAL,     INQ_NULL_TYPE,  OFF,    INQ_COOP  },
   {"DATA-ENTRY", "Data Entry Info",    OFF,    INQ_NONE,       INQ_NULL_TYPE,  OFF,    INQ_COOP  },
   {"IGNORED",  "Non-indexed Field",    OFF,    INQ_NONE,       INQ_NULL_TYPE,  OFF,    INQ_OVERRIDE},
   { NULL, NULL, 0, 0, 0, 0, 0 }
};


FieldDesc html_fields[] =
{  {"TITLE",    "Document Title",       ON,     INQ_NORMAL,     INQ_TITLE,      ON,     INQ_COOP},
   {"TEXT",     "Body of Text",         ON,     INQ_NONE,       INQ_TEXT,       ON,     INQ_COOP},
   {"SOURCE",   "URL of doc",           OFF,    INQ_NONE,       INQ_SOURCE,     OFF,    INQ_OVERRIDE},
   {"NOINDEX",  "Non-indexed Field",    OFF,    INQ_NONE,       INQ_NULL_TYPE,  OFF,    INQ_OVERRIDE},
   { NULL, NULL, 0, 0, 0, 0, 0 }
};
FieldDesc file_fields[] =
{
   {"TEXT",     "Body of Text",         ON,     INQ_NORMAL,     INQ_TEXT,       OFF,    INQ_COOP},
   { NULL, NULL, 0, 0, 0, 0, 0 }
};
FieldDesc marc_fields[] =
{
   {"RID",      "Record ID ",           OFF,    INQ_NORMAL,     INQ_ID,         OFF,    INQ_COOP },
   {"author",   "Author    ",           ON,     INQ_NORMAL,     INQ_SOURCE,     OFF,    INQ_COOP },
   {"subject",  "Subject   ",           ON,     INQ_NORMAL,     INQ_TEXT,       OFF,    INQ_COOP },
   {"title",    "Title     ",           ON,     INQ_NORMAL,     INQ_TITLE,      OFF,    INQ_COOP },
   {"body",     "Record Text",          ON,     INQ_NONE,       INQ_TEXT,       OFF,    INQ_COOP },
   { NULL, NULL, 0, 0, 0, 0, 0 }
};


/* The DocFormat table has the following format:
 *
 * DOCFORMAT-NAME, TRANSLATOR-FUNCT, NUM-FIELDS, FIELD-TABLE
 *
 */
DocFormat_t doc_types[] =
{
   { "email",     email_trans,     7, email_fields },
   { "sgml",      sgml_trans,      9, sgml_fields  },
   { "files",     file_trans,      1, file_fields  },
   { "cacm_dots", cacm_dots_trans, 9, cdots_fields },
   { "html",      html_trans,      4, html_fields  },
   { "fbis",      fbis_trans,      9, sgml_fields  },
   { "marc",      marc_trans,      5, marc_fields  },
   { NULL,        NULL,            0, NULL         }
};


