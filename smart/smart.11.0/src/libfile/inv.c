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
#include "inv.h"
#include "direct_type.h"
#include "io.h"

/* ARGSUSED */
int
create_inv (file_name, rh)
char *file_name;
REL_HEADER *rh;
{
    REL_HEADER temp_rh;
    temp_rh.type = S_RH_INV;
    return ( create_direct (file_name, &temp_rh));
}

int
open_inv (file_name, mode)
char *file_name;
long mode;
{
    if (mode & SCREATE) {
        if (UNDEF == create_inv (file_name, (REL_HEADER *) NULL)) {
            return (UNDEF);
        }
        mode &= ~SCREATE;
    }
    return ( open_direct (file_name, mode) );
}

int
seek_inv (index, inv_entry)
int index;
INV *inv_entry;
{
    return ( seek_direct (index, (char *) inv_entry) );
}

int
read_inv (index, inv_entry)
int index;
INV *inv_entry;
{
    return ( read_direct (index, (char *) inv_entry) );
}

int
write_inv (index, inv_entry)
int index;
INV *inv_entry;
{
    return ( write_direct (index, (char *) inv_entry) );
}

int
close_inv (index)
int index;
{
    return ( close_direct (index) );
}


int
destroy_inv(filename)
char *filename;
{
    return(destroy_direct(filename));
}

int
rename_inv(in_file_name, out_file_name)
char *in_file_name;
char *out_file_name;
{
    return(rename_direct(in_file_name, out_file_name));
}
