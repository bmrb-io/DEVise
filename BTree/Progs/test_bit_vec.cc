#include<stdlib.h>
#include<stdio.h>
#include"bit_vec.h"

void main()
{
  unsigned long long vec1[100];
  unsigned long long vec2[100];
  unsigned long long dummy = 0;

  for(int i=0; i<2; i++)
    vec1[i]=0;
  for(int i=0; i<100; i++)
    vec1[i]=(~0ULL)>>i;
  for(int i=0; i<100; i++)
    vec2[i]=~0;

  for(int i=0; i<5; i++)
    printf("%llx ", vec1[i]);
  printf("\n");
  for(int i=0; i<5; i++)
    printf("%llx ", vec2[i]);
  printf("\n");
  bit_vec_t bit_v1(vec1, 0), bit_v2(vec2, 0), bit_v3(&dummy, 0);


/*  printf("inserting\n");
  for (int j = 0; j<6; j++)
    {
    bit_v2.append(bit_v3, 32);
    for(int i=0; i<5; i++)
      printf("%llx ", vec2[i]);
    printf("\n");
  }
  printf("done inserting\n");
  bit_v2.init_vec(vec2, 0);
  for (int j = 0; j<8; j++)
    {
      for(int i=0; i<100; i++)
	vec1[i]=0;
      bit_v3.init_vec(vec1, 36);
      bit_v2.extract(bit_v3, 32);
      for(int i=0; i<5; i++)
	printf("%llx ", vec1[i]);
      printf("\n");
    }
*/

/*
  for (int j = 0; j<6; j++)
    {
    bit_v2.append(10, 47);
//    for(int i=0; i<5; i++)
//      printf("%llx ", vec2[i]);
//    printf("\n");
  }

  bit_v2.init_vec(vec2, 0);

  for (int j = 0; j<6; j++)
    {
      printf("%d\n", bit_v2.extract(47));
  }
*/


/*  bit_v1.yank(32, 1000);
  for(int i=0; i<10; i++)
    printf("%llx ", vec1[i]);
  printf("\n");*/
  
