#ifndef NCSA_H
#define NCSA_H

#define MAX_ENTRIES 10000

typedef struct {
    char *name;
    char *val;
} entry;

extern entry entries[MAX_ENTRIES];

int post_query();

#endif
