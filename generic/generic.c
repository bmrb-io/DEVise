/* dispAfsio.c: display tweety
*/
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include "DeviseTypes.h"
#include "Dispatcher.h"
#include "Display.h"
#include "ColorMgr.h"
#include "Control.h"
#include "Init.h"
#include "WinClassInfo.h"
#include "MapInterpClassInfo.h"
#include "VisualLinkClassInfo.h"
#include "VisualArg.h"
#include "View.h"
#include "AxisLabelClassInfo.h"
#include "ViewClassInfo.h"
#include "CompositeParser.h"
#include "RecInterp.h"
#include "AttrList.h"
#include "QueryProc.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "CursorClassInfo.h"
#include "ParseCat.h"

int debug = 0;

/* Generic composite parser */
class GenericComposite : public UserComposite {
public:

  GenericComposite() { }
  virtual void Decode(RecInterp *recInterp) { }

};

main(int argc, char **argv)
{
  Init::DoInit(argc,argv);

  /* Register composite parser */
  CompositeParser::Register("Generic-Schema", new GenericComposite);

  /* Register known classes  with control panel */
  ControlPanel::RegisterClass(new WinVerInfo);
  ControlPanel::RegisterClass(new ViewXInfo);
  ControlPanel::RegisterClass(new ViewScatterInfo);
  ControlPanel::RegisterClass(new VisualLinkClassInfo());
  ControlPanel::RegisterClass(new CursorClassInfo());

  /* hack to start control panel so that it'll read the RC files */
  ControlPanel *ctrl = ControlPanel::Instance();

  /* This is a hack to create a display before running Dispatcher.
     Otherwise, we'll get an error */
  DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();
  
  /* Start session (possibly restoring an old one */
  ctrl->StartSession();

  Dispatcher::RunNoReturn();
}
