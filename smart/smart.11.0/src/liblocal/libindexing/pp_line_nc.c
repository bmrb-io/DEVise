#ifdef RCSID
static char rcsid[] = "$Header$";
#endif
/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/

/********************   PROCEDURE DESCRIPTION   ************************
 *0 pre-parser back-end that preparses doc according to doc desc info
 *2 pp_line_nc (input_doc, out_pp)
 *3   TEXTLOC *input_doc;
 *3   SM_INDEX_TEXTDOC *output_doc;

 *4 init_pp_line_nc (pp_infop, pp_infile, inst)
 *4 close_pp_line_nc (inst)
 *4 pp_copy_nc (ptr) returns char *
 *4 pp_discard_nc (ptr) returns char *

 *7 Puts a preparsed document in output_doc which corresponds to either
 *7 the input_doc (if non-NULL), or the next document found from the list of
 *7 documents in doc_loc (or query_loc if indexing query)
 *7 Returns 1 if found doc to preparse, 0 if no more docs, UNDEF if error
 *7 Document pre-parsed according to info in pp_infop.  List of docs to
 *7 preparse comes from pp_infile.
 *7 This version (as opposed to pp_line) does no copying of text.  Instead
 *7 it just assigns pointers into the original files.  Thus no transformation
 *7 of text can be done here.

 *8 pp_infop contains list of tokens that delimit sections when they 
 *8 occur at the beginning of a line.  The other info in pp_infop gives the
 *8 actions to be taken when that section is being preparsed.
 *8 pp_copy_nc and pp_discard_nc are globally known actions that can be used.

 *9 Warning: filenames in pp_infile that do not exist are silently ignored.
 *9 May ignore last line if not ended by a newline.
***********************************************************************/

#include <ctype.h>
#include <fcntl.h>
#include "common.h"
#include "param.h"
#include "smart_error.h"
#include "preparser.h"
#include "docindex.h"
#include "functions.h"
#include "inst.h"

#ifdef MMAP
#include <sys/types.h>
#include <sys/mman.h>
#define MADVISE
int madvise();
#endif

/* Increment to increase size of buf by */ 
#define DOC_SIZE 16100 
/* Increment to increase size of section storage by */
#define DISP_SEC_SIZE 60

static int pp_put_section();
static int lookup_section_start(), init_lookup_section_start();
static int get_next_doctext();

/* Static info to be kept for each instantiation of this proc */
typedef struct {
    /* bookkeeping */
    int valid_info;

    PP_INFO *pp_info;            /* Preparse table of actions */
    unsigned char lookup_table[256]; /* lookup_table[c] is index pointing to
                                    pp_info->section which starts with
                                    ascii char c */
    char *pp_infile;             /* File containing list of files to preparse*/
    FILE *input_list_fd;         /* FILE pointer for above */

    char *doc_file;              /* Name of current file being preparsed */
    char *orig_buf;              /* incore copy of part of doc_file. */
    long end_orig_buf;           /* Current end of orig_buf */
    long max_orig_buf;           /* Max size of allocation for orig_buf */
    long doc_offset;             /* current byte offset within orig_buf */
    long orig_buf_offset;        /* Byte offset of orig_buf within doc_file */


    SM_DISP_SEC *mem_disp;        /* Section occurrence info for doc giving
                                     byte offsets within buf */
    int num_sections;             /* Number of sections seen in doc */
    unsigned max_num_sections;    /* max current storage reserved for
                                     mem_disp */
} STATIC_INFO;

static STATIC_INFO *info;
static int max_inst = 0;

