//******************************************************************************
// External Template Prototypes Module
// Copyright (c) 1997 DEVise Group
// DEVise Color Management
//******************************************************************************
// File: templates.C
// Last modified: Wed Nov 26 17:17:11 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 970429 [weaver]: Original file.
// 970509 [weaver]: Added instantiations required by linker for color classes.
// 970512 [weaver]: Added instantiations for all other templates in DEVise.
// 970925 [weaver]: Updated to C++ standard libraries.
// 971002 [weaver]: Added instantiations for recent, new DEVise template usage.
//
//******************************************************************************
//
// Collection of all template instantiations use in the project. For use with
// the -fno-implicit-templates g++ flag. Whenever a template instantiation is
// added anywhere which is not already on this list, add it. Think of this
// file as a common collection point for template typedefs.
//
// The purpose of this technique is to keep the code in a pristine state for
// template-savvy compilers/linkers. (Come on, g++ 2.8!!!)
//
// Special note: when using STL iterators with this method, you can use
// const iterators in const methods, non-const iterators in non-const methods.
// The first is required anyway, the second doesn't cost anything. This
// sometimes circumvents the "painful stuff", but changes the main body of code.
//
// The "painful stuff" is sometimes necessary when some types in the primary
// template are not instantiated by "template class." Paste this in as they
// come up at link time. It may also be necessary to create redundant code
// in junk functions to get instantiations of class-nested types.
//
//******************************************************************************
// Changed to non-pragma templates method in these files (leaving the pragmas
// in may be OK, as they serve other purposes - see the g++ man pages).
//
// DTE/Core/QueryTree.c
// DTE/Core/queue.h
// DTE/DeviseSpecific/TuplePtr.Plex.c		<-- Change back (GNU code)?
// DTE/DeviseSpecific/TuplePtr.Plex.h		<-- Change back (GNU code)?
// DTE/DeviseSpecific/TuplePtr.XPlex.c		<-- Change back (GNU code)?
// DTE/DeviseSpecific/TuplePtr.XPlex.h		<-- Change back (GNU code)?
// DTE/RTree/bulk_data.C
// DTE/RTree/port_genrtree.C
// DTE/RTree/solaris_stats.[hC]
//
// graphics/AssoArray.h
//
// graphics.new/DataSourceSegment.[hc]
// graphics.new/GdataStat.C
// graphics.new/MapIntToInt.C
// graphics.new/PointStorage.C
// graphics.new/SBufMgr.C
// graphics.new/StringStorage.C
//
// tape/BinaryTable.h
// tape/HashTable.h
// tape/SortedTable.h
// tape/Templates.C
//
//******************************************************************************

//******************************************************************************
// Libraries
//******************************************************************************

#include "Color.h"

#include "types.h"
#include "DateTime.h"
#include "ExecExpr.h"
#include "MemoryMgr.h"

#include <stl.h>
#include <string>

//******************************************************************************
// Template Instantiations (Easy Stuff, Main Code)
//******************************************************************************

// DTE/Core/QueryTree.c (and a slew of other files)
#include "queue.h"
#include "HashTable.h"
#include "Array.h"

// From Exit.h, needed for a #define dependency in SortedTable.h
#define DOASSERT(c,r)	{}
#include "SortedTable.h"

class BaseSelection;
class Site;
class JoinTable;
class String;
class TableAlias;
class ConstantSelection;

template class MemoryLoaderTemplate<EncodedDTF>;
template class MemoryLoaderTemplate<IDouble>;

struct StringLess {
	bool operator()(const string& s1, const string& s2) const {
		return s1 < s2;
	}
};

template class List<BaseSelection*>;
template class List<Tuple*>;
template class List<Site*>;
template class List<JoinTable*>;
template class List<String*>;
template class List<TableAlias*>;
template class List<char*>;
template class List<ConstantSelection*>;
template class List<List<Tuple*>*>;
template class List<void**>;		// Necessary; can't find original use
template class List<List<void**>*>;	// Necessary; can't find original use
template class set<basic_string<char, string_char_traits<char> >, ltstr>;
template class List<basic_string<char, string_char_traits<char> > *>;
template class List<void*>;
template class List<void *const *>;
template class vector<BaseSelection*>;
template class vector<TableAlias*>;
template class Array<ExecExpr*>;
template class Array<void const *>;
template class map<basic_string<char, string_char_traits<char> >, BaseSelection*, StringLess>;
template class rb_tree<basic_string<char, string_char_traits<char> >, pair<basic_string<char, string_char_traits<char> > const, BaseSelection *>, select1st<pair<basic_string<char, string_char_traits<char> > const, BaseSelection *>, basic_string<char, string_char_traits<char> > >, StringLess>;

template class deque<void*>;

// graphics.new/GdataStat.C
class BasicStats;
template class HashTable<int, BasicStats*>;

// graphics.new/MapIntToInt.C
template class HashTable<int, int>;

// graphics.new/PointStorage.C
#include "PointStorage.h"
template class HashTable<RecId, PointStorage::PointRec>;

// graphics.new/StringStorage.C
template class HashTable<char*, int>;
template class HashTable<int, char*>;

// graphics.new/TDataViewX.c
template class HashTable<double, BasicStats*>;

// tape/Templates.C
template class SortedTable<Coord, RecId>;

// generic/StateMap.C
class stateMapRec;
template class HashTable<char*, stateMapRec*>;

//******************************************************************************
// Template Instantiations (Easy Stuff, Color Management)
//******************************************************************************

// ActivePalette.h
template class vector<PColorID>;	// long

// ColorManager.h
template class vector<RGB>;
template class less<ColorID>;	// ulong
template class manager<ColorID, ColorManager::Color, less<ColorID> >;
template class less<RGB>;
template class map<RGB, ColorID, less<RGB> >;

// Coloring.h
template class vector<ColorID>;

// Palette.h
template class vector<PaletteColor>;

// PaletteManger.h
//template class less<PaletteID>;	// ulong (REDUNDANT)
template class manager<PaletteID, Palette, less<PaletteID> >;

// XColorManager.h

//******************************************************************************
// Template Instantiations (Painful Stuff, Main Code)
//******************************************************************************

static void		template_junk1(void);

void template_junk1(void)
{
	set<string, ltstr>	tmp, tables, tables2;
	bool				incl = includes(tables.begin(), tables.end(),
										tmp.begin(), tmp.end());

	string		alias;

	tables.insert(alias);

	set_union(tables.begin(), tables.end(), 
		tables2.begin(), tables.end(), inserter(tables, tables.end()));

	Array<ExecExpr*>*	mySelect;

	destroyArray(*mySelect);
}

//******************************************************************************
// Template Instantiations (Painful Stuff, Color Management)
//******************************************************************************

// ColorManager.h
template class
    rb_tree<RGB,
            pair<const RGB, unsigned long>,
            select1st<pair<const RGB, unsigned long>, RGB>,
            less<RGB> >;
template class
    rb_tree<unsigned long,
            pair<unsigned long const, ColorManager::Color>,
            select1st<pair<unsigned long const, ColorManager::Color>,
	                  unsigned long>,
	        less<unsigned long> >;

// XColorManager.h
static void		template_junk(void);
static bool		junk1(const RGB& rgb1, const RGB& rgb2);

void template_junk(void)
{
	RGBList		list;
	sort(list.begin(), list.end(), junk1);
}

bool	junk1(const RGB& rgb1, const RGB& rgb2) { return true; }

//******************************************************************************

