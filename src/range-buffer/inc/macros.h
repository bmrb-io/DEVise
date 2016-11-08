/* all numbers are assumed to be of type Coord */
typedef double Coord;

/* no more record */
#define NO_MORE_RECORD -1

/* trying to cache an object that's already cached in RBM */
#define ALREADY_CACHED -101

/* trying to stop caching an object that is actually not cached at all */
#define NOT_CACHED -102

/* try to stop caching an object when it is still pinned */
#define OBJECT_PINNED -103

/* try to open scan on an object which has already pinned */
#define OBJECT_ALREADY_PINNED -104

/* try to find a victim object but cannot */
#define NO_VICTIM_AVAILABLE -105

/* error happened when inserting a record returned by low level RAM into RBM */
#define ERROR_AT_INSERT -106

/* try to openScan when another scan is still outstanding */
#define SCAN_OUTSTANDING -107

/* cannot generate low level bounding box */
#define CANNOT_GENERATE_LLBB -108

/* Cannot find the BBoxEntry for a new record. */
/* This is an error since it should have been created in */
/* previous createHoleBBoxes() call */
#define BBOXENTRY_NOT_EXIST -109

/* When trying to insert a new record into RBM, failed to find/create an page */
/* for it. Has to give up */
#define NO_PAGE_FOR_REC -110

/* Telling the actions or status of the program ("FYI" to user) */
#ifdef ALWAYS_TELL_FACT
    #define TELL_FACT(f) 	{printf(f); printf("\n");}
#else
    #define TELL_FACT(f) 	{}
#endif

/* Report error ("FYI" to user) */
#ifdef ALWAYS_TELL_ERROR
    #define TELL_ERROR(e) 	{printf(e); printf("\n");}
#else
    #define TELL_ERROR(e) 	{}
#endif
