#include <stdio.h>

main()
{
  unsigned long x = 0x01020304;
  unsigned short y = 0x0102;
  unsigned char *c;

  c = (unsigned char *) &x;
  if ((*c) == 1) printf("unsigned long: MSB first\n");
  else if ((*c) == 4) printf("unsigned long: LSB first\n");
  else printf("unsigned long: strange byte order: 0x01020304 = %d %d %d %d\n",
    *c, *(c+1), *(c+2), *(c+3));

  c = (unsigned char *) &y;
  if ((*c) == 1) printf("unsigned short: MSB first\n");
  else printf("unsigned short: LSB first\n");
}

