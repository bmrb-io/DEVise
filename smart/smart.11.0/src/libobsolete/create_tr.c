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
#include "rel_header.h"
#include "tr.h"
#include "io.h"
#include "smart_error.h"

/* Create a new rel_rank relation with name 'file_name' */
/* Return UNDEF if impossible to create */
/* ARGSUSED */
int
create_tr (file_name, rh)
char * file_name;
REL_HEADER *rh;
{

    FILE *fd;
    REL_HEADER real_rh;

    real_rh.max_primary_value = 0;
    real_rh.num_entries = 0;
    real_rh._entry_size = sizeof (TR);
    real_rh.type = S_RH_TR;
    real_rh.data_type = S_RD_TR;
    real_rh._internal = 0;
    real_rh.max_secondary_value = 0;

    if (NULL == (fd = fopen (file_name, "w"))) {
        set_error (errno, file_name, "create_tr");
        return (UNDEF);
    }

    if (1 != fwrite ((char *) &real_rh, sizeof (REL_HEADER), 1, fd)) {
        set_error (errno, file_name, "create_tr");
        return (UNDEF);
    }

    (void) fclose (fd);
    
    return (0);
}

