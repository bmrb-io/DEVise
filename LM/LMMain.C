#include <iostream.h>
#include "LMControl.h"
#include "LMWindow.h"

LMControl *controlPanel;
int standalone = 1;
main()
{
  controlPanel  = LMControl::GetLMControl("LayoutControl.tk"); 
  controlPanel->Go();
}
