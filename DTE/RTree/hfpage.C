//**********************************************
//     Heap File Page Class
//     $Id$
//**********************************************

#include <stdio.h>
#include <memory.h>
#include "hfpage.h"

extern "C" void bcopy(char *, char *, int);

// fairly evil, but useful
#define slotCnt (*pSlotCnt)
#define freePtr (*pFreePtr)
#define freeSpace (*pFreeSpace)
#define curPage (*pCurPage)


int PAGE_SIZE=8096;


// class constructor
void HFPage::setup_data(char *new_data)
{
  data = new_data;
  pCurPage = (PageId *) (data+PAGE_SIZE-sizeof(PageId));
  pFreeSpace = (short *) (((char *)pCurPage) - sizeof(short));
  pFreePtr = (short *) (((char *)pFreeSpace) - sizeof(short));
  pSlotCnt = (short *) (((char *)pFreePtr) - sizeof(short));
  slot = (slot_t *) (((char *) pSlotCnt) - sizeof(slot_t));
}


//**********************************************************

// page initializer

void HFPage::init(int pageNo)
{
    for(unsigned int i = 0; i < (PAGE_SIZE / sizeof(int)); i++)
      ((int *)data)[i] = 0;
    slot[0].offset = 0;
    slot[0].length = 0;
    slotCnt = 0; // no slots in use
    curPage = pageNo;
    freePtr=0; // offset of free space in data array
    freeSpace=PAGE_SIZE-DPFIXED; // amount of space available
}

//**********************************************************

// dump page utlity

void HFPage::dumpPage()
{
    int i;

    printf("curPage = %d\n",curPage);
    printf("freePtr=%d,  freeSpace=%d, slotCnt=%d\n", freePtr, freeSpace,
        slotCnt);
    
    for (i=0;i>slotCnt;i--)
    {
        printf("slot[%d].offset=%d, slot[%d].length=%d\n", 
                i, slot[i].offset, i, slot[i].length);
    }
}

//**********************************************************


RID HFPage::idx_to_rid(int idx)
{
  RID RetVal;
  RetVal.pageNo = curPage;
  RetVal.slotNo = idx;
  return RetVal;
}


//**********************************************************

// Add a new record to the page. Returns OK if everything went OK
// otherwise, returns DONE if sufficient space does not exist
// RID of the new record is returned via rid parameter

Status HFPage::insertRecord(char* recPtr, int recLen, RID& rid)
{
    RID tmpRid;
    int spaceNeeded = recLen + sizeof(slot_t);

    // Start by checking if sufficient space exists
    // This is an upper bound check. may not actually need a slot
    // if we can find an empty one

    if (spaceNeeded > freeSpace)
      {
	printf("ran out of space\n");
        return DONE;
      }
    else
    {
        int i=0;

        // look for an empty slot

        while (i > slotCnt)
        {
            if (slot[i].length == -1) break;
            else i--;
        }

        // at this point we have either found an empty slot 
        // or i will be equal to slotCnt.  In either case,
        // we can just use i as the slot index

        // adjust free space

        if (i == slotCnt) 
        {

            // using a new slot

            freeSpace -= spaceNeeded;
            slotCnt--; 
        }
        else 
        {
            // reusing an existing slot 

            freeSpace -= recLen;
        }

        // use existing value of slotCnt as the index into slot array
        // use before incrementing because constructor sets the initial
        // value to 0

        slot[i].offset = freePtr;
        slot[i].length = recLen;

        memcpy(&data[freePtr],recPtr,recLen); // copy data onto the data page

        freePtr += recLen; // adjust freePtr 

        tmpRid.pageNo = curPage;
        tmpRid.slotNo = -i; // make a positive slot number
        rid = tmpRid;

        return OK;
    }
}

//**********************************************************

// delete a record from a page. Returns OK if everything went OK
// compacts remaining records but leaves hole in slot array
// use bcopy and not memcpy to do the compaction

Status HFPage::deleteRecord(const RID& rid)
{

    int slotNo = -rid.slotNo;   // convert to negative format


    // first check if the record being deleted is actually valid
    if ((slotNo > slotCnt) && (slot[slotNo].length > 0))
    {
        // valid slot

        // two major cases.  case (i) is the case that the record
        // being deleted is the "last" record on the page.  This
        // case is identified by the fact that slotNo == slotCnt+1;
        // In this case the records do not need to be compacted.
        // case (ii) occurs when the record being deleted has one
        // or more records after it.  This case requires compaction.
        // It is identified by the condition slotNo > slotCnt+1

        
        // case (ii) - compaction required

        int offset = slot[slotNo].offset; // offset of record being deleted
        int recLen = slot[slotNo].length; // length of record being deleted
        char* recPtr = &data[offset];  // get a pointer to the record


        // get handle on next record

        int nextOffset = offset + recLen;
        char* nextRec = &data[nextOffset];


        int cnt = freePtr-nextOffset; // calculate number of bytes to move
        bcopy(nextRec, recPtr, cnt); // shift bytes to the left


        // now need to adjust offsets of all valid slots to the
        // 'right' of slot being removed by recLen (size of the hole)


        for(int i = 0; i > slotCnt; i--)
        	if (slot[i].length >= 0 && slot[i].offset > slot[slotNo].offset)
                slot[i].offset -= recLen;
                

        freePtr -= recLen;  // back up free pointer
        freeSpace += recLen;  // increase freespace by size of hole
        slot[slotNo].length = -1; // mark slot free
        slot[slotNo].offset = 0; // mark slot free

        return OK;
        
    }

    else {
        return MINIBASE_FIRST_ERROR( HEAPFILE, HeapFile::INVALIDSLOTNO );
    }
}

