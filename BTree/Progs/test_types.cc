#include<stdlib.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include"typed_key.h"
#include"typed_rtree.h"
#include"dbunix.h"

char *bulk_temp_file_name="bulk_temp";

int _debug = 0;

main()
{
  remove("VOLUME");
  db_mgr_unix mgr("VOLUME", 10*1024*1024);
  typed_rtree_t my_rtree(&mgr);
  page_id_t hdr_pid;
  int fd=open("test_bulk_file", O_RDWR);
  printf("bulk_loading\n");
  my_rtree.bulk_load(fd, hdr_pid, "vv");
  /*  printf("outputting\n");
  FILE *debug_file=fopen("rtree_out.rec", "w");
  my_rtree.debug(debug_file);
  fclose(debug_file);*/
  close(fd);
  typed_cursor_t typed_cursor;
  long long query_bounds[4];
  query_bounds[0]=100;
  query_bounds[1]=100;
  query_bounds[2]=200;
  query_bounds[3]=200;

  typed_key_t query_key((char *)query_bounds, 2, "vv", 0);
  my_rtree.fetch_init(typed_cursor, query_key);
  bool eof=false;
  while (!eof)
    {
      typed_key_t *ret_key;
      void *ret_data;
      size_t data_sz;
      my_rtree.fetch(typed_cursor, ret_key, ret_data, data_sz, eof);
      assert(data_sz==0);
      if (!eof)
	{printf("answer:"); ret_key->debug(); printf("\n");}
    }
}
