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

#include"bulk_data.h"
#include"hfpage.h"
#include<stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include"type_compat.h"
#include<errno.h>
#include<stdio.h>
#include"globals.h"

#pragma implementation

#define minimum(A, B) ((A) < (B) ? (A) : (B))


#define maximum(A, B) ((A) > (B) ? (A) : (B))


cut_map_t::cut_map_t(int in_max_cuts)
{
  max_cuts=in_max_cuts;
  cut_map = (int **) malloc(max_cuts*sizeof(int *));
  for(int cut_num=0; cut_num<max_cuts; cut_num++)
    cut_map[cut_num]=(int *) malloc(2*sizeof(int));
}


cut_map_t::~cut_map_t()
{
  for(int cut_num=0; cut_num<max_cuts; cut_num++)
    free(cut_map[cut_num]);
  free(cut_map);
}


int cut_map_t::first(int cut_num)
{
  return cut_map[cut_num][0];
}


int cut_map_t::size(int cut_num)
{
  return cut_map[cut_num][1];
}


void cut_map_t::display()
{
  printf("cut map:\n");
  for(int cut_num=0; cut_num<num_cuts; cut_num++)
    printf("%d %d\n", cut_map[cut_num][0], cut_map[cut_num][1]);
  printf("\n");
}


bulk_data_t::bulk_data_t()
{
  pages_created=0;
}


bulk_data_t::~bulk_data_t()
{
  munmap(file_arr, (entry_sz)*num_el+off);
  if (QSortDirection != NULL)
    free(QSortDirection);
  QSortDirection=NULL;
}

void bulk_data_t::init(int       infile, 
		       int       offset,
		       int       num_dim,
		       bool      point_data,
		       int       num_elem,
		       int       data_size)
{
  if (QSortDirection==NULL)
    QSortDirection = (int *) malloc(sizeof(int)*num_dim);
  for(int i=0; i<num_dim; i++)
    QSortDirection[i]=1;
  dim = num_dim;
  pt_data = point_data;
  num_el = num_elem;
  data_sz = data_size;
  if (pt_data)
    entry_sz=dim*sizeof(int)+data_sz;
  else
    entry_sz=2*dim*sizeof(int)+data_sz;
  off=offset;
#ifdef HP
  file_arr = mmap((caddr_t) 0, entry_sz*num_elem+offset, 
		  PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FILE|MAP_VARIABLE, infile, 0);
#else
  file_arr = mmap((caddr_t) 0, entry_sz*num_elem+offset, 
		  PROT_READ|PROT_WRITE, MAP_SHARED|MAP_NORESERVE, infile, 0);
#endif
}


int bulk_data_t::SortCmp (int *A, 
			  int *B)
{
  double CenterA, CenterB;
  if (QSortPoint)
    {
      CenterA = A[QSortCol];
      CenterB = B[QSortCol];
    }
  else
    {
      CenterA = (A[QSortCol]+A[QSortCol+QSortDim])/(double)2.0;
      CenterB = (B[QSortCol]+B[QSortCol+QSortDim])/(double)2.0;
    }
  if (CenterA < CenterB)
    return -1*QSortDirection[QSortCol];
  else
    if (CenterA == CenterB)
      return 0;
    else
      return 1*QSortDirection[QSortCol];
}


int bulk_data_t::SortCmp2 (int *A, 
			   int *B)
{
  double CenterA, CenterB;
  for(int i=0; i<QSortDim; i++)
    {
      if (QSortPoint)
	{
	  CenterA = A[i];
	  CenterB = B[i];
	}
      else
	{
	  CenterA = (A[i]+A[i+QSortDim])/(double)2.0;
	  CenterB = (B[i]+B[i+QSortDim])/(double)2.0;
	}
      if (CenterA < CenterB)
	return -1;
      if (CenterA > CenterB)
	return 1;
    }
  return 0;
}


void downheap(void *base, size_t nel, size_t width,
	      int (*compar) (const void *, const void *), int k)
{
  int j;
  char v[width];

  memcpy(v, base+width*(k-1), width);
  while (k <= nel/2)
    {
      j = k+k;
      if (j<nel && (compar(base+width*(j-1), base+width*j) < 0)) j++;
      if (compar(v, base+width*(j-1)) >= 0) break;
      memcpy(base+width*(k-1), base+width*(j-1), width); k = j;
    }
  memcpy(base+width*(k-1), v, width);
}


void heapsort(void *base, size_t nel, size_t width,
	      int (*compar) (const void *, const void *))
{
  int k;
  char t[width];

  for(k=nel/2; k>=1; k--) downheap(base, nel, width, compar, k);
  while (nel > 1)
    {
      memcpy(t, base, width); 
      memcpy(base, base+width*(nel-1), width); 
      memcpy(base+width*(nel-1), t, width);
      downheap(base, --nel, width, compar, 1);
    }
}



