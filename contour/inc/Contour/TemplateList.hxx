/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   TemplateList.hxx
PURPOSE: Template for a dynamic list of specific data types 
USAGE:   Calling function defines 
	 DATA_TYPE : template data type
	 LIST_TYPE : Name of list class that holds data type

EXAMPLE: #This would define a list of instances of class foo 
	 #include <foo.hxx>

	 #define DATA_TYPE foo
	 #define LIST_TYPE fooList
	 #include<TemplateList.hxx>
	 #undef LIST_TYPE
	 #undef DATA_TYPE

DATE:    10/25/1997
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/

/*-------- NEW DATA TYPES --------*/

#include<malloc.h>

class LIST_TYPE 
{
/*Members*/
protected:
	/*Allocation size of list */
	int iAllocSize;

	/*Current number of items in list*/
	int iNItems;

	/*Array of DATA_TYPE */
	DATA_TYPE* ptList;

/*METHODS*/
protected:
	/*Expand list of pointers by iAllocSize*/
	void Expand()
	{
		iAllocSize *= 2;
		ptList = (DATA_TYPE*)realloc(ptList, 
			iAllocSize*sizeof(DATA_TYPE));
	}

public:
	/*Constructor*/
	LIST_TYPE(int i_AllocSize=128)
	{ 
		if ( i_AllocSize < 8 )
			i_AllocSize = 8;

		iAllocSize = i_AllocSize;
		iNItems = 0;
		ptList = (DATA_TYPE*)malloc(iAllocSize*sizeof(DATA_TYPE));
	}

	/*Remove all pointers from list*/
	void vClear()
		{ iNItems = 0; }

	/*Return current number of items in list*/
	int iGetNItems()
		{ return iNItems; }

	/*Append item to end of list*/
	void vAppendItem(DATA_TYPE& rtDATA_TYPE)
	{ 
		if ( iNItems >= iAllocSize )
			Expand();
		ptList[iNItems++] = rtDATA_TYPE;
	} 

	/*Return nth item*/
	DATA_TYPE tGetItem(unsigned n)
		{ return ptList[n]; }

 
	~LIST_TYPE()
	{
		free(ptList);
	}
};

/*-------- END DATA TYPES --------*/
