/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   StreamDrawable.hxx
PURPOSE: Implements StreamDrawable by sending commands for line drawing
	 to standard output
DATE:    10/26/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/


/*-------- NEW DATA TYPES --------*/

#ifndef StreamDrawable_H
	#include <stream.h>
	#include "Drawable2D.hxx"

class StreamDrawable : public Drawable2D
{
public:
	/*Pointer to output stream where graphics commands are sent*/
	ostream* pOStream;

protected:
	/*Draw a point at iPX, iPY using the current pen color.*/ 
	virtual void vDrawPoint(int iPX,int iPY)
	{	
		(*pOStream) << iPX << '\t' << iPY << '\t' << siColor << endl;
	}

	/*Draws a line from iPX1,iPY1 to iPX2,iPY2 using the current
	  pen color */
	virtual void vDrawLine(int iPX1,int iPY1,int iPX2,int iPY2)
	{
		(*pOStream) << iPX1 << '\t' << iPY1 << '\t';
		(*pOStream) << iPX2 << '\t' << iPY2 << '\t' << siColor << endl;
	}


public:
	/*Default constructor*/
	StreamDrawable(ostream& rOStream):
		Drawable2D()
	{
		pOStream = &rOStream;
	}

	/*Constructor with defined size*/
	StreamDrawable(int i_NX,int i_NY,ostream& rOStream):
		Drawable2D(i_NX,i_NY)
	{
		pOStream = &rOStream;
	}

	/*Set image size*/
	virtual void vSetImageSize(int i_NX,int i_NY)
	{
		Drawable2D::vSetImageSize(i_NX,i_NY);
		(*pOStream) << i_NX << '\t' << i_NY << endl;
	}

	/*Flushes the stream associated with this instance */
	virtual void vClear()
		{ (*pOStream) << flush; }
};

#define StreamDrawable_H
#endif

/*-------- END DATA TYPES --------*/
