#ifdef RCSID
static char rcsid[] = "$Header$";
#endif

/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/

/********************   PROCEDURE DESCRIPTION   ************************
 *0 print a set of tr evaluations of retrieval runs.
 *1 print.obj.tr_oeval
 *2 print_obj_tr_oeval (in_file, out_file, inst)
 *3   char *in_file;
 *3   char *out_file;
 *3   int inst;
 *4 init_print_obj_tr_oeval (spec, unused)
 *5   "print.trace"
 *4 close_print_obj_tr_oeval (inst)
 *7 in_file is taken to be a list of whitespace separated run specification
 *7 files.  If not VALID_FILE, then default is to use the smart invocation
 *7 spec file.
 *7 Tr_eval output to go into file "out_file" (if not VALID_FILE,
 *7 then stdout).
 *9 WARNING: Old style evaluation.  (21 point recall-prec instead of
 *9 11 point, 3pt average at .25 .50 .75)
***********************************************************************/

#include <fcntl.h>
#include "common.h"
#include "param.h"
#include "functions.h"
#include "smart_error.h"
#include "proc.h"
#include "spec.h"
#include "docindex.h"
#include "trace.h"
#include "buf.h"

static SPEC_PARAM spec_args[] = {
    TRACE_PARAM ("print.trace")
    };
static int num_spec_args = sizeof (spec_args) /
         sizeof (spec_args[0]);


static SPEC *save_spec;
static char *save_name = "Default_spec";
static SPEC_LIST save_spec_list = {&save_spec, &save_name, 1};

int
init_print_obj_tr_oeval (spec, unused)
SPEC *spec;
char *unused;
{

    if (UNDEF == lookup_spec (spec, &spec_args[0], num_spec_args))
        return (UNDEF);
    PRINT_TRACE (2, print_string, "Trace: entering init_print_obj_tr_oeval");

    save_spec = spec;

    PRINT_TRACE (2, print_string, "Trace: leaving init_print_obj_tr_oeval");
    return (0);
}

int
print_obj_tr_oeval (in_file, out_file, inst)
char *in_file;
char *out_file;
int inst;
{
    SPEC_LIST spec_list;
    SPEC_LIST *spec_list_ptr = &spec_list;
    FILE *output;
    SM_BUF output_buf;

    PRINT_TRACE (2, print_string, "Trace: entering print_obj_tr_oeval");

    if (VALID_FILE (in_file)) {
        if (UNDEF == get_spec_list (in_file, spec_list_ptr))
            return (UNDEF);
    }
    else {
        spec_list_ptr = &save_spec_list;
    }

    output = VALID_FILE (out_file) ? fopen (out_file, "w") : stdout;
    if (NULL == output)
        return (UNDEF);
    output_buf.size = 0;
    output_buf.end = 0;

    print_tr_oeval (spec_list_ptr, &output_buf);

    (void) fwrite (output_buf.buf, 1, output_buf.end, output);

    if (output != stdin)
        (void) fclose (output);

    PRINT_TRACE (2, print_string, "Trace: leaving print_obj_tr_oeval");
    return (1);
}


int
close_print_obj_tr_oeval (inst)
int inst;
{
    PRINT_TRACE (2, print_string, "Trace: entering close_print_obj_tr_oeval");

    PRINT_TRACE (2, print_string, "Trace: leaving close_print_obj_tr_oeval");
    return (0);
}
