#ifndef  _LM_WINDOW_H
#define  _LM_WINDOW_H
#include <iostream.h>
#include  "LMHashTable.h"
#include  "LMLayout.h"
#include  "DList.h"

#define VIEW_SYMBOL 17
typedef char * Color;
typedef double Coord;

const PropNum = 9;

enum PropType { NAME = 0, 
		ORIG_X, ORIG_Y, REL_X, REL_Y, 
		WIDTH,  HEIGHT, 
		BGCOLOR, FGCOLOR, 
		USER_DEF };

class LMWindow;
DefinePtrDList(LMWindowList, LMWindow *)

class LMLayout;
class LMWindow {

  friend class LMLayout;
  private :
  LMHashTable *_propTable;		// table of properties of the window
  LMWindowList *_childWindows;	// list of child windows

  LMWindow *_parent;
  int _level;			// level = 0 => root window
				// a leaf window has _childWindow list == NULL

  bool _remapOnResize;       // if this flag is off then, the child windows
				// of this window cannot change position 
				// can be used to create empty spaces
				// If this flag is off, the window cannot be 
				// made smaller for obvious reasons
  int _numChildren;
  LMLayout *_layout;		// the layout for this window
#ifndef STANDALONE		// Interface with DEVise - needs TData file 
  char *_tdataFile;		// name of the tdata file 
  char *_tdataName;		// tdata source name
  int _hasTDataChanged;		// if things have changed since last 
				// visualized. If this is set, the tdata
				// file is dumped when the view has to be 
				// visualized. 
  int _XwindowCreated;           // has a X window been created for this
				// this flag has significance only for a 
				// window at level 1 (children of the display)
#endif
  public :

  LMWindow(char *name, 
	   Coord ORIGX = 0, Coord ORIGY = 0, Coord relX = 0, Coord relY = 0,
	   int width = 0, int height = 0, 
	   Color bgColor = "black", Color fgColor = "red", 
	   LMWindow *parent = NULL);
  ~LMWindow();
  void DeleteThisWindow(bool remap);
  // get the property value, given  the property type
  // for USER_DEF property, the propName argument is needed

  void *GetPropVal(PropType type, char *propName = NULL); 
  void SetPropVal(PropType type, void *propVal, char *propName = NULL);
  void CreateProperty(char *propName, HashValType type, void *val );
  void DeleteProperty(char *propName);
  
  void AddChildWindow(LMWindow *win);
  bool DeleteChildWindow(LMWindow *win, bool remap = true);
  int NumChildren() { return _numChildren; }
  
  int InitChildListIterator() { 
    return _childWindows->InitIterator(); 
  }
  bool MoreChildren(int index) { 
    return _childWindows->More(index); 
  }
  LMWindow *NextChild(int index)  { 
    return _childWindows->Next(index); 
  }
  void DoneChildListIterator(int index) {
    _childWindows->DoneIterator(index);
  }
  
  int GetLevel() {
    return _level;
  }
  LMWindow *GetParent () {
    return _parent;
  }
  void SetParent (LMWindow *parent) {
    _parent = parent;
    if (parent) {
      _level = _parent->GetLevel() + 1;
    } else {
      _level = 0;
    }
  }
  
  char * Name() {
    return (char *)_propTable->LookUp(NAME);
  }
  Coord origX() {
    return *(Coord *)_propTable->LookUp(ORIG_X);
  }
  Coord origY() {
    return *(Coord *)_propTable->LookUp(ORIG_Y);
  }
  Coord relX() {
    return *(Coord *)_propTable->LookUp(REL_X);
  }
  Coord relY() {
    return *(Coord *)_propTable->LookUp(REL_Y);
  }

  int Width(){
    return *(int *)_propTable->LookUp(WIDTH);
  }
  int Height() {
    return *(int *)_propTable->LookUp(HEIGHT);
  }

  Color BgColor() {
    return (Color)_propTable->LookUp(BGCOLOR);
  }
  Color FgColor() {
    return (Color)_propTable->LookUp(FGCOLOR);
  }
  
  void SetBgColor(Color bg) {
    _propTable->Update(int(BGCOLOR), bg);
  }
  void SetFgColor(Color fg) {
    _propTable->Update(int(FGCOLOR), fg);
  }

  LayoutMode GetLayoutMode() {
    return _layout->GetLayoutMode();
  }
  
  void SetLayoutMode( LayoutMode mode ) {
    _layout->SetLayoutMode(mode);
  }

  void SetLayoutProperties(LayoutMode mode, int rows, int cols) {
    _layout->SetLayoutProperties(mode, rows, cols);
  }
  
  void GetLayoutProperties(LayoutMode &mode, int &rows, int &cols) {
    _layout->GetLayoutProperties(mode, rows, cols);
  }
#ifndef STANDALONE
  int HasTDataChanged()  { return _hasTDataChanged; }

  void SetTDataChanged (int flag) { _hasTDataChanged = flag; }
  int XWindowCreated() { return _XwindowCreated; }
  void SetXWindowCreated() { _XwindowCreated = 1; }
  void SetTDataFile(char *file) { _tdataFile = file ; }
  void SetTDataName(char *name) { _tdataName = name ; }
  char *GetTDataFile () { return _tdataFile; }
  char *GetTDataName () { return _tdataName; }
#endif

  void HandleMoveResize(Coord x, Coord y, unsigned w, unsigned h);
    

  void Map(Coord x, Coord y, unsigned w, unsigned h);
  
  //change rel posn of window within parent
  void MoveWindow(Coord x, Coord y);
  //change origin of reference for window 
  
  void SetOrigin(Coord x, Coord y);

  void ResizeWindow(unsigned w, unsigned h);

  // TEMP TEMP TEMP 
  int GetWeight() { return 1; } 
  int TotalWeight() { return _numChildren; }
  // TEMP TEMP TEMP 

#ifndef STANDALONE
  // No optimizations are made here for I/O. Later this mechanism should
  // be changed to use a Database style record/page-oriented read/write.
  
  int Write(void);		// writes the TData file 
				// all the records are written
				// shouldnt be too much of a problem as the 
				// windows within a window will be a handful.
  int Read(void);		// reads the Tdata file and creates the
				// main memory structures for child views.
				// Used when reading saved session files.
#endif
  // other utility functions
  friend 
    ostream &operator <<(ostream &os, LMWindow &w);
};  
#endif  
  
  
  
  
  
  
  




