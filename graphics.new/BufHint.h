/* BufHint.h: specification of buffer manager hints */
#ifndef BufHint_h
#define BufHint_h

/* Current == Move page to current list, making page reclaimable.
	Future == Move page into future list, making page unreclaimable.
	NoChange == Page stays at whichever list it's in.
	Toss == toss the page from memory. Place page at beginning of free list.
*/
enum BufHint { Current, Future, NoChange, Toss};

#endif