int
init_pp_line_nc (pp_infop, pp_infile)
PP_INFO *pp_infop;
char *pp_infile;
{
    STATIC_INFO *ip;
    int new_inst;

    NEW_INST (new_inst);
    if (UNDEF == new_inst)
        return (UNDEF);

    ip = &info[new_inst];

    /* Save input parameters */
    ip->pp_info = pp_infop;
    ip->pp_infile = pp_infile;

    /* Open file giving list of files to be parsed */
    if (pp_infile == NULL) {
        /* Cannot use list of files */
        ip->input_list_fd = NULL;
    }
    else if (! VALID_FILE (pp_infile)) {
        ip->input_list_fd = stdin;
    }
    else {
        if (NULL == (ip->input_list_fd = fopen (pp_infile, "r"))) {
            set_error (errno, pp_infile, "init_preparser");
            return (UNDEF);
        }
    }

    if (NULL == (ip->doc_file = malloc (PATH_LEN)) ||
        NULL == (ip->mem_disp =  (SM_DISP_SEC *) malloc (DISP_SEC_SIZE *
                             sizeof (SM_DISP_SEC))))
        return (UNDEF);

    if (UNDEF == init_lookup_section_start (ip->pp_info->pp_sections,
                                            ip->pp_info->num_pp_sections,
                                            ip))
        return (UNDEF);

    ip->max_num_sections = DISP_SEC_SIZE;
    ip->max_orig_buf = 0;
    ip->end_orig_buf = 0;
    ip->doc_offset = 0;
    ip->orig_buf_offset = 0;

    ip->valid_info = 1;

    return (new_inst);
}


/* Return the preparsed next document (in output_doc) to caller.  If input_doc
   is NULL, next document comes from the list of document files given in 
   in_fd (or from current doc file opened by last calls to pp_line_nc).
   If input_doc is non-NULL, ignore the list of files, and assume that
   input_doc contains exactly one complete document, to be preparsed.
*/
int
pp_line_nc (input_doc, output_doc, inst)
TEXTLOC *input_doc;
SM_INDEX_TEXTDOC *output_doc;
int inst;
{
    STATIC_INFO *ip;

    PP_SECTIONS *new_secptr, *current_secptr;
    int in_section_flag;
    long curr_loc;
    int status;

    if (! VALID_INST (inst)) {
        set_error (SM_ILLPA_ERR, "Instantiation", "pp_line_nc");
        return (UNDEF);
    }

    ip = &info[inst];

    if (1 != (status = get_next_doctext (input_doc, ip)))
        return (status);

    output_doc->textloc_doc.title = NULL;
    output_doc->mem_doc.title = NULL;
    output_doc->mem_doc.id_num = output_doc->id_num;
    output_doc->textloc_doc.id_num = output_doc->id_num;
    output_doc->textloc_doc.begin_text = ip->doc_offset + ip->orig_buf_offset;
    output_doc->textloc_doc.file_name = ip->doc_file;
    output_doc->textloc_doc.doc_type = ip->pp_info->doc_type;
    output_doc->doc_text = &ip->orig_buf[ip->doc_offset];
    ip->num_sections = 0;
    ip->mem_disp[0].begin_section = 0;
    ip->mem_disp[0].section_id = DISCARD_SECTION;

    current_secptr = &ip->pp_info->default_section;
    in_section_flag = FALSE;
    curr_loc = ip->doc_offset;

    while (1) {
        /* Does doc line start a new section? */
        if (lookup_section_start (&ip->orig_buf[curr_loc], &new_secptr, ip)) {
            /* If new section begins a new doc, then we are done (except
               of course if this is the first indexable line of doc) */
            if ((new_secptr->flags & PP_NEWDOC) &&
                ip->num_sections > 0) {
                break;
            }
            /* Add new section title (if needed) */
            if (UNDEF == pp_put_section (new_secptr, curr_loc, ip))
                return (UNDEF);
            current_secptr = new_secptr;
            curr_loc += new_secptr->name_len;
        }
        else if (in_section_flag == FALSE) {
            current_secptr = &ip->pp_info->default_section;
            if (UNDEF == pp_put_section (current_secptr, curr_loc, ip))
                return (UNDEF);
        }
        /* Skip to end of line */
        while (ip->orig_buf[curr_loc] != '\n')
            curr_loc++;
        curr_loc++;

        /* If current section is a one-line section, must ensure get
           new section next time through */
        in_section_flag = (current_secptr->flags & PP_ONELINE) 
                ? FALSE
                : TRUE;

        /* Break from loop if at end of doc file */
        if (curr_loc >= ip->end_orig_buf) {
            break;
        }
    }

    /* Output end of old document */
    output_doc->textloc_doc.end_text = ip->orig_buf_offset +
                                       MIN(curr_loc, ip->end_orig_buf);
    if (UNDEF == pp_put_section ((PP_SECTIONS *) NULL, curr_loc, ip))
        return (UNDEF);

    ip->doc_offset = curr_loc;
    output_doc->mem_doc.sections = ip->mem_disp;
    output_doc->mem_doc.num_sections = ip->num_sections;

    return (1);
}

