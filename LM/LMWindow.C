#include <iostream.h>
#include "LMWindow.h"
#include "LMLayout.h"
#include "LMControl.h"

extern char *ModeTypeToStr(LayoutMode mode);
LMWindow::LMWindow(char *name, 
		   Coord origX, Coord origY, Coord relX, Coord relY,
		   int width, int height, 
		   Color bgColor, Color fgColor, 
		   LMWindow *parent)
{
  _propTable = new LMHashTable(60, PropNum);
  _propTable->InsertAt(NAME, "Name", STRING, name);
  _propTable->InsertAt(ORIG_X, "origX", DOUBLE, &origX);
  _propTable->InsertAt(ORIG_Y, "origY", DOUBLE, &origY);
  _propTable->InsertAt(REL_X, "RelX", DOUBLE, &relX);
  _propTable->InsertAt(REL_Y, "RelY", DOUBLE, &relY);
  _propTable->InsertAt(WIDTH, "Width", INT, &width);
  _propTable->InsertAt(HEIGHT, "Height", INT, &height);
  _propTable->InsertAt(BGCOLOR, "BgColor", STRING, bgColor );
  _propTable->InsertAt(FGCOLOR, "FgColor", STRING, fgColor );

  _parent = parent;
  _remapOnResize = true;
  _childWindows = NULL;
  _numChildren = 0;
  _layout = new LMLayout(this);

  if (_parent) {
    // Add to parent, this also updates the positional parameters of the 
    // window depending on the parent's layout mode
    _parent->AddChildWindow(this);
#ifndef STANDALONE
    _parent->_hasTDataChanged = 1;
    _hasTDataChanged = 0;
#endif
  }
#ifndef STANDALONE
  _XwindowCreated = 0;
#endif
}

// Since a destructor cannot have params this is a pseudo destructor.
// The param when true, other siblings will be rearranged if the mode is 
// not custom, when false they are not (so that mapping can be bypassed
// on a recursive delete of subtree of windows.

void LMWindow::DeleteThisWindow(bool remap) {
  if (_parent) {
    _parent->DeleteChildWindow(this, remap);
#ifndef STANDALONE
    _parent->_hasTDataChanged = 1;
#endif
  }
  _parent = NULL; // Since this calls the destructor, this will skip 
		  // the if (_parent)...call in the destructor
  delete this;
}

LMWindow::~LMWindow()
{
  if (_numChildren != 0) {
    printf("Something is wrong!!!! : Children not deleted");
    return;
  }
  if (_parent) {
    _parent->DeleteChildWindow(this);
#ifndef STANDALONE
    _parent->_hasTDataChanged = 1;
#endif
  } 

  delete _propTable;
  delete _layout;
}

void *LMWindow::GetPropVal(PropType type, char *propName = NULL)
{
  if (type == USER_DEF) {
    return _propTable->LookUp(propName);
  } else {
    return _propTable->LookUp(type);
  }
}

void LMWindow::SetPropVal(PropType type, void *val,char *propName)
{
  if (type == USER_DEF) {
    _propTable->Update(propName, val);
  } else {
    _propTable->Update(type, val);
  }
}


void LMWindow::CreateProperty(char *propName, HashValType type, void *val )
{
  if (_propTable->LookUpElt(propName) != NULL ) {
    cout << "Property" << propName << "already exists" << endl;
    return;
  }
  _propTable->Insert(propName, type, val);
}

void LMWindow::DeleteProperty(char *propName) 
{
  if (_propTable->LookUpElt(propName)) {
    _propTable->Delete(propName);
  }
}

void LMWindow::AddChildWindow (LMWindow *win) 
{
  if (!_childWindows) {
    _childWindows = new LMWindowList;
  }
  // Append to layout 
  // which will position the new child within parent depending on the 
  // layout mode
  _layout->Append(win);
  _childWindows->Append(win);
  win->SetParent(this);
  _numChildren++;
}

void LMWindow::Map(Coord x, Coord y, unsigned w, unsigned h) 
{
  MoveWindow(x, y);
  ResizeWindow(w,h);
//#ifdef TCLTK_FRONT_END 

  LMControl *controlPanel = LMControl::GetLMControl();
  char buf[100];
  sprintf(buf, "MapWindow %s %f %f %f %f %u %u %s",
	  Name(), 
	  origX(), origY(), 
	  relX(), relY(), 
	  Width(), Height(),
	  BgColor());
  controlPanel->NotifyFrontEnd(buf);
//#endif
}

