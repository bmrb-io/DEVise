/* -*- C++ -*- */
#include <stdio.h>
#include <stdlib.h>
#include "setup.h"
#include "dbinterface.h"
#include "alignedsortedpage.h"
#include "btree.h"
#include "dbunix.h"

int _debug = 0;

main(int argc, char** argv)
{
  if(argc < 2) 
    {
      printf("Usage: test_btree input-filename\n");
      exit(1);
    }
  FILE *fdin = fopen(argv[1],"r");
  assert(fdin != NULL);

  char volume_name[256];
  fgets(volume_name, 256, fdin);
  volume_name[strlen(volume_name)-1] = 0;
  char buffer[4096];
  int dims, volume_size;
  fscanf(fdin,"%d%d",&volume_size,&dims);
  fgets(buffer, 4096, fdin);
  
  // the buffer manager
  db_mgr_unix mgr(volume_name+1,volume_size);
  
  // the B-tree
  btree_t* tree = new btree_t(&mgr);
  switch(volume_name[0]) {
  case 'c':
  case 'C':
    {
      page_id_t header;
      JTRY(mgr.alloc_page(header));
      JTRY(tree->create(header, dims));
    }
    break;
  case 'o':
  case 'O':
    {
      page_id_t header;
      header.data[0] = 0;
      header.data[1] = 0;
      header.data[2] = 0;
      header.data[3] = 1;
      JTRY(tree->open(header));
    }
    break;
  default:
    assert(0);
    break;
  }
  
  // The key
  dbword_t* key = new dbword_t[2*dims];
  assert(key != NULL);

  while(fgets(buffer, 256, fdin) != NULL)
    {
      switch(buffer[0]) {
      case 'i': // insert
      case 'I':
	{
	  for(int i = 0; i < dims; i++)
	    fscanf(fdin,DBWORD_FMT,key+i);
	  fgets(buffer, 256, fdin);
	  fgets(buffer, 256, fdin);
	  int_key_t K(key, dims, true);
	  JTRY(tree->insert(K, (void*)buffer, strlen(buffer) - 1));
	}
	break;
      case 'r': // remove
      case 'R':
	{
	  for(int i = 0; i < dims; i++)
	    fscanf(fdin,DBWORD_FMT,key+i);
	  fgets(buffer, 256, fdin);
	  fgets(buffer, 256, fdin);
	  int_key_t K(key, dims, true);
	  JTRY(tree->remove(K, (void*)buffer, strlen(buffer) - 1));
	}
	break;
      case 'o': // output
      case 'O':
	{
	  buffer[strlen(buffer)-1] = 0;
	  if(strlen(buffer) < 2)
	    tree->debug(stdout);
	  else
	    {
	      FILE* fdout = fopen(buffer+1,"w");
	      assert(fdout != NULL);
	      tree->debug(fdout);
	      fclose(fdout);
	    }
	}
	break;
      case 's': // btree search
      case 'S':
	{
	  buffer[strlen(buffer)-1] = 0;
	  FILE* fdout = fopen(buffer+1,"w");
	  assert(fdout != NULL);	  
	  for(int i = 0; i < 2*dims; i++)
	    fscanf(fdin,DBWORD_FMT,key+i);
	  fgets(buffer, 256, fdin);
	  int_key_t K(key, dims, false);
	  cursor_t cursor;
	  JTRY(tree->init_range_query(cursor, K));
	  bool eof = false;
	  int_key_t* Key;
	  void* element;
	  unsigned int elem_size;
	  JTRY(tree->get_next(cursor, Key, element, elem_size, eof));
	  while(! eof)
	    {
	      btree_entry_t entry(*Key, element, elem_size);
	      entry.print(fdout, false);
	      fprintf(fdout,"\n");
	      JTRY(tree->get_next(cursor, Key, element, elem_size, eof));
	    }
	  fclose(fdout);
	}
	break;
      case 'f': // multidimensional search 
      case 'F':
	{
	  buffer[strlen(buffer)-1] = 0;
	  FILE* fdout = fopen(buffer+1,"w");
	  assert(fdout != NULL);	  
	  for(int i = 0; i < 2*dims; i++)
	    fscanf(fdin,DBWORD_FMT,key+i);
	  fgets(buffer, 256, fdin);
	  int_key_t K(key, dims, false);
	  cursor_t cursor;
	  JTRY(tree->fetch_init(cursor, K));
	  bool eof = false;
	  int_key_t* Key;
	  void* element;
	  unsigned int elem_size;
	  JTRY(tree->fetch(cursor, Key, element, elem_size, eof));
	  while(! eof)
	    {
	      btree_entry_t entry(*Key, element, elem_size);
	      entry.print(fdout, false);
	      fprintf(fdout,"\n");
	      JTRY(tree->fetch(cursor, Key, element, elem_size, eof));
	    }
	  fclose(fdout);
	}
	break;
      default:
	assert(0);
	break;
      }
    }

  fclose(fdin);
  delete tree;
}



