#include"dbunix.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include"rtree.h"
#include<unistd.h>

char *bulk_temp_file_name="bulk_temp";

int _debug = 0;

main()
{
  remove("VOLUME");
  db_mgr_unix mgr("VOLUME", 10*1024*1024);
  rtree_t my_rtree(&mgr);
  page_id_t hdr_pid;
  int fd=open("test_bulk_file", O_RDWR);
  printf("bulk_loading\n");
  my_rtree.bulk_load(fd, hdr_pid);
  printf("outputting\n");
  FILE *debug_file=fopen("rtree_out.rec", "w");
  my_rtree.debug(debug_file);
  fclose(debug_file);
  close(fd);
  cursor_t rtree_cursor;
  int_key_t query_key("2.200.200.500.500",0);
  my_rtree.fetch_init(rtree_cursor, query_key);
  bool eof=false;
  while (!eof)
    {
      int_key_t *ret_key;
      void *ret_data;
      size_t data_sz;
      my_rtree.fetch(rtree_cursor, ret_key, ret_data, data_sz, eof);
      assert(data_sz==0);
      if (!eof)
	ret_key->print(stdout); printf("\n");
    }
}
