# include"ClientAPI.h"
main()
{

	ClientControl control;
	
	control.StartSession();
	while(1){
		control.Run();
	}

}


