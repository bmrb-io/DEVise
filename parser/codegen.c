/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.6  1995/09/23 00:03:51  ravim
  Parser changed to ANSI style - g++ now used uniformly

  Revision 1.5  1995/09/22 16:54:48  jussi
  Replacing file with real contents.

  Revision 1.3  1995/09/05 20:35:15  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse.h"
#include "codegen.h"

#define PROTODIR  "/p/devise/parser/proto"

/**************************************************************
Generate cpp definitions for shape class.
*****************************************************************/
void GenCppDefs(MappingRec *rec, char *string){
		if (rec->dynamicFields & BIT_X)
			strcat(string,"-DDYNAMIC_X ");

		if (rec->dynamicFields & BIT_Y)
			strcat(string,"-DDYNAMIC_Y ");

		if (rec->dynamicFields & BIT_COLOR)
			strcat(string,"-DDYNAMIC_COLOR ");

		if (rec->dynamicFields & BIT_SIZE)
			strcat(string,"-DDYNAMIC_SIZE ");

		if (rec->dynamicFields & BIT_PATTERN)
			strcat(string,"-DDYNAMIC_PATTERN ");

		if (rec->dynamicFields & BIT_ORIENTATION)
			strcat(string,"-DDYNAMIC_ORIENTATION ");

		if (rec->dynamicFields & BIT_SHAPE)
			strcat(string,"-DDYNAMIC_SHAPE ");
		
		if (rec->dynamicAttrs ){
			int i;
			strcat(string,"-DDNYAMIC_SHAPEATTR ");
			for (i=0; i < MAX_ATTRS; i++){
				if (rec->dynamicAttrs & (1<<i)){
					char buf[50];
					sprintf(buf,"-DDYNAMIC_SHAPEATTR_%d ",i);
					strcat(string,buf);
				}
			}
		}
		
}

/*************************************************************
Generate shape definitions
**************************************************************/
GenShapeDefs(MappingRec *rec, FILE *mapFile){
FILE *tmp;
char *name;
char tempName[1024];
char string[4096];
char temp[1024];
int i;
int c;

	tmpnam(tempName);

	for (i=0; i < rec->numShapeNames; i++){
		sprintf(string,"/lib/cpp -P -DCLASSNAME=%s_%s -DGDATANAME=%s_GData ",
			rec->name,rec->shapeNames[i],rec->name);

		GenCppDefs(rec, string);

		/* invoke cpp to preprocess it */
		sprintf(temp,"%s/%s_proto.h > %s", PROTODIR, rec->shapeNames[i], 
			tempName);
		strcat(string,temp);

		/*
		printf("system %s\n",string);
		*/

		if (system(string) == 127){
			fprintf(stderr,"can't execute shell\n");
			exit(2);
		}

		/* copy the result into file */
		if ((tmp = fopen(tempName,"r"))== NULL){
			fprintf(stderr,"can't open temp file %s\n",tempName);
			exit(2);
		}

		while ((c=fgetc(tmp)) != EOF)
			fputc(c,mapFile);
		fclose(tmp);
	}
	unlink(tempName);

	/* generate shape IDs */
	for (i=0; i < rec->numShapeNames; i++){
		fprintf(mapFile,"const int %s_%sID = %d;\n",
			rec->name,rec->shapeNames[i], i);
	}

}

/***********************************************************
Generate code for setting attribute list of GData
*************************************************************/
void GenAttrList(MappingRec *rec, FILE *mapFile){
/*
	int offset = 0;
	fprintf(mapFile,"\n\tAttrList *attrList = new AttrList(\"%s_Attrs\");\n",rec->name);
	if (rec->dynamicFields & BIT_X){
		fprintf(mapFile,"\tattrList->InsertAttr(0, \"x\", %d, sizeof(double), DoubleAttr, false, NULL, false, numDimensions == 1 && dimensionInfo[0] == VISUAL_X);\n",offset);
		offset += sizeof(double);
	}
	if (rec->dynamicFields & BIT_Y){
		fprintf(mapFile,"\tattrList->InsertAttr(1, \"y\", %d, sizeof(double), DoubleAttr, false, NULL, false, false);\n",offset);
		offset += sizeof(double);
	}
	if (rec->dynamicFields & BIT_COLOR){
		fprintf(mapFile,"\tattrList->InsertAttr(2, \"color\", %d, sizeof(double), DoubleAttr, false, NULL, false, false);\n",offset);
		offset += sizeof(double);
	}
	if (rec->dynamicFields & BIT_SIZE){
		fprintf(mapFile,"\tattrList->InsertAttr(3, \"size\", %d, sizeof(double), DoubleAttr, false, NULL, false, false);\n",offset);
		offset += sizeof(double);
	}
	if (rec->dynamicFields & BIT_PATTERN){
		fprintf(mapFile,"\tattrList->InsertAttr(4, \"pattern\", %d, sizeof(double), DoubleAttr, false, NULL, false, false);\n",offset);
		offset += sizeof(double);
	}
	if (rec->dynamicFields & BIT_SHAPE){
		fprintf(mapFile,"\tattrList->InsertAttr(5, \"shape\", %d, sizeof(double), DoubleAttr, false, NULL, false, false);\n",offset);
		offset += sizeof(double);
	}
	if (rec->dynamicFields & BIT_ORIENTATION){
		fprintf(mapFile,"\tattrList->InsertAttr(6, \"orientation\", %d, sizeof(double), DoubleAttr, false, NULL, false, false);\n",offset);
		offset += sizeof(double);
	}
	fprintf(mapFile,"\tSetGDataAttrList(attrList);\n");
*/
}

