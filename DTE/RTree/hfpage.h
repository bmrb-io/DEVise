/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
 */

#ifndef HFPAGE_H
#define HFPAGE_H

extern int PAGE_SIZE;
#define PageId int
#define Status int
#define OK 0
#define DONE -1
#define MINIBASE_FIRST_ERROR(A, B) (1)
const int INVALID_SLOT =  -1;

typedef struct RID{
PageId  pageNo;
int  slotNo;
int operator==(const RID rid) const
  {return pageNo==rid.pageNo&&slotNo==rid.slotNo;};
};

// Class definition for a minirel data page.   
// The design assumes that records are kept compacted when
// deletions are performed. Notice, however, that the slot
// array cannot be compacted.  Notice, this class does not keep
// the records aligned, relying instead on upper levels to take
// care of non-aligned attributes.

struct slot_t {
  short   offset;  
  short   length;  // equals -1 if slot is not in use
};
class HFPage {
public:
    slot_t 	*slot;	        // first element of slot array - grows backwards!
    short	*pSlotCnt;	// number of slots in use;
    short	*pFreePtr;	// offset of first free byte in data[]
    short	*pFreeSpace;	// number of bytes free in data[]
    PageId	*pCurPage;	// page number of current pointer
    char *data;
    static const int DPFIXED = sizeof(slot_t) + 3*sizeof(short) + sizeof(PageId);

    void setup_data(char *new_data);
    void init(PageId pageNo);		 // initialize a new page
    void dumpPage();			 // dump contents of a page

    PageId page_no() {return *pCurPage;} // returns the page number

    RID idx_to_rid(int idx);

    // inserts a new record pointed to by recPtr with length recLen onto
    // the page, returns RID of record 
    Status insertRecord(char* recPtr, int recLen, RID& rid);

    // delete the record with the specified rid
    Status deleteRecord(const RID& rid);

    // returns RID of first record on page
    // returns DONE if page contains no records.  Otherwise, returns OK
    Status firstRecord(RID& firstRid);

    // returns RID of next record on the page 
    // returns DONE if no more records exist on the page
    Status nextRecord (RID curRid, RID& nextRid);

    // copies out record with RID rid into recPtr
    Status getRecord(RID rid, char* recPtr, int& recLen);

    // returns a pointer to the record with RID rid
    Status returnRecord(RID rid, char*& recPtr, int& recLen);

    // returns the amount of available space on the page
    int    available_space(void);

    // Returns true if the HFPage is has no records in it, false otherwise.
    bool empty(void);


protected:
    // Compacts the slot directory on an HFPage.
    // WARNING -- this will probably lead to a change in the RIDs of
    //            records on the page.  You CAN'T DO THIS on most kinds
    //		  of pages.
    void compact_slot_dir();

};

#endif
