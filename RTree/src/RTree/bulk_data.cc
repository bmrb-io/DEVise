#include<stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include"setup.h"
#include"bulk_data.h"

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
    entry_sz=dim*sizeof(dbword_t)+data_sz;
  else
    entry_sz=2*dim*sizeof(dbword_t)+data_sz;
  off=offset;
  file_arr = (char *)mmap((caddr_t) 0, entry_sz*num_elem+offset, 
		  PROT_READ|PROT_WRITE, MAP_SHARED
#if !defined(SGI) && !defined(LINUX)
		  |MAP_NORESERVE
#endif
		  , infile, 0);
}


int bulk_data_t::SortCmp(dbword_t *A,
			 dbword_t *B)
{
  // Note, this is actually an approximation for rectangle data value
  dbword_t Al;
  dbword_t Bl;
  memcpy((void *)(&Al), (const void *)(A+QSortCol), sizeof(dbword_t));
  memcpy((void *)(&Bl), (const void *)(B+QSortCol), sizeof(dbword_t));
  if (Al < Bl)
    return -1*QSortDirection[QSortCol];
  if (Al > Bl)
    return 1*QSortDirection[QSortCol];
  return 0;
}


/* int bulk_data_t::SortCmp2 (dbword_t *A, 
			   dbword_t *B)
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
*/

void downheap(void *base, size_t nel, size_t width,
	      int (*compar) (const void *, const void *), size_t k)
{
  size_t j;
  char v[width];

  memcpy(v, ((char *)base)+width*(k-1), width);
  while (k <= nel/2)
    {
      j = k+k;
      if (j<nel && 
	  (compar(((char *)base)+width*(j-1), ((char *)base)+width*j) < 0)) j++;
      if (compar(v, ((char *)base)+width*(j-1)) >= 0) break;
      memcpy(((char *)base)+width*(k-1), ((char *)base)+width*(j-1), width); k = j;
    }
  memcpy(((char *)base)+width*(k-1), v, width);
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
      memcpy(base, ((char *)base)+width*(nel-1), width); 
      memcpy(((char *)base)+width*(nel-1), t, width);
      downheap(base, --nel, width, compar, 1);
    }
}



void bulk_data_t::sort_and_cut (int         start, 
				int         n, 
				int         dim_to_sort,
				cut_map_t * &cut_map,
				int         (*elem_per_page)(bulk_data_t &, int, int, int, int_key_t &),
				int         page_size)
{
  int_key_t bbox(dim);
  QSortDirection[dim_to_sort-1] *= -1;
  QSortCol = dim_to_sort-1;
  QSortDim = dim;
  QSortPoint = pt_data;
  int_key_t temp_key(dim);
  bbox.assign(key(start), dim, pt_data);

  // calculate bucket stuff
  int num_buckets = (n*entry_sz+mem_avail-1) / mem_avail;
  int entries_per_bucket = (n+num_buckets-1)/num_buckets; // (n/num_buckets) round up

  // sort sublists
  int beg_entry_num=start;
  while(beg_entry_num < n+start)
    {
#if !defined(LINUX)
      madvise((char *)key(beg_entry_num), entry_sz* 
	      minimum(entries_per_bucket, (n+start-beg_entry_num)), MADV_WILLNEED);
#endif
      for(int i=beg_entry_num; i<beg_entry_num+
	    minimum(entries_per_bucket, (n+start-beg_entry_num)); i++)
	{
	  temp_key.assign(key(i), dim, pt_data);
	  bbox.expand(temp_key);
	}
      heapsort((void *)key(beg_entry_num), 
	       minimum(entries_per_bucket, (n+start-beg_entry_num)), 
	       entry_sz, (int(*)(const void *, const void *))SortCmp);
      beg_entry_num += entries_per_bucket;
      msync((char *)key(beg_entry_num), entry_sz* 
	    minimum(entries_per_bucket, (n+start-beg_entry_num)), MS_SYNC);
    }

  // Keep track of place in sorted sublists
  int ra_place_holder[num_buckets];
  for(int bucket=0; bucket<num_buckets; bucket++)
    ra_place_holder[bucket] = bucket*entries_per_bucket;

  // Merge sublists
  FILE *temp_file = fopen(bulk_temp_file_name,"w+");
  assert(temp_file != NULL);

  int cur_min_bucket = 0;
  while (cur_min_bucket >= 0)
    {
      cur_min_bucket = -1;
      for(int bucket=0; bucket<num_buckets; bucket++)
	if (ra_place_holder[bucket] < 
	    minimum(((bucket+1)*entries_per_bucket), n)) // valid bucket
	  if (cur_min_bucket == -1) // no previous min
	    cur_min_bucket = bucket;
	  else if (SortCmp(key(ra_place_holder[bucket]+start), 
			   key(ra_place_holder[cur_min_bucket]+start)) 
		   == -1)
	      cur_min_bucket = bucket;
      if (cur_min_bucket > -1)
	{
	  fwrite((char *)key(ra_place_holder[cur_min_bucket]+start), entry_sz, 1, 
		 temp_file);
	  ra_place_holder[cur_min_bucket] += 1;
	}
    }

  // Do some calculations for calculating cutmap during merge phase
  int elem_pp = elem_per_page(*this, start, n, page_size, bbox);
  int num_pages = (n+elem_pp-1)/elem_pp;
  int num_cuts = ((int) ( pow(num_pages, 1.0/(dim+1-dim_to_sort))-0.5 ));
  int min_partition_size = (int)(0.5*(((double)n)/(num_cuts+1))+0.99);
  cut_map = new cut_map_t(num_cuts+1);


  // copy result back to original arrays and generate the cutmap
  fseek(temp_file, 0, SEEK_SET);
  cut_map->num_cuts=1;
  cut_map->cut_map[0][0]=start;
  int before=start;
  int next_cutpoint = start+(int)(((double)n)/(num_cuts+1)+0.5);
  for(int tuple_num=0; tuple_num<n; tuple_num++)
    {
      char buf[entry_sz];
      fread(buf, entry_sz, 1, temp_file);
      int current = start+tuple_num;
      memcpy(key(current), buf, entry_sz);
    repeat:
      if ( (current <= next_cutpoint) && // before or at the cut
	  (SortCmp(key(before), key(current)) != 0))
	before = current;
      else if ( (current > next_cutpoint) && // after the cut
		(SortCmp(key(next_cutpoint), key(current)) != 0))
	{
	  int cuts=cut_map->num_cuts;
	  if ((current-next_cutpoint < next_cutpoint-before) ||
	      (before-cut_map->cut_map[cuts-1][0] < min_partition_size))
	    {
	      cut_map->cut_map[cuts-1][1]=
		current-cut_map->cut_map[cuts-1][0];
	      before=cut_map->cut_map[cuts][0]=current;
	      cut_map->num_cuts = cuts = cuts+1;
	      next_cutpoint=
		maximum(start + (int)(cuts*(((double)n)/num_cuts) + 0.5),
			current+min_partition_size);
	    }
	  else
	    {
	      cut_map->cut_map[cuts-1][1]=
		before-cut_map->cut_map[cuts-1][0];
	      cut_map->cut_map[cuts][0]=before;
	      cut_map->num_cuts = cuts = cuts+1;
	      next_cutpoint=
		maximum(start + (int)(cuts*(((double)n)/num_cuts) + 0.5),
			before + min_partition_size);
	      goto repeat; // I know this is evil, but there is a guarantee that
	      // this only gets used once during a particular iteration of for...
	    }
	}
    }
  cut_map->cut_map[cut_map->num_cuts-1][1]=
    start+n-cut_map->cut_map[cut_map->num_cuts-1][0];
  fclose(temp_file);
  remove(bulk_temp_file_name);
}