/*************************************************************
Generate code for mapping
**************************************************************/
void CodeGen(MappingRec *rec, FILE *mapFile){
int i;


/* generate GData structure */
	fprintf(mapFile,"\nstruct %s_GData {\n",rec->name);
	if (rec->dynamicFields & BIT_X)
		fprintf(mapFile,"\tdouble x;\n");
	if (rec->dynamicFields & BIT_Y)
		fprintf(mapFile,"\tdouble y;\n");
	if (rec->dynamicFields & BIT_COLOR)
		fprintf(mapFile,"\tdouble color;\n");
	if (rec->dynamicFields & BIT_SIZE)
		fprintf(mapFile,"\tdouble size;\n");
	if (rec->dynamicFields & BIT_PATTERN)
		fprintf(mapFile,"\tdouble pattern;\n");
	if (rec->dynamicFields & BIT_SHAPE)
		fprintf(mapFile,"\tdouble shapeID;\n");
	if (rec->dynamicFields & BIT_ORIENTATION)
		fprintf(mapFile,"\tdouble orientation;\n");
	for (i=0; i < MAX_ATTRS; i++){
		if (rec->dynamicAttrs & (1<< i) )
			fprintf(mapFile,"\tfloat shapeAttr_%d;\n",i);
	}
	fprintf(mapFile,"};\n\n");

	GenShapeDefs(rec,mapFile);

/* generate mapping */
fprintf(mapFile,"class %s : public TDataMapDispatch{\n",rec->name);
fprintf(mapFile,"public:\n");
fprintf(mapFile,"\t%s(TData *tdata, char *gdataName, VisualFlag *dimensionInfo, int numDimensions,int maxGDataPages= -1, void *uData=NULL): TDataMapDispatch(\"%s\", tdata, gdataName, sizeof(%s_GData), 0",rec->name, rec->name, rec->name);
if (rec->dynamicFields & BIT_X)
	fprintf(mapFile,"|VISUAL_X");
if (rec->dynamicFields & BIT_Y)
	fprintf(mapFile,"|VISUAL_Y");
if (rec->dynamicFields & BIT_COLOR)
	fprintf(mapFile,"|VISUAL_COLOR");
if (rec->dynamicFields & BIT_SIZE)
	fprintf(mapFile,"|VISUAL_SIZE");
if (rec->dynamicFields & BIT_PATTERN)
	fprintf(mapFile,"|VISUAL_PATTERN");
if (rec->dynamicFields & BIT_SHAPE)
	fprintf(mapFile,"|VISUAL_SHAPE");
if (rec->dynamicFields & BIT_ORIENTATION)
	fprintf(mapFile,"|VISUAL_ORIENTATION");
fprintf(mapFile,",%d, maxGDataPages,dimensionInfo,numDimensions){\n", 
		rec->dynamicAttrs);

/* Generate attribute list */
GenAttrList(rec,mapFile);

fprintf(mapFile,"\t\tuserData = uData;\n");

fprintf(mapFile,"%s\n",rec->defaultText);
/* construct shapes */
	for (i=0; i < rec->numShapeNames; i++){
		fprintf(mapFile,"\t_shapes[%d] = new %s_%s;\n", i,rec->name,
			rec->shapeNames[i]);
	}
fprintf(mapFile,"\t}\n");

#ifdef TDATAMAP_MAP_TO_SYMBOL
fprintf(mapFile,"\tvoid Map(TData *, RecId , void *rec, Symbol *symbol){\n");
fprintf(mapFile,"\t\t%s *data= (%s *)rec;\n",rec->recType,rec->recType);
fprintf(mapFile,"%s\n",rec->mappingText);
fprintf(mapFile,"\t}\n");
#endif

/*
fprintf(mapFile,"\tvoid MapArray(TData *, RecId, void **recs, Symbol **syms, int num,\n");
fprintf(mapFile,"\t\tint startIndex, int incr){\n");
fprintf(mapFile,"\t\tint i;\n");
fprintf(mapFile,"\t\tfor (i= startIndex; i < num; i += incr){\n");
fprintf(mapFile,"\t\t%s *data = (%s *)recs[i];\n",rec->recType, rec->recType);
fprintf(mapFile,"\t\tSymbol *symbol = syms[i];\n");
fprintf(mapFile,"%s\n",rec->mappingText);
fprintf(mapFile,"\t\t}\n");
fprintf(mapFile,"\t}\n");
*/

/*
fprintf(mapFile,"\tvirtual int GDataRecordSize(){ return sizeof(%s_GData);}\n",
	rec->name);
*/

/* generate code to convert symbol to gdata */
fprintf(mapFile,"\tvirtual void ConvertToGData(RecId recId,void *buf, void **tRecs, int numRecs, void *gdataBuf){\n");
fprintf(mapFile,"\t\tint i;\n");
fprintf(mapFile,"\t\tint tRecSize= TDataRecordSize();\n");
fprintf(mapFile,"\t\tint gRecSize= GDataRecordSize();\n");
fprintf(mapFile,"\t\tchar *gBuf= (char *)gdataBuf;\n");
fprintf(mapFile,"\t\tif (tRecSize >0){\n");
fprintf(mapFile,"\t\t\tchar *tptr = (char *)buf;\n");
fprintf(mapFile,"\t\t\tfor (i=0; i < numRecs; i++){\n");
fprintf(mapFile,"\t\t\t\t%s_GData *symbol = (%s_GData *)gBuf;\n",rec->name,rec->name);
fprintf(mapFile,"\t\t\t\t%s *data = (%s *)tptr;\n",rec->recType, rec->recType);
fprintf(mapFile,"\t\t\t\ttptr += tRecSize;\n");
fprintf(mapFile,"%s\n",rec->mappingText);
fprintf(mapFile,"\t\t\t\trecId++;\n");
fprintf(mapFile,"\t\t\t\tgBuf += gRecSize;;\n");
fprintf(mapFile,"\t\t\t}\n");
fprintf(mapFile,"\t\t}\n");
fprintf(mapFile,"\t\telse{\n");
fprintf(mapFile,"\t\t\tfor(i=0; i < numRecs; i++){\n");
fprintf(mapFile,"\t\t\t\t%s_GData *symbol = (%s_GData *)gBuf;\n",rec->name,rec->name);
fprintf(mapFile,"\t\t\t\t%s *data = (%s *)tRecs[i];\n",rec->recType, rec->recType);
fprintf(mapFile,"%s\n",rec->mappingText);
fprintf(mapFile,"\t\t\t\trecId++;\n");
fprintf(mapFile,"\t\t\t\tgBuf += gRecSize;\n");
fprintf(mapFile,"\t\t\t}\n");


/*
fprintf(mapFile,"\t\t\tSymbol *symbol = syms[i];\n");
	if (rec->dynamicFields & BIT_X)
		fprintf(mapFile,"\t\t\tgdata->x = symbol->x;\n");
	if (rec->dynamicFields & BIT_Y)
		fprintf(mapFile,"\t\t\tgdata->y = symbol->y;\n");
	if (rec->dynamicFields & BIT_COLOR)
		fprintf(mapFile,"\t\t\tgdata->color = symbol->color;\n");
	if (rec->dynamicFields & BIT_PATTERN)
		fprintf(mapFile,"\t\t\tgdata->pattern = symbol->pattern;\n");
	if (rec->dynamicFields & BIT_SHAPE)
		fprintf(mapFile,"\t\t\tgdata->shapeID = symbol->shapeID;\n");
	if (rec->dynamicFields & BIT_ORIENTATION)
		fprintf(mapFile,"\t\t\tgdata->orientation = symbol->orientation;\n");
	if (rec->dynamicAttrs)
		for (i=0; i < MAX_ATTRS; i++){
			if (rec->dynamicAttrs & i)
				fprintf(mapFile,"\t\t\tgdata->shapeAttr_%d = symbol->shapAttrs[%d];\n",i+1,i+1);
		}
*/

fprintf(mapFile,"\t\t}\n");
fprintf(mapFile,"\t}\n\n");


/* generate code for virtual functions for shape registrar */
	/* constant shapes */
fprintf(mapFile,"\tvirtual void UpdateBoundingBox(int pageNum, void **syms, int numSyms){\n");
fprintf(mapFile,"\t\tif (TDataMap::TestAndSetPage(pageNum)) return;\n");
fprintf(mapFile,"\t\tCoord width, height;\n");

if (rec->dynamicFields & BIT_SHAPE){
fprintf(mapFile,"\t\tint i = 0;\n");
fprintf(mapFile,"\t\twhile (i < numSyms){\n");
fprintf(mapFile,"\t\t\t%s_GData *sym = (%s_GData *)syms[i];\n",rec->name,rec->name);
fprintf(mapFile,"\t\t\tint symIndex;\n");
fprintf(mapFile,"\t\t\tfor (symIndex = i+incr; symIndex < numSyms; symIndex += incr){\n");
fprintf(mapFile,"\t\t\t\tif(syms[symIndex]->shapeID != sym->shapeID)\n",rec->name);
fprintf(mapFile,"\t\t\t\t\tbreak;\n");
fprintf(mapFile,"\t\t\t}\n");

fprintf(mapFile,"\t\t\t/* syms[i..symIndex-incr] have the same shape */\n");
fprintf(mapFile,"\t\t\t_shapes[sym->shapeID]->BoundingBoxGData(this,&syms[i],symIndex-i,width, height);\n");
fprintf(mapFile,"TDataMap::UpdateBoundingBox(width,height);\n");
fprintf(mapFile,"\t\t\ti = symIndex;\n");
fprintf(mapFile,"\t\t}\n");

}
else {
fprintf(mapFile,"\t\t_shapes[0]->BoundingBoxGData(this, syms,numSyms,width,height);\n");
fprintf(mapFile,"\t\tTDataMap::UpdateBoundingBox(width, height);\n");

}

fprintf(mapFile,"\t}\n");

/*
fprintf(mapFile,"\tvirtual void DrawSymbolArray(WindowRep *win, Symbol **syms, int numSyms){\n");
if (rec->dynamicFields & BIT_SHAPE){
	fprintf(mapFile,"\t\tint i=0;\n");
	fprintf(mapFile,"\t\twhile(i < numSyms){\n");
	fprintf(mapFile,"\t\t\tSymbol *sym = syms[i];\n");
	fprintf(mapFile,"\t\t\tint symIndex;\n");
	fprintf(mapFile,"\t\t\tfor (symIndex = i+1; symIndex < numSyms; symIndex++){\n");
	fprintf(mapFile,"\t\t\t\tif (syms[symIndex]->shapeID != sym->shapeID)\n");
	fprintf(mapFile,"\t\t\t\t\tbreak;\n");
	fprintf(mapFile,"\t\t\t}\n");
	fprintf(mapFile,"\t\t\t_shape[sym->shapeID]->DrawArray(win,&syms[i],symIndex-i,this);\n");
	fprintf(mapFile,"\t\t\ti = symIndex;\n");
	fprintf(mapFile,"\t\t}\n");
}
else fprintf(mapFile,"\t\t_shapes[0]->DrawArray(win,syms,numSyms,this);\n");
fprintf(mapFile,"\t}\n");
*/

fprintf(mapFile,"\tvirtual void DrawGDataArray(WindowRep *win, void **syms, int numSyms, Coord xPerPixel, Coord yPerPixel){\n");
if (rec->dynamicFields & BIT_SHAPE){
fprintf(mapFile,"\t\tint i=0;\n");
fprintf(mapFile,"\t\twhile (i < numSyms){\n");
fprintf(mapFile,"\t\t\t%s_GData *sym = (%s_GData *)syms[i];\n",rec->name,rec->name);
fprintf(mapFile,"\t\t\tint symIndex;\n");
fprintf(mapFile,"\t\t\tfor(symIndex= i+1; symIndex < numSyms; symIndex++){\n");
fprintf(mapFile,"\t\t\t\tif ( ((%s_GData *)syms[symIndex])->shapeID != sym->shapeID)\n",rec->name);
fprintf(mapFile,"\t\t\t\tbreak;\n");
fprintf(mapFile,"\t\t\t}\n");
fprintf(mapFile,"\t\t\tmap_shape[sym->shapeID]->DrawGDataArray(win,&syms[i],symIndex-i, this, xPerPixel, yPerPixel, GetPixelWidth());\n");
fprintf(mapFile,"\t\t\ti = symIndex;\n");
fprintf(mapFile,"\t\t}\n");

}
else fprintf(mapFile,"\t\t_shapes[0]->DrawGDataArray(win,syms,numSyms,this,xPerPixel, yPerPixel, TDataMap::GetPixelWidth());\n");

fprintf(mapFile,"\t}\n");


/* private variables */
fprintf(mapFile,"private:\n");
fprintf(mapFile,"\tvoid *userData;\n");
fprintf(mapFile,"\tShape *_shapes[%d];\n",rec->numShapeNames);


fprintf(mapFile,"};\n\n");
}
