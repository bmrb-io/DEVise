/* testViewWin.c */
#include "Dispatcher.h"
#include "ViewWinVer.h"
#include "ViewWinHor.h"

main(){
	ViewWin *vw1 = new ViewWinVer("Vertical");
	ViewWin *vw2 = new ViewWinVer("sub1", vw1);
	ViewWin *vw3 = new ViewWinVer("sub2", vw1);

	ViewWin *vw4 = new ViewWinHor("Horizontal");
	ViewWin *vw5 = new ViewWinHor("sub5", vw4);
	ViewWin *vw6 = new ViewWinHor("sub6", vw4);
	Dispatcher::RunNoReturn();
}
