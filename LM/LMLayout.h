#ifndef _LAYOUT_H_
#define _LAYOUT_H_
//#include "Window.h"

/* A new layout class combining different kinds of layouts */

typedef double Coord;
enum LayoutMode { AUTO = 0, VERTICAL, HORIZONTAL, STACKED, CUSTOM };

class LMWindow;

class LMLayout {
  friend class LMWindow;

  private :
  LayoutMode _mode;
  int verRequested;
  int horRequested;

  LMWindow *_winLayout;

  protected: 
  static int _frameWidth;
  static float _xscale;
  static float _yscale;
  
  public :
  LMLayout(LMWindow *win, LayoutMode mode = AUTO);

  void SetLayoutMode(LayoutMode mode) {
    _mode = mode;
  }

  LayoutMode GetLayoutMode() {
    return _mode;
  }
  int InitIterator();
  bool More(int index);
  LMWindow *Next(int index);
  void DoneIterator(int index);

  // Add Window to layout (called when a new child window is created/destroyed)
  void Append(LMWindow *child);
  void Delete(LMWindow *child);

  void SetLayoutProperties(LayoutMode mode, int rows, int columns);
  void GetLayoutProperties(LayoutMode &mode, int &rows, int &columns);
  
  void UpdateLayout(Coord oldX, Coord oldY, unsigned oldW, unsigned oldH);

  // general functions to convert to and from string and layoutMode
  static LayoutMode ModeStrToType(char *str);
  static char *ModeTypeToStr(LayoutMode mode);
  static void SetFrameWidth(int width);
  static void SetScale(float xscale, float yscale);

  protected :
  void MapChildren(LMWindow *single = 0, bool resize = false,
		   int extraWeight = 0, Coord *x = 0, Coord *y = 0,
		   unsigned int *w = 0, unsigned int *h = 0,
		   Coord *origX = 0, Coord *origY = 0);
  void ComputeLayout(unsigned int w, unsigned int h,
		     unsigned int numViews,
		     int &horViews, int &verViews);
  void ScaleChildren(unsigned oldW, unsigned oldH,
		     unsigned newW, unsigned newH,
		     Coord newOrigX, Coord newOrigY);
  
};

#endif



