static char rcsid[] = "$Header$";
# include <string.h>
# include "common.h"
# include "io.h"
#include <stdio.h>
#define GETC(x)    getc(x);doc_offset++
#define PATH_LEN 1024

#define TITLE_LEN    150
#define AUTHOR_LEN   150
#define JOURNAL_LEN  150
#define KEYWORD_LEN  350
#define TOTAL_LEN    TITLE_LEN+AUTHOR_LEN+JOURNAL_LEN+KEYWORD_LEN

#define TITLE_SECTION     'T'
#define AUTHOR_SECTION    'A'
#define JOURNAL_SECTION   'B'
#define KEYWORD_SECTION   'K'
#define CITATION_SECTION  'z'

char *usage = "Usage: pre_parser <list_of_filenames\n";

/* Transform collection in old pre_indexed form to new form */

main (argc, argv)
int argc;
char *argv[];
{
    FILE *fd;
    int doc_offset;
    int not_beginning;
    int c;
    char current_file[PATH_LEN];
    char *citation;
    int end_old_section;
    char new_section;
    int  cit;
    char *make_cit();

    if (argc != 1) {
        fprintf (stderr, "%s\n", usage);
        exit (1);
    }

    reset_cit();

    while (NULL != (gets(current_file))) {
        if (NULL == (fd = fopen (current_file, "r"))) {
            perror ("pre_parse");
            exit (2);
        }
        not_beginning = 0;
        doc_offset = 0;
        c = GETC(fd);
        printf (".i 0 %s\n", current_file);
        
        while (c != EOF) {
            /* c is first character in line at this point */
            if (c == '.') {
                c = GETC(fd);
                if (c == 'I') {
                    if (not_beginning) {
		        if (NULL != (citation = make_cit())) {
			    printf (".z %d %d\n", doc_offset-2, doc_offset-2);
			    printf ("%s\n", citation);
			    reset_cit ();
			}
                        printf (".o %d\n", doc_offset - 2);
                        printf (".i %d %s\n", doc_offset - 2, current_file);
                    }
                    /* Ignore rest of line */
                    while (c != EOF && c != '\n') {
                        c = GETC (fd);
                    }
                }
                else if (c >= 'A' && c <= 'Z') {
		    if (c == TITLE_SECTION   || c == AUTHOR_SECTION ||
			c == JOURNAL_SECTION || c == KEYWORD_SECTION)
		      cit = c;
		    else
		      cit = '\0';
                    end_old_section = doc_offset - 2;
                    new_section = c;
                    /* Ignore rest of line */
                    while (c != EOF && c != '\n') {
                        c = GETC (fd);
                    }
                    printf (".%c %d %d\n", 
                            new_section - 'A' + 'a',
                            end_old_section,
                            doc_offset);
                }
                else {
                    printf ("*****ERROR*****\n");
                    fprintf (stderr, 
                             "Illegal section at offset %d\n",
                             doc_offset);
                }
                not_beginning = 1;
            }
            else {
                /* echo rest of line */
                while (c != EOF && c != '\n') {
                    putchar (c);
		    if (cit)
		        append_cit (c, cit);
                    c = GETC (fd);
                }
                putchar ('\n');
	        if (cit)
		    append_cit ('\n', cit);
            }
            if (c != EOF) {
                c = GETC (fd);
            }
        }

        if (NULL != (citation = make_cit())) {
	    printf (".z %d %d\n", doc_offset-1, doc_offset-1);
	    printf ("%s\n", citation);
	}
        printf (".o %d\n", doc_offset - 1);
    }

    exit (0);
}


char title[TITLE_LEN];          /* Title for current document */
char author[AUTHOR_LEN];        /* Author for current document */
char journal[JOURNAL_LEN];      /* Journal for current document */
char keyword[KEYWORD_LEN];      /* Keyword for current document */
 
static int length_title = 0;
static int length_author = 0;
static int length_journal = 0;
static int length_keyword = 0;
static int num_citations = 0;

char *title_ptr;   /* Position within current doc */
char *author_ptr;  /* Position within current doc */
char *journal_ptr; /* Position within current doc */
char *keyword_ptr; /* Position within current doc */

int title_section;          /* Boolean: 1 if currently scanning title */
int author_section;         /* Boolean: 1 if currently scanning author */
int journal_section;        /* Boolean: 1 if currently scanning journal */
int keyword_section;        /* Boolean: 1 if currently scanning keyword */

char *last_author;          /* Position of the last author included */
int  new_author;            /* Boolean: 1 if starting new author */

reset_cit()
{
    title_section = 1;
    title_ptr = title;
    *title_ptr = '\0';
    author_section = 1;
    author_ptr = last_author = author;
    *author_ptr = '\0';
    new_author = 0;
    journal_section = 1;
    journal_ptr = journal;
    *journal_ptr = '\0';
    keyword_section = 1;
    keyword_ptr = keyword;
    *keyword_ptr = '\0';

}

