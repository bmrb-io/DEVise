#include<stdlib.h>
#include<stdio.h>

main()
{
  int dim;
  long long new_bound[10];

  int RetVal = 5;
  printf("%d %d %d \n", 2, 4, 0);
  int BoxNum = 1;
  while (RetVal == 5)
    {
      RetVal = scanf("%d.%lld.%lld.%lld.%lld", &dim, 
		     &(new_bound[0]), &(new_bound[1]), &(new_bound[2]),
		     &(new_bound[3]));
      if (RetVal == 5)
	{
	  fwrite((void *)new_bound, 4*sizeof(long long), 1, stdout);
	  fwrite((void *)&(BoxNum), 4, 1, stdout);
	}
      BoxNum++;
    }
}
