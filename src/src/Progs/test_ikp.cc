#include"dbunix.h"
#include"int_keyed_p.h"

int _debug = 1;

main()
{
  db_mgr_unix mgr("VOLUME", 10*1024*1024);
  page_id_t pid;
  db_page_t *db_page;

  printf("ret_val:%d\n", mgr.alloc_page(pid));
  printf("ret_val:%d\n", mgr.pin(pid, db_page));

  int_keyed_p_t page(*db_page);
  page.init_p(2, 0, true);
  printf("num_entries:%d   max_num_entries:%d   point_keys:%d   dim:%d\n", 
	 page.num_entries(), page.max_num_entries(), page.point_keys(), page.dim());

  int_key_t key("2.0.5", true);
  printf("inserting:");
  key.print();
  printf("\n");
  char dummy[100];
  page.insert_entry(key, (void *)dummy);
  int_key_t key2(2);
  page.get_key(0, key2);
  printf("key_value returned was: ");
  key2.print();
  printf("\n");

  page.init_p(2, 0, true);
  printf("num_entries:%d   max_num_entries:%d   point_keys:%d   dim:%d\n", 
	 page.num_entries(), page.max_num_entries(), page.point_keys(), page.dim());
  dbword_t point[2];
//  page.dump();
  for (long long i=0; i<page.max_num_entries(); i++)
    {
//      page.dump();
      point[0]=point[1]=i;
      int_key_t my_key(point, 2, true);
//      my_key.print();
//      printf("\n");
      page.insert_entry(my_key, (void *)dummy);
    }
  printf("num_entries:%d   max_num_entries:%d   point_keys:%d   dim:%d\n", 
	 page.num_entries(), page.max_num_entries(), page.point_keys(), page.dim());
//  page.dump();
//  page.remove_entry(1);
//  page.dump();
  int_key_t my_key(2);
  int_key_t *key_ptr;
  for (int j=1; j<50000; j++)
    {
      ikp_cursor_t cursor;
      cursor.connect_to_ikp(page);
//      while (cursor.more_entries())
	key_ptr = cursor.next();
    }
  for (int i=0; i<page.num_entries(); i++)
    {
      page.get_key(i, my_key);
      my_key.print();
      printf("\n");
    }
  // page.dump();
  delete(db_page);
