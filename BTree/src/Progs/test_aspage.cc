/* -*- C++ -*- */
#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include "dbinterface.h"
#include "alignedsortedpage.h"

class my_page_t : public db_page_t {
 public:
  my_page_t(int size) { 
    space = new char[size+3]; 
    my_page_t::size = size;
    while(((int)space % 4) != 0) {
      printf("mod = %d\n",(int)space % 4);
      space++;
    }
  }
  ~my_page_t() { delete []space; }
  virtual size_t get_space_size() { return size; }
  virtual char * get_space_start () { return space; }
  virtual page_id_t get_page_id () { return id; }
 private:
  char* space;
  int size;
  page_id_t id;
};

int dbl_comp(const void* a, const void* b)
{
  double A,B;
  sscanf((char*)a, "%lf", &A);
  sscanf((char*)b, "%lf", &B);
  if(A < B) return -1;
  if(A == B) return 0;
  return 1;
}

char buffer[256];

void page_output(aligned_sorted_page_t& page, FILE* fdout)
{
  fprintf(fdout, "%s\n",buffer);
  fprintf(fdout, "number of slots = %d\n",page.slots_filled());
  fprintf(fdout, "space free = %d\n",page.space_free());
  fprintf(fdout, "space used = %d\n",page.space_used());
  for(int i = 1; i <= page.slots_filled(); i++)
    {
      int len = page.slot_size(i);
      fprintf(fdout, "slot %d  length %d value $",i,len);
      const char* b = (char*)page.slot_val(i);
      for(int j = 0; j < len; j++)
	fprintf(fdout, "%c",b[j]);
      fprintf(fdout,"$\n");
    }
  fprintf(fdout,"\n");
}

main(int argc, char** argv)
{
  if(argc < 4) 
    {
      printf("Usage: test_aspage input-filename output-filename page-size\n");
      exit(1);
    }
  FILE *fdin = fopen(argv[1],"r");
  assert(fdin != NULL);
  FILE *fdout = fopen(argv[2],"w");
  assert(fdout != NULL);
  int size = atoi(argv[3]);
  assert(size >= 12);

  aligned_sorted_page_t page(new my_page_t(size), aligned_sorted_page_t::INITIALIZE_NEW);

  while(fgets(buffer, 256, fdin) != NULL)
    {
      switch(buffer[0]) {
      case 'i':
      case 'I':
	page.insert_slot((void*)(buffer+1), strlen(buffer)-2, dbl_comp);
	break;
      case 'r':
      case 'R':
	page.remove_slot(atoi(buffer+1));
	break;
      case 'o':
      case 'O':
	page_output(page, fdout);
	break;
      case 's':
      case 'S':
	{
	  fprintf(fdout,"Split\n");
	  aligned_sorted_page_t new_page(new my_page_t(size), aligned_sorted_page_t::INITIALIZE_NEW);
	  page.split_insert((void*)(buffer+1), strlen(buffer)-2, dbl_comp, new_page);
	  fprintf(fdout,"this page:\n");
	  fprintf(fdout,"----------\n");
	  page_output(page,fdout);
	  fprintf(fdout,"right page:\n");
	  fprintf(fdout,"-----------\n");
	  page_output(new_page,fdout);
	  fprintf(fdout,"\n");
	}
	break;
      default:
	assert(0);
	break;
      }
    }

  fclose(fdin);
  fclose(fdout);
}


