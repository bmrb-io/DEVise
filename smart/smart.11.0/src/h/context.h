#ifndef CONTEXTH
#define CONTEXTH
/*  $Header$ */

/* Global variable describing context in which procedures are executing.
   For example, if weighting a vector, need to know whether to use
   doc_weight or query_weight values. (eg. context & INDEXING_DOC)
*/
extern long global_context;

#define CTXT_DOC (1)
#define CTXT_QUERY (1 << 1)
#define CTXT_INDEXING (1 << 2)
#define CTXT_INDEXING_DOC (CTXT_DOC | CTXT_INDEXING)
#define CTXT_INDEXING_QUERY (CTXT_QUERY | CTXT_INDEXING)

/* CTXT_LOCAL is defined for those procedures that want to define their
   own contexts, but don't want the definitions here.  All contexts without
   CTXT_LOCAL set are guaranteed to be accurate (and should be defined above).
   All contexts with CTXT_LOCAL set have no such guarantee. 
*/
#define CTXT_LOCAL (1 << 31)

#define set_context(x) global_context = (x)
#define check_context(x) ((x) == (global_context & (x)))
#define get_context() global_context
#define clr_context() global_context = 0
#define add_context(x) global_context |= (x)
#define del_context(x) global_context &= (~(x))

typedef long CONTEXT;

#endif /* CONTEXTH */
