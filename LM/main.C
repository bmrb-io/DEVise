#include <iostream.h>
#include "HashTable.h"


main()
{
  HashTable hashTable(60, 7);
  
  int vali = 7;
  hashTable.Insert("Shilpa", INT, &vali);
  float valf = 0.8;
  hashTable.Insert("Sowmya", FLOAT, &valf);
  double vald = 1.2334;
  hashTable.Insert("Jyothi", DOUBLE, &vald);
  hashTable.Insert("Shankar", STRING, "bhaiiya");
  int vali2 = 186;
  hashTable.Insert("Tintin", INT, &vali2);
  hashTable.Insert("Nitin", INT, &vali2);
  hashTable.Insert("Tints", INT, &vali2);

  cout << hashTable;

  cout << hashTable.LookUp("Sowmya");
  cout << hashTable.LookUp("Shankar");
  cout << hashTable.LookUp("Tints");
  cout << hashTable.LookUp("Jyothi");
  cout << hashTable.LookUp("Shilpa");

  hashTable.Delete("Shilpa");
  hashTable.Delete("Tintin");
  cout << hashTable;
}
