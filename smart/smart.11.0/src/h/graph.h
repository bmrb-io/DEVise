#ifndef GRAPHH
#define GRAPHH
/*  $Header$ */

/* a single node visible to the user */
typedef struct {
        long  node_num;         /* unique number for this node within  */
                                /* graph */
        long  info;             /* Information stored in node (often index */
                                /* into another relation or array) */
        short num_parents;      /* Number of other nodes which point to node*/
        short num_children;     /* Number of children of this node */
        long  *parents;         /* node_nums of parents of node. If NULL, */
                                /* there are no parents */
        long  *children;        /* node_nums of children of node. If NULL, */
                                /* there are no children */
        float *parent_weight;   /* Weights of each of the links in parents */
                                /* If NULL, there are no weights */
        float *children_weight; /* Weights of each of the links in children */
                                /* If NULL, there are no weights */
} GRAPH;

#define REL_GRAPH 0
#endif /* GRAPHH */