int
close_pp_line_nc (inst)
int inst;
{
    STATIC_INFO *ip;

    if (! VALID_INST (inst)) {
        set_error (SM_ILLPA_ERR, "Instantiation", "close_pp_line_nc");
        return (UNDEF);
    }
    ip = &info[inst];
    ip->valid_info = 0;

    (void) free (ip->doc_file);
    (void) free ((char *) ip->mem_disp);
    if (ip->max_orig_buf > 0) {
        (void) free (ip->orig_buf);
    }
#ifdef MMAP
    else if (ip->end_orig_buf > 0) {
        if (UNDEF == munmap (ip->orig_buf, (int) ip->end_orig_buf))
            return (UNDEF);
    }
#endif /* MMAP */

    if (VALID_FILE (ip->pp_infile)) {
        if (UNDEF == fclose (ip->input_list_fd)) {
            return (UNDEF);
        }
    }

    return (0);
}

static int
pp_put_section (section, curr_loc, ip)
PP_SECTIONS *section;
long curr_loc;
STATIC_INFO *ip;
{
    /* End previous section */
    if (ip->num_sections > 0 &&
        ip->mem_disp[ip->num_sections - 1].end_section == 0)
        ip->mem_disp[ip->num_sections - 1].end_section =
            curr_loc - ip->doc_offset;

    if (section == (PP_SECTIONS *) NULL || section->id == DISCARD_SECTION) {
        return (0);
    }

    if (ip->num_sections >= ip->max_num_sections) {
        if (NULL == (ip->mem_disp = (SM_DISP_SEC *)
                     realloc ((char *) ip->mem_disp,
                              (ip->max_num_sections + DISP_SEC_SIZE) *
                              sizeof (SM_DISP_SEC)))) {
            set_error (errno, "DISP_SEC malloc", "preparse");
            return (UNDEF);
        }
        ip->max_num_sections += DISP_SEC_SIZE;
    }
    ip->mem_disp[ip->num_sections].begin_section =
        curr_loc + section->name_len - ip->doc_offset;
    ip->mem_disp[ip->num_sections].end_section = 0;
    ip->mem_disp[ip->num_sections].section_id = section->id;
    ip->num_sections++;
    return (0);
}

/* Determine if this line starts a new section by comparing it
   with the section starters contained in pp_section
   Note section is sorted in descending alphabetic order.
   Return 1 if a new section is found, and set new_secptr.
   Return 0 if no new section is found
*/
static int
lookup_section_start (text, new_secptr, ip)
char *text;
PP_SECTIONS **new_secptr;
STATIC_INFO *ip;
{
    PP_SECTIONS *sec_ptr;

    if (255 != ip->lookup_table[*text]) {
        sec_ptr = &ip->pp_info->pp_sections[ip->lookup_table[*text]];
        do {
            if (0 == strncmp (text, 
                              sec_ptr->name, 
                              (int) sec_ptr->name_len)) {
                /* Have found new section starter. */
                *new_secptr = sec_ptr;
                return (1);
            }
            sec_ptr++;
        } while (*text == sec_ptr->name[0]);
    }
    return (0);
}

/* Initialize lookup_table and do some simple sanity checking on the
   preparser sections array */
