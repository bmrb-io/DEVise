// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Traits Definition
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _TRAITS_H
#define    _TRAITS_H

#include  <stdlib.h>
#include  <string.h>
#include  <iostream.h>

#include  "bitmap.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

  // Enumeration of traits
enum trait_t {
                             // The first trait should be set to be =0

    Sorted_Trait=0,          // Sorted on this field.
    Unique_Trait,            // There will be no duplicates in this field.
    Consecutive_Trait,       // This field is consectutive.

    Null_Allowed_Trait,      // It's okay for this field to be empty.

    No_of_Traits    // Must be last
};


extern char *trait_name[No_of_Traits+1];

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

// Intended to be expandable as more traits are added.
#define    BM_SIZE       2
#define    MAX_TRAITS    (BM_SIZE * CHAR_BIT)

// Check to make sure the bitmap is large enough.
#if (No_of_Traits > MAX_TRAITS)
#error  "BM_SIZE is too small in " __FILE__ " at " __LINE__
#endif


class Traits {

  private:
    char      _traits[BM_SIZE];    // bitmap marking which traits are set. 

  public:

    Traits() { empty(); }

    void empty() { memset(_traits, '\0', BM_SIZE); }

    void set(trait_t t) { SET_BIT( _traits, (int) t ); }
    void clr(trait_t t) { CLR_BIT( _traits, (int) t ); }
    
    int  is_set(trait_t t) { return IS_SET( _traits, (int) t ); }
    int  is_clr(trait_t t) { return IS_CLR( _traits, (int) t ); }

      // Make a duplicate 
    Traits *dup();

      // Stream printing (for debugging)
    friend ostream& operator<< (ostream& out, const Traits& traits); 
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _TRAITS_H
