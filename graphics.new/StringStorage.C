/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1997/01/30 19:47:13  jussi
  Added PopulateFromInitFile() method.

  Revision 1.1  1996/07/15 17:00:53  jussi
  Initial revision.
*/

#ifdef __GNUG__
#pragma implementation "HashTable.h"
#endif

#include <string.h>
#include <stdio.h>

#include "StringStorage.h"
#include "Util.h"

int StringStorage::_stringNum = 0;
HashTable<char *, int> StringStorage::_strings(100, StringHash, StringComp);
HashTable<int, char *> StringStorage::_keys(100, KeyHash, 0);

int StringStorage::PopulateFromInitFile()
{
    char *fname = "devise.strings";

    FILE *fp = fopen(fname, "r");
    if (!fp)
        return 0;

    printf("Initializing string table from %s\n", fname);

    char buf[256];
    while (fgets(buf, sizeof buf, fp)) {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        int key;
        int code = Insert(buf, key);
#ifdef DEBUG
        printf("Inserted \"%s\" with key %d, code %d\n", buf, key, code);
#endif
    }

    fclose(fp);

    return 0;
}