void bulk_data_t::sort (int start, 
			int n, 
			int dim_to_sort)
{
  QSortDirection[dim_to_sort-1] *= -1;
  QSortCol = dim_to_sort-1;
  QSortDim = dim;
  QSortPoint = pt_data;

  // calculate bucket stuff
  int num_buckets = 3*n*entry_sz / mem_avail + 1;
  int entries_per_bucket = (n+num_buckets-1)/num_buckets; // (n/num_buckets) round up

  // sort sublists
  int beg_entry_num=start;
  while(beg_entry_num < n+start)
    {
      heapsort(key(beg_entry_num), 
	       minimum(entries_per_bucket, (n+start-beg_entry_num)), 
	       entry_sz, (int(*)(const void *, const void *))SortCmp);
      beg_entry_num += entries_per_bucket;
    }

  // Keep track of place in sorted sublists
  int ra_place_holder[num_buckets];
  for(int bucket=0; bucket<num_buckets; bucket++)
    ra_place_holder[bucket] = bucket*entries_per_bucket;

  // Merge sublists
  FILE *temp_file = fopen(temp_file_name,"w+");
  if (temp_file == NULL)
    {
      printf("help! help! My hair is on fire!\n");
      exit(0);
    }

  int cur_min_bucket = 0;
  while (cur_min_bucket >= 0)
    {
      cur_min_bucket = -1;
      for(int bucket=0; bucket<num_buckets; bucket++)
	if (ra_place_holder[bucket] < 
	    minimum(((bucket+1)*entries_per_bucket), n)) // valid bucket
	  if (cur_min_bucket == -1) // no previous min
	    cur_min_bucket = bucket;
	  else if (SortCmp((int *)key(ra_place_holder[bucket]+start), 
			   (int *)key(ra_place_holder[cur_min_bucket]+start)) 
		   == -1)
	      cur_min_bucket = bucket;
      if (cur_min_bucket > -1)
	{
	  fwrite(key(ra_place_holder[cur_min_bucket]+start), entry_sz, 1, temp_file);
	  ra_place_holder[cur_min_bucket] += 1;
	}
    }


  // copy result back to original arrays
  fseek(temp_file, 0, SEEK_SET);
  for(int tuple_num=0; tuple_num<n; tuple_num++)
    {
      char buf[entry_sz];
      fread(buf, entry_sz, 1, temp_file);
      memcpy(key(tuple_num+start), buf, entry_sz);
    }
  fclose(temp_file);
  unlink(temp_file_name);
}


void bulk_data_t::btreesort (int start, 
			     int n, 
			     int dim_to_sort)
{
  QSortDirection[dim_to_sort-1] *= -1;
  QSortCol = dim_to_sort-1;
  QSortDim = dim;
  QSortPoint = pt_data;

  // calculate bucket stuff
  int num_buckets = 3*n*entry_sz / mem_avail + 1;
  int entries_per_bucket = (n+num_buckets-1)/num_buckets; // (n/num_buckets) round up

  // sort sublists
  int beg_entry_num=start;
  while(beg_entry_num < n+start)
    {
      heapsort(key(beg_entry_num), 
	       minimum(entries_per_bucket, (n+start-beg_entry_num)), 
	       entry_sz, (int(*)(const void *, const void *))SortCmp2);
      beg_entry_num += entries_per_bucket;
    }

  // Keep track of place in sorted sublists
  int ra_place_holder[num_buckets];
  for(int bucket=0; bucket<num_buckets; bucket++)
    ra_place_holder[bucket] = bucket*entries_per_bucket;

  // Merge sublists
  FILE *temp_file = fopen(temp_file_name,"w+");
  if (temp_file == NULL)
    {
      printf("help! help! My hair is on fire!\n");
      exit(0);
    }

  int cur_min_bucket = 0;
  while (cur_min_bucket >= 0)
    {
      cur_min_bucket = -1;
      for(int bucket=0; bucket<num_buckets; bucket++)
	if (ra_place_holder[bucket] < 
	    minimum(((bucket+1)*entries_per_bucket), n)) // valid bucket
	  if (cur_min_bucket == -1) // no previous min
	    cur_min_bucket = bucket;
	  else if (SortCmp2((int *)key(ra_place_holder[bucket]+start), 
			   (int *)key(ra_place_holder[cur_min_bucket]+start)) 
		   == -1)
	      cur_min_bucket = bucket;
      if (cur_min_bucket > -1)
	{
	  fwrite(key(ra_place_holder[cur_min_bucket]+start), entry_sz, 1, temp_file);
	  ra_place_holder[cur_min_bucket] += 1;
	}
    }


  // copy result back to original arrays
  fseek(temp_file, 0, SEEK_SET);
  for(int tuple_num=0; tuple_num<n; tuple_num++)
    {
      char buf[entry_sz];
      fread(buf, entry_sz, 1, temp_file);
      memcpy(key(tuple_num+start), buf, entry_sz);
    }
  fclose(temp_file);
  unlink(temp_file_name);
}


