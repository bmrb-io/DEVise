#include"typed_rtree.h"

typed_rtree_t::typed_rtree_t(db_mgr_t *my_mgr)
{
  my_int_rtree = new rtree_t(my_mgr);
}


typed_rtree_t::~typed_rtree_t()
{
  delete my_int_rtree;
}


rc typed_rtree_t::create(page_id_t  &hdr_pid, 
			 int        num_dim,
			 int        data_sz,
			 bool       point_keys,
			 const char *key_type_info)
{
  JTRY(my_int_rtree->create(hdr_pid, num_dim, data_sz, point_keys, key_type_info));
  return RC_OK;
}


rc typed_rtree_t::open(const page_id_t& hdr_pid)
{
  JTRY(my_int_rtree->open(hdr_pid));
  return RC_OK;
}


rc typed_rtree_t::close()
{
  JTRY(my_int_rtree->close());
  return RC_OK;
}


rc typed_rtree_t::bulk_load(int        data_file,
			    page_id_t& hdr_pid,
			    const char *key_type_info)
{
  JTRY(my_int_rtree->bulk_load(data_file, hdr_pid, key_type_info));
  return RC_OK;
}


rc typed_rtree_t::fetch_init(typed_cursor_t    &cursor, 
			     const typed_key_t &key) const
{
  cursor.query_key=new typed_key_t(key);
  cursor.int_query_key=key.conv_int_key();
  cursor.type_info = new char [strlen(key.type_info)+1];
  strcpy(cursor.type_info, key.type_info);
  JTRY(my_int_rtree->fetch_init(cursor.int_cursor, *cursor.int_query_key, 
			       cursor.type_info));
  cursor.ret_key=NULL;
  cursor.int_ret_key=NULL;
  printf("query:");cursor.query_key->debug();printf("\n");
  printf("query:");cursor.int_query_key->print(stdout);printf("\n");
  return RC_OK;
}


rc typed_rtree_t::fetch(typed_cursor_t &cursor, 
			typed_key_t *  &key, 
			void *         &data, 
			size_t         &data_sz,
			bool           &eof) const
{
  if (cursor.ret_key != NULL)
    delete(cursor.ret_key);
  bool done = false;
  eof = false;
  while ((!done) && (!eof))
    {
      JTRY(my_int_rtree->fetch(cursor.int_cursor, cursor.int_ret_key, data, 
			      data_sz, eof));
      if (!eof)
	{
	  cursor.ret_key = new typed_key_t(*cursor.int_ret_key, (char *)data, 
					   cursor.type_info);
	  if (cursor.ret_key->overlaps(*cursor.query_key))
	    done = true;
	}
    }
  if (eof)
    {
      delete [] cursor.type_info;
      delete(cursor.int_query_key);
      delete(cursor.query_key);
    }
  else
    key = cursor.ret_key;
  return RC_OK;
}


rc typed_rtree_t::debug(FILE *outfile) const
{
  JTRY(my_int_rtree->debug(outfile));
  return RC_OK;
}
