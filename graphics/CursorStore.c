#include <iostream.h>
#include "CursorStore.h"
#include "Exit.h"

#if defined(USERGB)
CursorStore::CursorStore(): _inited(0), _need_draw(1), rgba(0), depth(0), _tot(-1) {}
#else
CursorStore::CursorStore(): _inited(0), _need_draw(1), color_index(0), depth(0), _tot(-1) {}
#endif

CursorStore::~CursorStore() {
#if defined(USERGB)
  if (rgba) delete [] rgba;
#else
  if (color_index) delete [] color_index;
#endif
  if (depth) delete [] depth;
}

void CursorStore::Dump() {

  DOASSERT(_inited, "CursorStore not inited before used");
  
  cout << "CursorStore Dump " << endl;
  for(int i=0; i<_tot; i++) {
#if defined(USERGB)
     if ( i % 3 == 0) cout << endl;
     cout << rgba[i*4] <<"\t" << rgba[i*4+1] << "\t"
          << rgba[i*4+2] << "\t" << rgba[i*4+3] << "\t"; 
#else
     if ( i % 10 == 0) cout << endl;
     cout << color_index[i] <<"\t"; 
#endif
  }
  cout << endl;
}


// to calculate the region that cursor will cover
void CursorStore::Clear() {
  _inited=0;
  _need_draw=1;
}

void CursorStore::OneCoord(int x, int y) // one coord in the region
{
  if (!_inited) {
    _min_x=x;
    _min_y=y;
    _max_x=x;
    _max_y=y;
    _inited=1;
  }
  else {
    if (x<_min_x)
      _min_x=x;
    if (y<_min_y)
      _min_y=y;
    if (x>_max_x)
      _max_x=x;
    if (y>_max_y)
      _max_y=y;
  }
}

void CursorStore::Expand(int min_x, int min_y, int max_x, int max_y)
{

  // expand the region to cover a bit more to make sure the edges are covered

  DOASSERT(_inited, "CursorStore not inited before used");
  _min_x-=1;
  _min_y-=1;
  _max_x+=1;
  _max_y+=1;
  if (_min_x > max_x || _max_x < min_x || _min_y > max_y || _max_y < min_y) {
    _need_draw=0;
    return;
  }
  if (_min_x < min_x)
    _min_x=min_x;
  if (_max_x > max_x)
    _max_x=max_x;
  if (_min_y < min_y)
    _min_y=min_y;
  if (_max_y > max_y)
    _max_y=max_y;

  int width=_max_x-_min_x+1;
  int height=_max_y-_min_y+1;

  if (_tot != width*height) {
#if defined(USERGB)
    if (rgba) delete [] rgba;
#else
    if (color_index) delete [] color_index;
#endif
    if (depth) delete [] depth;
#if defined(USERGB)
    rgba = new GLfloat[width*height*4];
#else
    color_index = new GLfloat[width*height];
#endif
    depth = new GLfloat[width*height];
    _tot  = width*height;
  }
}

int CursorStore::Valid() {
  return _inited && _need_draw;
}