int bulk_data_t::elem_per_page(int elem_to_start, 
			       int num_elem)
{
  int result = ((PAGE_SIZE - HFPage::DPFIXED - 5*sizeof(slot_t) - 
		 page_id_t::page_repr_size-3*sizeof(int2))*8) /
		   (entry_sz*8 + 1);
  result -= 1;
  return result;
}


int bulk_data_t::leftmost(int start_val,
			  int num_elem)
{
  if (num_elem==1)
    return start_val;
  else
    {
      int halfway=(start_val-(num_elem)/2);
      if (SortCmp((int *)key(halfway), (int *)key(start_val)) == 0)
	return leftmost(halfway, halfway-(start_val-num_elem));
      else
	return leftmost(start_val, start_val-halfway);
    }
}


int bulk_data_t::rightmost(int start_val,
			   int num_elem)
{
  if (num_elem==1)
    return start_val;
  else
    {
      int halfway=(start_val+(num_elem)/2);
      if (SortCmp((int *)key(halfway), (int *)key(start_val)) == 0)
	return rightmost(halfway, (start_val+num_elem)-halfway);
      else
	return rightmost(start_val, halfway-start_val);
    }
}


void bulk_data_t::generate_poss_cuts(cut_map_t &cut_map,
				     int       elem_to_start,
				     int       num_elem,
				     int       dim_to_cut)
{
  int orig_max_cuts=cut_map.max_cuts/2;

  QSortCol = dim_to_cut-1;
  cut_map.num_cuts=0;
  for(int center=0; center<orig_max_cuts; center++)
    {
      int cen_loc = elem_to_start + ((long long)center*num_elem)/orig_max_cuts;
      cut_map.cut_map[2*center][0]=leftmost(cen_loc, cen_loc-elem_to_start+1);
//      cut_map.cut_map[2*center+1][0]=rightmost(cen_loc, num_elem-cen_loc)+1;
      cut_map.num_cuts+=2;
    }
}


void bulk_data_t::calc_cuts(cut_map_t &cut_map,
			    int       elem_to_start,
			    int       num_elem,
			    int       dim_to_cut)
{
  cut_map_t poss_cuts(cut_map.max_cuts*2);

  generate_poss_cuts(poss_cuts, elem_to_start, num_elem, dim_to_cut);
  int last_cut_start=elem_to_start-1;
  cut_map.num_cuts=0;
  for(int cut_num=0; cut_num < cut_map.max_cuts; cut_num++)
    if ((poss_cuts.first(cut_num*2) != last_cut_start) &&      // avoid duplict cuts
	(poss_cuts.first(cut_num*2) < elem_to_start+num_elem)) // avoid illegal cuts
      {
	cut_map.cut_map[cut_map.num_cuts][0] = poss_cuts.first(cut_num*2);
	if (cut_map.num_cuts != 0)
	  cut_map.cut_map[cut_map.num_cuts-1][1] = cut_map.first(cut_map.num_cuts)-
	                                  cut_map.first(cut_map.num_cuts-1);
	last_cut_start=cut_map.first(cut_map.num_cuts);
	cut_map.num_cuts++;
      }
  if (cut_map.num_cuts != 0)
    cut_map.cut_map[cut_map.num_cuts-1][1] = elem_to_start+num_elem-
                                             cut_map.first(cut_map.num_cuts-1);
}


char *bulk_data_t::key(int entry_num)
{
  return &(file_arr[entry_num*entry_sz+off]);
}


char *bulk_data_t::data(int entry_num)
{
  return &(file_arr[entry_num*entry_sz+(entry_sz-data_sz)+off]);
}


void bulk_data_t::display()
{
  printf("bulk data:\n");
  for(int i=0; i<num_el; i++)
    {
      for (int j=0; j<2*dim; j++)
	printf("%d ", ((int *)key(i))[j]);
      printf("\n");
    }
}


int bulk_data_t::QSortCol;
int bulk_data_t::QSortDim;
int *bulk_data_t::QSortDirection;
bool bulk_data_t::QSortPoint;
