typedef union{
     String* string;
	int integer;
	BaseSelection* sel;
	Path* path;
	List<BaseSelection*>* selList;
	List<TableAlias*>* tableList;
	TableAlias* tabAlias;
} YYSTYPE;
#define	INT	258
#define	SELECT	259
#define	FROM	260
#define	WHERE	261
#define	STRING_CONST	262
#define	OR	263
#define	AND	264
#define	LESSGREATER	265
#define	STRING	266


extern YYSTYPE yylval;
