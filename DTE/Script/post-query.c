#include <stdio.h>
#include <stdlib.h>

#define MAX_ENTRIES 10000

typedef struct {
    char *name;
    char *val;
} entry;

char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);

entry entries[MAX_ENTRIES];

int post_query(int content_length, FILE* fromsocket) {
    register int x,m=0;
    int cl = content_length;

	for(x=0;cl && (!feof(fromsocket));x++) {
        m=x;
        entries[x].val = fmakeword(fromsocket,'&',&cl);
        plustospace(entries[x].val);
        unescape_url(entries[x].val);
        entries[x].name = makeword(entries[x].val,'=');
    }

    return m + 1;
}
