#ifndef _DTEPAGE_H_
#define _DTEPAGE_H_

// (C) 1999- by Kevin S. Beyer

#include "DTE/misc/DteConfig.h"
#include "DTE/misc/DteBasicTypes.h"
#include <unistd.h>             // for read/write

// Note that this a handle to a PageBuf; it is not valid until
// initPage() or setPage() is called.
// getNumSlots() can be called without setting a page (it returns 0)

class DtePage
{
protected:

  typedef int4 Slot;
  static const int SLOT_ALIGN = INT4_ALIGN;

  struct header_t {
    uint4 numSlots;
  };

  char* page;
  char* data;
  char* startFree;
  char* endFree;             // pointer to byte after free space
  Slot* slot;

  header_t* header() const { return (header_t*)page; }

  char* getFreeStart() { return startFree; } // EndData returned this before

  void addSlot(uint len);

  //kb: const char*?
  char* getSlot(int i) const { return page + slot[-i]; }


public:

  DtePage();

  //kb: should I make this virtual??
  ~DtePage() {}

  // p is a page buffer of size DTE_PAGESIZE that **contains garbage**
  void initPage(PageBuf* p) {
    page = p->data;
    reinitPage();
  }

  // p is a page buffer of size DTE_PAGESIZE that **contains valid data**
  void setPage(PageBuf* p) {
    page = p->data;
    resetPage();
  }

  // the number of slots that are used on a this page
  int getNumSlots() const { return header()->numSlots; }

  // the number of bytes that are still free on this page
  int getBytesFree() { return endFree - startFree; }

  // uses this->page to init this; page contains garbage
  void reinitPage();

  // uses this->page to init this; page contains valid data
  void resetPage();  //kb: should this be protected?

private:

  DtePage(const DtePage& other);
  DtePage& operator=(const DtePage& other);
};


inline
DtePage::DtePage()
{ 
  static header_t dummy_header = {0};
  page = (char*)&dummy_header;
  data = NULL;
  startFree = NULL;
  slot = NULL;
}

inline
void DtePage::reinitPage()
{
  data = Align(page + sizeof(header_t), MAX_ALIGN);
  slot = (Slot*)AlignDown((page + DTE_PAGESIZE - sizeof(Slot)),
                             SLOT_ALIGN);
  header()->numSlots = 0;
  startFree = data;
  endFree = (char*)(&slot[-1]); // leave room for the next slot
  slot[0] = startFree - page;   // last slot always has offset of free
}


inline
void DtePage::resetPage()
{
  data = Align(page + sizeof(header_t), MAX_ALIGN);
  slot = (Slot*)AlignDown((page + DTE_PAGESIZE - sizeof(Slot)), SLOT_ALIGN);
  int r = header()->numSlots;
  startFree = page + slot[-r];
  endFree = (char*)(&slot[-r]);
}



inline
void DtePage::addSlot(uint len)
{
  startFree += len;
  endFree -= sizeof(Slot);
  int i = ++header()->numSlots;
  slot[-i] = startFree - page;
  assert(startFree <= (char*)&slot[-i]);
}


#endif // _DTEPAGE_H_
