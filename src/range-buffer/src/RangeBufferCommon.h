#ifndef __RANGEBUFFERCOMMON_H__
#define __RANGEBUFFERCOMMON_H__

/* max number of objects that are allowed to be cached in RBM */
#define MAX_NUM_RBMOBJECTS_CACHED 20

/* number of entries in the object hash table in range buffer manager */
#define NUM_RANGE_BUF_OBJ_HASH_ENTRY 7

/* hash the object to the right bucket */
#define RBM_HASH(obj)	(((int)(obj)) % NUM_RANGE_BUF_OBJ_HASH_ENTRY)

/* random is the only supported policy now */
enum ReplacementPolicyType {REPLACE_RANDOM};

class RangeBufferManager;
class ReplacementPolicy;
class ReplacementPolicyRandom;

#endif
