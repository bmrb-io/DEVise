#define yylval  query_yylval

typedef union
#ifdef __cplusplus
	YYSTYPE
#endif
 {
        char         *text;
        IN_node_type *IN_node_type1; 
        IN_node_list *IN_node_list; } YYSTYPE;
extern YYSTYPE yylval;
# define AND 257
# define BOOLEAN_AND 258
# define BOOLEAN_AND_NOT 259
# define FILTER_REQUIRE 260
# define FILTER_REJECT 261
# define LEFT_PAREN 262
# define MAX_OP 263
# define NOT 264
# define NUMBER 265
# define ODn 266
# define OR 267
# define PARSUMn 268
# define PHRASE 269
# define PHRASEB 270
# define RIGHT_PAREN 271
# define SUM 272
# define SYN 273
# define UWn 274
# define WGTplus 275
# define WGTminus 276
# define WORD 277
# define WPARSUMn 278
# define WSUM 279
# define FIELD_OP 280
# define EQ_RELOP 281
# define RELOP 282
# define MIN_RELOP 283
# define MAX_RELOP 284
# define RANGE_RELOP 285
# define RANGE_LIMIT 286
# define LITERAL 287