static int
init_lookup_section_start (sections, num_sections, ip)
PP_SECTIONS *sections;
long num_sections;
STATIC_INFO *ip;
{
    long i;
    char c;
    for (i = 0; i < 256; i++)
        ip->lookup_table[i] = 255;
    if (0 >= num_sections || num_sections > 254) {
        set_error (SM_INCON_ERR,
                   "Improper number of sections. Max 254",
                   "pp_preparse");
        return (UNDEF);
    }
    for (i = 1; i < num_sections; i++) {
        if (sections[i].name[0] > sections[i-1].name[0]) {
            set_error (SM_INCON_ERR,
                       "Out of order section start",
                       "pp_preparse");
            return (UNDEF);
        }
    }

    for (i = 0; i < num_sections; i++) {
        c = sections[i].name[0];
        if (! isascii(c)) {
            set_error (SM_INCON_ERR, "Nonascii section start", "pp_preparse");
            return (UNDEF);
        }
        if (ip->lookup_table[c] == 255)
            ip->lookup_table[c] = i;
    }
    return (0);
}

        
static int
get_next_doctext (input_doc, ip)
TEXTLOC *input_doc;
STATIC_INFO *ip;
{
    int doc_fd;
    int doc_length;

    if (input_doc != NULL) {
        /* Open doc_file and read doc into memory */
        (void) strcpy (ip->doc_file, input_doc->file_name);
        doc_length = input_doc->end_text - input_doc->begin_text;
        if (doc_length > ip->max_orig_buf) {
            if (ip->max_orig_buf > 0) {
                (void) free (ip->orig_buf);
            }
            ip->max_orig_buf += doc_length + 1;
            if (NULL == (ip->orig_buf = malloc ((unsigned) ip->max_orig_buf)))
                return (UNDEF);
        }
        if (-1 == (doc_fd = open (ip->doc_file, O_RDONLY)) ||
            -1 == lseek (doc_fd, input_doc->begin_text, 0) ||
            doc_length != read (doc_fd, ip->orig_buf, doc_length) ||
            -1 == close (doc_fd)) {
            if (doc_fd != -1)
                (void) close (doc_fd);
            return (0);
        }
        ip->orig_buf[doc_length] = '\n';
        ip->end_orig_buf = doc_length;
        ip->orig_buf_offset = input_doc->begin_text;
        ip->doc_offset = 0;
        return (1);
    }

    /* Document should come from list of docs contained in pp_infile */
    if (NULL == ip->input_list_fd)
        return (0);
    while (ip->doc_offset >= ip->end_orig_buf) {
        /* Need to get new file for the next document */
        if (NULL == fgets (ip->doc_file, PATH_LEN, ip->input_list_fd)) {
            /* No more doc file names to read */
            return (0);
        }
        /* Remove trailing \n from doc_file */
        ip->doc_file[strlen(ip->doc_file) - 1] = '\0';
        if (-1 == (doc_fd = open (ip->doc_file, O_RDONLY)) ||
            -1 == (doc_length = lseek (doc_fd, 0L, 2)) ||
            -1 == lseek (doc_fd, 0L, 0) ||
            doc_length == 0) {
            /* Just skip this file (instead of returning 0?) */
            continue;
        }
#ifdef MMAP
        /* Remove any previous mapping */
        if (ip->end_orig_buf > 0) {
            if (-1 == munmap (ip->orig_buf, (int) ip->end_orig_buf))
                return (UNDEF);
        }
        if ((char *) -1 == (ip->orig_buf = (char *)
                               mmap ((caddr_t) 0,
                                     (size_t) doc_length,
                                     PROT_READ,
                                     MAP_SHARED,
                                     doc_fd,
                                     (off_t) 0)))
            return (UNDEF);
#ifdef MADVISE
        /* Advise that we are doing sequential access to this file */
        if (-1 == madvise ((caddr_t) ip->orig_buf,
                           (size_t) doc_length,
                           MADV_SEQUENTIAL))
            return (UNDEF);
#endif MADVISE

        /* Must check sentinel (end of file must be '\n', if not then ignore
           last line) */
        if (ip->orig_buf[doc_length-1] != '\n') {
            while (doc_length > 0 && ip->orig_buf[doc_length-1] != '\n') {
                doc_length--;
            }
        }

        ip->end_orig_buf = doc_length;
        if (-1 == close (doc_fd))
            continue;
    
#else
        if (doc_length > ip->max_orig_buf) {
            if (ip->max_orig_buf > 0) {
                (void) free (ip->orig_buf);
                (void) free (ip->buf);
            }
            ip->max_orig_buf += doc_length + 1;
            if (NULL == (ip->orig_buf = malloc ((unsigned) ip->max_orig_buf))||
                NULL == (ip->buf = malloc ((unsigned) ip->max_orig_buf)))
                return (UNDEF);
        }
        if (doc_length != read (doc_fd, ip->orig_buf, doc_length) ||
            -1 == close (doc_fd)) {
            if (doc_fd != -1)
                (void) close (doc_fd);
            continue;
        }
        ip->orig_buf[doc_length] = '\n';
        ip->end_orig_buf = doc_length;
#endif  /* MMAP */
        ip->orig_buf_offset = 0;
        ip->doc_offset = 0;
        return (1);
    }

    /* Already in the middle of getting docs from a file */
    return (1);
}
