#ifdef VIEW_SHAPE
#include <sys/param.h>
#include <iostream.h>

#include "ViewShape.h"
#include "MappingInterp.h"
#include "Map3D.h"
#include "Init.h"
#include "Util.h"
#include "DevError.h"
#include "ClassDir.h"
#include "Control.h"
#include "StringStorage.h"
//#define DEBUG 1

int FullMapping_ViewShape::NumShapeAttrs()
{
  return 3;
}

void FullMapping_ViewShape::MaxSymSize(TDataMap *map, void *gdata, 
				       int numSyms, 
				       Coord &width, Coord &height)
{
  width = 0.0;
  height = 0.0;
}


void FullMapping_ViewShape::DrawGDataArray(WindowRep *win, 
					   void **gdataArray,
					   int numSyms, TDataMap *map, 
					   ViewGraph *view, 
					   int pixelSize,
					   int &recordsProcessed)
{
#if defined(DEBUG)
  printf("%s\n", __PRETTY_FUNCTION__);
#endif

  if (view->GetNumDimensions() == 3) {
    Draw3DGDataArray(win, gdataArray, numSyms, map, view, pixelSize,
      recordsProcessed);
    return;
  }
  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  //TEMP TEMP
  
  // For now display a rectangular shape with the bgcolor being the 
  // color of the view specified

  Coord x0, y0, x1, y1;
  win->Transform(0, 0, x0, y0);
  win->Transform(1, 1, x1, y1);
  Coord pixelWidth = 1 / fabs(x1 - x0);
  Coord pixelHeight = 1 / fabs(y1 - y0);
  //TEMP TEMP
  
  //
  // Shape attributes should store:
  //   attr[0] = name of view
  //   attr[1] = width of symbol to be drawn 
  //   attr[2] = height of symbol to be drawn 
  
  ShapeAttr *defaultAttrs = map->GetDefaultShapeAttrs();
  unsigned long attrflags = map->GetDynamicShapeAttrs();
  Coord attrValue;
  AttrInfo *attrinfo;
  int gdataOffset;
  GDataAttrOffset *offset = map->GetGDataOffset();
  char *gdata, *viewname;

  Coord x, y;
  for (int i = 0; i < numSyms; i++) {
    gdata = (char *) gdataArray[i];
    x = ShapeGetX(gdata, map, offset);
    y = ShapeGetY(gdata, map, offset);
    
    viewname = NULL;
    
    // The color is obtained from the viewname 
    if (!(attrflags & (1 << 0))) {
      reportError("No view name specified" , devNoSyserr);
    } 
    char *label;
    //    if ((offset->shapeAttrOffset[0] >= 0)  ||
    //	isStr(((MappingInterp *)map)->GetMappingCmd()->shapeAttrCmd[0])) {
    int key = (int)GetShapeAttr0(gdata, map, offset);
    int code = StringStorage::Lookup(key, viewname);
    if (code < 0) {
      label = "X";
#ifdef DEBUG
      printf("Key %d returns \"%s\", code %d\n", key, viewname, code);
#endif
    } else {
#ifdef DEBUG
      // Should not occur any more
      printf("Using default label \"%s\"\n",viewname);
#endif
    }
    if (viewname == NULL)
      {
	reportError("No view name specified", devNoSyserr);
	continue;
      }
    //    Coord width = GetShapeAttr1(gdata, map, offset);
    //    Coord height = GetShapeAttr2(gdata, map, offset);
    //#ifdef DEBUG
    //    printf("viewname = %s, width = %f , height = %f \n",
    //	   viewname, width, height );
    //#endif
    
    // TEMP TEMP
    // Do transformations to get width and height
    // For now display a rectangular shape with the bgcolor being the 
    // color of the view specified

    Coord tx, ty;
    Coord width, height;

    win->Transform(x, y, tx, ty);
    win->Transform(fabs(GetSize(gdata, map, offset) 
			* GetShapeAttr1(gdata, map, offset)),
		   0.0, x1, y1);
    width = x1 - x0;
    width = ABS(width);
    
    height = fabs(GetShapeAttr2(gdata, map, offset)) /
      fabs(GetShapeAttr1(gdata, map, offset)) * width;
    height = ABS(height);
    
    width *= pixelSize;
    height *= pixelSize;
    
    width = MAX(width, pixelSize);
    height = MAX(height, pixelSize);
    //TEMP TEMP
    
    ViewWin *viewsym = (ViewWin *)classDir->FindInstance(viewname);
    DOASSERT(viewsym, "View not found");
    GlobalColor color = viewsym->GetBgColor();
//    printf("Color = %ld \n", color);
    if (color == XorColor)
      win->SetXorMode();
    else
      win->SetFgColor(color);
    win->SetPattern(GetPattern(gdata, map, offset));
    win->SetLineWidth(GetLineWidth(gdata, map, offset));
    
    /*    win->FillPixelRect(tx, ty, width, height); */
    
    if (view->GetDisplayDataValues())
      DisplayDataLabel(win, x, y, y);

#ifdef 0
    int argc;
    char **argv;
    
    classDir->CreateParams("view" , "Scatter",  viewsym->GetName(), 
			   argc, argv);
    printf("Printing view parameters");
    for (int j = 0; j < argc; j++) {
      cout << "argv[" << j << "] = " << argv[j] << endl;
    }
    sprintf(argv[0], "new%s", viewsym->GetName());
    classDir->CreateWithParams("view", "Scatter", argc, argv);
    ViewGraph *newview = (ViewGraph *) classDir->FindInstance(argv[0]);

    /* check last mapping only */
    TDataMap *mmap;
    int index = ((ViewGraph *)viewsym)->InitMappingIterator();
    while(((ViewGraph *)viewsym)->MoreMapping(index)) {
      mmap = ((ViewGraph *)viewsym)->NextMapping(index)->map;
    }
    ((ViewGraph *)viewsym)->DoneMappingIterator(index);
    
    newview->InsertMapping(mmap);
    newview->AppendToParent(view);
    newview->Map(tx, ty, width, height);
    newview->Refresh();
#endif
    viewsym->AppendToParent(view);
    viewsym->Map((int)tx, (int)ty, (unsigned int) width, (unsigned int)height);
    ((View *)viewsym)->Refresh();
    if (color == XorColor)
      win->SetCopyMode();
  }
  
  recordsProcessed = numSyms;
}
#endif













