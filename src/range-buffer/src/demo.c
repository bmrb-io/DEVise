/************************************************************************/
/* This a demo program that shows how to use the Range Buffer Manager	*/
/************************************************************************/



/* The Following Is the Style of Comments: */

/************************************************************************/
/* Major demo steps 							*/
/************************************************************************/
/*-------------- Segments inside each step --------------*/
/* sub-segments inside each segments or regular comments */




#include <stdio.h>

/* include Kevin's page/tuple stuff */
#include "dte/types/DteInt4Adt.h"
#include "dte/types/DteFloat8Adt.h"
#include "dte/types/DteVarCharAdt.h"
#include "dte/types/DteTupleAdt.h"
#include "dte/comm/DteAdtPage.h"

#include "PhonyRAM.h"
#include "BoundingBoxList.h"
#include "RangeBufferManager.h"
#include "RangeBufferManagerAccessMethod.h"

/* There is one Global RangeBufferManager in the system */
RangeBufferManager *globalRBM;

/* Range Access Method used as the level below globalRBM in the demo */
PhonyRAM *globalPhonyRAM;

/* In the demo, we use two objects. The first's id is 1 */
const RBMObject TEST_OBJECTID_1 = 1;

/* In the demo, we use two objects. The second's id is 2 */
const RBMObject TEST_OBJECTID_2 = 2;

/* All meta-info about object 1 used demo */
RBMObjectInfo globalRBMObjectInfo_1;

/* All meta-info about object 2 used demo */
RBMObjectInfo globalRBMObjectInfo_2;

/* In the demo, the first attr of object 1 is its bounding box attribute */
#define TEST_BBOX_ATTR_NUM_1 0

/* In the demo, the fourth attr of object 2 is its bounding box attribute */
#define TEST_BBOX_ATTR_NUM_2 3

/* the replacement policy used for the demo */
#define TEST_REPLACEMENT_POLICY REPLACE_RANDOM

/* the max number of pages in the Global Ranger Buffer Manager used for demo */
#define TEST_NUM_RBM_PAGE 7

