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
  Test driver (main program) for "Devise Reader" software.
 */

/*
    $Id$
    $Log$
 */

#include <stdio.h>
#include <sys/types.h>

#include "DevRead.h"
#include "ApInit.h"
#include "types.h"
#include "marshal.h"

/*------------------------------------------------------------------------------
 * function: main
 * Main function for the drtest program.
 */
int                /* description */
main(
    int        argc,
    char **    argv)

{                                                    /* start main */
    char *        dataFile = "/p/devise/dat/3d_test.dat";
    int            result = 0;
    char *        schemaFile = "/p/devise/schema/3d_test.schema";

    if (argc > 1)
    {
        schemaFile = argv[1];
    }

    if (argc > 2)
    {
        dataFile = argv[2];
    }

    /* Initialize Devise stuff for command-line arguments. */
    Init::DoInit();

    /*  DevRead *drP = new DevRead(schemaFile, dataFile);  */
    DevRead *drP = new DevRead();

    drP->Open(schemaFile,dataFile);
    drP->Close();

    int attrCount = drP->getNumFlds();
    printf("record attribute count (when closed)= %d\n\n", attrCount);

    drP->Open(schemaFile,dataFile);

    attrCount = drP->getNumFlds();
    printf("record attribute count = %d\n\n", attrCount);

    String *type_names = drP->getTypeIDs();

    int j;
    for (j=0; j < attrCount; j++) {
        printf("Attr %d Type: %s\n", j, (const char*) type_names[j] );
    }

    printf("\n");

    {
        String *attr_names = drP->getAttributeNames();

        int j;
        for (j=0; j < attrCount; j++) {
            printf("Attr %d Name: %s\n", j, (const char*) attr_names[j] );
        }

        delete [] attr_names;
    }

    printf("\n");
    Tuple *tuple = NULL;

    while ((tuple = drP->getNext()) != NULL) {

        printf("\nRecord:\n");
        for (int i=0; i < attrCount; i++) {

            if (type_names[i] == "IDouble") {
                double *d = (double*) (*tuple)[i];
                printf("%f ", *d);
            } else if (type_names[i] == "IInt") {
                int *j = (int*) (*tuple)[i];
                printf("%d ", *j);
            } else if (type_names[i] == "IString") {
                char *s = (char*) (*tuple)[i];
                printf("%s ", s);
            } else {
                printf("<Unknown> ");
            }
        }
        printf("\n");
    }

    delete [] type_names;
    drP->Close();
    delete drP;

    return result;
}                                                    /* end main */

/*============================================================================*/
