#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char buff_[4096];

bool question(const char* message)
{
  printf("%s? ",message);
  fflush(stdout);
  gets(buff_);
  if(strcmp(buff_,"y") == 0)
    return true;
  if(strcmp(buff_,"yes") == 0)
    return true;
  if(strcmp(buff_,"Y") == 0)
    return true;
  if(strcmp(buff_,"YES") == 0)
    return true;
  if(strcmp(buff_,"Yes") == 0)
    return true;
  return false;
}

int get_int(const char* message)
{
  printf("%s: ",message);
  fflush(stdout);
  int i = 0;
  while(1)
    {
      gets(buff_);
      if(sscanf(buff_,"%d",&i) > 0)
	break;
      printf("Please enter an integer: ");
      fflush(stdout);
    }
  return i;
}

double get_real(const char* message)
{
  printf("%s: ",message);
  fflush(stdout);
  double i = 0;
  while(1)
    {
      gets(buff_);
      if(sscanf(buff_,"%lf",&i) > 0)
	break;
      printf("Please enter a float: ");
      fflush(stdout);
    }
  return i;
}

char* get_string(const char* message)
{
  printf("%s: ",message);
  fflush(stdout);
  gets(buff_);
  char* result = new char[strlen(buff_)+1];
  strcpy(result, buff_);
  return result;
}

FILE* open_file(char* filename, char* args)
{
  FILE *fd = fopen(filename, args);
  if(fd == NULL)
    {
      printf("Can't open file %s\n",filename);
      exit(1);
    }
  return fd;
}
