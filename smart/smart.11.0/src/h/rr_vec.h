#ifndef RR_VECH
#define RR_VECH
/*        $Header$*/

typedef struct {
    long  did;          /* document id */
    long  rank;         /* Rank of this document */
    float sim;          /* similarity of did to qid */
} RR_TUP;

typedef struct {
    long  qid;          /* query id */
    long  num_rr;       /* Number of tuples for rr_vec */
    RR_TUP *rr;         /* tuples.  Invariant: rr sorted increasing did */
} RR_VEC;

#endif /* RR_VECH */