bool LMWindow::DeleteChildWindow (LMWindow *win, bool remap)
{
  _numChildren--;
  bool val = _childWindows->Delete(win);
  // If all other children are also being deleted then they need not be 
  // remapped
  if (val && remap ) {
    _layout->Delete(win);
  }
  return val;
}

void LMWindow::SetOrigin(Coord x, Coord y)
{
  _propTable->Update(int(ORIG_X), &x);
  _propTable->Update(int(ORIG_Y), &y);
}

void LMWindow::HandleMoveResize(Coord x, Coord y, unsigned w, unsigned h)
{
  Coord oldX, oldY;
  unsigned oldW, oldH;

  if (_numChildren > 0) {
    oldX = relX();
    oldY = relY();
    oldW = Width();
    oldH = Height();
  }
  
  if ((oldX == x) && (oldY == y) && (oldW == w) && (oldH == h)) {
    return;
  }
  Map(x, y, w, h);
      
  // redraw/remap children according to layout mode
  // old information may be required for scaling purposes
  if (_numChildren > 0) {
    _layout->UpdateLayout(oldX, oldY, oldW, oldH);
  }
#ifndef STANDALONE
  _parent->_hasTDataChanged = 1;
#endif
}

void LMWindow::MoveWindow(Coord x, Coord y) 
{
  _propTable->Update(int(REL_X), &x);
  _propTable->Update(int(REL_Y), &y);
#ifndef STANDALONE
  _parent->_hasTDataChanged = 1;
#endif
}

void LMWindow::ResizeWindow(unsigned w, unsigned h)
{
  _propTable->Update(int(WIDTH), &w);
  _propTable->Update(int(HEIGHT), &h);
#ifndef STANDALONE
  _parent->_hasTDataChanged = 1;
#endif
}

ostream &operator <<(ostream &os, LMWindow &w)
{
  os << "-----------------------------------------------------------" << endl;
  os << "Window : " << w.Name() << endl;
  os << "origin :(" << w.origX() << "," << w.origY() << ")" << endl;
  os << "rel posn :(" << w.relX() << "," << w.relY() << ")" << endl;
  os << "width : " << w.Width() << " height : " << w.Height() << endl;
  os << "bg : " << w.BgColor() << " fg : " << w.FgColor() << endl;
  os << "LayoutMode : " << LMLayout::ModeTypeToStr(w.GetLayoutMode()) << endl;
  os << "Level : " << w._level  << " , " ; 
  os << "Parent : ";
  if (w._parent) {
    os << w._parent->Name();
  }
  os  << endl;
  os << "Children :  " << endl;
  
  if (w._childWindows) {
    int index = w._childWindows->InitIterator();
    while(w._childWindows->More(index)) {
      LMWindow *child = w._childWindows->Next(index);
      os << child->Name() << " ";
    }
    w._childWindows->DoneIterator(index);
  }
  os << endl;
  os << "-----------------------------------------------------------" << endl;
  return os;
}

int LMWindow::Write(void)
{
  FILE *fp = fopen(_tdataFile, "w");
  if (!fp) {
    fprintf(stderr, "Could not open tdataFile, %s\n", _tdataFile);
    return 0;
  } else {
    printf("Dumping to file %s\n", _tdataFile);
  }
  int index = InitChildListIterator();
  while(MoreChildren(index)) {
    LMWindow *child = NextChild(index);
    // Change coords to DEVise format 
//    Coord x = ( child->origX() + child->relX() ) /  LMLayout::_xscale;
//    Coord y = ( child->origY() + child->relY() ) /  LMLayout::_yscale; 
    Coord x = ( child->relX() ) /  LMLayout::_xscale;
    Coord y = ( child->relY() ) /  LMLayout::_yscale; 
    unsigned width = (unsigned) (child->Width() / LMLayout::_xscale);
    unsigned height = (unsigned) (child->Height() / LMLayout::_yscale);

    // Schema for the TData is 
    // x y z color size pattern orientation shape shapeattr1 shapeattr2
    //  where shape = 17, shapeattr1 = viewname 
    //  shapeattr2 = width , shapeattr3 = height
   
    fprintf(stdout, "%f %f %f %d %d %d %d %d %s %u %u\n",
	    x, y, 1.0, 2, 1, 1, 1, VIEW_SYMBOL, child->Name(), width, height);
 
    fprintf(fp, "%f %f %f %d %d %d %d %d %s %u %u\n",
	    x, y, 1.0, 2, 1, 1, 1, VIEW_SYMBOL, child->Name(), width, height);
  }
  DoneChildListIterator(index);
  fclose(fp);
  return 1;
}

int LMWindow::Read(void) 
{
  printf("Read: Not implemented yet\n");
  return 0;
}





