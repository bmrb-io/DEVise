//******************************************************************************
// Debug Utilities Module v1.0
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
// File: debug.h
// Last modified: Thu Sep 25 16:31:01 1997 by Chris Weaver
//******************************************************************************
// Modification History:
//
// 961107 [weaver]: Original file.
// 961109 [weaver]: Added streams options. Separated debugging and tracing.
// 961121 [weaver]: Reordered definitions to fix warnings to due mutual inlines.
// 970925 [weaver]: Updated to C++ standard libraries.
//
//******************************************************************************
//
// Debug utilities. #define INLINE_DEBUG and INLINE_TRACE to use.
//
// Loosely based on (typically swanky) code by Nick Leavy. Thanks Nick!
//
// The non-locality of _LF_ in Assert() and Imply() needs to be fixed (pass
// in line and file like other functions).
//
//******************************************************************************

#ifndef __DEBUG_H
#define __DEBUG_H

//******************************************************************************
// Libraries
//******************************************************************************

#include <iostream.h>
#include <iomanip.h>
#include <cstring>
#include <errno.h>

//******************************************************************************
// Constants and Types
//******************************************************************************

#define _LF_ __LINE__, __FILE__

#define gDebugStream cout
#define gTraceStream cout
//#define gDebugStream cerr
//#define gTraceStream cerr

//******************************************************************************
// Externed globals
//******************************************************************************

//******************************************************************************
// Function Prototypes
//******************************************************************************

// Utility Functions
void    Message(int line, const char* file, const char* src = "",
            const char* msg = "", ostream& s = gTraceStream);
void    Fatal(int line, const char* file,
            const char* msg = "", ostream& s = gDebugStream);
void    Warn(int line, const char* file,
            const char* msg = "", ostream& s = gDebugStream);
void    Locate(int line, const char* file,
            const char* msg = "", ostream& s = gTraceStream);

// Debug Functions
void    Assert(int line, const char* file, bool test = true,
               const char* msg = "Assert failed", ostream& s = gDebugStream);
void    Imply(int line, const char* file, bool test = true,
              const char* msg = "Imply failed", ostream& s = gDebugStream);

// Trace Functions
void    Trace(const char* msg = "", ostream& s = gTraceStream);


//******************************************************************************
// Utility Functions (Inline)
//******************************************************************************

#if defined (INLINE_DEBUG) || defined (INLINE_TRACE)

inline void     Message(int line, const char* file, const char* src,
    const char* msg, ostream& s)
{
    s << endl << endl;
    s << src << ": " << msg << endl;
    s << "    in " << file << '(' << setw(4) << line << ')' << endl;

    if (errno)
        s << "    Errno: " << setw(4) << errno << ' '
             << strerror(errno) << endl;

    s << endl;
}

inline void     Fatal(int line, const char* file, const char* msg, ostream& s)
{
    Message(line, file, "FATAL ERROR", msg, s);

    s << "*** DUMPING CORE ***" << endl;

    *((int*)0) = 42;                                // Cause SIGSEGV
    exit(-1);
}

inline void     Warn(int line, const char* file, const char* msg, ostream& s)
{
    Message(line, file, "WARNING", msg, s);
}

inline void     Locate(int line, const char* file, const char* msg, ostream& s)
{
    s << msg << ": " << file << '(' << setw(4) << line << ')' << endl;
}

#else       // Nullify inline functions with no compiler warnings

inline void     Message(int line, const char* file, const char* src,
    const char* msg, ostream& s)
{
    ((void)line);
    ((void)file);
    ((void)src);
    ((void)msg);
    ((void)s);
}

inline void     Fatal(int line, const char* file, const char* msg, ostream& s)
{
    ((void)line);
    ((void)file);
    ((void)msg);
    ((void)s);
}

inline void     Warn(int line, const char* file, const char* msg, ostream& s)
{
    ((void)line);
    ((void)file);
    ((void)msg);
    ((void)s);
}

inline void     Locate(int line, const char* file, const char* msg, ostream& s)
{
    ((void)line);
    ((void)file);
    ((void)msg);
    ((void)s);
}

#endif

//******************************************************************************
// Debug Functions (Inline)
//******************************************************************************

#ifdef INLINE_DEBUG

inline void     Assert(int line, const char* file, bool test,
                       const char* msg, ostream& s)
{
    if (!test)
        Fatal(line, file, msg, s);
}

inline void     Imply(int line, const char* file, bool test,
                      const char* msg, ostream& s)
{
    if (!test)
        Warn(line, file, msg, s);
}

#else       // Nullify inline functions with no compiler warnings

inline void     Assert(int line, const char* file, bool test,
                       const char* msg, ostream& s)
{
    ((void)line);
    ((void)file);
    ((void)test);
    ((void)msg);
    ((void)s);
}

inline void     Imply(int line, const char* file, bool test,
                      const char* msg, ostream& s)
{
    ((void)line);
    ((void)file);
    ((void)test);
    ((void)msg);
    ((void)s);
}

#endif

//******************************************************************************
// Trace Functions (Inline)
//******************************************************************************

#ifdef INLINE_TRACE

inline void     Trace(const char* msg, ostream& s)
{
    s << msg << endl;
}

#else       // Nullify inline functions with no compiler warnings

inline void     Trace(const char* msg, ostream& s)
{
    ((void)msg);
    ((void)s);
}

#endif

//******************************************************************************
#endif

