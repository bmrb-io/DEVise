#ifndef SIMP_INVH
#define SIMP_INVH
/*  $Header$ */

/* a single node visible to the user */
typedef struct {
        long  id_num;           /* key to access this inverted list with*/
        long  num_list;         /* Number of elements in this list */
        long  *list;            /* pointer to list elements  */
} SIMP_INV;

#define REL_SIMP_INV 4
#endif /* SIMP_INVH */
