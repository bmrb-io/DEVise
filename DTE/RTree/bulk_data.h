/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
 */

#ifndef BULK_DATA_H
#define BULK_DATA_H
#include<stdlib.h>

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


    void sort (int start, 
	       int n, 
	       int dim_to_sort);


    void btreesort (int start, 
		    int n, 
		    int dim_to_sort);


    virtual int elem_per_page(int elem_to_start, 
			      int num_elem);


    void calc_cuts(cut_map_t &cut_map,
		   int       elem_to_start,
		   int       num_elem,
		   int       dim_to_cut);


    char *key(int entry_num);


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

  private:
    static int SortCmp (int *A, 
			int *B);


    static int SortCmp2 (int *A, 
			 int *B);


    void generate_poss_cuts(cut_map_t &cut_map,
			    int       elem_to_start,
			    int       num_elem,
			    int       dim_to_cut);


    int leftmost(int start_val,
		 int num_elem);


    int rightmost(int start_val,
		  int num_elem);

};

#endif