append_cit(c,section)
char  c;
char  section;
{
    register char *iptr;

    if (TITLE_SECTION == section && title_section) {
        /* Add c to the currently constructed title */
        switch (c) {
            case ' ':
            case '\t':
            case '\n':
                if (title_ptr != title &&  *(title_ptr - 1) != ' ') {
		    *title_ptr++ = ' ';
                 }
                 break;
             default:
                 *title_ptr++ = c;
        }
        if (title_ptr == &title[TITLE_LEN-1]) {
            *title_ptr = '\0';
            title_ptr = title;
            title_section = 0;
        }
    }
        
    if (AUTHOR_SECTION == section && author_section) {
        /* Add c to the currently constructed author */
        switch (c) {
            case ' ':
            case '\t':
                if (author_ptr != author && *(author_ptr - 1) != ' ') {
                    *author_ptr++ = ' ';
                }
                break;
            case '\n':
                new_author = 1;
                break;
            default:
		if (new_author) {
		    if (author_ptr != author) {
		        *author_ptr++ = ',';
			*author_ptr++ = ' ';
			last_author = author_ptr;
		    }
		    new_author = 0;
		}
                *author_ptr++ = c;
        }
        if (author_ptr == &author[AUTHOR_LEN-4]) {
	    if (last_author != author) {
	        for (iptr = author_ptr - 1; iptr >= last_author; iptr--)
		    *(iptr+4) = *iptr;
		author_ptr += 4;
		*last_author++ = 'a';
		*last_author++ = 'n';
		*last_author++ = 'd';
		*last_author++ = ' ';
		last_author = author;
	    }
	      
            *author_ptr = '\0';
            last_author = author;
	    new_author = 0;
            author_section = 0;
        }
    }
        
    if (JOURNAL_SECTION == section && journal_section) {
        /* Add c to the currently constructed journal */
        switch (c) {
            case ' ':
            case '\t':
            case '\n':
                if (journal_ptr != journal && *(journal_ptr - 1) != ' ') {
                    *journal_ptr++ = ' ';
                }
                break;
            default:
                *journal_ptr++ = c;
        }
        if (journal_ptr == &journal[JOURNAL_LEN-1]) {
            *journal_ptr = '\0';
            journal_ptr = journal;
            journal_section = 0;
        }
    }
        
    if (KEYWORD_SECTION == section && keyword_section) {
        /* Add c to the currently constructed keyword */
        switch (c) {
            case ' ':
            case '\t':
            case '\n':
                if (keyword_ptr != keyword && *(keyword_ptr - 1) != ' ') {
                    *keyword_ptr++ = ' ';
                }
                break;
            default:
                *keyword_ptr++ = c;
        }
        if (keyword_ptr == &keyword[KEYWORD_LEN-1]) {
            *keyword_ptr = '\0';
            keyword_ptr = keyword;
            keyword_section = 0;
        }
    }
}        



char *make_cit()
{
    int  len_title;
    int  len_author;
    int  len_journal;
    int  len_keyword;
    static char buffer[TOTAL_LEN];
    int  not_null = 0;
    register char *iptr;

    if (title_ptr != title)
        *title_ptr = '\0';
    if (author_ptr != author) {
        if (last_author != author) {
	    for (iptr = author_ptr - 1; iptr >= last_author; iptr--)
	        *(iptr+4) = *iptr;
	    author_ptr += 4;
	    *last_author++ = 'a';
	    *last_author++ = 'n';
	    *last_author++ = 'd';
	    *last_author++ = ' ';
	}
        *author_ptr = '\0';
    }
    if (journal_ptr != journal)
        *journal_ptr = '\0';
    if (keyword_ptr != keyword)
        *keyword_ptr = '\0';

    len_title = strlen(title);
    len_author = strlen(author);
    len_journal = strlen(journal);
    len_keyword = strlen(keyword);

    length_title += len_title;
    length_author += len_author;
    length_journal += len_journal;
    length_keyword += len_keyword;
    num_citations++;

    *buffer = '\0';

    if (len_title) {
	not_null = 1;
        iptr = &title[len_title-1];
	while (*iptr == ' ')
	     iptr--;
	if (*iptr != '.' && *iptr != '!' && *iptr != '?') {
	    *++iptr = '.';
	    *++iptr = '\0';
	}
	strcat(buffer,title);
    }

    if (len_author) {
        if (not_null)
	    strcat(buffer," ");
	else
	    not_null = 1;
        strcat(buffer,author);
    }

    if (len_journal) {
        if (not_null)
	    strcat(buffer," ");
	else
	    not_null = 1;
        iptr = &journal[len_journal-1];
	while (*iptr == ' ')
	     iptr--;
	if (*iptr != '.' && *iptr != '!' && *iptr != '?') {
	    *++iptr = '.';
	    *++iptr = '\0';
	}
        strcat(buffer,journal);
    }

/***********
    if (len_keyword) {
        if (not_null)
            strcat(buffer,"\n");
	else
	    not_null = 1;
        strcat(buffer,keyword);
    }
***********/

    if (buffer[0] == '\0')
        return(NULL);
    return (buffer);
}



