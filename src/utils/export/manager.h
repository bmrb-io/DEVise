//******************************************************************************
// template manager
// Copyright (c) 1996-1997 Christopher E. Weaver
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted, provided
// that the above copyright notice appear in all copies and that both the
// copyright notice and this permission notice appear in supporting
// documentation, and that the name(s) of the copyright holder(s) not be used
// in advertising or publicity pertaining to distribution of the software
// without specific, written prior permission. The copyright holder(s) make no
// representations about the suitability of this software for any purpose. It
// is provided "as is" without express or implied warranty.
//
//******************************************************************************
// File: manager.h
// Last modified: Thu Sep 25 16:31:15 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 961104 [weaver]: Original file.
// 961105 [weaver]: Cleanup and documentation.
// 961107 [weaver]: Eliminated free queue and overflow avoidance (idmanager is
//					the new name for the previous incarnation of this template).
// 961115 [weaver]: Changed return type of PutID() to bool.
// 970325 [weaver]: Commented out synchronization.
// 970328 [weaver]: Added GetIDList().
// 970402 [weaver]: Added GetHeap() for easier external use of iterators.
// 970402 [weaver]: Added Lock() and Unlock() for external synchronization.
// 970402 [weaver]: Changed type ItemHeap to Heap.
// 970328 [weaver]: Removed GetIDList() (not needed with GetHeap(), iterators).
// 970505 [weaver]: Added begin() and end() to complement iterator typedefs.
// 970505 [weaver]: Added const subscript operator.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// A manager organizes and maintains a homogeneous collection of objects (T)
// accessed by unique identification objects (Key). Four interface methods may
// be used:
//
//	   GetID     - create a new T on the heap and return its unique Key
//	   PutID     - destroy the T associated with the supplied Key argument
//	   Find	     - return a pointer to the T associated with the supplied Key
//
//	   GetHeap   - return a constant reference to the heap for iteration
//
// Additionally, the heap may be accessed indirectly via the iterator mechanism.
//
// The first three methods are synchronized to disallow concurrent access (which
// could lead to inconsistent manager state). Use the Lock and Unlock methods
// the synchronize externally, particularly when using the last two methods.
//
// The manager operates using a heap (map). The heap is ordered on Key and
// contains all {key,t} pairs for keys checked-out (GetID). Whenever a new pair
// needs to be generated (in GetID()), the maximal Key in the heap is found and
// incremented; the result is the id (using ctor Key()) returned. A potential
// problem is overflow in the Key space if too many ids are requested. (See
// template idmanager, which maintains the Key space more intelligently using a
// free Key queue.)
//
// Key, T, and Compare have the usual STL requirements (specifically, those
// required by map<Key, T, Compare> and queue<list<Key> >). Key must follow two
// additional requirements:
//
//	   1) The default ctor Key() must produce a unique, special Key value
//		  which is used as a sentinel value and returned by GetID when
//		  expansion of the heap with a new {key,t} pair fails.
//	   2) Key must define a postfix increment operator.
//
// Note that using any integer type as Key satisfies these requirements (the
// default ctor produces 0).
//
// *** MUTEX SYNCHRONIZATION HAS BEEN COMMENTED OUT ***
// To restore, uncomment the synch.h include, the mutex member, the contents of
// Lock() and Unlock(), the mutex_init() calls in the two ctors, and the
// mutex_destroy() call in the dtor. Comment out the four begin() and end()
// methods and the const Find() and subscript operator methods. (The latter two
// probably will lead to a glorious const chase, so be warned.)
//
// It isn't really a synchronizable template anymore, is it?
//
//******************************************************************************

#ifndef __TEMPLATE_MANAGER
#define __TEMPLATE_MANAGER

//******************************************************************************
// Libraries
//******************************************************************************

#include <stl.h>
//#include <synch.h>

//******************************************************************************
// template manager
//******************************************************************************

template <class Key, class T, class Compare = less<Key> >
class manager
{
	public:

		typedef map<Key, T, Compare>	Heap;
		typedef Heap::iterator			iterator;
		typedef Heap::const_iterator	const_iterator;

	private:

		Heap		heap;		// Heap of T, indexed on Key
		Key			nextKey;	// The next Key

//		mutex_t		mutex;		// Synchronize manager accesses

	public:

		// Constructors and Destructors
		manager(void);
		manager(const manager& x);
		manager&	operator=(const manager& x);
		~manager(void);

		// Getters and Setters
		const Heap&	GetHeap(void) const		{ return heap;		}

