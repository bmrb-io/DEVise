#ifdef RCSID
static char rcsid[] = "$Header$";
#endif

/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/

#include "common.h"
#include "param.h"
#include "functions.h"
#include "direct.h"
#include "smart_error.h"

/* Destroy an instance of a 'direct' relational object. */
int
destroy_direct (file_name)
char *file_name;
{
    char buf[PATH_LEN];

    if (-1 == unlink (file_name)) {
        set_error (errno, file_name, "destroy_direct");
        return (UNDEF);
    }

    /* Unlink the var file */
    (void) sprintf (buf, "%s.var", file_name);
    if (-1 == unlink (buf)) {
        set_error (errno, buf, "destroy_direct");
        return (UNDEF);
    }
    return (0);
}
