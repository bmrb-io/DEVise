/* parse.h */
#ifndef PARSE_H
#define PARSE_H
#include <stdio.h>

#define BIT_X	(1<<0)
#define BIT_Y	(1<<1)
#define BIT_COLOR (1<<2)
#define BIT_SIZE (1<<3)
#define BIT_PATTERN (1<<4)
#define BIT_SHAPE	(1<<5)
#define BIT_ORIENTATION (1<<6)

#define MAX_ATTRS 10
#define MAX_SHAPE_DEFS 10
typedef struct MappingRec{
	char *name;
	char *recType;
	int dynamicFields; /* which visual args are dynamic?  */
	int dynamicAttrs; /* bith i set to 1 if ith shape attr is dynamic */
	char *defaultText;
	char *mappingText;
	char *shapeNames[MAX_SHAPE_DEFS];
	int numShapeNames;

} MappingRec;

extern void Parse(FILE *mapFile);

#endif