int main()
{
 


printf("
/************************************************************************/
/*		    	Range Buffer Manager Demo		  	*/
/************************************************************************/
");



/************************************************************************/
/* Global Preparation Work						*/
/************************************************************************/

/*-------------- Creating Object 1 --------------*/

    /* generating the tuple format */

    DteAdtList adt(5);
    adt[0] = DteFloat8Adt();
    adt[1] = DteInt4Adt();
    adt[2] = DteVarCharAdt(13);
    adt[3] = DteFloat8Adt();
    adt[4] = DteVarCharAdt(5);
    DteTupleAdt rec(adt);

    /* defining all meta-info about object 1 */

    globalRBMObjectInfo_1.boundingBoxAttrNumber = TEST_BBOX_ATTR_NUM_1;
    globalRBMObjectInfo_1.tupleAdt = &rec;

/*-------------- Creating Object 2 --------------*/

    /* defining all meta-info about object 2 */

    globalRBMObjectInfo_2.boundingBoxAttrNumber = TEST_BBOX_ATTR_NUM_2;
    globalRBMObjectInfo_2.tupleAdt = &rec;

/*-------------- Creating phony RAM as the data provider --------------*/
/*-------------- under the Global Range Buffer Mangager  --------------*/

    /* create pages used to create phony RAM */

    char readBuf[DTE_PAGESIZE];
    char packBuf[DTE_PAGESIZE];

    DteAdtPage readPage(rec);
    DteAdtPage packPage(rec);
    packPage.initPage(packBuf);

    /* read data into pages from cin */

    while (cin)
    {
	readPage.initPage(readBuf);
	bool fitOnPage = readPage.addFromAscii(cin);
	if( !cin ) break;
    	assert(fitOnPage);

 	const Tuple* tup = readPage.getTuple(0);
       	fitOnPage = packPage.addTuple(tup);
	assert (fitOnPage);
    }

    /* Create a PhonyRAM with the page. */
    /* We query on object 1 in the following DEMOs 1 through 5. */

    globalPhonyRAM = new PhonyRAM(TEST_OBJECTID_1, &packPage);

/*-------------- Creating the global Range Buffer Manager --------------*/

    globalRBM = new RangeBufferManager(TEST_REPLACEMENT_POLICY,
				       TEST_NUM_RBM_PAGE);

/*-------------- Creating first RangeBufferManageAccessMethod --------------*/

    RangeBufferManagerAccessMethod *rbmam_1;

    /* We'll query on object 1 with the first RangeBufferManageAccessMethod. */
    rbmam_1 = new RangeBufferManagerAccessMethod(TEST_OBJECTID_1, globalPhonyRAM);

/*-------------- Creating second RangeBufferManageAccessMethod --------------*/
    RangeBufferManagerAccessMethod *rbmam_2;

    /* We'll query on object 2 with the second RangeBufferManageAccessMethod. */
    rbmam_2 = new RangeBufferManagerAccessMethod(TEST_OBJECTID_2, globalPhonyRAM);



printf("
/************************************************************************/
/* DEMO 1: Query on Object 1. The first attr is the Bounding Box Attr.	*/
/* Issue the following query:						*/
/* In = (50, 60) ; NotIn = empty					*/
/*									*/
/* Based on our input files, There should be 0 record returned.		*/
/************************************************************************/
/* What this demo shows:						*/
/*									*/
/* RangeBufferManagerAccessMethod can understand the query passed in 	*/
/* correctly and can respond correctly when there is no record that	*/
/* satisfy the query.							*/
/************************************************************************/
");

/*-------------- Creating the in and the notIn --------------*/

    BoundingBox *in;
    BoundingBoxList *notIn;

    /* In is (50, 60) */
    /* true means interval end is open, false means interval end is closed */
    in = new BoundingBox(50, true, 60, true);

    /* NotIn is empty */
    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    int result = rbmam_1->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    void *tup;
    int i = 0;
    while (rbmam_1->nextRec(tup) >= 0)
    {
	/* print out the record */
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    /* print out total number of records */
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_1->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 1 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 2: Query on Object 1. The first attr is the Bounding Box Attr.	*/
/* Issue the following query:						*/
/* In = [40, 45) ; NotIn = empty					*/
/*									*/ 
/* Based on our input files, 10 records should be returned, all of 	*/
/* which are fetched from lower level.					*/
/************************************************************************/
/* What this demo shows:						*/
/*									*/ 
/* RangeBufferManagerAccessMethod can get the correct query result when */
/* the query has an *in* bounding box but the *notIn* list is empty,	*/
/* and when all records are fetched from low level data provider.	*/
/************************************************************************/
");

/*-------------- Creating the in and the notIn --------------*/

    /* In is [40, 45) */
    in = new BoundingBox(40, false, 45, true);

    /* NotIn is empty */
    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_1->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_1->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_1->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 2 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 3: Query on Object 1. The first attr is the Bounding Box Attr.	*/
/* Issue the same query as in DEMO 2:					*/
/* In = [40, 45) ; NotIn = empty					*/
/*									*/ 
/* Based on our input files, 10 records should be returned. 		*/
/* All records should be returned directly from Memory,			*/
/* since we have just asked the same query in DEMO 2 above		*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* RangeBufferManagerAccessMethod can get the correct query result when */
/* the query has an *in* bounding box but the *notIn* list is empty,    */
/* and when all records have already been cached in memory previously 	*/
/* so that this time the query can be satisfied with and only with	*/
/* records in memory.							*/
/************************************************************************/
");

/*-------------- Creating the in and the notIn --------------*/

    /* In is [40, 45) */
    in = new BoundingBox(40, false, 45, true);

    /* NotIn is empty */
    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_1->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_1->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_1->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 3 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 4: Query on Object 1. The first attr is the Bounding Box Attr.	*/
/* Issue the following query: In = (35.5, 44) ; NotIn = empty		*/
/*									*/ 
/* Based on our input files, 12 records should be returned. 		*/
/* 8 of them are in [40, 44), and will be returned from Memory directly */
/* 4 of them are in (35.5, 40), and will be fetched from low level	*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* RangeBufferManagerAccessMethod can get the correct query result when */
/* the query has an *in* bounding box but the *notIn* list is empty,    */
/* and when part of the query is already cached in memory but some more	*/
/* records still must be fetched from low level data provider		*/ 
/************************************************************************/
");

/*-------------- Creating the in and the notIn --------------*/

    /* In is (35.5, 44) */
    in = new BoundingBox(35.5, true, 44, true);

    /* NotIn is empty */
    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_1->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_1->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_1->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 4 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 5: Query on Object 1. The first attr is the Bounding Box Attr.	*/
/* Issue the following query:						*/
/* In = (25, 47.3];							*/
/* NotIn = {(30, 33.3) (33.3, 37) (38, 41] [42, 47.3)}			*/
/*									*/
/* This is a complicated query with *notIn* holes. The *in* range and	*/
/* the *notIn* ranges all have overlaps with existing in-memory ranges.	*/ 
/*									*/ 
/* Based on our input files, 11 records should be returned. 		*/
/* 2 of them should be returned from Memory directly 			*/
/* 9 of them should be fetched from low level				*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* RangeBufferManagerAccessMethod can get correct query result when:	*/
/*                                                                      */
/* - The query has an *in* bounding box 				*/
/* - The query's *notIn* box has several elements			*/
/* - The *in* and *notIn* overlaps what's already in memory in a rather	*/
/*   complicated way							*/
/* - some of the records are already in memory but some must be fetched	*/
/*   from low level data provider					*/
/************************************************************************/
");

/*-------------- Creating the in and the notIn --------------*/

    BoundingBox *notInEntries;

    /* In is (25, 47.3] */
    in = new BoundingBox(25, true, 47.3, false);

    /* NotIn = {(30, 33.3) (33.3, 37) (38, 41] [42, 47.3)} */

    notIn = new BoundingBoxList;

    notInEntries = new BoundingBox(30, true, 33.3, true);
    notIn->insert(notInEntries);
    delete notInEntries;

    notInEntries = new BoundingBox(33.3, true, 37, true);
    notIn->insert(notInEntries);
    delete notInEntries;

    notInEntries = new BoundingBox(38, true, 41, false);
    notIn->insert(notInEntries);
    delete notInEntries;

    notInEntries = new BoundingBox(42, false, 47.3, true);
    notIn->insert(notInEntries);
    delete notInEntries;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_1->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_1->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_1->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 5 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 6: Query on Object 2. The second attr is the Bounding Box Attr. */
/* Issue the following query:						*/
/* In = [2.7, 2.9];							*/
/* NotIn = empty							*/
/*									*/
/* Based on our input files, 6 records should be returned. 		*/
/* All 6 should be fetched from low level				*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* A second RangeBufferManagerAccessMethod can run query on ANOTHER	*/
/* object and the two objects can be organized correctly in the		*/
/* Range Buffer Manager.						*/
/************************************************************************/
");

/*-------------- Switch to object 2 --------------*/

    /* Tell the lower level to switch to object 2 */
    delete globalPhonyRAM;
    globalPhonyRAM = new PhonyRAM(TEST_OBJECTID_2, &packPage);

/*-------------- Creating the in and the notIn --------------*/

    /* In is [2.7, 2.9] */
    in = new BoundingBox(2.7, false, 2.9, false);

    /* NotIn = empty */

    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_2->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_2->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_2->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 6 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 7: Query on Object 1. The first attr is the Bounding Box Attr.  */
/* Issue the following query:						*/
/* In = (25, 47.3];							*/
/* NotIn = {[33.3, 33.3] (39, 41) [44, 47)}				*/
/*									*/
/* This is another complicated query with *notIn* holes. 		*/
/*									*/ 
/* Based on our input files, 21 records should be returned. 		*/
/* 16 of them should be returned from Memory directly 			*/
/* 5 of them should be fetched from low level				*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* RangeBufferManagerAccessMethod can get correct query result when:    */
/*                                                                      */
/* - The query has an *in* bounding box                                 */
/* - The query's *notIn* box has several elements                       */
/* - The *in* and *notIn* overlaps what's already in memory in a rather */
/*   complicated way                                                    */
/* - some of the records are already in memory but some must be fetched */
/*   from low level data provider                                       */
/*									*/
/* In addition, this demo shows that when the RangeBufferManager runs	*/
/* out of pages, it can choose a victim to kick out and make pages 	*/
/* availabe to the current query. See below where it says object 2	*/
/* (with only one page cached) has been chosen as a victim.		*/
/************************************************************************/
");

/*-------------- Switch to object 1 --------------*/

    /* Tell the lower level to switch to object 1 */
    delete globalPhonyRAM;
    globalPhonyRAM = new PhonyRAM(TEST_OBJECTID_1, &packPage);

/*-------------- Creating the in and the notIn --------------*/

    /* In is (25, 47.3] */
    in = new BoundingBox(25, true, 47.3, false);

    /* NotIn = {[33.3, 33.3] (39, 41) [44, 47)} */

    notIn = new BoundingBoxList;

    notInEntries = new BoundingBox(33.3, false, 33.3, false);
    notIn->insert(notInEntries);
    delete notInEntries;

    notInEntries = new BoundingBox(39, true, 41, true);
    notIn->insert(notInEntries);
    delete notInEntries;

    notInEntries = new BoundingBox(44, false, 47, true);
    notIn->insert(notInEntries);
    delete notInEntries;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_1->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_1->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_1->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 7 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 8: Query on Object 2. The second attr is the Bounding Box Attr. */
/* Issue the following query:						*/
/* In = [2.6, 3];							*/
/* NotIn = empty							*/
/*									*/
/* Based on our input files, 107 records should be returned. 		*/
/* All 107 should be fetched from low level				*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* When there is a query on (pages of) records previously cached but	*/
/* later on chosen as victim, the records are re-brought into memory  	*/
/* and, in turn, another object is chosen to be kicked out.		*/
/* That is, we show replacement works here.				*/
/************************************************************************/
");

/*-------------- Switch to object 2 --------------*/

    /* Tell the lower level to switch to object 2 */
    delete globalPhonyRAM;
    globalPhonyRAM = new PhonyRAM(TEST_OBJECTID_2, &packPage);

/*-------------- Creating the in and the notIn --------------*/

    /* In is [2.6, 3] */
    in = new BoundingBox(2.6, false, 3, false);

    /* NotIn = empty */

    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_2->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_2->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_2->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 8 ended successfully!\n");



printf("
/************************************************************************/
/* DEMO 9: Query on Object 2. The second attr is the Bounding Box Attr. */
/* Issue the following query:						*/
/* In = [2.6, 3];							*/
/* NotIn = empty							*/
/*									*/
/* Based on our input files, 107 records should be returned. 		*/
/* All 107 should be fetched from memory directly			*/
/************************************************************************/
/* What this demo shows:                                                */
/*                                                                      */
/* This is the same query as the previous one. HOWEVER, this time all	*/
/* records are memory residing already.					*/
/* Just to show that in previous query we indeed brought all records	*/
/* into the memory.							*/
/************************************************************************/
");

/*-------------- Creating the in and the notIn --------------*/

    /* In is [2.6, 3] */
    in = new BoundingBox(2.6, false, 3, false);

    /* NotIn = empty */

    notIn = new BoundingBoxList;

/*-------------- Open scan on Range Buffer Manager --------------*/

    result = rbmam_2->openScan(in, notIn);
    if (result < 0)
    {
	TELL_ERROR("Error in global RBM's opening scan");
	exit(1);
    }

/*-------------- Iterate to get all records --------------*/

    printf("\nQuery Result Records:\n\n");

    i = 0;
    while (rbmam_2->nextRec(tup) >= 0)
    {
        rec.print(tup, cout);
        printf("\n");

        i++;
    }
    
    printf("\nEnd of all records. Total number of records = %d\n", i);

/*-------------- Clean up --------------*/
 
    rbmam_2->closeScan();
    delete in;
    delete notIn;

    TELL_FACT("\nDEMO 9 ended successfully!\n");

/************************************************************************/
/* Global Clean-up Work 						*/ 
/************************************************************************/

    delete rbmam_1;
    delete rbmam_2;
    delete globalRBM;
    delete globalPhonyRAM;

printf("
/************************************************************************/
/*  		All demos ran successfully! Goodbye!			*/
/************************************************************************/
");

    return 0;
}
