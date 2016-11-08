#include <stdio.h>

main()
{
  unsigned long x = 0x01020304;
  unsigned char *c;

  c = (unsigned char *) &x;
  if ((*c) == 1) printf("VR_ULONG_MSB_FIRST\n");
  else printf("VR_ULONG_LSB_FIRST\n");
}

