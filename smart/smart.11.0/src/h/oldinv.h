#ifndef OLDINVH
#define OLDINVH
/*  $Header$ */

/* a single node visible to the user */
typedef struct {
        long  key_num;          /* key to access this inverted list with*/
        unsigned short num_list; /* Number of elements in this list */
        long  *list;            /* pointer to list elements  */
        float *list_weights;    /* Pointer to weights for corresponding */
                                /* elements */
} OLDINV;

#define REL_OLDINV 1
#endif /* OLDINVH */
