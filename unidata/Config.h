// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Various machine specific things that might need configuration
//   when porting to new platforms.
//
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _CONFIG_H
#define    _CONFIG_H

#include <time.h>

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

  // The user can use this special string in a converter string
  // to specify the place for the filename.
#define  CNV_FILE    "$file"

  // The shell to use when forking converters, and the option
  // needed by the shell to run a command in this way. 
#define  CNV_SHELL         "/bin/sh"
#define  CNV_SH_CMMD_OPT   "-c"

  // Characters with special meaning to the above shell, that
  // could be considered a security risk.
#define  SHELL_CHARS    ";&|!$~`*?+<>()[]{}"

  // A program equivilent to 'cat', read stdin, and dump it to stdout.
  // This must be in your path, or be an absolute path (starting with '/').
#define  CNV_CAT           "cat"

  // The default place to put (possibly large) temporary files.
#define  DFLT_TMP_DIR      "/tmp"

  // Name of the environment variable that overrides DFLT_TMP_DIR
#define  TMP_DIR           "DEVISE_TMP"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

// The alignments required by types.

// We're not concerned with alignments less than the size of the
// type, or machines that have no alignment restrictions.
// The alignments given here should always be large enough to hold
// the type.

#define DOUB_ALIGN  sizeof(double)
#define FLT_ALIGN   sizeof(float)
#define INT_ALIGN   sizeof(int)

  // The alignment required of struct tm, defined in <time.h>.
#define TM_ALIGN    sizeof(int)

  // The maximum alignment required by the above.
#define MAX_ALIGN   DOUB_ALIGN

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _CONFIG_H
