/* Display.h */
#ifndef Display_h
#define Display_h
#include "DeviseTypes.h"
#include "Geom.h"
#include "Dispatcher.h"
#include "DList.h"
#include "ColorMgr.h"

const int InitColorMapSize = 512;
const int AdditionalColorMapSize = 512;

class WindowRep;
class DeviseDisplay;

DefinePtrDList(DeviseDisplayList, DeviseDisplay *);

class DeviseDisplay: private DispatcherCallback {
public:
	DeviseDisplay();

	virtual ~DeviseDisplay();

	/* get the default display */
	static DeviseDisplay *DefaultDisplay();

	virtual int NumPlanes()=0;
	virtual void Dimensions(Coord &width, Coord &height) =0;


	/* Create a new window rep, 
	relative==1 if in relative dimensions.*/
	virtual WindowRep* CreateWindowRep(char *name,
		Coord x, Coord y, Coord width, Coord height, 
		Color fgnd=WhiteColor, Color bgnd=BlackColor, 
		WindowRep *parentRep=NULL,Coord min_width=0.05,Coord min_height=0.05,
		Boolean relative= true, Boolean winBoundary = true)=0;
	
	/* Destroy a window rep*/
	virtual void DestroyWindowRep(WindowRep *c)=0;

	/* Do internal event processing, but do not block*/
	virtual void InternalProcessing()=0;

	/* iterator to go through all displays */
	static int InitIterator() { return _displays.InitIterator(); };
	static Boolean More(int index){ return _displays.More(index); };
	static DeviseDisplay *Next(int index) { return _displays.Next(index); };
	static void DoneIterator(int index) { _displays.DoneIterator(index); };
protected:
	/* must be called from within the initializer of derived class
	to get the initial set of colors from the color manager */
	void InitializeColors(){
		ColorMgr::InitializeColors(this);
	};

	friend class ColorMgr;
	/* allocate internal colors for the given global color */
	virtual void AllocColor(char *name, Color globalColor) =0;

	/* allocate internal color by rgb, from 0.0 to 1.0, for the 
	given global color. */
	virtual void AllocColor(double r, double g, double b, Color golbalColor) =0;

	/* functions for derived classes to facilitate color processing.
	Each display instance must match the global color to its own
	local color. MapColor() maps local color to global color, while
	GetLocalColor() retrieeves the local color given global color. */

	/* map local color to global color.*/
	void MapColor(Color localColor, Color globalColor);

	friend class WindowRep;

	/* get local color given global color. */
	Color GetLocalColor(Color globalColor);

private:
	/* from DispatcherCallback */
	virtual char *DispatchedName();
	void Run(){
		InternalProcessing();
	};

	/* static DList<DeviseDisplay *> _displays; */
	static DeviseDisplayList _displays; /* list of all displays */

	unsigned long _numColors;
	unsigned long _colorMapSize;
	Color *_colorMap;
	static DeviseDisplay *_defaultDisplay;
	Dispatcher *_dispatcher;
};

#endif