		// Utility Functions
		const Key	GetID(void);
		bool		PutID(const Key& key);
		T*			Find(const Key& key);
		const T*	Find(const Key& key) const;

		iterator		begin(void)			{ return heap.begin();	}
		const_iterator	begin(void) const	{ return heap.begin();	}
		iterator		end(void)			{ return heap.end();	}
		const_iterator	end(void) const		{ return heap.end();	}

		void		Lock(void)		{}// mutex_lock(&mutex);	}
		void		Unlock(void)	{}// mutex_unlock(&mutex);	}

		// Conversions and Operators
		T*			operator[](const Key& key)			{ return Find(key);	}
		const T*	operator[](const Key& key) const	{ return Find(key);	}
};

//******************************************************************************
// Constructors and Destructors (Inline)
//******************************************************************************

template <class Key, class T, class Compare = less<Key> >
inline manager<Key, T, Compare>::manager(void)
{
	nextKey = Key();
//	  mutex_init(&mutex, 0, NULL);			  // Initialize synchronization
}

template <class Key, class T, class Compare = less<Key> >
inline manager<Key, T, Compare>::manager(const manager<Key, T, Compare>& x)
	: heap(x.heap), nextKey(x.nextKey)
{
//	  mutex_init(&mutex, 0, NULL);			  // Initialize a *unique* mutex
}

template <class Key, class T, class Compare = less<Key> >
inline manager<Key, T, Compare>&
manager<Key, T, Compare>::operator=(const manager<Key, T, Compare>& x)
{
	Lock();									// *** BEGIN CRITICAL SECTION ***
	{
		heap = x.heap;
		nextKey = x.nextKey;
	}
	Unlock();								// *** END CRITICAL SECTION ***

	return *this;
}

template <class Key, class T, class Compare = less<Key> >
inline manager<Key, T, Compare>::~manager(void)
{
//	mutex_destroy(&mutex);
}

//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

// Returns a unique Key after allocating a new {key,t} pair in the heap. The
// Key is created by expanding the Key space. The heap[key] statement has
// unusual semantics; refer to p.37 of the STL spec for a description.
template <class Key, class T, class Compare = less<Key> >
inline const Key	manager<Key, T, Compare>::GetID(void)
{
	Key		key;

	Lock();									// *** BEGIN CRITICAL SECTION ***
	{
		nextKey++;								// Create a new key value
		heap[nextKey];							// Creates {key,T()} in heap
		key = nextKey;
	}
	Unlock();								// *** END CRITICAL SECTION ***

	return key;
}

// Deallocates a {key,t} pair from the heap if it exists for the supplied
// argument key. Returns true if the key was successfully returned.
template <class Key, class T, class Compare = less<Key> >
inline bool		manager<Key, T, Compare>::PutID(const Key& key)
{
	bool	result = true;

	Lock();									// *** BEGIN CRITICAL SECTION ***
	{
		iterator	i = heap.find(key);			// Find T for this key

		if (i != heap.end())					// Does T exist for this key?
			heap.erase(i);						// Remove {key,T} from heap
		else
			result = true;
	}
	Unlock();								// *** END CRITICAL SECTION ***

	return result;
}

// If the pair {key,t} exists in the heap for the supplied argument key, returns
// a pointer to t is returned, else returns NULL.
template <class Key, class T, class Compare = less<Key> >
inline T*	manager<Key, T, Compare>::Find(const Key& key)
{
	T*	t_ptr = NULL;

	Lock();									// *** BEGIN CRITICAL SECTION ***
	{
		iterator	i = heap.find(key);			// Find T for this key

		if (i != heap.end())					// Does T exist for this key?
			t_ptr = &((*i).second);				// Get pointer to T
	}
	Unlock();								// *** END CRITICAL SECTION ***

	return t_ptr;
}

// If the pair {key,t} exists in the heap for the supplied argument key, returns
// a pointer to t is returned, else returns NULL.
template <class Key, class T, class Compare = less<Key> >
inline const T*		manager<Key, T, Compare>::Find(const Key& key) const
{
	const T*	t_ptr = NULL;

//	Lock();									// *** BEGIN CRITICAL SECTION ***
	{
		const_iterator	i = heap.find(key);		// Find T for this key

		if (i != heap.end())					// Does T exist for this key?
			t_ptr = &((*i).second);				// Get pointer to T
	}
//	Unlock();								// *** END CRITICAL SECTION ***

	return t_ptr;
}

//******************************************************************************
#endif
