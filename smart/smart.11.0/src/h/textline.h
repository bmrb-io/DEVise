#ifndef TEXTLINEH
#define TEXTLINEH
/*        $Header$*/

/* structure used for passing around tokens of an input line.
   see textline.c for text_textline(). */
typedef struct {
    char *text;                  /* text buffer */
    char **tokens;               /* Array of pointers into text */
    long num_tokens;             /* Number of tokens in tokens */
    long max_num_tokens;         /* Number of pointers space reserved for */
} SM_TEXTLINE;

#endif /* TEXTLINEH */
