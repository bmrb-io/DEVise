/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1999/03/10 19:11:05  wenger
  Implemented DataReader schema GUI; made other improvements to schema
  editing GUI, such as sorting the schema lists.

 */

#include <stdio.h>
#include <strings.h>

#include "DRUtils.h"
#include "DataReader.h"
#include "DevError.h"

#define DEBUG 0

//-----------------------------------------------------------------------------
void
TranslateChars(char *input, ostrstream &listStr)
{
#if (DEBUG >= 3)
    printf("TranslateChars(%s)\n", input);
#endif

    while (*input) {
	switch (*input) {
	case '\n':
	    listStr << "\\n";
	    break;

	case '\r':
	    listStr << "\\r";
	    break;

	case '\t':
	    listStr << "\\t";
	    break;

	case '"':
	    listStr << "\\\"";
	    break;

	default:
	    listStr << *input;
	    break;
	}
        input++;
    }
}

//-----------------------------------------------------------------------------
void
OutputType(AttrType type, ostrstream &listStr)
{
#if (DEBUG >= 3)
    printf("OutputType(%d)\n", type);
#endif

    switch (type) {
    case TYPE_INT:
	listStr << "int";
        break;

    case TYPE_STRING:
	listStr << "string";
        break;

    case TYPE_DOUBLE:
	listStr << "double";
        break;

    case TYPE_DATE:
	listStr << "date";
        break;

    default:
	listStr << "(unknown) ";
    	break;
    }
}

//-----------------------------------------------------------------------------
char *
DRSchema2TclList(const char *schemaFile)
{
#if (DEBUG >= 1)
    printf("DRSchema2TclList(%s)\n", schemaFile);
#endif

    char *list = NULL;

#if defined(NO_DTE)
    reportErrNosys("DataReader schemas cannot be parsed in the no-DTE "
      "version of DEVise");
#else
    DRSchema *schema = DataReader::CreateSchema(schemaFile, false);
    if (schema == NULL) {
	char errBuf[MAXPATHLEN * 2];
        sprintf(errBuf, "Unable to read and/or parse schema file <%s>",
	  schemaFile);
        reportErrNosys(errBuf);
    } else {
        ostrstream listStr;

	Holder *recSep = schema->getDelimiter();
	listStr << "{[";
	if (recSep) {
	    TranslateChars(recSep->data, listStr);
	    listStr << "]";
	    if (recSep->repeating) {
	        listStr << "+";
	    }
	}
	listStr << "} ";

	Holder *fldSep = schema->getSeparator();
	listStr << "{[";
	if (fldSep) {
	    TranslateChars(fldSep->data, listStr);
	    listStr << "]";
	    if (fldSep->repeating) {
	        listStr << "+";
	    }
	}
	listStr << "} ";

	Holder *comment = schema->getComment();
	listStr << "{";
	if (comment) TranslateChars(comment->data, listStr);
	listStr << "} ";

	for (unsigned attrNum = 0; attrNum < schema->qAttr; attrNum++) {
	    Attribute *attr = schema->tableAttr[attrNum];
	    listStr << "{";

	    listStr << "{";
            OutputType(attr->getType(), listStr);
	    listStr << "} ";

	    listStr << "{";
	    if (attr->getFieldName()) listStr << attr->getFieldName();
	    listStr << "} ";

	    listStr << "{";
	    if (attr->getMaxLen() >= 0) listStr << attr->getMaxLen();
	    listStr << "} ";

	    listStr << "{";
	    if (attr->getDateFormat()) TranslateChars(attr->getDateFormat(),
	      listStr);
	    listStr << "} ";

	    listStr << "} ";
	}

        listStr << ends;

        list = strdup(listStr.str());
    }
#endif

#if (DEBUG >= 2)
    printf("list = <%s>\n", list ? list : "(null)");
#endif

    return list;
}

/*============================================================================*/
