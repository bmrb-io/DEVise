#ifndef BULK_DATA_H
#define BULK_DATA_H
#include<stdlib.h>
#include"int_key.h"

extern char *bulk_temp_file_name;

class cut_map_t
{
  friend class bulk_data_t;
  public:
    cut_map_t(int in_max_cuts);
    
    ~cut_map_t();

    int first(int cut_num);

    int size(int cut_num);

    int num_cuts;

    void display();

  private:
    int max_cuts;
    int **cut_map;
};


class bulk_data_t
{
  public:
    bulk_data();

    virtual ~bulk_data_t();

    void init(int       infile, 
	      int       offset,
	      int       num_dim,
	      bool      point_data,
	      int       num_elem,
	      int       data_size);


    void sort_and_cut (int         start, 
		       int         n, 
		       int         dim_to_sort,
		       cut_map_t * &cut_map,
		       int         (*elem_per_page)(bulk_data_t &, int, int, int, int_key_t &),
		       int         page_size);


    /*    void btreesort (int       start, 
		    int       n, 
		    int       dim_to_sort,
		    int_key_t &bbox);*/


    void read_data(int infile);


    dbword_t *key(int entry_num);


    char *data(int entry_num);

    void display();

    int dim;
    bool pt_data;
    int num_el;
    int data_sz;
    char *file_arr;
    int entry_sz;
    int pages_created;
    int off;
    const int mem_avail=20*1024*1024;

    static int QSortCol;
    static int QSortDim;
    static int *QSortDirection=NULL;
    static bool QSortPoint;

    static void get_line(char *line,
			 int infile);

  private:

    static int SortCmp (dbword_t *A, 
			dbword_t *B);


    /* static int SortCmp2 (dbword_t *A, 
			 dbword_t *B);*/
};

#endif