/*void bulk_data_t::btreesort (int       start, 
			     int       n, 
			     int       dim_to_sort,
			     int_key_t &bbox)
{
  QSortDirection[dim_to_sort-1] *= -1;
  QSortCol = dim_to_sort-1;
  QSortDim = dim;
  QSortPoint = pt_data;
  int_key_t temp_key(dim);
  bbox.assign((dbword_t *)key(start), dim, pt_data);

  // calculate bucket stuff
  int num_buckets = 3*n*entry_sz / mem_avail + 1;
  int entries_per_bucket = (n+num_buckets-1)/num_buckets; // (n/num_buckets) round up

  // sort sublists
  int beg_entry_num=start;
  while(beg_entry_num < n+start)
    {
      madvise(key(beg_entry_num), entry_sz* 
	      minimum(entries_per_bucket, (n+start-beg_entry_num)), MADV_WILLNEED);
      for(int i=beg_entry_num; i<beg_entry_num+
	    minimum(entries_per_bucket, (n+start-beg_entry_num)); i++)
	{
	  temp_key.assign((dbword_t *)key(i), dim, pt_data);
	  bbox.expand(temp_key);
	}
      heapsort(key(beg_entry_num), 
	       minimum(entries_per_bucket, (n+start-beg_entry_num)), 
	       entry_sz, (int(*)(const void *, const void *))SortCmp2);
      beg_entry_num += entries_per_bucket;
      msync(key(beg_entry_num), entry_sz* 
	    minimum(entries_per_bucket, (n+start-beg_entry_num)), MS_SYNC);
    }

  // Keep track of place in sorted sublists
  int ra_place_holder[num_buckets];
  for(int bucket=0; bucket<num_buckets; bucket++)
    ra_place_holder[bucket] = bucket*entries_per_bucket;

  // Merge sublists
  FILE *temp_file = fopen(bulk_temp_file_name,"w+");
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
  remove(bulk_temp_file_name);
}
*/

dbword_t *bulk_data_t::key(int entry_num)
{
  return (dbword_t *)&(file_arr[entry_num*entry_sz+off]);
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


void bulk_data_t::get_line(char *line, 
			   int  infile)
{
  char read_ret=0;
  for(int place=0; read_ret != '\n'; place++)
    {
      read(infile, &read_ret, 1);
      line[place]=read_ret;
      line[place+1]='\0';
    }
}


void bulk_data_t::read_data(int infile)
{
  char line[160];
  int point_data, num_elem, num_dim, data_size;

  get_line(line, infile);
  sscanf(line, "%d %d %d", &num_dim, &data_size, &point_data);
  int curr_place = lseek(infile, 0, SEEK_CUR);
  lseek(infile, 0, SEEK_END);
  int last_place = lseek(infile, 0, SEEK_CUR);
  lseek(infile, curr_place, SEEK_SET);
  if (point_data)
    num_elem = (last_place-curr_place)/(num_dim*sizeof(dbword_t)+data_size);
  else
    num_elem = (last_place-curr_place)/(2*num_dim*sizeof(dbword_t)+data_size);
  lseek(infile, 0, SEEK_SET);
  init(infile, curr_place, num_dim, (point_data==1), num_elem, 
       data_size);
}


int bulk_data_t::QSortCol;
int bulk_data_t::QSortDim;
int *bulk_data_t::QSortDirection;
bool bulk_data_t::QSortPoint;

