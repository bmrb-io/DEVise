#include "utils.cc"
#include <assert.h>

#define SIZE 1000

void insert(FILE* fd, int dims)
{
  int size = get_int("Enter number of entries");
  int seed = get_int("Enter randomization seed");
  srand48(seed);
  for(int j = 0; j < size; j++)
    {
      fprintf(fd,"i\n");
      for(int i = 0; i < dims; i++)
	fprintf(fd,"%d ",(int)(drand48() * SIZE));
      fprintf(fd,"\n%g\n",drand48()*100.0);
    }    
  char* name = get_string("Enter output file name");
  fprintf(fd,"o%s\n",name);
  delete []name;
}

void search(FILE* fd, int dims)	
{
  int size = get_int("Enter size of input");
  char* name = get_string("Enter output file name");
  fprintf(fd,"%s\n",name);
  delete name;
  int i;
  for(i = 0; i < dims; i++)
    {
      int j = 0;
      if(i < size)
	j = get_int("Enter low");
      fprintf(fd,"%d ",j);
    }
  fprintf(fd,"\n");
  for(i = 0; i < dims; i++)
    {
      int j = SIZE;
      if(i < size)
	j = get_int("Enter high");
      fprintf(fd,"%d ",j);
    }
  fprintf(fd,"\n");
}

void volume(FILE* fd, int dims)
{
  char* name = get_string("Enter volume name");
  fprintf(fd,"%s\n",name); // Create the tree
  delete []name;
  int size = get_int("Enter volume size (in K)");
  fprintf(fd,"%d %d \n",size * 1024, dims);
}

main(int argc, char** argv)
{
  char* data = get_string("Enter data file name");
  FILE* fd = open_file(data,"w");
  delete []data;
  int dims = get_int("Enter number of dimensions");

  if(question("Do you want to create a new tree"))
    {
      fprintf(fd,"c"); // Create the tree
      volume(fd,dims);
      insert(fd, dims);
    }
  else
    {
      fprintf(fd,"o"); // open the tree
      volume(fd, dims);
      printf("Choose from the following:\n");
      printf("1. insert more entries\n");
      printf("2. B-tree search\n");
      printf("3. MD search\n");
      int choice = get_int("Enter choice");
      switch(choice) {
      case 1:
	insert(fd,dims);
	break;
      case 2:
	fprintf(fd,"s");
	search(fd,dims);
        break;
      case 3:
	fprintf(fd,"f");
	search(fd,dims);
	break;
      default:
	break;
      }
    }
  fclose(fd);
} 