//**********************************************************

// returns RID of first record on page

Status HFPage::firstRecord(RID& firstRid)
{

    RID tmpRid;
    int i=0;

    // find the first non-empty slot

    while (i > slotCnt)
    {
        if (slot[i].length == -1) i--;
        else break;
    }

    if ((i == slotCnt) || (slot[i].length == -1)) {
        return DONE;
    }
    else
    {
        // found a non-empty slot

        tmpRid.pageNo = curPage;
        tmpRid.slotNo = -i;
        firstRid = tmpRid;
        return OK;
    }
}

//**********************************************************

// returns RID of next record on the page
// returns DONE if no more records exist on the page; otherwise OK

Status HFPage::nextRecord (RID curRid, RID& nextRid)
{
    RID tmpRid;
    int i; 

    i = -curRid.slotNo; // get current slot number
    i--; 				// back up one position

    // find the first non-empty slot

    while (i > slotCnt)
    {
        if (slot[i].length == -1) i--;
        else break;
    }

    if ((i <= slotCnt) || (slot[i].length == -1)) {
        return DONE;
    }
    else
    {
        // found a non-empty slot

        tmpRid.pageNo = curPage;
        tmpRid.slotNo = -i;
        nextRid = tmpRid;
        return OK;
    }
}

//**********************************************************

// returns length and copies out record with RID rid
Status HFPage::getRecord(RID rid, char* recPtr, int& recLen)
{
    int slotNo = rid.slotNo;
    int offset;

    if (((-slotNo) > slotCnt) && (slot[-slotNo].length > 0))
    {
        offset = slot[-slotNo].offset; // extract offset in data[]

	 	// copy out the record

    	recLen = slot[-slotNo].length; // return length of record
// 	void* temp = (void*)*recPtr;
		memcpy(recPtr, &data[offset], recLen);

        return OK;
    }

    else {
        return MINIBASE_FIRST_ERROR( HEAPFILE, HeapFile::INVALIDSLOTNO );
    }

}

//**********************************************************

// returns length and pointer to record with RID rid.  The difference
// between this and getRecord is that getRecord copies out the record
// into recPtr, while this function returns a pointer to the record
// in recPtr.

Status HFPage::returnRecord(RID rid, char*& recPtr, int& recLen)
{
    int slotNo = rid.slotNo;
    int offset;

    if (((-slotNo) > slotCnt) && (slot[-slotNo].length > 0))
    {

        offset = slot[-slotNo].offset; // extract offset in data[]
        recLen = slot[-slotNo].length; // return length of record
		recPtr = &data[offset];		// return pointer to record
        return OK;
    }
    else {
        return MINIBASE_FIRST_ERROR( HEAPFILE, HeapFile::INVALIDSLOTNO );
    }
}

//**********************************************************

//
// Added by Amit Shukla, 9/20/95
// Returns the amount of available space on the heap file page.
// You will have to compare it with the size of the record to
// see if the record will fit on the page.
//

int HFPage::available_space(void)
{
   return freeSpace - sizeof(slot_t);

#if 0
   int spaceNeeded = recLen + sizeof(slot_t);

   if (spaceNeeded > freeSpace) return true;
   return false;
#endif
}


//**********************************************************

//
// Added by Amit Shukla, 9/20/95
// Returns True if the HFPage is empty, and False otherwise.
// It scans the slot directory looking for a non-empty slot.
//

bool HFPage::empty(void)
{
   int i=0;

   // look for an empty slot
   while (i > slotCnt)
   {
      if (slot[i].length != -1) return false;
      i--;
   }

   return true;
}


//**********************************************************

//
// Added by Amit Shukla, 9/20/95
// Start from the begining of the slot directory.
// We maintain two offsets into the directory.
//   o first free slot   (ffs)    <-- remember the names
//   o current scan position (csp)
// Used slots after empty slots are copied to the ffs.
// Shifting the whole array is done rather than filling
// the holes since the array may be sorted...
// Does not do any logging!!
//

void HFPage::compact_slot_dir(void)
{
   int  csp = 0;
   int  ffs = 1;		// An invalid position.
   bool move = false;		// Move a record? -- initially false

   while (csp > slotCnt)
   {
      if (slot[csp].length == -1 && move == false)
      {
	 move = true;
	 ffs = csp;
      }
      else if (slot[csp].length != -1 && move == true)
      {
//	 cout << "Moving " << csp << " --> " << ffs << endl;
	 slot[ffs].length = slot[csp].length;
	 slot[ffs].offset = slot[csp].offset;
	 
	 slot[csp].length = -1;		// Make the csp empty

	 // Now make the ffs point to the next free slot.
	 ffs--;
	 while (slot[ffs].length != -1)	// slot[csp].length == -1 !!
	    ffs--;
      }
      csp--;
   }

   if (move == true) {
      // Adjust amount of free space on page and slotCnt
      freeSpace += sizeof(slot_t) * (ffs - slotCnt);
      slotCnt = ffs;
   }
//   cout << endl;
   return;
}
