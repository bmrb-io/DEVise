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

#ifndef BOX_FILE__
#define BOX_FILE__

class gen_key_t;

class dbl_box {
  private :
	double *low,*high;
	int size;
  public :
	dbl_box();
	~dbl_box();
	dbl_box& operator =(dbl_box &box);
	dbl_box& operator =(gen_key_t &box);
	void set_box(const gen_key_t &box);
	void set_box(const gen_key_t &box, char *types);
        inline double min(int dim_num) {return(low[dim_num]);}
	inline double max(int dim_num) {return(high[dim_num]);}
	inline void set_min(int dim_num, double value) 
	  {low[dim_num] = value;}
	inline void set_max(int dim_num, double value) 
	  {high[dim_num] = value;}
        void print();
	int intersect(const dbl_box& B);
};

#endif
