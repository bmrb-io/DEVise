#include <iostream.h>
#include "HashTable.h"
#include "Window.h"

main()
{
  LMWindow w1("w1", 0,  0,  10, 10, 0, 1);
  LMWindow w2("w2", 0,  0,  10, 10, 0, 1);
  LMWindow w3("w3", 0,  0,  10, 10, 0, 1);
  
  w1.AddChildWindow(&w2);
  w1.AddChildWindow(&w3);
  cout << w1;
  w1.DeleteChildWindow(&w2);
  cout << w1;
}
