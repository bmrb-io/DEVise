static char rcsid[] = "$Header$";
#include <stdio.h>
#define GETC(x)    getc(x);doc_offset++
#define PATH_LEN 1024

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
    int end_old_section;
    char new_section;

    if (argc != 1) {
        fprintf (stderr, "%s\n", usage);
        exit (1);
    }

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
                        printf (".o %d\n", doc_offset - 2);
                        printf (".i %d %s\n", doc_offset - 2, current_file);
                    }
                    /* Ignore rest of line */
                    while (c != EOF && c != '\n') {
                        c = GETC (fd);
                    }
                }
                else if (c >= 'A' && c <= 'Z') {
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
                    c = GETC (fd);
                }
                putchar ('\n');
            }
            if (c != EOF) {
                c = GETC (fd);
            }
        }

        printf (".o %d\n", doc_offset - 1);
    }

    exit (0);
}
